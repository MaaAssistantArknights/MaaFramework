#include "InstanceMgr.h"

#include "Controller/ControllerMgr.h"
#include "MaaMsg.h"
#include "MaaUtils/Logger.hpp"
#include "Resource/ResourceMgr.h"
#include "Task/PipelineTask.h"

MAA_NS_BEGIN

InstanceMgr::InstanceMgr(MaaInstanceCallback callback, MaaCallbackTransparentArg callback_arg)
    : notifier(callback, callback_arg)
{
    LogFunc << VAR_VOIDP(callback) << VAR_VOIDP(callback_arg);

    task_runner_ = std::make_unique<AsyncRunner<TaskPtr>>(
        std::bind(&InstanceMgr::run_task, this, std::placeholders::_1, std::placeholders::_2));
}

InstanceMgr::~InstanceMgr()
{
    LogFunc;

    if (task_runner_) {
        task_runner_->release();
    }
}

bool InstanceMgr::bind_resource(MaaResourceAPI* resource)
{
    LogInfo << VAR_VOIDP(this) << VAR_VOIDP(resource);

    if (!resource) {
        LogError << "Invalid resource";
        return false;
    }

    if (!resource->loaded()) {
        LogError << "Resource not loaded";
        return false;
    }

    if (resource_) {
        LogWarn << "Resource already binded" << VAR_VOIDP(resource_);
    }

    resource_ = resource;
    return true;
}

bool InstanceMgr::bind_controller(MaaControllerAPI* controller)
{
    LogInfo << VAR_VOIDP(this) << VAR_VOIDP(controller);

    if (!controller) {
        LogError << "Invalid controller";
        return false;
    }

    if (!controller->connected()) {
        LogError << "Controller not connected";
        return false;
    }

    if (controller_) {
        LogWarn << "Controller already binded" << VAR_VOIDP(controller_);
    }

    controller_ = controller;
    return true;
}

bool InstanceMgr::inited() const
{
    return resource_ && controller_ && resource_->loaded() && controller_->connected();
}

// bool InstanceMgr::set_option(MaaInstOption key, const std::string& value)
// {
//     std::ignore = key;
//     std::ignore = value;
//     return false;
// }

MaaTaskId InstanceMgr::post_task(std::string_view task, std::string_view param)
{
    LogInfo << VAR(task) << VAR(param);

    TaskPtr task_ptr = std::make_shared<TaskNS::PipelineTask>(task, this);

    auto param_opt = json::parse(param);
    if (!param_opt) {
        LogError << "Invalid param:" << param;
        return MaaInvalidId;
    }

    bool param_ret = task_ptr->set_param(*param_opt);
    if (!param_ret) {
        LogError << "Set task param failed:" << param;
        return MaaInvalidId;
    }

    if (!task_runner_) {
        LogError << "task_runner is nullptr";
        return MaaInvalidId;
    }
    auto id = task_runner_->post(task_ptr);
    LogTrace << task_ptr->type() << VAR(id);

    return id;
}

bool InstanceMgr::set_task_param(MaaTaskId task_id, std::string_view param)
{
    LogInfo << VAR(task_id) << VAR(param);

    auto param_opt = json::parse(param);
    if (!param_opt) {
        LogError << "Invalid param:" << param;
        return false;
    }

    bool ret = false;
    task_runner_->for_each([&](TaskId id, TaskPtr task_ptr) {
        if (static_cast<TaskId>(task_id) != id) {
            return;
        }
        ret = task_ptr->set_param(*param_opt);
        LogTrace << task_ptr->type() << VAR(ret);
    });

    return ret;
}

MaaStatus InstanceMgr::status(MaaTaskId task_id) const
{
    if (!task_runner_) {
        LogError << "task_runner is nullptr";
        return MaaStatus_Invalid;
    }
    return task_runner_->status(task_id);
}

MaaStatus InstanceMgr::wait(MaaTaskId task_id) const
{
    if (!task_runner_) {
        LogError << "task_runner is nullptr";
        return MaaStatus_Invalid;
    }
    task_runner_->wait(task_id);
    return task_runner_->status(task_id);
}

MaaBool InstanceMgr::all_finished() const
{
    if (!task_runner_) {
        LogError << "task_runner is nullptr";
        return false;
    }
    return !task_runner_->running();
}

void InstanceMgr::stop()
{
    LogFunc;

    if (resource_) {
        resource_->on_stop();
    }
    if (controller_) {
        controller_->on_stop();
    }

    task_runner_->for_each([](TaskId id, TaskPtr task_ptr) {
        std::ignore = id;
        if (task_ptr) {
            task_ptr->on_stop();
        }
    });
    task_runner_->clear();
}

std::string InstanceMgr::get_resource_hash() const
{
    return resource_ ? resource_->get_hash() : std::string();
}

std::string InstanceMgr::get_controller_uuid() const
{
    return controller_ ? controller_->get_uuid() : std::string();
}

MAA_RES_NS::ResourceMgr* InstanceMgr::resource()
{
    return dynamic_cast<MAA_RES_NS::ResourceMgr*>(resource_);
}

MAA_CTRL_NS::ControllerMgr* InstanceMgr::controller()
{
    return dynamic_cast<MAA_CTRL_NS::ControllerMgr*>(controller_);
}

InstanceStatus* InstanceMgr::status()
{
    return &status_;
}

bool InstanceMgr::run_task(TaskId id, TaskPtr task_ptr)
{
    LogFunc << VAR(id) << VAR(task_ptr);

    const json::value details = {
        { "id", id },
        { "type", std::string(task_ptr->type()) },
        { "uuid", get_controller_uuid() },
        { "hash", get_resource_hash() },
    };

    notifier.notify(MaaMsg_Task_Started, details);

    LogInfo << "task start:" << VAR(details);
    bool ret = task_ptr->run();
    LogInfo << "task end:" << VAR(details) << VAR(ret);

    notifier.notify(ret ? MaaMsg_Task_Completed : MaaMsg_Task_Failed, details);

    return ret;
}

MAA_NS_END
