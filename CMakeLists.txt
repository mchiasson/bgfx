cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(cmake/Hunter/HunterGate.cmake)
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.23.89.tar.gz"
    SHA1 "a370290a8b32987755a36d64807bf8c7f5b61730"
    LOCAL
)

project(bgfx VERSION 1.0.0)

option(BGFX_CONFIG_DEBUG "Build bgfx with graphic debugging feature enabled" OFF)
option(BGFX_CONFIG_PROFILER "Build bgfx with profiling feature enabled" OFF)
option(BGFX_BUILD_TOOLS "Build bgfx tools." ON)
option(BGFX_BUILD_EXAMPLES "Build bgfx examples." ON)

hunter_add_package(bx)
find_package(bx CONFIG REQUIRED)

if (BGFX_BUILD_EXAMPLES OR BGFX_BUILD_TOOLS)
    hunter_add_package(bimg)
    find_package(bimg CONFIG REQUIRED)
endif()

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

if(BGFX_CONFIG_DEBUG)
    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_DEBUG=1)
endif()

if(BGFX_CONFIG_PROFILER)
    target_compile_definitions(bgfx PUBLIC BGFX_CONFIG_PROFILER=1)
endif()

if (MSVC)
    target_compile_definitions(bgfx PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries(bgfx PUBLIC bkaradzic::bx bkaradzic::bimg)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    find_library(COCOA_LIBRARY Cocoa)
    mark_as_advanced(COCOA_LIBRARY)
    find_package(metal REQUIRED)
    find_package(quartzcore REQUIRED)
    target_link_libraries(bgfx PUBLIC ${COCOA_LIBRARY} metal::metal quartzcore::quartzcore)
endif()

find_package(D3D12)
if(D3D12_FOUND)
    target_link_libraries(bgfx PUBLIC ${D3D12_LIBRARIES})
    target_include_directories(bgfx PUBLIC ${D3D12_INCLUDE_DIRS} PRIVATE 3rdparty/dxsdk/include)
endif()

find_package(OpenGL)

if(TARGET OpenGL::GLX)
    target_link_libraries(bgfx PUBLIC OpenGL::GLX)
endif()

if(TARGET OpenGL::EGL)
    target_link_libraries(bgfx PUBLIC OpenGL::EGL)
endif()

find_package(X11)
if(${X11_FOUND})
    target_link_libraries(bgfx PUBLIC ${X11_LIBRARIES})
endif()

################################################################################
# BGFX Tools
################################################################################

if (BGFX_BUILD_TOOLS)

    add_library(glslang
        3rdparty/glslang/OGLCompilersDLL/InitializeDll.cpp
        3rdparty/glslang/SPIRV/GlslangToSpv.cpp
        3rdparty/glslang/SPIRV/InReadableOrder.cpp
        3rdparty/glslang/SPIRV/Logger.cpp
        3rdparty/glslang/SPIRV/SPVRemapper.cpp
        3rdparty/glslang/SPIRV/SpvBuilder.cpp
        3rdparty/glslang/SPIRV/SpvPostProcess.cpp
        3rdparty/glslang/SPIRV/SpvTools.cpp
        3rdparty/glslang/SPIRV/disassemble.cpp
        3rdparty/glslang/SPIRV/doc.cpp
        3rdparty/glslang/glslang/GenericCodeGen/CodeGen.cpp
        3rdparty/glslang/glslang/GenericCodeGen/Link.cpp
        3rdparty/glslang/glslang/MachineIndependent/Constant.cpp
        3rdparty/glslang/glslang/MachineIndependent/InfoSink.cpp
        3rdparty/glslang/glslang/MachineIndependent/Initialize.cpp
        3rdparty/glslang/glslang/MachineIndependent/IntermTraverse.cpp
        3rdparty/glslang/glslang/MachineIndependent/Intermediate.cpp
        3rdparty/glslang/glslang/MachineIndependent/ParseContextBase.cpp
        3rdparty/glslang/glslang/MachineIndependent/ParseHelper.cpp
        3rdparty/glslang/glslang/MachineIndependent/PoolAlloc.cpp
        3rdparty/glslang/glslang/MachineIndependent/RemoveTree.cpp
        3rdparty/glslang/glslang/MachineIndependent/Scan.cpp
        3rdparty/glslang/glslang/MachineIndependent/ShaderLang.cpp
        3rdparty/glslang/glslang/MachineIndependent/SymbolTable.cpp
        3rdparty/glslang/glslang/MachineIndependent/Versions.cpp
        3rdparty/glslang/glslang/MachineIndependent/attribute.cpp
        3rdparty/glslang/glslang/MachineIndependent/glslang_tab.cpp
        3rdparty/glslang/glslang/MachineIndependent/intermOut.cpp
        3rdparty/glslang/glslang/MachineIndependent/iomapper.cpp
        3rdparty/glslang/glslang/MachineIndependent/limits.cpp
        3rdparty/glslang/glslang/MachineIndependent/linkValidate.cpp
        3rdparty/glslang/glslang/MachineIndependent/parseConst.cpp
        3rdparty/glslang/glslang/MachineIndependent/pch.cpp
        3rdparty/glslang/glslang/MachineIndependent/preprocessor/Pp.cpp
        3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpAtom.cpp
        3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpContext.cpp
        3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpScanner.cpp
        3rdparty/glslang/glslang/MachineIndependent/preprocessor/PpTokens.cpp
        3rdparty/glslang/glslang/MachineIndependent/propagateNoContraction.cpp
        3rdparty/glslang/glslang/MachineIndependent/reflection.cpp
        3rdparty/glslang/hlsl/hlslAttributes.cpp
        3rdparty/glslang/hlsl/hlslGrammar.cpp
        3rdparty/glslang/hlsl/hlslOpMap.cpp
        3rdparty/glslang/hlsl/hlslParseHelper.cpp
        3rdparty/glslang/hlsl/hlslParseables.cpp
        3rdparty/glslang/hlsl/hlslScanContext.cpp
        3rdparty/glslang/hlsl/hlslTokenStream.cpp
        3rdparty/glslang/hlsl/pch.cpp
        )
    if (MSVC)
        target_sources(glslang PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/glslang/glslang/OSDependent/Windows/ossource.cpp)
    elseif(UNIX)
        target_sources(glslang PUBLIC ${PROJECT_SOURCE_DIR}/3rdparty/glslang/glslang/OSDependent/Unix/ossource.cpp)
    endif()
    target_compile_definitions(glslang PUBLIC
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        ENABLE_OPT=1
        ENABLE_HLSL=1
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )
    target_include_directories(glslang PUBLIC
        3rdparty/glslang
        3rdparty/spirv-tools/include
        3rdparty/spirv-tools/source
        )

    add_library(fcpp
        3rdparty/fcpp/cpp1.c
        3rdparty/fcpp/cpp2.c
        3rdparty/fcpp/cpp3.c
        3rdparty/fcpp/cpp4.c
        3rdparty/fcpp/cpp5.c
        3rdparty/fcpp/cpp6.c
        )
    target_compile_definitions(fcpp PUBLIC
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        NINCLUDE=64
        NWORK=65536
        NBUFF=65536
        OLD_PREPROCESSOR=0
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )

    add_library(glsl-optimizer
        3rdparty/glsl-optimizer/src/glsl/ast_array_index.cpp
        3rdparty/glsl-optimizer/src/glsl/ast_expr.cpp
        3rdparty/glsl-optimizer/src/glsl/ast_function.cpp
        3rdparty/glsl-optimizer/src/glsl/ast_to_hir.cpp
        3rdparty/glsl-optimizer/src/glsl/ast_type.cpp
        3rdparty/glsl-optimizer/src/glsl/builtin_functions.cpp
        3rdparty/glsl-optimizer/src/glsl/builtin_types.cpp
        3rdparty/glsl-optimizer/src/glsl/builtin_variables.cpp
        3rdparty/glsl-optimizer/src/glsl/glcpp/glcpp-lex.c
        3rdparty/glsl-optimizer/src/glsl/glcpp/glcpp-parse.c
        3rdparty/glsl-optimizer/src/glsl/glcpp/pp.c
        3rdparty/glsl-optimizer/src/glsl/glsl_lexer.cpp
        3rdparty/glsl-optimizer/src/glsl/glsl_optimizer.cpp
        3rdparty/glsl-optimizer/src/glsl/glsl_parser.cpp
        3rdparty/glsl-optimizer/src/glsl/glsl_parser_extras.cpp
        3rdparty/glsl-optimizer/src/glsl/glsl_symbol_table.cpp
        3rdparty/glsl-optimizer/src/glsl/glsl_types.cpp
        3rdparty/glsl-optimizer/src/glsl/hir_field_selection.cpp
        3rdparty/glsl-optimizer/src/glsl/ir.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_basic_block.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_builder.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_clone.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_constant_expression.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_equals.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_expression_flattening.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_function.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_function_can_inline.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_function_detect_recursion.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_hierarchical_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_hv_accept.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_import_prototypes.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_print_glsl_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_print_metal_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_print_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_rvalue_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_stats.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_unused_structs.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_validate.cpp
        3rdparty/glsl-optimizer/src/glsl/ir_variable_refcount.cpp
        3rdparty/glsl-optimizer/src/glsl/link_atomics.cpp
        3rdparty/glsl-optimizer/src/glsl/link_functions.cpp
        3rdparty/glsl-optimizer/src/glsl/link_interface_blocks.cpp
        3rdparty/glsl-optimizer/src/glsl/link_uniform_block_active_visitor.cpp
        3rdparty/glsl-optimizer/src/glsl/link_uniform_blocks.cpp
        3rdparty/glsl-optimizer/src/glsl/link_uniform_initializers.cpp
        3rdparty/glsl-optimizer/src/glsl/link_uniforms.cpp
        3rdparty/glsl-optimizer/src/glsl/link_varyings.cpp
        3rdparty/glsl-optimizer/src/glsl/linker.cpp
        3rdparty/glsl-optimizer/src/glsl/loop_analysis.cpp
        3rdparty/glsl-optimizer/src/glsl/loop_controls.cpp
        3rdparty/glsl-optimizer/src/glsl/loop_unroll.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_clip_distance.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_discard.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_discard_flow.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_if_to_cond_assign.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_instructions.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_jumps.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_mat_op_to_vec.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_named_interface_blocks.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_noise.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_offset_array.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_output_reads.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_packed_varyings.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_packing_builtins.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_ubo_reference.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_variable_index_to_cond_assign.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_vec_index_to_cond_assign.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_vec_index_to_swizzle.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_vector.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_vector_insert.cpp
        3rdparty/glsl-optimizer/src/glsl/lower_vertex_id.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_algebraic.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_array_splitting.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_constant_folding.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_constant_propagation.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_constant_variable.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_copy_propagation.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_copy_propagation_elements.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_cse.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_dead_builtin_variables.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_dead_builtin_varyings.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_dead_code.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_dead_code_local.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_dead_functions.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_flatten_nested_if_blocks.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_flip_matrices.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_function_inlining.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_if_simplification.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_minmax.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_noop_swizzle.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_rebalance_tree.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_redundant_jumps.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_structure_splitting.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_swizzle_swizzle.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_tree_grafting.cpp
        3rdparty/glsl-optimizer/src/glsl/opt_vectorize.cpp
        3rdparty/glsl-optimizer/src/glsl/s_expression.cpp
        3rdparty/glsl-optimizer/src/glsl/standalone_scaffolding.cpp
        3rdparty/glsl-optimizer/src/glsl/strtod.c
        3rdparty/glsl-optimizer/src/mesa/main/imports.c
        3rdparty/glsl-optimizer/src/mesa/program/prog_hash_table.c
        3rdparty/glsl-optimizer/src/mesa/program/symbol_table.c
        3rdparty/glsl-optimizer/src/util/hash_table.c
        3rdparty/glsl-optimizer/src/util/ralloc.c
        )
    target_compile_definitions(glsl-optimizer PUBLIC
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )
    target_include_directories(glsl-optimizer PUBLIC
        3rdparty/glsl-optimizer/src
        3rdparty/glsl-optimizer/include
        3rdparty/glsl-optimizer/src/mesa
        3rdparty/glsl-optimizer/src/mapi
        3rdparty/glsl-optimizer/src/glsl
        )

    add_library(spirv-opt
        3rdparty/spirv-tools/source/assembly_grammar.cpp
        3rdparty/spirv-tools/source/binary.cpp
        3rdparty/spirv-tools/source/diagnostic.cpp
        3rdparty/spirv-tools/source/disassemble.cpp
        3rdparty/spirv-tools/source/enum_string_mapping.cpp
        3rdparty/spirv-tools/source/ext_inst.cpp
        3rdparty/spirv-tools/source/extensions.cpp
        3rdparty/spirv-tools/source/id_descriptor.cpp
        3rdparty/spirv-tools/source/libspirv.cpp
        3rdparty/spirv-tools/source/name_mapper.cpp
        3rdparty/spirv-tools/source/opcode.cpp
        3rdparty/spirv-tools/source/operand.cpp
        3rdparty/spirv-tools/source/opt/aggressive_dead_code_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/basic_block.cpp
        3rdparty/spirv-tools/source/opt/block_merge_pass.cpp
        3rdparty/spirv-tools/source/opt/block_merge_util.cpp
        3rdparty/spirv-tools/source/opt/build_module.cpp
        3rdparty/spirv-tools/source/opt/ccp_pass.cpp
        3rdparty/spirv-tools/source/opt/cfg.cpp
        3rdparty/spirv-tools/source/opt/cfg_cleanup_pass.cpp
        3rdparty/spirv-tools/source/opt/code_sink.cpp
        3rdparty/spirv-tools/source/opt/combine_access_chains.cpp
        3rdparty/spirv-tools/source/opt/common_uniform_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/compact_ids_pass.cpp
        3rdparty/spirv-tools/source/opt/composite.cpp
        3rdparty/spirv-tools/source/opt/const_folding_rules.cpp
        3rdparty/spirv-tools/source/opt/constants.cpp
        3rdparty/spirv-tools/source/opt/copy_prop_arrays.cpp
        3rdparty/spirv-tools/source/opt/dead_branch_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/dead_insert_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/dead_variable_elimination.cpp
        3rdparty/spirv-tools/source/opt/decoration_manager.cpp
        3rdparty/spirv-tools/source/opt/def_use_manager.cpp
        3rdparty/spirv-tools/source/opt/dominator_analysis.cpp
        3rdparty/spirv-tools/source/opt/dominator_tree.cpp
        3rdparty/spirv-tools/source/opt/eliminate_dead_constant_pass.cpp
        3rdparty/spirv-tools/source/opt/eliminate_dead_functions_pass.cpp
        3rdparty/spirv-tools/source/opt/eliminate_dead_functions_util.cpp
        3rdparty/spirv-tools/source/opt/eliminate_dead_members_pass.cpp
        3rdparty/spirv-tools/source/opt/feature_manager.cpp
        3rdparty/spirv-tools/source/opt/flatten_decoration_pass.cpp
        3rdparty/spirv-tools/source/opt/fold.cpp
        3rdparty/spirv-tools/source/opt/fold_spec_constant_op_and_composite_pass.cpp
        3rdparty/spirv-tools/source/opt/folding_rules.cpp
        3rdparty/spirv-tools/source/opt/freeze_spec_constant_value_pass.cpp
        3rdparty/spirv-tools/source/opt/function.cpp
        3rdparty/spirv-tools/source/opt/if_conversion.cpp
        3rdparty/spirv-tools/source/opt/inline_exhaustive_pass.cpp
        3rdparty/spirv-tools/source/opt/inline_opaque_pass.cpp
        3rdparty/spirv-tools/source/opt/inline_pass.cpp
        3rdparty/spirv-tools/source/opt/inst_bindless_check_pass.cpp
        3rdparty/spirv-tools/source/opt/instruction.cpp
        3rdparty/spirv-tools/source/opt/instruction_list.cpp
        3rdparty/spirv-tools/source/opt/instrument_pass.cpp
        3rdparty/spirv-tools/source/opt/ir_context.cpp
        3rdparty/spirv-tools/source/opt/ir_loader.cpp
        3rdparty/spirv-tools/source/opt/licm_pass.cpp
        3rdparty/spirv-tools/source/opt/local_access_chain_convert_pass.cpp
        3rdparty/spirv-tools/source/opt/local_redundancy_elimination.cpp
        3rdparty/spirv-tools/source/opt/local_single_block_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/local_single_store_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/local_ssa_elim_pass.cpp
        3rdparty/spirv-tools/source/opt/loop_dependence.cpp
        3rdparty/spirv-tools/source/opt/loop_dependence_helpers.cpp
        3rdparty/spirv-tools/source/opt/loop_descriptor.cpp
        3rdparty/spirv-tools/source/opt/loop_fission.cpp
        3rdparty/spirv-tools/source/opt/loop_fusion.cpp
        3rdparty/spirv-tools/source/opt/loop_fusion_pass.cpp
        3rdparty/spirv-tools/source/opt/loop_peeling.cpp
        3rdparty/spirv-tools/source/opt/loop_unroller.cpp
        3rdparty/spirv-tools/source/opt/loop_unswitch_pass.cpp
        3rdparty/spirv-tools/source/opt/loop_utils.cpp
        3rdparty/spirv-tools/source/opt/mem_pass.cpp
        3rdparty/spirv-tools/source/opt/merge_return_pass.cpp
        3rdparty/spirv-tools/source/opt/module.cpp
        3rdparty/spirv-tools/source/opt/optimizer.cpp
        3rdparty/spirv-tools/source/opt/pass.cpp
        3rdparty/spirv-tools/source/opt/pass_manager.cpp
        3rdparty/spirv-tools/source/opt/pch_source_opt.cpp
        3rdparty/spirv-tools/source/opt/private_to_local_pass.cpp
        3rdparty/spirv-tools/source/opt/process_lines_pass.cpp
        3rdparty/spirv-tools/source/opt/propagator.cpp
        3rdparty/spirv-tools/source/opt/reduce_load_size.cpp
        3rdparty/spirv-tools/source/opt/redundancy_elimination.cpp
        3rdparty/spirv-tools/source/opt/register_pressure.cpp
        3rdparty/spirv-tools/source/opt/remove_duplicates_pass.cpp
        3rdparty/spirv-tools/source/opt/replace_invalid_opc.cpp
        3rdparty/spirv-tools/source/opt/scalar_analysis.cpp
        3rdparty/spirv-tools/source/opt/scalar_analysis_simplification.cpp
        3rdparty/spirv-tools/source/opt/scalar_replacement_pass.cpp
        3rdparty/spirv-tools/source/opt/set_spec_constant_default_value_pass.cpp
        3rdparty/spirv-tools/source/opt/simplification_pass.cpp
        3rdparty/spirv-tools/source/opt/ssa_rewrite_pass.cpp
        3rdparty/spirv-tools/source/opt/strength_reduction_pass.cpp
        3rdparty/spirv-tools/source/opt/strip_debug_info_pass.cpp
        3rdparty/spirv-tools/source/opt/strip_reflect_info_pass.cpp
        3rdparty/spirv-tools/source/opt/struct_cfg_analysis.cpp
        3rdparty/spirv-tools/source/opt/type_manager.cpp
        3rdparty/spirv-tools/source/opt/types.cpp
        3rdparty/spirv-tools/source/opt/unify_const_pass.cpp
        3rdparty/spirv-tools/source/opt/upgrade_memory_model.cpp
        3rdparty/spirv-tools/source/opt/value_number_table.cpp
        3rdparty/spirv-tools/source/opt/vector_dce.cpp
        3rdparty/spirv-tools/source/opt/workaround1209.cpp
        3rdparty/spirv-tools/source/parsed_operand.cpp
        3rdparty/spirv-tools/source/print.cpp
        3rdparty/spirv-tools/source/reduce/change_operand_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/change_operand_to_undef_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/merge_blocks_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/merge_blocks_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/operand_to_const_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/operand_to_dominating_id_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/operand_to_undef_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/pch_source_reduce.cpp
        3rdparty/spirv-tools/source/reduce/reducer.cpp
        3rdparty/spirv-tools/source/reduce/reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/reduction_pass.cpp
        3rdparty/spirv-tools/source/reduce/reduction_util.cpp
        3rdparty/spirv-tools/source/reduce/remove_function_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/remove_function_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/remove_instruction_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/remove_opname_instruction_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/remove_unreferenced_instruction_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/reduce/structured_loop_to_selection_reduction_opportunity.cpp
        3rdparty/spirv-tools/source/reduce/structured_loop_to_selection_reduction_opportunity_finder.cpp
        3rdparty/spirv-tools/source/software_version.cpp
        3rdparty/spirv-tools/source/spirv_endian.cpp
        3rdparty/spirv-tools/source/spirv_optimizer_options.cpp
        3rdparty/spirv-tools/source/spirv_reducer_options.cpp
        3rdparty/spirv-tools/source/spirv_target_env.cpp
        3rdparty/spirv-tools/source/spirv_validator_options.cpp
        3rdparty/spirv-tools/source/table.cpp
        3rdparty/spirv-tools/source/text.cpp
        3rdparty/spirv-tools/source/text_handler.cpp
        3rdparty/spirv-tools/source/util/bit_vector.cpp
        3rdparty/spirv-tools/source/util/parse_number.cpp
        3rdparty/spirv-tools/source/util/string_utils.cpp
        3rdparty/spirv-tools/source/val/basic_block.cpp
        3rdparty/spirv-tools/source/val/construct.cpp
        3rdparty/spirv-tools/source/val/function.cpp
        3rdparty/spirv-tools/source/val/instruction.cpp
        3rdparty/spirv-tools/source/val/validate.cpp
        3rdparty/spirv-tools/source/val/validate_adjacency.cpp
        3rdparty/spirv-tools/source/val/validate_annotation.cpp
        3rdparty/spirv-tools/source/val/validate_arithmetics.cpp
        3rdparty/spirv-tools/source/val/validate_atomics.cpp
        3rdparty/spirv-tools/source/val/validate_barriers.cpp
        3rdparty/spirv-tools/source/val/validate_bitwise.cpp
        3rdparty/spirv-tools/source/val/validate_builtins.cpp
        3rdparty/spirv-tools/source/val/validate_capability.cpp
        3rdparty/spirv-tools/source/val/validate_cfg.cpp
        3rdparty/spirv-tools/source/val/validate_composites.cpp
        3rdparty/spirv-tools/source/val/validate_constants.cpp
        3rdparty/spirv-tools/source/val/validate_conversion.cpp
        3rdparty/spirv-tools/source/val/validate_datarules.cpp
        3rdparty/spirv-tools/source/val/validate_debug.cpp
        3rdparty/spirv-tools/source/val/validate_decorations.cpp
        3rdparty/spirv-tools/source/val/validate_derivatives.cpp
        3rdparty/spirv-tools/source/val/validate_execution_limitations.cpp
        3rdparty/spirv-tools/source/val/validate_extensions.cpp
        3rdparty/spirv-tools/source/val/validate_function.cpp
        3rdparty/spirv-tools/source/val/validate_id.cpp
        3rdparty/spirv-tools/source/val/validate_image.cpp
        3rdparty/spirv-tools/source/val/validate_instruction.cpp
        3rdparty/spirv-tools/source/val/validate_interfaces.cpp
        3rdparty/spirv-tools/source/val/validate_layout.cpp
        3rdparty/spirv-tools/source/val/validate_literals.cpp
        3rdparty/spirv-tools/source/val/validate_logicals.cpp
        3rdparty/spirv-tools/source/val/validate_memory.cpp
        3rdparty/spirv-tools/source/val/validate_memory_semantics.cpp
        3rdparty/spirv-tools/source/val/validate_mode_setting.cpp
        3rdparty/spirv-tools/source/val/validate_non_uniform.cpp
        3rdparty/spirv-tools/source/val/validate_primitives.cpp
        3rdparty/spirv-tools/source/val/validate_scopes.cpp
        3rdparty/spirv-tools/source/val/validate_type.cpp
        3rdparty/spirv-tools/source/val/validation_state.cpp
        )
    target_compile_definitions(spirv-opt PUBLIC
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )
    target_include_directories(spirv-opt PUBLIC
        3rdparty/spirv-tools/include
        3rdparty/spirv-tools/include/generated
        3rdparty/spirv-tools/source
        3rdparty/spirv-tools
        3rdparty/spirv-headers/include
        )

    add_library(spirv-cross
        3rdparty/spirv-cross/spirv_cfg.cpp
        3rdparty/spirv-cross/spirv_cpp.cpp
        3rdparty/spirv-cross/spirv_cross.cpp
        3rdparty/spirv-cross/spirv_cross_parsed_ir.cpp
        3rdparty/spirv-cross/spirv_cross_util.cpp
        3rdparty/spirv-cross/spirv_glsl.cpp
        3rdparty/spirv-cross/spirv_hlsl.cpp
        3rdparty/spirv-cross/spirv_msl.cpp
        3rdparty/spirv-cross/spirv_parser.cpp
        3rdparty/spirv-cross/spirv_reflect.cpp
        )
    target_compile_definitions(spirv-cross PUBLIC
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )
    target_include_directories(spirv-cross PUBLIC
        3rdparty/spirv-cross/include
        )

    ########################
    # BGFX Geometry Compiler
    ########################

    add_executable(geometryc
        3rdparty/forsyth-too/forsythtriangleorderoptimizer.cpp
        3rdparty/ib-compress/indexbuffercompression.cpp
        3rdparty/ib-compress/indexbufferdecompression.cpp
        examples/common/bounds.cpp
        src/vertexdecl.cpp
        tools/geometryc/geometryc.cpp
        )
    target_compile_definitions(geometryc PRIVATE __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
    target_include_directories(geometryc PRIVATE include 3rdparty examples/common)
    target_link_libraries(geometryc PRIVATE bkaradzic::bx)

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
    target_compile_definitions(shaderc PRIVATE
        __STDC_LIMIT_MACROS
        __STDC_FORMAT_MACROS
        __STDC_CONSTANT_MACROS
        $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        )
    target_include_directories(shaderc PRIVATE
        include
        3rdparty/dxsdk/include
        3rdparty/fcpp
        3rdparty/glslang/glslang/Public
        3rdparty/glslang/glslang/Include
        3rdparty/glslang
        3rdparty/spirv-cross
        3rdparty/spirv-tools/include
        )
    target_link_libraries(shaderc PRIVATE bkaradzic::bx fcpp glslang glsl-optimizer spirv-opt spirv-cross)

endif()

if (BGFX_BUILD_EXAMPLES)

    add_library(example-common
        3rdparty/dear-imgui/imgui.cpp
        3rdparty/dear-imgui/imgui_demo.cpp
        3rdparty/dear-imgui/imgui_draw.cpp
        3rdparty/dear-imgui/imgui_widgets.cpp
        3rdparty/ib-compress/indexbuffercompression.cpp
        3rdparty/ib-compress/indexbufferdecompression.cpp
        examples/common/bgfx_utils.cpp
        examples/common/bounds.cpp
        examples/common/camera.cpp
        examples/common/cube_atlas.cpp
        examples/common/debugdraw/debugdraw.cpp
        examples/common/entry/cmd.cpp
        examples/common/entry/entry.cpp
        examples/common/entry/entry_android.cpp
        examples/common/entry/entry_asmjs.cpp
        examples/common/entry/entry_glfw.cpp
        examples/common/entry/entry_noop.cpp
        examples/common/entry/entry_sdl.cpp
        examples/common/entry/entry_windows.cpp
        examples/common/entry/entry_x11.cpp
        examples/common/entry/input.cpp
        examples/common/font/font_manager.cpp
        examples/common/font/text_buffer_manager.cpp
        examples/common/font/text_metrics.cpp
        examples/common/font/utf8.cpp
        examples/common/imgui/imgui.cpp
        examples/common/imgui/scintilla.cpp
        examples/common/nanovg/nanovg.cpp
        examples/common/nanovg/nanovg_bgfx.cpp
        examples/common/ps/particle_system.cpp
        )
    target_compile_definitions(example-common PUBLIC __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
    target_include_directories(example-common PUBLIC include 3rdparty)
    target_link_libraries(example-common PUBLIC bkaradzic::bx bkaradzic::bimg )

    if (BGFX_BUILD_TOOLS)
        #####################
        # BGFX Texture Viewer
        #####################
        add_executable(texturev tools/texturev/texturev.cpp)
        target_compile_definitions(texturev PRIVATE __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
        target_include_directories(texturev PRIVATE examples/common)
        target_link_libraries(texturev PRIVATE example-common bkaradzic::bimg_decode bkaradzic::bimg bgfx bkaradzic::bx)
    endif()

    add_library(example-glue examples/common/example-glue.cpp)
    target_compile_definitions(example-glue PUBLIC __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
    target_include_directories(example-glue PUBLIC include 3rdparty)
    target_link_libraries(example-glue PUBLIC bkaradzic::bx bkaradzic::bimg)

    add_executable(examples
        examples/00-helloworld/helloworld.cpp
        examples/01-cubes/cubes.cpp
        examples/02-metaballs/metaballs.cpp
        examples/03-raymarch/raymarch.cpp
        examples/04-mesh/mesh.cpp
        examples/05-instancing/instancing.cpp
        examples/06-bump/bump.cpp
        examples/07-callback/callback.cpp
        examples/08-update/update.cpp
        examples/09-hdr/hdr.cpp
        examples/10-font/font.cpp
        examples/11-fontsdf/fontsdf.cpp
        examples/12-lod/lod.cpp
        examples/13-stencil/stencil.cpp
        examples/14-shadowvolumes/shadowvolumes.cpp
        examples/15-shadowmaps-simple/shadowmaps_simple.cpp
        examples/16-shadowmaps/shadowmaps.cpp
        examples/17-drawstress/drawstress.cpp
        examples/18-ibl/ibl.cpp
        examples/19-oit/oit.cpp
        examples/20-nanovg/nanovg.cpp
        examples/21-deferred/deferred.cpp
        examples/22-windows/windows.cpp
        examples/23-vectordisplay/main.cpp
        examples/23-vectordisplay/vectordisplay.cpp
        examples/24-nbody/nbody.cpp
        examples/25-c99/helloworld.c
        examples/26-occlusion/occlusion.cpp
        examples/27-terrain/terrain.cpp
        examples/28-wireframe/wireframe.cpp
        examples/29-debugdraw/debugdraw.cpp
        examples/30-picking/picking.cpp
        examples/31-rsm/reflectiveshadowmap.cpp
        examples/32-particles/particles.cpp
        examples/33-pom/pom.cpp
        examples/34-mvs/mvs.cpp
        examples/35-dynamic/dynamic.cpp
        examples/36-sky/sky.cpp
        examples/37-gpudrivenrendering/gpudrivenrendering.cpp
        examples/38-bloom/bloom.cpp
        )
    target_compile_definitions(examples PRIVATE __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS __STDC_CONSTANT_MACROS $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)
    target_include_directories(examples PRIVATE include 3rdparty examples/common)
    target_link_libraries(examples PRIVATE example-common example-glue bgfx bkaradzic::bimg_decode bkaradzic::bimg bkaradzic::bx)

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
install(DIRECTORY include/bgfx DESTINATION include COMPONENT Development)
install(FILES src/bgfx_shader.sh DESTINATION include/bgfx COMPONENT Development)

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

if (BGFX_BUILD_EXAMPLES)
    install(TARGETS examples
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
    NAMESPACE "bkaradzic::"
    DESTINATION ${ConfigPackageLocation}
    )

install(
    FILES
    ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfigVersion.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/bgfxConfig.cmake
    DESTINATION
    ${ConfigPackageLocation}
    )
