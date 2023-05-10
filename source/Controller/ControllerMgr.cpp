#include "ControllerMgr.h"

MAA_CTRL_NS_BEGIN

ControllerMgr::ControllerMgr(MaaControllerCallback callback, void* callback_arg)
    : callback_(callback), callback_arg_(callback_arg)
{
    runner_ =
        new AsyncRunner<CallItem>([this](auto id, CallItem item) { this->runnerDispatcher(id, std::move(item)); });
}

ControllerMgr::~ControllerMgr() {}

bool ControllerMgr::set_option(std::string_view key, std::string_view value)
{
    return false;
}

bool ControllerMgr::connecting() const
{
    return false;
}

bool ControllerMgr::connected() const
{
    return false;
}

std::vector<unsigned char> ControllerMgr::get_image() const
{
    return std::vector<unsigned char>();
}

std::string ControllerMgr::get_uuid() const
{
    return std::string();
}

void ControllerMgr::runnerDispatcher(MaaCtrlId id, CallItem item)
{
    // TODO: 任务计时

    switch (item.type) {
    case CallItem::Connect: {
        const auto& [adb_path, address, config] = std::get<CallItem::ConnectParams>(item.param);
        this->do_connect(adb_path, address, config);
        break;
    }
    }
}

MAA_CTRL_NS_END
