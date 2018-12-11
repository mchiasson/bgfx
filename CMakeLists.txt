cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Hunter/HunterGate)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.69.tar.gz"
    SHA1 "3d6b1267844f7bb64ba9981ac5a1ff78a0237d41"
    LOCAL
)

project(bgfx VERSION 1.0.0)

option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
option(BGFX_BUILD_EXAMPLES "Build bgfx examples." ON)

################################################################################
# BGFX 3rdparty libs
################################################################################
add_library(ib-compress
    3rdparty/ib-compress/indexbuffercompression.cpp
    3rdparty/ib-compress/indexbufferdecompression.cpp
)
target_include_directories(ib-compress PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rdparty>
    $<INSTALL_INTERFACE:include>
)

add_library(dear-imgui
    3rdparty/dear-imgui/imgui_draw.cpp
    3rdparty/dear-imgui/imgui.cpp
    3rdparty/dear-imgui/imgui_widgets.cpp
)
target_include_directories(dear-imgui PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/3rdparty>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(dear-imgui PUBLIC bx::bx)

add_library(glslang
    3rdparty/glslang/SPIRV/doc.cpp
    3rdparty/glslang/SPIRV/disassemble.cpp
    3rdparty/glslang/SPIRV/SpvPostProcess.cpp
    3rdparty/glslang/SPIRV/SPVRemapper.cpp
    3rdparty/glslang/SPIRV/GlslangToSpv.cpp
    3rdparty/glslang/SPIRV/Logger.cpp
    3rdparty/glslang/SPIRV/InReadableOrder.cpp
    3rdparty/glslang/SPIRV/SpvTools.cpp
    3rdparty/glslang/SPIRV/SpvBuilder.cpp
    3rdparty/glslang/hlsl/hlslAttributes.cpp
    3rdparty/glslang/hlsl/hlslParseables.cpp
    3rdparty/glslang/hlsl/hlslGrammar.cpp
    3rdparty/glslang/hlsl/hlslOpMap.cpp
    3rdparty/glslang/hlsl/hlslScanContext.cpp
    3rdparty/glslang/hlsl/hlslTokenStream.cpp
    3rdparty/glslang/hlsl/hlslParseHelper.cpp
    3rdparty/glslang/hlsl/pch.cpp
    3rdparty/glslang//OGLCompilersDLL/InitializeDll.cpp
    3rdparty/glslang/glslang/MachineIndependent/iomapper.cpp
    3rdparty/glslang/glslang/MachineIndependent/reflection.cpp
    3rdparty/glslang/glslang/MachineIndependent/ParseContextBase.cpp
    3rdparty/glslang/glslang/MachineIndependent/InfoSink.cpp
    3rdparty/glslang/glslang/MachineIndependent/propagateNoContraction.cpp
    3rdparty/glslang/glslang/MachineIndependent/IntermTraverse.cpp
    3rdparty/glslang/glslang/MachineIndependent/ParseHelper.cpp
    3rdparty/glslang/glslang/MachineIndependent/Intermediate.cpp
    3rdparty/glslang/glslang/MachineIndependent/RemoveTree.cpp
    3rdparty/glslang/glslang/MachineIndependent/linkValidate.cpp
    3rdparty/glslang/glslang/MachineIndependent/intermOut.cpp
    3rdparty/glslang/glslang/MachineIndependent/Constant.cpp
    3rdparty/glslang/glslang/MachineIndependent/Initialize.cpp
    3rdparty/glslang/glslang/MachineIndependent/attribute.cpp
    3rdparty/glslang/glslang/MachineIndependent/glslang_tab.cpp
    3rdparty/glslang/glslang/MachineIndependent/Scan.cpp
    3rdparty/glslang/glslang/MachineIndependent/parseConst.cpp
    3rdparty/glslang/glslang/MachineIndependent/ShaderLang.cpp
    3rdparty/glslang/glslang/MachineIndependent/Versions.cpp
    3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpContext.cpp
    3rdparty/glslang/glslang/MachineIndependent/preprocessor/Pp.cpp
    3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpAtom.cpp
    3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpTokens.cpp
    3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpScanner.cpp
    3rdparty/glslang/glslang/MachineIndependent/SymbolTable.cpp
    3rdparty/glslang/glslang/MachineIndependent/pch.cpp
    3rdparty/glslang/glslang/MachineIndependent/PoolAlloc.cpp
    3rdparty/glslang/glslang/MachineIndependent/limits.cpp
    3rdparty/glslang/glslang/GenericCodeGen/CodeGen.cpp
    3rdparty/glslang/glslang/GenericCodeGen/Link.cpp
)

if (MSVC)
    target_sources(glslang PUBLIC
        ${PROJECT_SOURCE_DIR}/3rdparty/glslang/glslang/OSDependent/Windows/ossource.cpp
    )
else()
    target_sources(glslang INTERFACE
        ${PROJECT_SOURCE_DIR}/3rdparty/glslang/glslang/OSDependent/Unix/ossource.cpp
    )
endif()

target_compile_definitions(glslang PUBLIC ENABLE_HLSL=1)
target_include_directories(glslang PUBLIC 
    3rdparty/glslang 
    3rdparty/glslang/glslang/Include
    3rdparty/glslang/glslang/Public
)

add_library(fcpp
    3rdparty/fcpp/cpp1.c
    3rdparty/fcpp/cpp2.c
    3rdparty/fcpp/cpp3.c
    3rdparty/fcpp/cpp4.c
    3rdparty/fcpp/cpp5.c
    3rdparty/fcpp/cpp6.c
    #3rdparty/fcpp/usecpp.c
)

file(GLOB glcpp-library_sources 
    3rdparty/glsl-optimizer/src/glsl/glcpp/*.c 
    3rdparty/glsl-optimizer/src/util/*.c
)

add_library(glcpp-library ${glcpp-library_sources})

target_include_directories(glcpp-library 
    PUBLIC 
        3rdparty/glsl-optimizer/include
    PRIVATE
        3rdparty/glsl-optimizer/src 
        3rdparty/glsl-optimizer/src/mesa
)

file(GLOB mesa_sources 
    3rdparty/glsl-optimizer/src/mesa/program/*.c 
    3rdparty/glsl-optimizer/src/mesa/main/*.c
)

add_library(mesa ${mesa_sources})

target_include_directories(mesa 
    PUBLIC 
        3rdparty/glsl-optimizer/include
    PRIVATE
        3rdparty/glsl-optimizer/src 
        3rdparty/glsl-optimizer/src/mesa
)

file(GLOB glsl_sources 
    3rdparty/glsl-optimizer/src/glsl/*.cpp 
    3rdparty/glsl-optimizer/src/glsl/*.c
)

file(GLOB glsl_sources_remove 
    3rdparty/glsl-optimizer/src/glsl/main.cpp 
    3rdparty/glsl-optimizer/src/glsl/builtin_stubs.cpp
)

list(REMOVE_ITEM glsl_sources ${glsl_sources_remove})

add_library(glsl_optimizer ${glsl_sources})
target_link_libraries(glsl_optimizer PUBLIC glcpp-library mesa ${OPENGL_LIBRARIES})
target_include_directories(glsl_optimizer 
    PUBLIC 
        3rdparty/glsl-optimizer/include
        3rdparty/glsl-optimizer/src/glsl
    PRIVATE 
        3rdparty/glsl-optimizer/src
        3rdparty/glsl-optimizer/src/mesa
)

target_include_directories(fcpp PUBLIC 3rdparty/fcpp)

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

hunter_add_package(bx)
find_package(bx CONFIG REQUIRED)
target_link_libraries(bgfx PUBLIC bx::bx)

hunter_add_package(bimg)
find_package(bimg CONFIG REQUIRED)
target_link_libraries(bgfx PUBLIC bimg::bimg)

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
    #find_package(Vulkan)
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

target_link_libraries(bgfx PUBLIC
    ib-compress
    dear-imgui
)

################################################################################
# BGFX Tools
################################################################################

if (BGFX_BUILD_TOOLS)

    ########################
    # BGFX Geometry Compiler
    ########################

    add_executable(geometryc
        3rdparty/forsyth-too/forsythtriangleorderoptimizer.cpp
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

    add_executable(shaderc
        src/shader_spirv.cpp
        src/vertexdecl.cpp
        tools/shaderc/shaderc.cpp
        tools/shaderc/shaderc_glsl.cpp
        tools/shaderc/shaderc_hlsl.cpp
        tools/shaderc/shaderc_pssl.cpp
        tools/shaderc/shaderc_spirv.cpp
    )

    target_include_directories(shaderc PRIVATE include)
    target_link_libraries(shaderc PRIVATE glslang fcpp glsl_optimizer bx::bx)

endif()

################################################################################
# BGFX Examples
################################################################################

if (BGFX_BUILD_EXAMPLES)

    file(GLOB_RECURSE EXAMPLE_SOURCES RELATIVE ${PROJECT_SOURCE_DIR}
        examples/*.c*
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
        ib-compress
        dear-imgui
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

install(FILES
    3rdparty/dear-imgui/imconfig.h
    3rdparty/dear-imgui/imgui.h
    3rdparty/dear-imgui/imgui_internal.h
    3rdparty/dear-imgui/imgui_user.h
    3rdparty/dear-imgui/imgui_user.inl
    3rdparty/dear-imgui/imstb_rectpack.h
    3rdparty/dear-imgui/imstb_textedit.h
    3rdparty/dear-imgui/imstb_truetype.h
    DESTINATION include/dear-imgui COMPONENT Development
)

install(FILES
    3rdparty/dear-imgui/widgets/color_picker.h
    3rdparty/dear-imgui/widgets/color_picker.inl
    3rdparty/dear-imgui/widgets/color_wheel.h
    3rdparty/dear-imgui/widgets/color_wheel.inl
    3rdparty/dear-imgui/widgets/dock.h
    3rdparty/dear-imgui/widgets/dock.inl
    3rdparty/dear-imgui/widgets/file_list.h
    3rdparty/dear-imgui/widgets/file_list.inl
    3rdparty/dear-imgui/widgets/gizmo.h
    3rdparty/dear-imgui/widgets/gizmo.inl
    3rdparty/dear-imgui/widgets/memory_editor.h
    3rdparty/dear-imgui/widgets/memory_editor.inl
    3rdparty/dear-imgui/widgets/range_slider.h
    3rdparty/dear-imgui/widgets/range_slider.inl
    DESTINATION include/dear-imgui/widgets COMPONENT Development
)

install(FILES
    3rdparty/sdf/sdf.h
    DESTINATION include/sdf COMPONENT Development
)

install(TARGETS bgfx ib-compress dear-imgui
    EXPORT ${targets_export_name}
    LIBRARY DESTINATION lib COMPONENT Runtime
    ARCHIVE DESTINATION lib COMPONENT Development
    RUNTIME DESTINATION bin COMPONENT Runtime
    BUNDLE DESTINATION bin COMPONENT Runtime
    PUBLIC_HEADER DESTINATION include COMPONENT Development
    BUNDLE DESTINATION bin COMPONENT Runtime
)

if (BGFX_BUILD_TOOLS)
    install(TARGETS geometryc shaderc
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
