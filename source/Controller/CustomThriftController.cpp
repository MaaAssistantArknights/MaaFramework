#include "CustomThriftController.h"

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "MaaUtils/Logger.hpp"
#include "Utils/NoWarningCV.h"

MAA_CTRL_NS_BEGIN

CustomThriftController::CustomThriftController(MaaThriftControllerType type, const std::string param,
                                               MaaControllerCallback callback, void* callback_arg)
    : ControllerMgr(callback, callback_arg)
{
    LogTrace << VAR(type) << VAR(param);

    using namespace apache::thrift;

    std::shared_ptr<transport::TSocket> socket;

    switch (type) {
    case MaaThriftControllerTypeEnum::MaaThriftControllerType_Socket:
        if (auto pos = param.find(':')) {
            auto host = param.substr(0, pos);
            auto port = param.substr(pos + 1);
            socket = std::make_shared<transport::TSocket>(host, std::stoi(port));
            break;
        }
        else {
            LogError << "Invalid param: " << param;
            throw std::runtime_error("MaaThriftController: Invalid param");
        }
    case MaaThriftControllerTypeEnum::MaaThriftControllerType_UnixDomainSocket:
        socket = std::make_shared<transport::TSocket>(param);
        break;
    default:
        LogError << "Unknown type: " << type;
        throw std::runtime_error("MaaThriftController: Unknown type");
    }

    transport_ = std::make_shared<transport::TBufferedTransport>(socket);
    auto protocol = std::make_shared<protocol::TBinaryProtocol>(transport_);

    client_ = std::make_shared<ThriftCustomController::ThriftControllerClient>(protocol);
}

CustomThriftController::~CustomThriftController()
{
    if (transport_) {
        transport_->close();
    }
}

std::string CustomThriftController::get_uuid() const
{
    LogFunc;

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return {};
    }

    std::string uuid;
    client_->get_uuid(uuid);
    return uuid;
}

bool CustomThriftController::_connect()
{
    try {
        transport_->open();
    }
    catch (const std::exception& e) {
        LogError << "transport_->open() failed: " << e.what();
        return false;
    }

    return client_->connect();
}

std::pair<int, int> CustomThriftController::_get_resolution() const
{
    LogFunc;

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return {};
    }

    ThriftCustomController::Size resolution;
    client_->get_resolution(resolution);
    return { resolution.x, resolution.y };
}

void CustomThriftController::_click(ClickParams param)
{
    LogFunc << VAR(param.x) << VAR(param.y);

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return;
    }

    ThriftCustomController::ClickParams click_param;
    click_param.point.x = param.x;
    click_param.point.y = param.y;

    client_->click(click_param);
}

void CustomThriftController::_swipe(SwipeParams param)
{
    LogFunc << VAR(param.steps.size()) << VAR(param.steps.front()) << VAR(param.steps.back());

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return;
    }

    ThriftCustomController::SwipeParams swipe_param;
    for (const auto& step : param.steps) {
        ThriftCustomController::SwipeStep thrift_step;
        thrift_step.point.x = step.x;
        thrift_step.point.y = step.y;
        thrift_step.delay = step.delay;
        swipe_param.steps.push_back(thrift_step);
    }

    client_->swipe(swipe_param);
}

cv::Mat CustomThriftController::_screencap()
{
    LogFunc;

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return {};
    }

    ThriftCustomController::CustomImage img;
    client_->screencap(img);
    if (img.data.empty()) {
        LogError << "client_->screencap() return empty buffer";
        return {};
    }

    cv::Mat orig_mat(img.size.x, img.size.y, img.type, img.data.data());
    cv::Mat out_mat;
    orig_mat.copyTo(out_mat);

    return out_mat;
}

bool CustomThriftController::_start_app(AppParams param)
{
    LogFunc << VAR(param.package);

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return false;
    }

    return client_->start_game(param.package);
}

bool CustomThriftController::_stop_app(AppParams param)
{
    LogFunc << VAR(param.package);

    if (!client_ || !transport_->isOpen()) {
        LogError << "client_ is nullptr or transport_ is not open";
        return false;
    }

    return client_->stop_game(param.package);
}

MAA_CTRL_NS_END
