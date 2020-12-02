cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Sandbox main.cpp)

target_link_libraries(Sandbox PUBLIC 
	EulerCore
)

target_include_directories(Sandbox PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src/core"
)

set_target_properties( Sandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	"${PROJECT_SOURCE_DIR}/out"
)