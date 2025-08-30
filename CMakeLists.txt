cmake_minimum_required(VERSION 3.8)

include("${CMAKE_CURRENT_SOURCE_DIR}/Scripts/CompileShaders.cmake")
project(DXD12FluidSim)

set(CMAKE_CXX_STANDARD 17)

option(PRODUCTION_BUILD "Build in production mode (Changes path of resources )" OFF)

# Enable MSVC Edit & Continue if available
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
        "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,"
        "$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,"
        "$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
    )
endif()


file(GLOB_RECURSE DX_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim/*.cpp"
)

file(GLOB_RECURSE DX_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim/*.hpp"
)

add_executable(${PROJECT_NAME}
    ${DX_SOURCES}
    ${DX_HEADERS}
)


add_dependencies(${PROJECT_NAME} Shaders)
target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

if (PRODUCTION_BUILD)
    target_compile_definitions(${PROJECT_NAME} PUBLIC SHADER_PATH="./Resources/CompiledShaders/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/CompiledShaders/")
    target_compile_definitions(${PROJECT_NAME} PUBLIC PRODUCTION_BUILD=0)
endif()


target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    dxgi
    dxguid
    d3d12
)