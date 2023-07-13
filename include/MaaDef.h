#pragma once

#include "MaaPort.h"
#include <stdint.h>

struct MaaResourceAPI;
typedef struct MaaResourceAPI* MaaResourceHandle;
struct MaaControllerAPI;
typedef struct MaaControllerAPI* MaaControllerHandle;
struct MaaInstanceAPI;
typedef struct MaaInstanceAPI* MaaInstanceHandle;

typedef uint8_t MaaBool;
typedef uint64_t MaaSize;
#define MaaNullSize ((MaaSize)-1)

typedef const char* MaaString;
typedef MaaString MaaJsonString;

typedef int32_t MaaStatus;
enum MaaStatusEnum
{
    MaaStatus_Invalid = 0,
    MaaStatus_Pending = 1000,
    MaaStatus_Running = 2000,
    MaaStatus_Success = 3000,
    MaaStatus_Failed = 4000,
    // MaaStatus_Timeout = 5000,
};

typedef int64_t MaaId;
typedef MaaId MaaCtrlId;
typedef MaaId MaaResId;
typedef MaaId MaaTaskId;
#define MaaInvalidId ((MaaId)0)

typedef int32_t MaaOption;
typedef MaaOption MaaGlobalOption;
typedef void* MaaOptionValue;
typedef uint64_t MaaOptionValueSize;

enum MaaGlobalOptionEnum
{
    MaaGlobalOption_Invalid = 0,

    // value: string, log dir, eg: "C:\\Users\\Administrator\\Desktop\\log"; val_size: string length
    MaaGlobalOption_Logging = 1,
};

typedef MaaOption MaaResOption;
enum MaaResOptionEnum
{
    MaaResOption_Invalid = 0,
};

typedef MaaOption MaaCtrlOption;
enum MaaCtrlOptionEnum
{
    MaaCtrlOption_Invalid = 0,

    // Only one of width and height can be set, and the other is automatically scaled according to the aspect ratio.
    // value: int, eg: 1920; val_size: sizeof(int)
    MaaCtrlOption_ScreenshotTargetWidth = 1,

    // Only one of width and height can be set, and the other is automatically scaled according to the aspect ratio.
    // value: int, eg: 1920; val_size: sizeof(int)
    MaaCtrlOption_ScreenshotTargetHeight = 2,

    // For StartApp
    // value: string, eg: "com.hypergryph.arknights/com.u8.sdk.U8UnityContext"; val_size: string length
    MaaCtrlOption_DefaultAppPackageEntry = 3,

    // For StopApp
    // value: string, eg: "com.hypergryph.arknights"; val_size: string length
    MaaCtrlOption_DefaultAppPackage = 4,
};

typedef MaaOption MaaInstOption;
enum MaaInstOptionEnum
{
    MaaInstOption_Invalid = 0,
};

#define MaaTaskParam_Empty "{}"

typedef int32_t MaaAdbControllerType;
enum MaaAdbControllerTypeEnum
{
    MaaAdbControllerType_Touch_Adb = 1,
    MaaAdbControllerType_Touch_MiniTouch = 2,
    MaaAdbControllerType_Touch_MaaTouch = 3,
    MaaAdbControllerType_Touch_Mask = 0xFF,

    MaaAdbControllerType_Key_Adb = 1 << 8,
    MaaAdbControllerType_Key_MaaTouch = 2 << 8,
    MaaAdbControllerType_Key_Mask = 0xFF00,

    MaaAdbControllerType_Screencap_FastestWay = 1 << 16,
    MaaAdbControllerType_Screencap_RawByNetcat = 2 << 16,
    MaaAdbControllerType_Screencap_RawWithGzip = 3 << 16,
    MaaAdbControllerType_Screencap_Encode = 4 << 16,
    MaaAdbControllerType_Screencap_EncodeToFile = 5 << 16,
    MaaAdbControllerType_Screencap_MinicapDirect = 6 << 16,
    MaaAdbControllerType_Screencap_MinicapStream = 7 << 16,
    MaaAdbControllerType_Screencap_Mask = 0xFF0000,

    MaaAdbControllerType_Input_Preset_Adb = MaaAdbControllerType_Touch_Adb | MaaAdbControllerType_Key_Adb,
    MaaAdbControllerType_Input_Preset_Minitouch = MaaAdbControllerType_Touch_MiniTouch | MaaAdbControllerType_Key_Adb,
    MaaAdbControllerType_Input_Preset_Maatouch =
        MaaAdbControllerType_Touch_MaaTouch | MaaAdbControllerType_Key_MaaTouch,
};

typedef int32_t MaaThriftControllerType;
enum MaaThriftControllerTypeEnum
{
    // param format should be "host:port"
    MaaThriftControllerType_Socket = 1,

    // param should be unix domain socket path
    MaaThriftControllerType_UnixDomainSocket = 2,
};

typedef void* MaaCallbackTransparentArg;

typedef void (*MaaAPICallback)(MaaString msg, MaaJsonString details_json, MaaCallbackTransparentArg callback_arg);
typedef MaaAPICallback MaaResourceCallback;
typedef MaaAPICallback MaaControllerCallback;
typedef MaaAPICallback MaaInstanceCallback;

struct MaaCustomControllerAPI;
typedef MaaCustomControllerAPI* MaaCustomControllerHandle;

struct MaaCustomTaskAPI;
typedef MaaCustomTaskAPI* MaaCustomTaskHandle;
