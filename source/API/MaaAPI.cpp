#include "MaaAPI.h"

#include <meojson/json.hpp>

#include "Controller/AdbController.h"
#include "Controller/CustomController.h"
#include "Instance/InstanceMgr.h"
#include "MaaControlUnit/ControlUnitAPI.h"
#include "MaaUtils/Logger.hpp"
#include "Option/GlobalOptionMgr.h"
#include "Resource/ResourceMgr.h"
#include "Utils/Platform.hpp"

MaaBool MaaSetGlobalOption(MaaGlobalOption key, MaaString value)
{
    LogFunc << VAR(key) << VAR(value);

    return MAA_NS::GlobalOptionMgr::get_instance().set_option(key, value);
}

MaaResourceHandle MaaResourceCreate(MaaString user_path, MaaResourceCallback callback,
                                    MaaCallbackTransparentArg callback_arg)
{
    LogFunc << VAR(user_path) << VAR_VOIDP(callback) << VAR_VOIDP(callback_arg);

    return new MAA_RES_NS::ResourceMgr(MAA_NS::path(user_path), callback, callback_arg);
}

void MaaResourceDestroy(MaaResourceHandle* res)
{
    LogFunc << VAR_VOIDP(res);

    if (res == nullptr || *res == nullptr) {
        return;
    }
    delete *res;
    *res = nullptr;
}

MaaResId MaaResourcePostResource(MaaResourceHandle res, MaaString path)
{
    LogFunc << VAR_VOIDP(res) << VAR(path);

    if (!res) {
        return MaaInvalidId;
    }
    return res->post_resource(MAA_NS::path(path));
}

MaaStatus MaaResourceStatus(MaaResourceHandle res, MaaResId id)
{
    // LogFunc << VAR_VOIDP(res) << VAR(id);

    if (!res) {
        return MaaStatus_Invalid;
    }
    return res->status(id);
}

MaaStatus MaaResourceWait(MaaResourceHandle res, MaaResId id)
{
    // LogFunc << VAR_VOIDP(res) << VAR(id);

    if (!res) {
        return MaaStatus_Invalid;
    }
    return res->wait(id);
}

MaaBool MaaResourceLoaded(MaaResourceHandle res)
{
    // LogFunc << VAR_VOIDP(res);

    if (!res) {
        return false;
    }
    return res->loaded();
}

MaaBool MaaResourceSetOption(MaaResourceHandle res, MaaResOption key, MaaString value)
{
    LogFunc << VAR_VOIDP(res) << VAR(key) << VAR(value);

    if (!res) {
        return false;
    }
    return res->set_option(key, value);
}

MaaSize MaaResourceGetHash(MaaResourceHandle res, char* buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(res) << VAR_VOIDP(buff) << VAR(buff_size);

    if (!res || !buff) {
        return MaaNullSize;
    }
    auto hash = res->get_hash();
    size_t size = hash.size();
    if (size >= buff_size) {
        return MaaNullSize;
    }
    memcpy(buff, hash.c_str(), size);
    return size;
}

MaaControllerHandle MaaAdbControllerCreate(MaaString adb_path, MaaString address, MaaAdbControllerType type,
                                           MaaJsonString config, MaaControllerCallback callback,
                                           MaaCallbackTransparentArg callback_arg)
{
    LogFunc << VAR(adb_path) << VAR(address) << VAR_VOIDP(callback) << VAR_VOIDP(callback_arg);

    auto unit_mgr = MAA_CTRL_UNIT_NS::create_adb_controller_unit(adb_path, address, type, config);
    if (!unit_mgr) {
        LogError << "Failed to create controller unit";
        return nullptr;
    }

    return new MAA_CTRL_NS::AdbController(adb_path, address, std::move(unit_mgr), callback, callback_arg);
}

MaaControllerHandle MaaCustomControllerCreate(MaaCustomControllerHandle handle, MaaControllerCallback callback,
                                              MaaCallbackTransparentArg callback_arg)
{
    LogFunc << VAR(handle) << VAR_VOIDP(callback) << VAR_VOIDP(callback_arg);

    if (!handle) {
        return nullptr;
    }

    return new MAA_CTRL_NS::CustomController(handle, callback, callback_arg);
}

void MaaControllerDestroy(MaaControllerHandle* ctrl)
{
    LogFunc << VAR_VOIDP(ctrl);

    if (ctrl == nullptr || *ctrl == nullptr) {
        return;
    }
    delete *ctrl;
    *ctrl = nullptr;
}

MaaBool MaaControllerSetOption(MaaControllerHandle ctrl, MaaCtrlOption key, MaaString value)
{
    LogFunc << VAR_VOIDP(ctrl) << VAR(key) << VAR(value);

    if (!ctrl) {
        return false;
    }
    return ctrl->set_option(key, value);
}

MaaCtrlId MaaControllerPostConnection(MaaControllerHandle ctrl)
{
    LogFunc << VAR_VOIDP(ctrl);
    if (!ctrl) {
        return false;
    }
    return ctrl->post_connection();
}

MaaCtrlId MaaControllerPostClick(MaaControllerHandle ctrl, int32_t x, int32_t y)
{
    LogFunc << VAR_VOIDP(ctrl) << VAR(x) << VAR(y);

    if (!ctrl) {
        return MaaInvalidId;
    }
    return ctrl->post_click(x, y);
}

MaaCtrlId MaaControllerPostSwipe(MaaControllerHandle ctrl, int32_t* x_steps_buff, int32_t* y_steps_buff,
                                 int32_t* step_delay_buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(ctrl) << VAR_VOIDP(x_steps_buff) << VAR_VOIDP(y_steps_buff) << VAR_VOIDP(step_delay_buff)
            << VAR(buff_size);

    if (!ctrl || !x_steps_buff || !y_steps_buff || !step_delay_buff || buff_size < 2) {
        return MaaInvalidId;
    }

    std::vector<int32_t> x_steps(x_steps_buff, x_steps_buff + buff_size);
    std::vector<int32_t> y_steps(y_steps_buff, y_steps_buff + buff_size);
    std::vector<int32_t> step_delay(step_delay_buff, step_delay_buff + buff_size);

    return ctrl->post_swipe(std::move(x_steps), std::move(y_steps), std::move(step_delay));
}

MaaCtrlId MaaControllerPostScreencap(MaaControllerHandle ctrl)
{
    LogFunc << VAR_VOIDP(ctrl);

    if (!ctrl) {
        return MaaInvalidId;
    }
    return ctrl->post_screencap();
}

MaaStatus MaaControllerStatus(MaaControllerHandle ctrl, MaaCtrlId id)
{
    // LogFunc << VAR_VOIDP(ctrl) << VAR(id);

    if (!ctrl) {
        return MaaStatus_Invalid;
    }
    return ctrl->status(id);
}

MaaStatus MaaControllerWait(MaaControllerHandle ctrl, MaaCtrlId id)
{
    // LogFunc << VAR_VOIDP(ctrl) << VAR(id);

    if (!ctrl) {
        return MaaStatus_Invalid;
    }
    return ctrl->wait(id);
}

MaaBool MaaControllerConnected(MaaControllerHandle ctrl)
{
    LogFunc << VAR_VOIDP(ctrl);
    if (!ctrl) {
        return false;
    }
    return ctrl->connected();
}

MaaSize MaaControllerGetImage(MaaControllerHandle ctrl, void* buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(ctrl) << VAR_VOIDP(buff) << VAR(buff_size);

    if (!ctrl || !buff) {
        return MaaNullSize;
    }
    auto image = ctrl->get_image();
    size_t size = image.size();
    if (size >= buff_size) {
        return MaaNullSize;
    }
    memcpy(buff, image.data(), size);
    return size;
}

MaaSize MaaControllerGetUUID(MaaControllerHandle ctrl, char* buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(ctrl) << VAR_VOIDP(buff) << VAR(buff_size);

    if (!ctrl || !buff) {
        return MaaNullSize;
    }
    auto uuid = ctrl->get_uuid();
    size_t size = uuid.size();
    if (size >= buff_size) {
        return MaaNullSize;
    }
    memcpy(buff, uuid.c_str(), size);
    return size;
}

// TODO: 考虑重构而非模仿
static MaaInstanceHandle handle = nullptr;

MaaBool MaaInit(MaaInstanceCallback callback, MaaCallbackTransparentArg callback_arg)
{
    LogFunc << VAR_VOIDP(callback) << VAR_VOIDP(callback_arg);

    if (handle) {
        return false;
    }

    handle = new (std::nothrow) MAA_NS::InstanceMgr(callback, callback_arg);

    return !!handle;
}

MaaBool MaaDeinit()
{
    LogFunc;

    if (!handle) {
        return false;
    }

    delete handle;
    handle = nullptr;
    return true;
}

MaaBool MaaBindResource(MaaResourceHandle res)
{
    LogFunc << VAR_VOIDP(res);

    if (!handle || !res) {
        return false;
    }
    return handle->bind_resource(res);
}

MaaBool MaaBindController(MaaControllerHandle ctrl)
{
    LogFunc << VAR_VOIDP(ctrl);

    if (!handle || !ctrl) {
        return false;
    }
    return handle->bind_controller(ctrl);
}

MaaBool MaaInited()
{
    LogFunc;
    if (!handle) {
        return false;
    }
    return handle->inited();
}

MaaTaskId MaaPostTask(MaaString task, MaaJsonString param)
{
    LogFunc << VAR(task) << VAR(param);

    if (!handle) {
        return MaaInvalidId;
    }
    return handle->post_task(task, param);
}

MaaBool MaaSetTaskParam(MaaTaskId id, MaaJsonString param)
{
    LogFunc << VAR(id) << VAR(param);

    if (!handle) {
        return false;
    }
    return handle->set_task_param(id, param);
}

MaaStatus MaaTaskStatus(MaaTaskId id)
{
    // LogFunc << VAR(id);

    if (!handle) {
        return MaaStatus_Invalid;
    }
    return handle->status(id);
}

MaaStatus MaaTaskWait(MaaTaskId id)
{
    // LogFunc << VAR(id);

    if (!handle) {
        return MaaStatus_Invalid;
    }
    return handle->wait(id);
}

MaaBool MaaTaskAllFinished()
{
    // LogFunc << VAR(id);
    if (!handle) {
        return false;
    }
    return handle->all_finished();
}

void MaaStop()
{
    LogFunc;

    if (!handle) {
        return;
    }

    handle->stop();
}

MaaSize MaaGetResourceHash(char* buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(buff) << VAR(buff_size);

    if (!handle || !buff) {
        return MaaNullSize;
    }
    auto hash = handle->get_resource_hash();
    size_t size = hash.size();
    if (size >= buff_size) {
        return MaaNullSize;
    }
    memcpy(buff, hash.c_str(), size);
    return size;
}

MaaSize MaaGetControllerUUID(char* buff, MaaSize buff_size)
{
    LogFunc << VAR_VOIDP(buff) << VAR(buff_size);

    if (!handle || !buff) {
        return MaaNullSize;
    }
    auto uuid = handle->get_controller_uuid();
    size_t size = uuid.size();
    if (size >= buff_size) {
        return MaaNullSize;
    }
    memcpy(buff, uuid.c_str(), size);
    return size;
}

MaaString MaaVersion()
{
    return MAA_VERSION;
}
