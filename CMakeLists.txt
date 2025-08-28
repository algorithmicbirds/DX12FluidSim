cmake_minimum_required (VERSION 3.8)

#include("${CMAKE_CURRENT_SOURCE_DIR}/Scripts/CompileShaders.cmake")

set(CMAKE_CXX_STANDARD 17)
option(PRODUCTION_BUILD "Make this a production build" ON)


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (DXD12FluidSim)

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim/*.cpp")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim/*.hpp")

#add_executable("${CMAKE_PROJECT_NAME}")

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES} ${MY_HEADERS})

target_compile_definitions("${CMAKE_PROJECT_NAME}" PRIVATE UNICODE _UNICODE) 
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)



if(PRODUCTION_BUILD)
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC SHADER_PATH="./Resources/CompiledShaders/")
	set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") #no console

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=1) 

else()
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Resources/CompiledShaders/")
	message(STATUS "Compiled Shaders: ${SHADER_PATH}")
	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC PRODUCTION_BUILD=0) 
endif()

#add_dependencies(${PROJECT_NAME} Shaders)


target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/DXD12FluidSim" ${Vulkan_INCLUDE_DIRS})


target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE dxgi dxguid d3d12)