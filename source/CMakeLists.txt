file(GLOB_RECURSE maa_src *.cpp *.h *.hpp ../include/*.h)
file(GLOB_RECURSE maa_modules_src modules/*.cpp modules/*.h modules/*.hpp)
list(REMOVE_ITEM maa_src ${maa_modules_src})

add_subdirectory(modules/MaaUtils)
add_subdirectory(modules/MaaControlUnit)

add_library(MaaFramework SHARED ${maa_src})

set(MaaFramework_PUBLIC_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/../include/MaaAPI.h)
set(MaaFramework_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_include_directories(MaaFramework PUBLIC ../include PRIVATE . modules/include)
set_target_properties(MaaFramework PROPERTIES
    PUBLIC_HEADER "${MaaFramework_PUBLIC_HEADERS}"
)
target_compile_definitions(MaaFramework PRIVATE MAA_DLL_EXPORTS)

if(UNIX)
    set_target_properties(MaaFramework PROPERTIES INSTALL_RPATH "$ORIGIN/")
endif()

target_link_libraries(MaaFramework MaaControlUnit MaaUtils)
target_link_libraries(MaaFramework ${OpenCV_LIBS} MaaDerpLearning ONNXRuntime::ONNXRuntime HeaderOnlyLibraries) #asio::asio cpr::cpr

# clang 15之后有ranges
# if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
#     find_package(range-v3 REQUIRED)
#     target_link_libraries(MaaFramework range-v3::range-v3)
# endif ()

if(INSTALL_DEVEL)
    set(MaaFramework_install_extra_args PUBLIC_HEADER DESTINATION devel/include ARCHIVE DESTINATION devel/lib)
endif()

install(TARGETS MaaFramework
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
    PUBLIC_HEADER DESTINATION .
    ${MaaFramework_install_extra_args}
)
