cmake_minimum_required(VERSION 3.10)

# if (BUILD_IOS)
#     set(CMAKE_SYSTEM_NAME iOS)
#     set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "iOS architecture")
#     # Use the exact SDK path from your system
#     set(CMAKE_OSX_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS18.1.sdk" CACHE STRING "iOS SDK")
#     set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "Minimum iOS version")
# endif()


project(scalatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optional archive output directory
option(SCALATRIX_SET_ARCHIVE_OUTPUT_DIR "Set custom archive output directory" OFF)
if(SCALATRIX_SET_ARCHIVE_OUTPUT_DIR)
    set(SCALATRIX_ARCHIVE_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build/native" CACHE PATH "Archive output directory")
endif()

set(Eigen3_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/share/eigen3/cmake")
find_package(Eigen3 REQUIRED)

set(SOURCES
    src/affine_transform.cpp
    src/scale.cpp
    src/lattice.cpp
    src/params.cpp
    src/mos.cpp
    src/pitchset.cpp
    src/main.cpp  # Ensure included
)

# iOS target
# option(BUILD_IOS "Build iOS library" OFF)
# if (BUILD_IOS)
#     set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "YES") # Match Mitona if needed
#     set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
#     set(CMAKE_XCODE_ATTRIBUTE_SDKROOT "iphoneos")
#     set(CMAKE_XCODE_ATTRIBUTE_SUPPORTED_PLATFORMS "iphoneos")
#     set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "YES")
#     set(CMAKE_XCODE_ATTRIBUTE_BITCODE_GENERATION_MODE "bitcode")
# 
#     add_library(scalatrix_ios STATIC ${SOURCES})
#     target_include_directories(scalatrix_ios PUBLIC include)
#     target_link_libraries(scalatrix_ios PRIVATE Eigen3::Eigen)
#     set_target_properties(scalatrix_ios PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/ios     
#     )
# else()
#     add_library(scalatrix STATIC ${SOURCES})
#     target_include_directories(scalatrix PUBLIC include)
#     target_link_libraries(scalatrix PRIVATE Eigen3::Eigen)
#     set_target_properties(scalatrix PROPERTIES
#         ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
#     )
# endif()

add_library(scalatrix STATIC ${SOURCES})
target_include_directories(scalatrix PUBLIC include)
target_link_libraries(scalatrix PRIVATE Eigen3::Eigen)

# Optionally set archive output directory
if(SCALATRIX_SET_ARCHIVE_OUTPUT_DIR)
    set_target_properties(scalatrix PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${SCALATRIX_ARCHIVE_OUTPUT_DIR}
    )
endif()

if (EMSCRIPTEN)
    add_executable(scalatrix_wasm ${SOURCES})
    target_include_directories(scalatrix_wasm PUBLIC include)
    target_link_libraries(scalatrix_wasm PRIVATE Eigen3::Eigen)
    target_link_options(scalatrix_wasm PRIVATE
        --emit-tsd "$<TARGET_FILE_DIR:scalatrix_wasm>/scalatrix.d.ts"
    )
    set_target_properties(scalatrix_wasm PROPERTIES
        OUTPUT_NAME "scalatrix"
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
        LINK_FLAGS "--bind -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORT_NAME='Scalatrix' -s USE_WEBGL2=1 -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]' -s SINGLE_FILE=1"
    )
endif()


# Python bindings setup (only if building Python target)
option(BUILD_PYTHON "Build Python bindings" OFF)
if(BUILD_PYTHON)
    # Find Python and Pybind11
    set(Python3_ROOT_DIR "/opt/homebrew/Cellar/python@3.13/3.13.2" CACHE PATH "Python 3 root directory")
    set(Python3_INCLUDE_DIR "/opt/homebrew/Cellar/python@3.13/3.13.2/Frameworks/Python.framework/Headers" CACHE PATH "Python 3 include directory")
    set(Python3_LIBRARY "/opt/homebrew/Cellar/python@3.13/3.13.2/Frameworks/Python.framework/Versions/3.13/lib/libpython3.13.dylib" CACHE PATH "Python 3 library")
    find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

    set(pybind11_DIR "/opt/homebrew/Cellar/pybind11/2.13.6_1`/share/cmake/pybind11")
    find_package(pybind11 REQUIRED)


    # Python bindings target
    add_library(scalatrix_python MODULE ${SOURCES} src/python_bindings.cpp)
    target_include_directories(scalatrix_python PUBLIC include)
    target_link_libraries(scalatrix_python PRIVATE Eigen3::Eigen pybind11::pybind11 Python3::Python)
    set_target_properties(scalatrix_python PROPERTIES
        OUTPUT_NAME "scalatrix"
        PREFIX ""  # Avoid lib prefix
        SUFFIX ".so"  # Ensure .so for Python
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/python
    )

    install(TARGETS scalatrix_python
        LIBRARY DESTINATION ${Python3_SITELIB}
    )


endif()


# if (NOT EMSCRIPTEN AND NOT BUILD_IOS)
#     add_library(scalatrix_alias ALIAS scalatrix)
# endif()

# if (NOT EMSCRIPTEN)
#     add_library(scalatrix_alias ALIAS scalatrix)
# endif()


add_custom_target(native
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
)

# Update custom targets
# add_custom_target(ios
#     COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/ios --target scalatrix_ios
#     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/ios
# )

add_custom_target(wasm
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target scalatrix_wasm
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
)

add_custom_target(python
    COMMAND ${CMAKE_COMMAND} -DBUILD_PYTHON=ON -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_SOURCE_DIR}/build/python
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/python --target scalatrix_python
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/python
)

add_custom_target(python_install
    COMMAND ${CMAKE_COMMAND} -DBUILD_PYTHON=ON -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_SOURCE_DIR}/build/python
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/python --target scalatrix_python
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_SOURCE_DIR}/build/python
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/python
)

add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/python --target clean
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/ios --target clean
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(build_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target scalatrix_wasm
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/python --target scalatrix_python
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/ios --target scalatrix_ios
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)