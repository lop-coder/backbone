#ifndef _ROUTER_DATASET_H_
#define _ROUTER_DATASET_H_

#include <string>

#include "annotation/annotationschema.h"
#include "dataset/datasetschema.h"
#include "filter/filterschema.h"
#include "http.h"
#include "image/imageschema.h"
#include "log/logschema.h"
#include "model/modelschema.h"
#include "nlohmann/json.hpp"
#include "project/projectschema.h"
#include "tag/tagschema.h"
using nlohmann::json;

class DatasetRouter {
 public:
  DatasetRouter();
  ~DatasetRouter();

  DECLARE_ROUTER(create);
  DECLARE_ROUTER(list);
  DECLARE_ROUTER(merge);
  DECLARE_ROUTER(get);
  DECLARE_ROUTER(put);
  DECLARE_ROUTER(deleteDataset);
  DECLARE_ROUTER(duplicate);
  DECLARE_ROUTER(listImages);
  DECLARE_ROUTER(addImages);
  DECLARE_ROUTER(getImage);
  DECLARE_ROUTER(rotateImage);

  DECLARE_ROUTER(addAnnotataions);
  DECLARE_ROUTER(getAnnotataions);
  DECLARE_ROUTER(updateAnnotataions);
  DECLARE_ROUTER(deleteAnnotataions);
  DECLARE_ROUTER(getRelatedModels);
  DECLARE_ROUTER(split);
  DECLARE_ROUTER(getStatistics);
  DECLARE_ROUTER(clearAnnotataions);
  DECLARE_ROUTER(setBgAnnotataions);
  DECLARE_ROUTER(deleteImages);
  DECLARE_ROUTER(getRelatedInferModels);
  DECLARE_ROUTER(restore);
  DECLARE_ROUTER(createClsAnnotations);
  DECLARE_ROUTER(checkDatasetUsedByPipeline);
  DECLARE_ROUTER(queryDatasetImageSamples);
  DECLARE_ROUTER(saveDatasetImageSamples);
  DECLARE_ROUTER(cropReiszeDatasetImages);

  DECLARE_ROUTER(createV2);
  DECLARE_ROUTER(mergeV2);
  DECLARE_ROUTER(duplicateV2);
  DECLARE_ROUTER(addImagesV2);

  // DECLARE_ROUTER(listTrainModelTags);
  DECLARE_ROUTER(listTags);
  DECLARE_ROUTER(createTag);
  DECLARE_ROUTER(getTag);
  DECLARE_ROUTER(updateTag);
  DECLARE_ROUTER(deleteTag);
  // DECLARE_ROUTER(listTagsColor);
  DECLARE_ROUTER(mergeTag);

  DECLARE_ROUTER(SaveLatestAnnotatedImage);
  DECLARE_ROUTER(GetLatestAnnotatedImage);
  DECLARE_ROUTER(SaveDatasetPredictions);

  //attr
  DECLARE_ROUTER(ListDatasetAttrs);
  DECLARE_ROUTER(AddDatasetAttrs);
  DECLARE_ROUTER(DeleteDatasetAttrs);
  DECLARE_ROUTER(AddDatasetImageAttrs);
  DECLARE_ROUTER(DeleteDatasetImageAttrs);

  // visualization
  DECLARE_ROUTER(exportVisualization);
  DECLARE_ROUTER(cancelExportVisualization);
  DECLARE_ROUTER(getExportVisualizationProgress);

  // annotation
  DECLARE_ROUTER(exportAnnotation);
  DECLARE_ROUTER(cancelExportAnnotation);
  DECLARE_ROUTER(getExportAnnotationProgress);
};

#endif  //_ROUTER_DATASET_H_
