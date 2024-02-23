
#include "sam.h"
#include <onnxruntime_cxx_api.h>
#include <onnxruntime_session_options_config_keys.h>
#include <stdio.h>
#include <array>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
//#include "logger.h"
// #include <cnpy.h>
using json = nlohmann::json;
namespace backbone {
struct SegResult {
  std::vector<cv::Point> outer_contour;
  std::vector<std::vector<cv::Point>> inner_contour_list;
};

class SAMImpl : public SAM {
  // friend class SegmentAnythingModel;
 public:
  SAMImpl();
  ~SAMImpl();
  virtual void Init(const Params& params) final override;
  virtual void Release() final override;
  virtual std::shared_ptr<backbone::ImageEmbedding> GetImageEmbedding(
      const std::string& imagePath) final override;
  virtual std::vector<Polygon> GetPolygons(
      const std::shared_ptr<backbone::ImageEmbedding>,
      const std::vector<Point>& points = {},
      const std::vector<BBox>& bboxes = {}) final override;
  virtual void DrawResult(const std::string& imagePath,
                          const std::vector<Polygon>& polygons,
                          const std::vector<Point>& points,
                          const std::vector<BBox>& bboxes) final override;
  std::vector<SegResult> PostProcessMask(const cv::Mat& mask);
  std::vector<SegResult> GetSegResult(
      const std::shared_ptr<backbone::ImageEmbedding>,
      const std::vector<Point>& points, const std::vector<BBox>& bboxes);

 private:
  std::atomic<int> bModelLoaded_ = 0;

  Ort::Env env_{ORT_LOGGING_LEVEL_WARNING, "SAM"};
  Ort::SessionOptions* sessionOptions_;
  Ort::RunOptions* runOptions_;

  std::unique_ptr<Ort::Session> sessionEncoder_, sessionDecoder_;

  std::vector<int64_t> encoderInputShape_, encoderOutputShape_;

  cv::Size inputSize_;

  Ort::MemoryInfo memoryInfo{
      Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault)};

  // std::vector<float> outputTensorValuesPre;
  std::string encoderInputName_;
  std::string encoderOutputName_;

  const char* decoderInputNames_[6]{"image_embeddings", "point_coords",
                                    "point_labels",     "mask_input",
                                    "has_mask_input",   "orig_im_size"};
  const char* decoderOutputNames_[3]{"masks", "iou_predictions",
                                     "low_res_masks"};
  // bool getImageEmbedding(cv::Mat& img);
  const std::array<float, 3> pixelMean_{123.675, 116.28, 103.53};
  const std::array<float, 3> pixelStd_{58.395, 57.12, 57.375};

  Params params_;
};
SAMImpl::SAMImpl() : bModelLoaded_(0) {
  sessionOptions_ = NULL;
  runOptions_ = NULL;
}
SAMImpl::~SAMImpl() {
  Release();
}

void SAMImpl::Init(const Params& params) {
  if (bModelLoaded_) {
    throw std::runtime_error("Encoder/Decoder model already loaded");
  }
  sessionOptions_ = new Ort::SessionOptions[2];
  runOptions_ = new Ort::RunOptions[2];
  bModelLoaded_ = 1;
  params_ = params;
  for (int i = 0; i < 2; i++) {
    auto& option = sessionOptions_[i];
    // option.SetIntraOpNumThreads(params.threadsNumber);

    option.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
    Device dev = i == 0 ? params.encoder_device : params.decoder_device;
    if (dev == Device::CUDA) {
      //spdlog::debug("use gpu");
      OrtCUDAProviderOptions options;
      options.device_id = params.device_id;
      options.arena_extend_strategy = 1;  // 1 = kSameAsRequested
      runOptions_[i].AddConfigEntry(
          "memory.enable_memory_arena_shrinkage",
          ("gpu:" + std::to_string(params.device_id)).c_str());
      if (params.gpu_mem_limit_mb > 0) {
        //spdlog::debug("set gpu mem lilmit %d MB", params.gpu_mem_limit_mb);
        options.gpu_mem_limit = params.gpu_mem_limit_mb * 1024 * 1024;
      }
      option.AppendExecutionProvider_CUDA(options);
    } else {
      //spdlog::debug("use cpu!!\n");
    }
  }

#if _MSC_VER
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  auto encModelPath = converter.from_bytes(params.encoder_onnx);
  auto decModelPath = converter.from_bytes(params.decoder_onnx);
#else
  auto encModelPath = params.encoder_onnx;
  auto decModelPath = params.decoder_onnx;
#endif

  sessionEncoder_ = std::make_unique<Ort::Session>(env_, encModelPath.c_str(),
                                                   sessionOptions_[0]);
  if (sessionEncoder_->GetInputCount() != 1 ||
      sessionEncoder_->GetOutputCount() != 1) {
    if (sessionOptions_) {
      delete[] sessionOptions_;
      sessionOptions_ = nullptr;
    }
    if (runOptions_) {
      delete[] runOptions_;
      runOptions_ = nullptr;
    }
    bModelLoaded_ = 0;
    throw std::runtime_error(
        "Encoder model not loaded (invalid input/output count)");
  }
  // spdlog::debug("encoder model init ok ");

  sessionDecoder_ = std::make_unique<Ort::Session>(env_, decModelPath.c_str(),
                                                   sessionOptions_[1]);
  if (sessionDecoder_->GetInputCount() != 6 ||
      sessionDecoder_->GetOutputCount() != 3) {
    bModelLoaded_ = 0;
    if (sessionDecoder_) {
      sessionDecoder_ = nullptr;
    }
    if (sessionEncoder_) {
      sessionEncoder_ = nullptr;
    }
    if (sessionOptions_) {
      delete[] sessionOptions_;
      sessionOptions_ = nullptr;
    }
    if (runOptions_) {
      delete[] runOptions_;
      runOptions_ = nullptr;
    }
    throw std::runtime_error(
        "Decoder model not loaded (invalid input/output count)");
  }
  //spdlog::debug("decoder model init ok ");
  Ort::AllocatorWithDefaultOptions allocator;
  encoderInputShape_ = sessionEncoder_->GetInputTypeInfo(0)
                           .GetTensorTypeAndShapeInfo()
                           .GetShape();
  encoderOutputShape_ = sessionEncoder_->GetOutputTypeInfo(0)
                            .GetTensorTypeAndShapeInfo()
                            .GetShape();
  auto inputNamePtr = sessionEncoder_->GetInputNameAllocated(0, allocator);
  encoderInputName_ = std::string(inputNamePtr.get());
  //spdlog::debug("input name %s", encoderInputName_.c_str());
  auto outputNamePtr = sessionEncoder_->GetOutputNameAllocated(0, allocator);
  encoderOutputName_ = std::string(outputNamePtr.get());
  //spdlog::debug("output name %s", encoderOutputName_.c_str());
  /*if (encoderInputShape_.size() != 4 || encoderOutputShape_.size() != 4)
   {
     throw std::runtime_error("Encoder model not loaded (invalid shape)");
   }*/
  if (encoderInputShape_.size() != 4 || encoderInputShape_[1] != 3) {
    bModelLoaded_ = 0;
    if (sessionDecoder_) {
      sessionDecoder_ = nullptr;
    }
    if (sessionEncoder_) {
      sessionEncoder_ = nullptr;
    }
    if (sessionOptions_) {
      delete[] sessionOptions_;
      sessionOptions_ = nullptr;
    }
    if (runOptions_) {
      delete[] runOptions_;
      runOptions_ = nullptr;
    }
    throw std::runtime_error("Encoder model not loaded (invalid shape)");
  }
  encoderInputShape_[0] =
      encoderInputShape_[0] == -1 ? 1 : encoderInputShape_[0];
  if (encoderInputShape_[0] != 1) {
    bModelLoaded_ = 0;
    if (sessionDecoder_) {
      sessionDecoder_ = nullptr;
    }
    if (sessionEncoder_) {
      sessionEncoder_ = nullptr;
    }
    if (sessionOptions_) {
      delete[] sessionOptions_;
      sessionOptions_ = nullptr;
    }
    if (runOptions_) {
      delete[] runOptions_;
      runOptions_ = nullptr;
    }
    throw std::runtime_error("Encoder model not loaded (invalid shape)");
  }
  // for (int i = 0; i < encoderInputShape_.size(); i++) {
  //   printf("encoder input[%d]=%ld\n", i, encoderInputShape_[i]);
  // }
  for (int i = 0; i < encoderOutputShape_.size(); i++) {
    // printf("encoder output[%d]=%ld\n", i, encoderOutputShape_[i]);
    if (encoderOutputShape_[i] <= 0) {
      bModelLoaded_ = 0;
      if (sessionDecoder_) {
        sessionDecoder_ = nullptr;
      }
      if (sessionEncoder_) {
        sessionEncoder_ = nullptr;
      }
      if (sessionOptions_) {
        delete[] sessionOptions_;
        sessionOptions_ = nullptr;
      }
      if (runOptions_) {
        delete[] runOptions_;
        runOptions_ = nullptr;
      }
      throw std::runtime_error("Encoder model not loaded (invalid shape)");
    }
  }
  inputSize_ = cv::Size(encoderInputShape_[3], encoderInputShape_[2]);
  bModelLoaded_ = 2;
}
void SAMImpl::Release() {
  if (bModelLoaded_ == 2) {
    if (sessionDecoder_) {
      sessionDecoder_ = nullptr;
    }
    if (sessionEncoder_) {
      sessionEncoder_ = nullptr;
    }
    if (sessionOptions_) {
      delete[] sessionOptions_;
      sessionOptions_ = nullptr;
    }
    if (runOptions_) {
      delete[] runOptions_;
      runOptions_ = nullptr;
    }
  }
  bModelLoaded_ = 0;
}
std::shared_ptr<backbone::ImageEmbedding> SAMImpl::GetImageEmbedding(
    const std::string& imagePath) {
  if (bModelLoaded_ != 2) {
    throw std::runtime_error("Encoder/Decoder model not loaded");
  }
  cv::Mat image;

  try {
    image = cv::imread(imagePath, cv::IMREAD_COLOR);
  } catch (const std::exception& e) {
    //LogError(e.what());
    return false;
  };
  // void setErrorStatus(const std::string& statusDetail);
  if (image.channels() != 3 || image.type() != CV_8UC3) {
    image.release();
    throw std::runtime_error("Input is not a 3-channel image");
  }

  auto h = inputSize_.height;
  auto w = inputSize_.width;

  cv::Mat img(h, w, CV_8UC3);
  if (image.size() != cv::Size(w, h)) {
    //spdlog::debug("Image size not match, Resize it");
    cv::resize(image, img, img.size(), 0, 0, cv::INTER_NEAREST);
  } else {
    img = image;
  }
  // cv::imwrite("resize.jpg", img);

  // bgr -> rgb & hwc -> chw
  std::vector<uint8_t> inputTensorValues(1 * 3 * w * h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      inputTensorValues[i * w + j] = img.at<cv::Vec3b>(i, j)[2];
      inputTensorValues[h * w + i * w + j] = img.at<cv::Vec3b>(i, j)[1];
      inputTensorValues[2 * h * w + i * w + j] = img.at<cv::Vec3b>(i, j)[0];
    }
  }

  auto inputTensor = Ort::Value::CreateTensor<uint8_t>(
      memoryInfo, inputTensorValues.data(), inputTensorValues.size(),
      encoderInputShape_.data(), encoderInputShape_.size());

  auto embedding = std::make_shared<ImageEmbedding>();

  embedding->origin_height = image.size().height;
  embedding->origin_width = image.size().width;
  embedding->data.resize(1 * encoderOutputShape_[1] * encoderOutputShape_[2] *
                         encoderOutputShape_[3]);
  auto outputTensor = Ort::Value::CreateTensor<float>(
      memoryInfo, embedding->data.data(), embedding->data.size(),
      encoderOutputShape_.data(), encoderOutputShape_.size());

  const char* inputNames[] = {encoderInputName_.c_str()};
  const char* outputNames[] = {encoderOutputName_.c_str()};

  Ort::RunOptions run_options;
  sessionEncoder_->Run(runOptions_[0], inputNames, &inputTensor, 1, outputNames,
                       &outputTensor, 1);

  image.release();
  img.release();
  // for (int i = 0; i < 10; i++)
  //   printf("output tensor [%d] -> %f \n", i, embedding.data[i]);
  return std::move(embedding);
};

std::vector<SegResult> SAMImpl::PostProcessMask(const cv::Mat& mask) {
  if (bModelLoaded_ != 2) {
    throw std::runtime_error("Encoder/Decoder model not loaded");
  }
  std::vector<SegResult> res;

  std::vector<std::vector<cv::Point>> contours_all;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(mask, contours_all, hierarchy, cv::RETR_CCOMP,
                   cv::CHAIN_APPROX_NONE);
  //cv::findContours(mask, contours_all, hierarchy, cv::RETR_EXTERNAL,
  //                 cv::CHAIN_APPROX_SIMPLE);

  if (hierarchy.empty()) {
    return res;
  }
#if 0
#if 0
  for (int j = 0; j >= 0; j = hierarchy[j][0]) {
    double area = cv::contourArea(contours_all[j]);

    LogDebug("outter contour area {} vs {} ", area, params_.filter_area);
    if (area < params_.filter_area) {
      LogDebug("outter mask filtered {}", area);
      continue;
    }

    std::vector<int> inner_idxs;
    for (int k = hierarchy[j][2]; k >= 0; k = hierarchy[k][0]) {
      float inner_area = cv::contourArea(contours_all[k]);
      LogDebug("inner contour area {} vs {}", inner_area, params_.filter_area);
      if (inner_area < params_.filter_area) {
        LogDebug("inner mask filtered {}", inner_area);
        continue;
      }
      inner_idxs.push_back(k);
    }
    LogDebug("contour area {}", area);

    SegResult v;
    v.outer_contour.resize(contours_all[j].size());
    for (int u = 0; u < contours_all[j].size(); u++) {
      const auto& it = contours_all[j][u];
      v.outer_contour[u] = it;
    }
    v.inner_contour_list.resize(inner_idxs.size());
    int count = 0;
    for (int k : inner_idxs) {
      v.inner_contour_list[count].resize(contours_all[k].size());
      for (int u = 0; u < contours_all[k].size(); u++) {
        const auto& it = contours_all[k][u];
        v.inner_contour_list[count][u] = it;
      }
      count++;
    }
    res.push_back(v);
  }
  return std::move(res);
#endif
  double img_area = mask.rows * mask.cols;
  double total_outter_area = 0;
  for (int j = 0; j >= 0; j = hierarchy[j][0]) {

    std::vector<cv::Point> contour;
    double epsilon = 0.001 * cv::arcLength(contours_all[j], true);
    cv::approxPolyDP(contours_all[j], contour, epsilon, true);

    double area = cv::contourArea(contour);
    //printf("outter contour area %f vs %f \n", area, params_.filter_area);
    if (area < params_.filter_area) {
      //printf("outter mask filtered %f\n", area);
      continue;
    }
    // anylabeling做法
    // https://github.com/vietanhdev/anylabeling/blob/master/anylabeling/services/auto_labeling/segment_anything.py#L123C11-L123C56
    // Remove too big contours ( >90% of image size)
    if (area > 0.9 * img_area) {
      //printf("outter mask filtered %f > 0.9 * %f \n", area, img_area);
      continue;
    }
    total_outter_area += area;

    //std::vector<int> inner_idxs;
    std::vector<std::vector<cv::Point>> inner_contours;
    for (int k = hierarchy[j][2]; k >= 0; k = hierarchy[k][0]) {
      std::vector<cv::Point> inner_contour;
      double epsilon = 0.001 * cv::arcLength(contours_all[k], true);
      cv::approxPolyDP(contours_all[k], inner_contour, epsilon, true);

      float inner_area = cv::contourArea(inner_contour);
      //printf("inner contour area %f vs %f \n", inner_area, params_.filter_area);
      if (inner_area < params_.filter_area) {
        //printf("inner mask filtered %f\n", inner_area);
        continue;
      }
      //inner_idxs.push_back(k);
      inner_contours.push_back(inner_contour);
    }

    SegResult v;
    v.outer_contour.resize(contour[j].size());
    for (int u = 0; u < contour[j].size(); u++) {
      const auto& it = contour[j][u];
      v.outer_contour[u] = it;
    }

    v.inner_contour_list.resize(inner_contours.size());
    int count = 0;
    for (auto& cc : inner_contours) {
      v.inner_contour_list[count].resize(cc.size());
      for (int u = 0; u < cc.size(); u++) {
        const auto& it = cc[u];
        v.inner_contour_list[count][u] = it;
      }
      count++;
    }
    res.push_back(v);
  }
  if (res.empty()) {
    return std::move(res);
  }

  // Remove small contours (area < 20% of average area)
  std::vector<SegResult> filtered_res;
  double mean_area = total_outter_area / res.size();
  for (int i = 0; i < res.size(); i++) {
    double area = cv::contourArea(res[i].outer_contour);
    if (area < 0.2 * mean_area) {
      //printf("filterd area(%lf) < 0.2*mean_area(%lf)\n", area, 0.2*mean_area);
      continue;
    }
    filtered_res.push_back(std::move(res[i]));
  }
  return std::move(filtered_res);
#else
  double img_area = mask.rows * mask.cols;
  double total_outter_area = 0;
  for (int j = 0; j >= 0; j = hierarchy[j][0]) {

    std::vector<cv::Point> contour;
    double epsilon = 0.001 * cv::arcLength(contours_all[j], true);
    cv::approxPolyDP(contours_all[j], contour, epsilon, true);

    double area = cv::contourArea(contour);
    //printf("outter contour area %f vs %f \n", area, params_.filter_area);
    if (area < params_.filter_area) {
      //printf("outter mask filtered %f\n", area);
      continue;
    }
    // anylabeling做法
    // https://github.com/vietanhdev/anylabeling/blob/master/anylabeling/services/auto_labeling/segment_anything.py#L123C11-L123C56
    // Remove too big contours ( >90% of image size)
    if (area > 0.9 * img_area) {
      //printf("outter mask filtered %f > 0.9 * %f \n", area, img_area);
      continue;
    }
    total_outter_area += area;

    //std::vector<int> inner_idxs;
    std::vector<std::vector<cv::Point>> inner_contours;
    for (int k = hierarchy[j][2]; k >= 0; k = hierarchy[k][0]) {
      std::vector<cv::Point> inner_contour;
      double epsilon = 0.001 * cv::arcLength(contours_all[k], true);
      cv::approxPolyDP(contours_all[k], inner_contour, epsilon, true);

      float inner_area = cv::contourArea(inner_contour);
      //printf("inner contour area %f vs %f \n", inner_area, params_.filter_area);
      if (inner_area < params_.filter_area) {
        //printf("inner mask filtered %f\n", inner_area);
        continue;
      }
      //inner_idxs.push_back(k);
      inner_contours.push_back(inner_contour);
    }

    SegResult v;
    v.outer_contour.resize(contour.size());
    for (int u = 0; u < contour.size(); u++) {
      const auto& it = contour[u];
      v.outer_contour[u] = it;
    }

    v.inner_contour_list.resize(inner_contours.size());
    int count = 0;
    for (auto& cc : inner_contours) {
      v.inner_contour_list[count].resize(cc.size());
      for (int u = 0; u < cc.size(); u++) {
        const auto& it = cc[u];
        v.inner_contour_list[count][u] = it;
      }
      count++;
    }
    res.push_back(v);
  }
  if (res.empty()) {
    return std::move(res);
  }

  // Remove small contours (area < 20% of average area)
  std::vector<SegResult> filtered_res;
  double mean_area = total_outter_area / res.size();
  for (int i = 0; i < res.size(); i++) {
    double area = cv::contourArea(res[i].outer_contour);
    if (area < 0.2 * mean_area) {
      //printf("filterd area(%lf) < 0.2*mean_area(%lf)\n", area, 0.2*mean_area);
      continue;
    }
    filtered_res.push_back(std::move(res[i]));
  }
  return std::move(filtered_res);
#endif
}

std::vector<SegResult> SAMImpl::GetSegResult(
    const std::shared_ptr<backbone::ImageEmbedding> embedding,
    const std::vector<Point>& points, const std::vector<BBox>& bboxes) {

#if 1
  if (bModelLoaded_ != 2) {
    throw std::runtime_error("Encoder/Decoder model not loaded");
  }
  size_t embeddingDataSize =
      encoderOutputShape_[1] * encoderOutputShape_[2] * encoderOutputShape_[3];
  if (embedding->data.size() != embeddingDataSize) {
    throw std::runtime_error("invalid embedding data size");
  }
  if (embedding->origin_height == 0 || embedding->origin_width == 0) {
    throw std::runtime_error("invalid embedding origin image size");
  }
  float scaleX = float(inputSize_.width) / float(embedding->origin_width);
  float scaleY = float(inputSize_.height) / float(embedding->origin_height);
  // spdlog::debug("in getSegResult sx: %f, sy: %f", scaleX, scaleY);
  const size_t maskInputSize = 256 * 256;
  float maskInputValues[maskInputSize];
  memset(maskInputValues, 0, sizeof(maskInputValues));
  float hasMaskValues[] = {0};
  float orig_im_size_values[] = {(float)inputSize_.height,
                                 (float)inputSize_.width};
  //LogWarn("=============================================");
  std::vector<float> inputPointValues, inputLabelValues;
  for (auto& point : points) {
    inputPointValues.push_back((float)point.x * scaleX);
    inputPointValues.push_back((float)point.y * scaleY);
    inputLabelValues.push_back((float)point.label);
    // LogWarn("x:{} y:{} label:{}", (float)point.x, (float)point.y,
    //       (float)point.label);
  }
  for (auto& bbox : bboxes) {
    inputPointValues.push_back((float)bbox.x_min * scaleX);
    inputPointValues.push_back((float)bbox.y_min * scaleY);
    inputPointValues.push_back((float)bbox.x_max * scaleX);
    inputPointValues.push_back((float)bbox.y_max * scaleY);
    inputLabelValues.push_back(2);
    inputLabelValues.push_back(3);
  }
  /*
  for (int i = 0; i < inputLabelValues.size(); i++) {
     LogWarn("(%f %f -> %f)\n", inputPointValues[2 * i],
           inputPointValues[2 * i + 1], inputLabelValues[i]);
  }*/
  const int numPoints = inputLabelValues.size();
  std::vector<int64_t> inputPointShape = {1, numPoints, 2},
                       pointLabelsShape = {1, numPoints},
                       maskInputShape = {1, 1, 256, 256},
                       hasMaskInputShape = {1}, origImSizeShape = {2};
  std::vector<Ort::Value> inputTensors;

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, (float*)(embedding->data).data(), embedding->data.size(),
      encoderOutputShape_.data(), encoderOutputShape_.size()));

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, inputPointValues.data(), 2 * numPoints,
      inputPointShape.data(), inputPointShape.size()));

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, inputLabelValues.data(), numPoints, pointLabelsShape.data(),
      pointLabelsShape.size()));

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, maskInputValues, maskInputSize, maskInputShape.data(),
      maskInputShape.size()));

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, hasMaskValues, 1, hasMaskInputShape.data(),
      hasMaskInputShape.size()));

  inputTensors.push_back(Ort::Value::CreateTensor<float>(
      memoryInfo, orig_im_size_values, 2, origImSizeShape.data(),
      origImSizeShape.size()));

  Ort::RunOptions runOptions;
  auto outputTensors = sessionDecoder_->Run(
      runOptions_[1], decoderInputNames_, inputTensors.data(),
      inputTensors.size(), decoderOutputNames_, 3);

  // select max iou mask
  auto masksTensorShape =
      outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
  auto iousTensorShape =
      outputTensors[1].GetTensorTypeAndShapeInfo().GetShape();
  /*
  for (int i = 0; i < masksTensorShape.size(); i++)
    printf("mask.shape[%d]=%ld\n", i, masksTensorShape[i]);
  for (int i = 0; i < iousTensorShape.size(); i++)
    printf("ious.shape[%d]=%ld\n", i, iousTensorShape[i]);
   */

  if (iousTensorShape[1] != masksTensorShape[1]) {
    throw std::runtime_error("masks shape miss match ious shape");
  }
  float* iouValue = outputTensors[1].GetTensorMutableData<float>();
  float maxIou = iouValue[0];
  size_t maxIdx = 0;
  for (int i = 1; i < iousTensorShape[1]; i++) {
    maxIdx = iouValue[i] > maxIou ? i : maxIdx;
    maxIou = iouValue[maxIdx];
  }

  int offset = maxIdx * inputSize_.height * inputSize_.width;
  auto outputMasksValues = outputTensors[0].GetTensorMutableData<float>();
  auto outputMask = cv::Mat(inputSize_.height, inputSize_.width, CV_8UC1);
  for (int i = 0; i < outputMask.rows; i++) {
    for (int j = 0; j < outputMask.cols; j++) {
      outputMask.at<uchar>(i, j) =
          outputMasksValues[offset + i * outputMask.cols + j] > 0 ? 255 : 0;
    }
  }
  // cv::imwrite("mask.jpg", outputMask);

  if (scaleY != 1.0 || scaleX != 1.0) {
    cv::Mat remapMask;
    cv::resize(outputMask, remapMask,
               {(int)embedding->origin_width, (int)embedding->origin_height}, 0,
               0, cv::INTER_NEAREST);
    // cv::imwrite("mask_remap.jpg", remapMask);
    return PostProcessMask(remapMask);
  }
  return PostProcessMask(outputMask);
#endif
}

std::vector<Polygon> SAMImpl::GetPolygons(
    const std::shared_ptr<backbone::ImageEmbedding> embedding,
    const std::vector<Point>& points, const std::vector<BBox>& bboxes) {
  std::vector<backbone::SegResult> segs;

  try {
    segs = GetSegResult(embedding, points, bboxes);
  } catch (const std::exception& e) {
    throw std::runtime_error(e.what());
  };
//#define DRAW_CONTOUR
#ifdef DRAW_CONTOUR
  {
    auto image = cv::imread(embedding->path, cv::IMREAD_COLOR);
    for (auto& p : points) {
      if (p.label == 1) {
        cv::circle(image, {int(p.x), int(p.y)}, 4, {220, 20, 60}, -1);
      } else if (p.label == 0) {
        cv::circle(image, {int(p.x), int(p.y)}, 4, {161, 140, 209}, -1);
      }
    }
    for (auto& bbox : bboxes) {
      cv::rectangle(image, cv::Point{int(bbox.x_min), int(bbox.y_min)},
                    cv::Point{int(bbox.x_max), int(bbox.y_max)},
                    {255, 246, 143}, 4);
    }
    cv::imwrite("current-out-bg.png", image);
    //printf("contours: %ld\n", seg.size());
    for (int i = 0; i < segs.size(); i++) {
      auto poly = segs[i];
      cv::Mat img = image.clone();

      std::vector<std::vector<cv::Point>> contours;
      contours.push_back(poly.outer_contour);
      for (auto c : poly.inner_contour_list) {
        contours.push_back(c);
      }
      cv::Scalar color = cv::Scalar(0, 0, 255);
      cv::drawContours(img, contours, -1, color, 3, 8);

      std::string outName = "current-out_" + std::to_string(i) + ".png";
      cv::imwrite(outName, img);
    }
  }
#endif
  std::vector<Polygon> polygons;
  for (auto& r : segs) {
    Polygon pg;
    pg.points_in.clear();
    pg.points_out.clear();
    for (auto& p1 : r.outer_contour) {
      pg.points_out.push_back(p1.x);
      pg.points_out.push_back(p1.y);
    }
    for (auto& contour : r.inner_contour_list) {
      std::vector<int> pts;
      pts.clear();
      for (auto& p2 : contour) {
        pts.push_back(p2.x);
        pts.push_back(p2.y);
      }
      pg.points_in.push_back(pts);
    }
    polygons.push_back(pg);
  }
  return polygons;
}
void SAMImpl::DrawResult(const std::string& imagePath,
                         const std::vector<Polygon>& polygons,
                         const std::vector<Point>& points,
                         const std::vector<BBox>& bboxes) {
  auto image = cv::imread(imagePath, cv::IMREAD_COLOR);

  if (!points.empty() && !bboxes.empty()) {
    for (auto& p : points) {
      if (p.label == 1) {
        cv::circle(image, {int(p.x), int(p.y)}, 4, {220, 20, 60}, -1);
      } else if (p.label == 0) {
        cv::circle(image, {int(p.x), int(p.y)}, 4, {161, 140, 209}, -1);
      }
    }
    for (auto& bbox : bboxes) {
      cv::rectangle(image, cv::Point{int(bbox.x_min), int(bbox.y_min)},
                    cv::Point{int(bbox.x_max), int(bbox.y_max)},
                    {255, 246, 143}, 4);
    }
  }

  std::vector<std::vector<cv::Point>> inner_contours;
  for (int i = 0; i < polygons.size(); i++) {
    std::vector<cv::Point> contours;
    for (int j = 0; j < polygons[i].points_in.size(); j++) {
      for (int k = 0; k < polygons[i].points_in[j].size() - 1; k += 2) {
        contours.push_back(cv::Point(polygons[i].points_in[j][k],
                                     polygons[i].points_in[j][k + 1]));
      }
    }
    if (contours.size()) {
      inner_contours.push_back(contours);
    }
    std::vector<cv::Point> outer_contours;
    for (int j = 0; j < polygons[i].points_out.size() - 1; j += 2) {
      outer_contours.push_back(
          cv::Point(polygons[i].points_out[j], polygons[i].points_out[j + 1]));
    }
    if (outer_contours.size()) {
      inner_contours.push_back(outer_contours);
    }
  }

  cv::Scalar color = cv::Scalar(0, 0, 255);
  cv::drawContours(image, inner_contours, -1, color);

  cv::imwrite(imagePath + "-out.png", image);
  image.release();
}
std::shared_ptr<SAM> newSAM() {
  return std::make_shared<SAMImpl>();
}

}  // namespace backbone
