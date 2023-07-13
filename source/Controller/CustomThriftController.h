#pragma once
#include "ControllerMgr.h"

#include "ThriftController.h"

MAA_CTRL_NS_BEGIN

class CustomThriftController : public ControllerMgr
{
public:
    CustomThriftController(MaaThriftControllerType type, const std::string param, MaaControllerCallback callback,
                           void* callback_arg);
    virtual ~CustomThriftController() override;

    virtual std::string get_uuid() const override;

protected:
    virtual bool _connect() override;
    virtual std::pair<int, int> _get_resolution() const override;
    virtual void _click(ClickParams param) override;
    virtual void _swipe(SwipeParams param) override;
    virtual cv::Mat _screencap() override;
    virtual bool _start_app(AppParams param) override;
    virtual bool _stop_app(AppParams param) override;

private:
    std::shared_ptr<ThriftCustomController::ThriftControllerClient> client_ = nullptr;
    std::shared_ptr<apache::thrift::transport::TTransport> transport_ = nullptr;
};

MAA_CTRL_NS_END
