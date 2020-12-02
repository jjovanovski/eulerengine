cmake_minimum_required(VERSION 3.10)

project(EulerEngine VERSION 0.1)

option(EULER_INCLUDE_TESTS "Include tests for EulerEngine" "FALSE")

# add third-party dependencies
add_subdirectory("thirdparty/glfw")
if(EULER_INCLUDE_TESTS)
	add_subdirectory("thirdparty/googletest")
endif()

# add our projects
add_subdirectory("src/core")
add_subdirectory("src/sandbox")
if(EULER_INCLUDE_TESTS)
	add_subdirectory("src/tests")
endif()
