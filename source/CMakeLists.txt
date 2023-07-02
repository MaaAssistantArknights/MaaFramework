file(GLOB_RECURSE maa_src *.cpp *.h *.hpp ../include/*.h)
file(GLOB_RECURSE maa_modules_src modules/*.cpp modules/*.h modules/*.hpp)
list(REMOVE_ITEM maa_src ${maa_modules_src})

add_subdirectory(modules/ControlUnit)

add_library(MaaFramework SHARED ${maa_src})

set(MaaFramework_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/MaaAPI.h)
set(MaaFramework_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_include_directories(MaaFramework PUBLIC ../include PRIVATE .)
set_target_properties(MaaFramework PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    PUBLIC_HEADER "${MaaFramework_PUBLIC_HEADERS}"
)

if(UNIX)
    set_target_properties(MaaFramework PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif()

target_link_libraries(MaaFramework MaaControlUnit)
target_link_libraries(MaaFramework ${OpenCV_LIBS} MaaDerpLearning ONNXRuntime::ONNXRuntime ZLIB::ZLIB HeaderOnlyLibraries) #asio::asio cpr::cpr

if(WIN32)
    target_link_libraries(MaaFramework ws2_32)
endif()