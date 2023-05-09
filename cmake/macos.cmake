if (BUILD_XCFRAMEWORK)
    add_custom_command(OUTPUT MaaFramework.xcframework
        COMMAND rm -rf MaaFramework.xcframework
        COMMAND xcodebuild -create-xcframework -library libMaaFramework.dylib -headers ${PROJECT_SOURCE_DIR}/include -output MaaFramework.xcframework
        DEPENDS MaaFramework
    )

    add_custom_command(OUTPUT OpenCV.xcframework
        COMMAND rm -rf OpenCV.xcframework
        COMMAND xcodebuild -create-xcframework -library "${PROJECT_SOURCE_DIR}/MaaDeps/runtime/${MAADEPS_TRIPLET}/libopencv_world4.407.dylib" -output OpenCV.xcframework
    )

    add_custom_command(OUTPUT ONNXRuntime.xcframework
        COMMAND rm -rf ONNXRuntime.xcframework
        COMMAND xcodebuild -create-xcframework -library "${PROJECT_SOURCE_DIR}/MaaDeps/runtime/${MAADEPS_TRIPLET}/libonnxruntime.1.14.1.dylib" -output ONNXRuntime.xcframework
    )

    add_custom_command(OUTPUT MaaDerpLearning.xcframework
        COMMAND rm -rf MaaDerpLearning.xcframework
        COMMAND xcodebuild -create-xcframework -library "${PROJECT_SOURCE_DIR}/MaaDeps/runtime/${MAADEPS_TRIPLET}/libMaaDerpLearning.dylib" -output MaaDerpLearning.xcframework
    )

    add_custom_target(MaaXCFramework ALL
        DEPENDS MaaFramework MaaFramework.xcframework OpenCV.xcframework ONNXRuntime.xcframework MaaDerpLearning.xcframework
    )
endif (BUILD_XCFRAMEWORK)

target_compile_options(MaaFramework PRIVATE
    -Wno-deprecated-declarations
    -Wno-gnu-zero-variadic-macro-arguments)
