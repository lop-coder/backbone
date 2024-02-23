#pragma once
#ifndef _SAM_H_
#define _SAM_H_

#include <nlohmann/json.hpp>
#include <vector>

#ifndef _WIN32
#define SAM_API
#elif defined SAM_DLL_EXPORTS
#define SAM_API __declspec(dllexport)
#else
#define SAM_API __declspec(dllimport)
#endif

namespace backbone {

struct SAM_API Error {
  int code;
  std::string msg;
};

struct SAM_API Point {
  size_t x;
  size_t y;
  int label;  // 正点1，负点0
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Point, x, y, label);
};
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Point, x, y, label);

struct SAM_API BBox {

  size_t x_min;
  size_t y_min;
  size_t x_max;
  size_t y_max;
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(BBox, x_min, y_min, x_max, y_max);
};

struct SAM_API Polygon {
  std::vector<int> points_out;
  std::vector<std::vector<int>> points_in;
};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Polygon, points_out, points_in);

enum SAM_API Device {
  CPU = 0,
  CUDA = 1,
};

struct SAM_API Params {
  std::string encoder_onnx = "sammodels/sam_preprocess_vitb_quant.onnx";
  std::string decoder_onnx = "sammodels/sam_vitb.onnx";

  Device encoder_device = Device::CPU;
  Device decoder_device = Device::CPU;
  int device_id = 0;
  float filter_area = 100;
  int gpu_mem_limit_mb = 0;
};

struct SAM_API ImageEmbedding {
  std::string path;
  std::vector<float> data;
  size_t origin_height;
  size_t origin_width;
};

class SAM_API SAM {
 public:
  SAM(){};
  ~SAM(){};
  virtual void Init(const Params& params) = 0;
  virtual void Release() = 0;
  virtual std::shared_ptr<backbone::ImageEmbedding> GetImageEmbedding(
      const std::string& imagePath) = 0;

  virtual std::vector<Polygon> GetPolygons(
      const std::shared_ptr<backbone::ImageEmbedding> embedding,
      const std::vector<Point>& points = {},
      const std::vector<BBox>& bboxes = {}) = 0;
  virtual void DrawResult(const std::string& imagePath,
                          const std::vector<Polygon>& polygons,
                          const std::vector<Point>& points = {},
                          const std::vector<BBox>& bboxes = {}) = 0;
};

SAM_API std::shared_ptr<SAM> newSAM();
}  // namespace backbone

#endif  // _SAM_H_
