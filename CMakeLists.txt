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

project(bgfx VERSION 1.0.0)

add_library(bgfx src/amalgamated.cpp)

target_include_directories(bgfx
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
        3rdparty
)

hunter_add_package(bimg)
hunter_add_package(bx)

find_package(bimg CONFIG REQUIRED)
find_package(bx CONFIG REQUIRED)
find_package(OpenGL)
find_package(Vulkan)

target_link_libraries(bgfx PUBLIC bimg::bimg bx::bx)

if(TARGET Vulkan::Vulkan)
    target_compile_definitions(bgfx PUBLIC
        BGFX_CONFIG_RENDERER_DIRECT3D9=0
        BGFX_CONFIG_RENDERER_DIRECT3D11=0
        BGFX_CONFIG_RENDERER_DIRECT3D12=0
        BGFX_CONFIG_RENDERER_METAL=0
        BGFX_CONFIG_RENDERER_OPENGL=0
        BGFX_CONFIG_RENDERER_OPENGLES=0
        BGFX_CONFIG_RENDERER_VULKAN=1
        BGFX_CONFIG_RENDERER_GNM=0
    )
    target_link_libraries(bgfx PUBLIC Vulkan::Vulkan)
elseif(TARGET OpenGL::OpenGL)
    target_compile_definitions(bgfx PUBLIC
        BGFX_CONFIG_RENDERER_DIRECT3D9=0
        BGFX_CONFIG_RENDERER_DIRECT3D11=0
        BGFX_CONFIG_RENDERER_DIRECT3D12=0
        BGFX_CONFIG_RENDERER_METAL=0
        BGFX_CONFIG_RENDERER_OPENGL=1
        BGFX_CONFIG_RENDERER_OPENGLES=0
        BGFX_CONFIG_RENDERER_VULKAN=0
        BGFX_CONFIG_RENDERER_GNM=0
    )
    target_link_libraries(bgfx PUBLIC OpenGL::OpenGL)
elseif(TARGET OpenGL::GL)
    target_compile_definitions(bgfx PUBLIC
        BGFX_CONFIG_RENDERER_DIRECT3D9=0
        BGFX_CONFIG_RENDERER_DIRECT3D11=0
        BGFX_CONFIG_RENDERER_DIRECT3D12=0
        BGFX_CONFIG_RENDERER_METAL=0
        BGFX_CONFIG_RENDERER_OPENGL=1
        BGFX_CONFIG_RENDERER_OPENGLES=0
        BGFX_CONFIG_RENDERER_VULKAN=0
        BGFX_CONFIG_RENDERER_GNM=0
    )
    target_link_libraries(bgfx PUBLIC OpenGL::GL)
endif()

target_compile_definitions(bgfx PUBLIC
    $<$<CONFIG:Debug>:BGFX_CONFIG_DEBUG=1>
    $<$<CONFIG:Release>:BGFX_CONFIG_DEBUG=0>
)

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
