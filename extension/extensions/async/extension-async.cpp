#pragma once
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <future>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <queue>
#include <regex>
#include <sstream>
#include <thread>

#include "extension-async.h"

using nlohmann::json;

class AsyncImpl : public Async {
 public:
  explicit AsyncImpl(const std::string& config = {});
  virtual ~AsyncImpl() = default;

  // this uses the fluent api design
  virtual std::shared_ptr<Async> run(
      std::function<std::string(std::shared_ptr<Async>, Status&,
                                const std::string& config,
                                const std::string& runconfig)>,
      const std::string& runconfig = {}) final override;
  virtual std::string waitResult() final override;

  virtual Status getStatus() final override;

  virtual std::shared_ptr<Async> setProgress(
      float progress, const std::string& subchannel = {}) final override;
  virtual float getProgress(const std::string& subchannel = {}) final override;

  virtual std::optional<std::string> getMessage() final override;
  virtual int getMessagesSize() final override;
  virtual std::shared_ptr<Async> sendMessage(
      const std::string& message, const std::string& channel = "default",
      const std::string& level = "info",
      const bool structured = false) final override;

  virtual std::shared_ptr<Async> sendError(
      const std::string& what, const std::string& reason = "",
      const std::string& suggestion = "",
      const std::string& channel = "default") final override;

  virtual std::shared_ptr<Async> sendWarn(
      const std::string& what, const std::string& reason = "",
      const std::string& suggestion = "",
      const std::string& channel = "default") final override;

  virtual std::optional<std::string> getCommand() final override;
  virtual std::optional<std::string> peekCommand() final override;
  virtual std::shared_ptr<Async> sendCommand(
      const std::string& command, const std::string& channel = "default",
      const std::string& level = "info") final override;

 protected:
  std::string wrapRunTask(
      std::function<std::string(std::shared_ptr<Async>, Status&,
                                const std::string& config,
                                const std::string& runconfig)>
          func,
      const std::string& runconfig);

  std::string _config;
  std::queue<std::string> _messages = std::queue<std::string>();
  std::queue<std::string> _commands = std::queue<std::string>();
  Status _status = Status::IDLE;
  std::map<std::string, float> _progress{{{}, 0.f}};
  std::string _result;

  std::mutex _message_mutex{};
  std::condition_variable _message_condition{};

  std::mutex _command_mutex{};
  std::condition_variable _command_condition{};

  std::mutex _task_mutex{};
  std::mutex _progress_mutex{};

 private:
  std::future<std::string> _future_result;
};

AsyncImpl::AsyncImpl(const std::string& config) : _config(config) {}

std::shared_ptr<Async> AsyncImpl::run(
    std::function<std::string(std::shared_ptr<Async>, Status&,
                              const std::string& config,
                              const std::string& runconfig)>
        func,
    const std::string& runconfig) {
  _status = Status::STARTING;
  _future_result = std::async(std::launch::async, &AsyncImpl::wrapRunTask, this,
                              func, std::ref(runconfig));
  return shared_from_this();
}

std::string AsyncImpl::waitResult() {
  auto result = _future_result.get();
  return result;
}

std::shared_ptr<Async> AsyncImpl::setProgress(float progress,
                                              const std::string& subchannel) {
  {
    std::scoped_lock lock(_progress_mutex);
    this->_progress[subchannel] = progress;
  }
  sendMessage(
      fmt::format("{:.4f}", progress),
      (subchannel == "") ? "progress" : fmt::format("progress/{}", subchannel));
  return shared_from_this();
}

Status AsyncImpl::getStatus() {
  return _status;
}

float AsyncImpl::getProgress(const std::string& subchannel) {
  if (_progress.count(subchannel) > 0) {
    return _progress.at(subchannel);
  }
  return 0;
}

std::optional<std::string> AsyncImpl::getMessage() {
  if (_status == Status::IDLE && _messages.empty()) {
    return {};
  }
  std::unique_lock lock(_message_mutex);
  if (((_status == Status::FINISHED) || (_status == Status::CANCELED) ||
       (_status == Status::FAILED) || (_status == Status::PAUSED)) &&
      _messages.empty()) {
    return {};
  }
  _message_condition.wait(lock, [this] {
    return !_messages.empty() ||
           ((_status == Status::FINISHED) || (_status == Status::CANCELED) ||
            (_status == Status::FAILED) || (_status == Status::PAUSED));
  });
  if (_messages.empty()) {
    return {};
  }
  auto msg = _messages.front();
  _messages.pop();
  _message_condition.notify_all();
  return msg;
}

int AsyncImpl::getMessagesSize() {
  return _messages.size();
}

std::shared_ptr<Async> AsyncImpl::sendMessage(const std::string& message,
                                              const std::string& channel,
                                              const std::string& level,
                                              const bool structured) {
  json j;
  j["level"] = level;
  j["channel"] = channel;
  if (structured) {
    j["message"] = json::parse(message);
  } else {
    j["message"] = message;
  }
  j["time"] = fmt::format("{}", std::chrono::system_clock::now());
  {
    std::scoped_lock lock(_message_mutex);
    _messages.push(j.dump());
  }
  _message_condition.notify_all();
  return shared_from_this();
}

std::shared_ptr<Async> AsyncImpl::sendError(const std::string& what,
                                            const std::string& reason,
                                            const std::string& suggestion,
                                            const std::string& channel) {
  json j;
  j["id"] = "E0000";
  std::regex rgx(R"( \((E\d{4}(-[A-Z0-9]+)*)\)$)");
  std::smatch matches;
  if (std::regex_search(what, matches, rgx)) {
    if (matches.size() >= 2) {
      j["id"] = matches[1];
    }
  }
  j["what"] = what;
  j["reason"] = reason;
  j["suggestion"] = suggestion;
  _status = Status::FAILED;
  return sendMessage(j.dump(), channel, "error", true);
}

std::shared_ptr<Async> AsyncImpl::sendWarn(const std::string& what,
                                           const std::string& reason,
                                           const std::string& suggestion,
                                           const std::string& channel) {
  json j;
  j["id"] = "W0000";
  std::regex rgx(R"( \((W\d{4}(-[A-Z0-9]+)*)\)$)");
  std::smatch matches;
  if (regex_search(what, matches, rgx)) {
    if (matches.size() >= 2) {
      j["id"] = matches[1];
    }
  }
  j["what"] = what;
  j["reason"] = reason;
  j["suggestion"] = suggestion;
  return sendMessage(j.dump(), channel, "warning", true);
}

std::optional<std::string> AsyncImpl::getCommand() {
  std::unique_lock lock(_command_mutex);
  if (_commands.empty()) {
    return {};
  }
  auto msg = _commands.front();
  _commands.pop();
  return msg;
}

std::optional<std::string> AsyncImpl::peekCommand() {
  std::unique_lock lock(_command_mutex);
  if (_commands.empty()) {
    return {};
  }
  return _commands.front();
}

std::shared_ptr<Async> AsyncImpl::sendCommand(const std::string& command,
                                              const std::string& channel,
                                              const std::string& level) {

  json j;
  j["level"] = level;
  j["channel"] = channel;
  j["command"] = command;
  j["time"] = fmt::format("{}", std::chrono::system_clock::now());
  {
    std::scoped_lock lock(_command_mutex);
    _commands.push(j.dump());
  }
  return shared_from_this();
}

std::string AsyncImpl::wrapRunTask(
    std::function<std::string(std::shared_ptr<Async>, Status& status,
                              const std::string& config,
                              const std::string& runconfig)>
        func,
    const std::string& runconfig) {
  sendMessage("preparing to run task, waiting for resource");
  std::scoped_lock lock(_task_mutex);
  std::stringstream ss;
  ss << std::this_thread::get_id();
  sendMessage(fmt::format("task deployed at thread {}", ss.str()));
  try {
    _status = Status::RUNNING;
    auto result = func(shared_from_this(), _status, _config, runconfig);
    switch (_status) {
      case Status::PAUSING:
        _status = Status::PAUSED;
        break;
      case Status::CANCELLING:
        _status = Status::CANCELED;
        break;
      case Status::FAILED:
        break;
      default:
        _status = Status::FINISHED;
        break;
    }
    _message_condition.notify_all();
    return result;
  } catch (std::exception const& e) {
    sendError(e.what(), "Unhandled exception in task", "");
    _message_condition.notify_all();
    return "";
  }
}

#ifdef __cplusplus
extern "C" {
#endif
std::string identity(void) {
  return ASYNC_IDENTITY;
}
std::string version(void) {
  return ASYNC_VERSION;
}
std::string description(void) {
  return ASYNC_DESCRIPTION;
}
void* create(const std::string& cfg) {
  return new AsyncImpl(cfg);
}
void destroy(void* p) {
  if (p) {
    auto tp = reinterpret_cast<AsyncImpl*>(p);
    delete tp;
  }
}
#ifdef __cplusplus
}
#endif
