cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Hunter/HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.30.tar.gz"
    SHA1 "c062b0a2316cf8dcb57b6a9ad49fea1025a928f6"
    LOCAL
)

project(bgfx VERSION 1.0.0)

option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
option(BGFX_BUILD_EXAMPLES "Build bgfx examples." ON)

################################################################################
# BGFX Library
################################################################################

file(GLOB BGFX_SOURCES src/*)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/amalgamated.cpp PROPERTIES HEADER_FILE_ONLY ON)
set_source_files_properties(${PROJECT_SOURCE_DIR}/src/amalgamated.mm PROPERTIES HEADER_FILE_ONLY ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    set_source_files_properties(${PROJECT_SOURCE_DIR}/src/glcontext_glx.cpp PROPERTIES HEADER_FILE_ONLY ON)
else()
    set_source_files_properties(${PROJECT_SOURCE_DIR}/src/glcontext_eagl.mm PROPERTIES HEADER_FILE_ONLY ON)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/src/glcontext_nsgl.mm PROPERTIES HEADER_FILE_ONLY ON)
    set_source_files_properties(${PROJECT_SOURCE_DIR}/src/renderer_mtl.mm PROPERTIES HEADER_FILE_ONLY ON)
endif()

add_library(bgfx ${BGFX_SOURCES})

target_include_directories(bgfx
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        src
        3rdparty
        3rdparty/khronos
)

target_compile_definitions(bgfx PUBLIC
    $<$<CONFIG:Debug>:BGFX_CONFIG_DEBUG=1>
    $<$<CONFIG:Release>:BGFX_CONFIG_DEBUG=0>
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
)

hunter_add_package(bimg)
hunter_add_package(bx)
find_package(bimg CONFIG REQUIRED)
find_package(bx CONFIG REQUIRED)
target_link_libraries(bgfx PUBLIC bimg::bimg bx::bx)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")

    find_library(COCOA_LIBRARY Cocoa)
    find_package(metal REQUIRED)
    find_package(quartzcore REQUIRED)
    mark_as_advanced(COCOA_LIBRARY)

    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_METAL=1)
    target_link_libraries(bgfx
        PUBLIC
            ${COCOA_LIBRARY}
            metal::metal
            quartzcore::quartzcore
    )

elseif(UNIX AND NOT EMSCRIPTEN)

    find_package(OpenGL REQUIRED)
    find_package(Vulkan)
    find_package(X11 REQUIRED)

    target_link_libraries(bgfx PUBLIC ${X11_LIBRARIES})

    if(TARGET Vulkan::Vulkan)
        target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_RENDERER_VULKAN=1)
        target_link_libraries(bgfx PUBLIC Vulkan::Vulkan)
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

endif()

################################################################################
# BGFX Tools
################################################################################

if (BGFX_BUILD_TOOLS)

    ########################
    # BGFX Geometry Compiler
    ########################

    add_executable(geometryc
        3rdparty/forsyth-too/forsythtriangleorderoptimizer.cpp
        3rdparty/ib-compress/indexbuffercompression.cpp
        3rdparty/ib-compress/indexbufferdecompression.cpp
        examples/common/bounds.cpp
        tools/geometryc/geometryc.cpp
    )
    target_compile_definitions(geometryc PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
    )
    target_include_directories(geometryc PRIVATE
        ${PROJECT_SOURCE_DIR}/3rdparty
        ${PROJECT_SOURCE_DIR}/examples/common
    )
    target_link_libraries(geometryc PRIVATE bgfx)

    ######################
    # BGFX Shader Compiler
    ######################
#    add_executable(shaderc
#        tools/shaderc/shaderc.cpp
#        tools/shaderc/shaderc_glsl.cpp
#        tools/shaderc/shaderc_hlsl.cpp
#        tools/shaderc/shaderc_pssl.cpp
#        tools/shaderc/shaderc_spirv.cpp
#    )
#    target_link_libraries(shaderc PRIVATE bgfx)

endif()

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

install(FILES
    3rdparty/ib-compress/indexbuffercompressionformat.h
    3rdparty/ib-compress/indexbuffercompression.h
    3rdparty/ib-compress/indexbufferdecompression.h
    3rdparty/ib-compress/indexcompressionconstants.h
    3rdparty/ib-compress/readbitstream.h
    3rdparty/ib-compress/writebitstream.h
    DESTINATION include/ib-compress COMPONENT Development
)

install(FILES
    3rdparty/iconfontheaders/icons_font_awesome_4.h
    3rdparty/iconfontheaders/icons_font_awesome_5_brands.h
    3rdparty/iconfontheaders/icons_font_awesome_5.h
    3rdparty/iconfontheaders/icons_font_awesome.h
    3rdparty/iconfontheaders/icons_fork_awesome.h
    3rdparty/iconfontheaders/icons_ionicons.h
    3rdparty/iconfontheaders/icons_kenney.h
    3rdparty/iconfontheaders/icons_material_design.h
    3rdparty/iconfontheaders/icons_material_design_icons.h
    DESTINATION include/iconfontheaders COMPONENT Development
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

if (BGFX_BUILD_TOOLS)
    install(TARGETS geometryc
        EXPORT ${targets_export_name}
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development
        RUNTIME DESTINATION bin COMPONENT Runtime
        BUNDLE DESTINATION bin COMPONENT Runtime
        PUBLIC_HEADER DESTINATION include COMPONENT Development
        BUNDLE DESTINATION bin COMPONENT Runtime
    )
endif()

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
