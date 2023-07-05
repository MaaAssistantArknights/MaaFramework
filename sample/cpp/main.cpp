#include "MaaAPI.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

std::string read_adb_config(const std::filesystem::path& cur_dir);
bool demo_waiting(const std::filesystem::path& cur_dir);
bool demo_polling(const std::filesystem::path& cur_dir);

std::string adb = "adb";
std::string adb_address = "127.0.0.1:5555";
std::string adb_config;

int main([[maybe_unused]] int argc, char** argv)
{
    if (argc == 3) {
        adb = argv[1];
        adb_address = argv[2];
    }

    const auto cur_dir = std::filesystem::path(argv[0]).parent_path();
    adb_config = read_adb_config(cur_dir);

    MaaSetGlobalOption(MaaGlobalOption_Logging, (cur_dir / "debug").string().c_str());

    // demo_polling(cur_dir);
    demo_waiting(cur_dir);

    return 0;
}

bool demo_waiting(const std::filesystem::path& cur_dir)
{
    MaaInit(nullptr, nullptr);
    auto resource_handle = MaaResourceCreate((cur_dir / "cache").string().c_str(), nullptr, nullptr);
    auto controller_handle =
        MaaAdbControllerCreate(adb.c_str(), adb_address.c_str(),
                               MaaAdbControllerType_Input_Preset_Minitouch | MaaAdbControllerType_Screencap_FastestWay,
                               adb_config.c_str(), nullptr, nullptr);

    MaaBindResource(resource_handle);
    MaaBindController(controller_handle);

    auto resource_id = MaaResourcePostResource(resource_handle, (cur_dir / "resource").string().c_str());
    auto connection_id = MaaControllerPostConnection(controller_handle);

    MaaResourceWait(resource_handle, resource_id);
    MaaControllerWait(controller_handle, connection_id);

    auto destroy = [&]() {
        MaaDeinit();
        MaaResourceDestroy(&resource_handle);
        MaaControllerDestroy(&controller_handle);
    };

    if (!MaaInited()) {
        destroy();
        return false;
    }

    auto task_id = MaaPostTask("Test", MaaTaskParam_Empty);
    MaaTaskWait(task_id);

    destroy();

    return true;
}

bool demo_polling(const std::filesystem::path& cur_dir)
{
    auto resource_handle = MaaResourceCreate((cur_dir / "cache").string().c_str(), nullptr, nullptr);
    auto resource_id = MaaResourcePostResource(resource_handle, (cur_dir / "resource").string().c_str());

    auto controller_handle =
        MaaAdbControllerCreate(adb.c_str(), adb_address.c_str(),
                               MaaAdbControllerType_Input_Preset_Minitouch | MaaAdbControllerType_Screencap_FastestWay,
                               adb_config.c_str(), nullptr, nullptr);
    auto connection_id = MaaControllerPostConnection(controller_handle);

    for (auto status = MaaResourceStatus(resource_handle, resource_id);
         status == MaaStatus_Pending || status == MaaStatus_Running;
         status = MaaResourceStatus(resource_handle, resource_id)) {
        std::this_thread::yield();
    }
    for (auto status = MaaControllerStatus(controller_handle, connection_id);
         status == MaaStatus_Pending || status == MaaStatus_Running;
         status = MaaControllerStatus(controller_handle, connection_id)) {
        std::this_thread::yield();
    }

    MaaInit(nullptr, nullptr);
    MaaBindResource(resource_handle);
    MaaBindController(controller_handle);

    auto destroy = [&]() {
        MaaDeinit();
        MaaResourceDestroy(&resource_handle);
        MaaControllerDestroy(&controller_handle);
    };

    if (!MaaInited()) {
        destroy();
        return false;
    }

    MaaPostTask("Test", MaaTaskParam_Empty);

    while (!MaaTaskAllFinished()) {
        std::this_thread::yield();
    }

    destroy();

    return true;
}

std::string read_adb_config(const std::filesystem::path& cur_dir)
{
    std::ifstream ifs(cur_dir / "controller_config.json", std::ios::in);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open controller_config.json\n"
                  << "Please copy sample/cpp/config/controller_config.json to " << cur_dir << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << ifs.rdbuf();
    return buffer.str();
}
