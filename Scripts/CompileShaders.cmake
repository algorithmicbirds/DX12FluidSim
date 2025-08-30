cmake_minimum_required(VERSION 3.8)
project(DX12FluidSim)

set(DXC_EXECUTABLE "C:/DXC/bin/x64/dxc.exe")
if(NOT EXISTS ${DXC_EXECUTABLE})
    message(FATAL_ERROR "DXC not found at ${DXC_EXECUTABLE}")
endif()

set(RAW_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/RawShaders")
set(COMPILED_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/CompiledShaders")
file(MAKE_DIRECTORY "${COMPILED_SHADERS_DIR}")

file(GLOB_RECURSE SHADERS "${RAW_SHADERS_DIR}/*.hlsl")

set(CSO_FILES "")
foreach(SHADER ${SHADERS})
    file(RELATIVE_PATH REL_PATH "${RAW_SHADERS_DIR}" ${SHADER})
    get_filename_component(REL_DIR ${REL_PATH} DIRECTORY)
    get_filename_component(NAME ${SHADER} NAME)
    get_filename_component(NAME_WE ${SHADER} NAME_WE)
    get_filename_component(EXT ${SHADER} LAST_EXT)

    if(NAME MATCHES "\\_vs\\.hlsl$")
        set(SHADER_TARGET "vs_6_8")
        set(OUT_FILE "${COMPILED_SHADERS_DIR}/${REL_DIR}/${NAME_WE}.cso")
    elseif(NAME MATCHES "\\_ps\\.hlsl$")
        set(SHADER_TARGET "ps_6_8")
        set(OUT_FILE "${COMPILED_SHADERS_DIR}/${REL_DIR}/${NAME_WE}.cso")
    else()
        message(FATAL_ERROR "Unknown shader type for ${SHADER}. Use *_vs.hlsl or *_ps.hlsl")
    endif()

    get_filename_component(OUT_DIR ${OUT_FILE} DIRECTORY)
    file(MAKE_DIRECTORY ${OUT_DIR})

    add_custom_command(
        OUTPUT ${OUT_FILE}
        COMMAND ${DXC_EXECUTABLE} -T ${SHADER_TARGET} -E main -Fo "${OUT_FILE}" "${SHADER}"
        DEPENDS ${SHADER}
        COMMENT "Compiling HLSL shader ${REL_PATH}"
    )

    list(APPEND CSO_FILES ${OUT_FILE})
endforeach()

add_custom_target(Shaders ALL DEPENDS ${CSO_FILES})
