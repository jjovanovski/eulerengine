cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SkeletalAnimation main.cpp)

target_link_libraries(SkeletalAnimation PUBLIC 
	EulerCore
)

target_include_directories(SkeletalAnimation PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src/core"
)

set_target_properties( SkeletalAnimation PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	"${CMAKE_SOURCE_DIR}/bin/SkeletalAnimation"
)

set_target_properties( SkeletalAnimation PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/SkeletalAnimation")