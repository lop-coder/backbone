#include <algorithm>
#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <thread>
#include "sam.h"

using namespace backbone;

#include "backward_wrapper.h"

int main(int argc, char** argv) {
  auto backward = newBackwardWrapper();
  backward->Init("./backward.dumpcore");
  try {
    backbone::Params params;
    params.encoder_onnx = "../../res/sammodels/sam_preprocess_vitb_quant.onnx";
    params.decoder_onnx = "../../res/sammodels/sam_vitb.onnx";
    auto sam = newSAM();
    sam->Init(params);
    auto embedding = sam->GetImageEmbedding("../../res/image.jpg");
    auto polygons = sam->GetPolygons(embedding);
    sam->DrawResult("../../res/image.jpg", polygons);
    sam->Release();
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return false;
  };

  return 0;
}
