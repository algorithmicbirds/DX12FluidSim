cmake_minimum_required(VERSION 3.8)

project(DX12FluidSim)
set(CMAKE_CXX_STANDARD 20)

include("${CMAKE_CURRENT_SOURCE_DIR}/Scripts/CompileShaders.cmake")

option(PRODUCTION_BUILD "Build in production mode (Changes path of resources)" OFF)

# Enable MSVC Edit & Continue if available
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,"
        "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>," 
        "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
endif()

set(IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui_tables.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui_widgets.cpp"
)

# ImGui DX12 and Win32 backends
set(IMGUI_BACKENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/backends/imgui_impl_dx12.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/backends/imgui_impl_win32.cpp"
)

set(FFX_SDK "F:/FidelityFX-SDK-v1.1.4/sdk")

include_directories(
    ${FFX_SDK}/include
    ${FFX_SDK}/src/shared
    ${FFX_SDK}/src/backends/dx12
    ${FFX_SDK}/src/backends/shared 
    ${FFX_SDK}/src/components/parallelsort
    ${FFX_SDK}/libs
)


set(FFX_SOURCES
    ${FFX_SDK}/src/components/parallelsort/ffx_parallelsort.cpp
    ${FFX_SDK}/src/shared/ffx_assert.cpp
    ${FFX_SDK}/src/shared/ffx_breadcrumbs_list.cpp
    ${FFX_SDK}/src/shared/ffx_message.cpp
    ${FFX_SDK}/src/shared/ffx_object_management.cpp
    ${FFX_SDK}/src/backends/dx12/ffx_dx12.cpp
)


add_library(ffx_parallelsort ${FFX_SOURCES})

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12FluidSim/*.cpp"
)

file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12FluidSim/*.hpp"
)

file(GLOB_RECURSE WIN_RC CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12FluidSim/*.rc"
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${WIN_RC}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKENDS}
)


add_dependencies(${PROJECT_NAME} Shaders)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if (PRODUCTION_BUILD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SHADER_PATH="./Resources/CompiledShaders/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC FONTS_PATH="./Resources/Fonts/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/CompiledShaders/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC FONTS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/Fonts/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/DX12FluidSim"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/backends"
    "${CMAKE_CURRENT_SOURCE_DIR}/External/nano-signal-slot"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    dxgi
    dxguid
    d3d12
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE ON
)