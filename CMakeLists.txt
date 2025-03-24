cmake_minimum_required(VERSION 3.10)
project(scalatrix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Eigen3_DIR "/opt/homebrew/Cellar/eigen/3.4.0_1/share/eigen3/cmake")
find_package(Eigen3 REQUIRED)

set(SOURCES
    src/affine_transform.cpp
    src/scale.cpp
    src/lattice.cpp
    src/params.cpp
    src/mos.cpp
    src/main.cpp  # Ensure included
)

add_library(scalatrix STATIC ${SOURCES})
target_include_directories(scalatrix PUBLIC include)
target_link_libraries(scalatrix PRIVATE Eigen3::Eigen)
set_target_properties(scalatrix PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
)

if (EMSCRIPTEN)
    add_executable(scalatrix_wasm ${SOURCES})
    target_include_directories(scalatrix_wasm PUBLIC include)
    target_link_libraries(scalatrix_wasm PRIVATE Eigen3::Eigen)
    set_target_properties(scalatrix_wasm PROPERTIES
        OUTPUT_NAME "scalatrix"
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/wasm
        LINK_FLAGS "--bind -s MODULARIZE=1 -s EXPORT_NAME='Scalatrix' -s USE_WEBGL2=1 -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]' -s SINGLE_FILE=1"
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





if (NOT EMSCRIPTEN)
    add_library(scalatrix_alias ALIAS scalatrix)
endif()

add_custom_target(native
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/build/native
)

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
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(build_all
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/native --target scalatrix
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/wasm --target scalatrix_wasm
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_SOURCE_DIR}/build/python --target scalatrix_python
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)