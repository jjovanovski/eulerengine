cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE CORE_SRC_FILES "*.h" "*.cpp")
add_library(EulerCore ${CORE_SRC_FILES})

target_include_directories(EulerCore PUBLIC
	"${PROJECT_SOURCE_DIR}/thirdparty/glfw/include"
	"${PROJECT_SOURCE_DIR}/thirdparty/vulkan/include"
	"${PROJECT_SOURCE_DIR}/thirdparty/stb"
	"${PROJECT_SOURCE_DIR}/thirdparty/tinyobjloader"
)

target_link_directories(EulerCore PUBLIC
	"${PROJECT_SOURCE_DIR}/thirdparty/vulkan/lib"
)

target_link_libraries(EulerCore PUBLIC
	"glfw"
	"vulkan-1"
)

set_target_properties( EulerCore PROPERTIES LIBRARY_OUTPUT_DIRECTORY
	"${CMAKE_SOURCE_DIR}/bin"
)

target_compile_definitions(EulerCore PUBLIC -DEULER_EXPORTS)