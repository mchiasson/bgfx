cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Hunter/HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.30.tar.gz"
    SHA1 "c062b0a2316cf8dcb57b6a9ad49fea1025a928f6"
    LOCAL
)

option(BGFX_BUILD_EXAMPLES "Build Examples" OFF)

project(bgfx VERSION 1.0.0)

hunter_add_package(bimg)
hunter_add_package(bx)

find_package(bimg CONFIG REQUIRED)
find_package(bx CONFIG REQUIRED)
find_package(OpenGL)
find_package(Vulkan)
find_package(X11)

################################################################################
# BGFX Library
################################################################################

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    find_package(metal REQUIRED)
    set(APPLE_SOURCE
        src/glcontext_eagl.mm
        src/glcontext_nsgl.mm
        src/renderer_mtl.mm
    )
endif()

add_library(bgfx
    ${APPLE_SOURCE}
    src/bgfx.cpp
    src/debug_renderdoc.cpp
    src/dxgi.cpp
    src/glcontext_egl.cpp
    src/glcontext_glx.cpp
    src/glcontext_wgl.cpp
    src/nvapi.cpp
    src/renderer_d3d11.cpp
    src/renderer_d3d12.cpp
    src/renderer_d3d9.cpp
    src/renderer_gl.cpp
    src/renderer_gnm.cpp
    src/renderer_noop.cpp
    src/renderer_vk.cpp
    src/shader.cpp
    src/shader_dx9bc.cpp
    src/shader_dxbc.cpp
    src/shader_spirv.cpp
    src/topology.cpp
    src/vertexdecl.cpp
)

target_include_directories(bgfx
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
        3rdparty
        3rdparty/khronos
)

target_link_libraries(bgfx PUBLIC bimg::bimg bx::bx ${X11_LIBRARIES})

if(TARGET Vulkan::Vulkan)
    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_VULKAN=1)
    target_link_libraries(bgfx PUBLIC Vulkan::Vulkan)
elseif(TARGET metal::metal)
        target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_METAL=1)
        target_link_libraries(bgfx PUBLIC metal::metal)
elseif(TARGET OpenGL::OpenGL)
    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_OPENGL=1)
    target_link_libraries(bgfx PUBLIC OpenGL::OpenGL)
elseif(TARGET OpenGL::GL)
    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_OPENGL=1)
    target_link_libraries(bgfx PUBLIC OpenGL::GL)
endif()

if(TARGET OpenGL::GLX)
    target_link_libraries(bgfx PUBLIC OpenGL::GLX)
elseif(TARGET OpenGL::EGL)
    target_link_libraries(bgfx PUBLIC OpenGL::EGL)
endif()

target_compile_definitions(bgfx PUBLIC
    $<$<CONFIG:Debug>:BGFX_CONFIG_DEBUG=1>
    $<$<CONFIG:Release>:BGFX_CONFIG_DEBUG=0>
)

################################################################################
# BGFX Examples
################################################################################

if (BGFX_BUILD_EXAMPLES)

    file(GLOB_RECURSE EXAMPLE_SOURCES RELATIVE ${PROJECT_SOURCE_DIR}
        examples/*.c*
        3rdparty/dear-imgui/*.c*
        3rdparty/ib-compress/*.c*
    )

    add_executable(bgfx_examples ${EXAMPLE_SOURCES})
    target_compile_definitions(bgfx_examples PRIVATE ENTRY_CONFIG_USE_SDL=1)
    target_include_directories(bgfx_examples PRIVATE
        examples/common
        examples/imgui
        3rdparty
    )

    hunter_add_package(SDL2)
    find_package(SDL2 CONFIG REQUIRED)

    target_link_libraries(bgfx_examples PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        bgfx
    )

    file(GLOB_RECURSE RUNTIMES RELATIVE ${PROJECT_SOURCE_DIR}/examples/runtime examples/runtime/*)
    foreach(RUNTIME IN LISTS RUNTIMES)
        configure_file(examples/runtime/${RUNTIME} ${PROJECT_BINARY_DIR}/${RUNTIME} COPYONLY)
    endforeach()


endif()

################################################################################
# CMake-friendly Install target configuration
################################################################################

set(targets_export_name "bgfxTargets")
set(ConfigPackageLocation lib/cmake/bgfx)

# Create the CMake version file.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Create the Config file.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/bgfxConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfig.cmake
    INSTALL_DESTINATION ${ConfigPackageLocation}
)

# Install the different headers and libraries.
install(
    DIRECTORY include/bgfx
    DESTINATION include COMPONENT Development
)

install(TARGETS bgfx
    EXPORT ${targets_export_name}
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    BUNDLE DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

# Install the generated CMake files.
install(
    EXPORT ${targets_export_name}
    NAMESPACE "bgfx::"
    DESTINATION ${ConfigPackageLocation}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfig.cmake
    DESTINATION
        ${ConfigPackageLocation}
)
