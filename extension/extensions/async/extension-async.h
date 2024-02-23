#pragma once
#ifndef _ASYNC_H_
#define _ASYNC_H_

#include <memory>
#include <optional>
#include "extension.h"

enum class BACKBONE_PLUGIN_API Status {
  RUNNING,
  FINISHED,
  FAILED,
  PAUSED,
  CANCELED,
  IDLE,
  STARTING,
  PAUSING,
  RESUMING,
  CANCELLING
};

class BACKBONE_PLUGIN_API Async : public std::enable_shared_from_this<Async> {
 public:
  explicit Async(const std::string& config = {}){};
  virtual ~Async() = default;

  // this uses the fluent api design
  virtual std::shared_ptr<Async> run(
      std::function<std::string(std::shared_ptr<Async>, Status&,
                                const std::string& config,
                                const std::string& runconfig)>,
      const std::string& runconfig = {}) = 0;
  virtual std::string waitResult() = 0;

  virtual Status getStatus() = 0;

  virtual std::shared_ptr<Async> setProgress(
      float progress, const std::string& subchannel = {}) = 0;
  virtual float getProgress(const std::string& subchannel = {}) = 0;

  virtual std::optional<std::string> getMessage() = 0;
  virtual int getMessagesSize() = 0;
  virtual std::shared_ptr<Async> sendMessage(
      const std::string& message, const std::string& channel = "default",
      const std::string& level = "info", const bool structured = false) = 0;

  virtual std::shared_ptr<Async> sendError(
      const std::string& what, const std::string& reason = "",
      const std::string& suggestion = "",
      const std::string& channel = "default") = 0;

  virtual std::shared_ptr<Async> sendWarn(
      const std::string& what, const std::string& reason = "",
      const std::string& suggestion = "",
      const std::string& channel = "default") = 0;

  virtual std::optional<std::string> getCommand() = 0;
  virtual std::optional<std::string> peekCommand() = 0;
  virtual std::shared_ptr<Async> sendCommand(
      const std::string& command, const std::string& channel = "default",
      const std::string& level = "info") = 0;
};
#define ASYNC_IDENTITY "Async"
#define ASYNC_VERSION "v1.0.0"
#define ASYNC_DESCRIPTION "Async"
#endif
