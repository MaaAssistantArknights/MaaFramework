#include "Unit.h"

#include "Utils/Logger.hpp"
#include "Utils/StringMisc.hpp"

MAA_CTRL_UNIT_NS_BEGIN

UnitHelper::UnitHelper(std::shared_ptr<PlatformIO> io_ptr) : io_ptr_(io_ptr) {}

void UnitHelper::set_replacement(std::map<std::string, std::string> argv_replace)
{
    argv_replace_ = std::move(argv_replace);
}

bool UnitHelper::parse_argv(const std::string& key, const json::value& config, Argv& argv)
{
    auto opt = config.find<json::value>(key);
    if (!opt || !opt->is_array()) {
        return false;
    }

    const auto& arr = opt->as_array();
    if (std::any_of(arr.begin(), arr.end(), [](const json::value& val) { return !val.is_string(); })) {
        LogError << "Parse config failed:" << VAR(key);
        return false;
    }

    argv.clear();
    argv.reserve(arr.size());
    ranges::transform(arr, std::back_inserter(argv), [](const json::value& val) { return val.as_string(); });
    return true;
}

std::optional<std::string> UnitHelper::command(const std::vector<std::string>& cmd, bool recv_by_socket,
                                               int64_t timeout)
{
    auto start_time = std::chrono::steady_clock::now();

    std::string pipe_data;
    std::string sock_data;
    int ret = io_ptr_->call_command(cmd, recv_by_socket, pipe_data, sock_data, timeout);

    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time).count();

    LogInfo << VAR(cmd) << VAR(ret) << VAR(pipe_data.size()) << VAR(sock_data.size()) << VAR(duration);
    if (!pipe_data.empty() && pipe_data.size() < 4096) {
        LogTrace << "stdout output:" << Logger::separator::newline << pipe_data;
    }
    if (recv_by_socket && !sock_data.empty() && sock_data.size() < 4096) {
        LogTrace << "socket output:" << Logger::separator::newline << sock_data;
    }

    if (ret != 0) {
        LogError << "call_command failed" << VAR(cmd) << VAR(ret);
        return std::nullopt;
    }

    return recv_by_socket ? sock_data : pipe_data;
}

bool Connection::parse(const json::value& config)
{
    return parse_argv("Connect", config, connect_argv_) && parse_argv("KillServer", config, kill_server_argv_);
}

bool Connection::connect()
{
    LogFunc;

    auto argv = connect_argv_;
    for (auto& s : argv) {
        s = string_replace_all(s, argv_replace_);
    }

    auto cmd_ret = command(argv, false, 60LL * 1000);

    return cmd_ret ? cmd_ret->find("error") == std::string::npos : false;
}

bool Connection::kill_server()
{
    LogFunc;

    auto argv = kill_server_argv_;
    for (auto& s : argv) {
        s = string_replace_all(s, argv_replace_);
    }

    return command(argv, false, 60LL * 1000).has_value();
}

MAA_CTRL_UNIT_NS_END