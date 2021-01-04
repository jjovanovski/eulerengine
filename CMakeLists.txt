cmake_minimum_required(VERSION 3.10)

project(EulerEngine VERSION 0.1)

option(EULER_INCLUDE_TESTS "Include tests for EulerEngine" "FALSE")

# force static runtime libraries for msvc builds
if(MSVC)
  set(variables 
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
	CMAKE_CXX_FLAGS_RELWITHDEBINFO
	CMAKE_CXX_FLAGS_MINSIZEREL
  )
  foreach(variable ${variables})
	if(${variable} MATCHES "/MD")
	  string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
	endif()
  endforeach()
endif()

# add third-party dependencies
add_subdirectory("thirdparty/glfw")
if(EULER_INCLUDE_TESTS)
	enable_testing()
	add_subdirectory("thirdparty/googletest")
endif()

# add our projects
add_subdirectory("src/core")
add_subdirectory("src/sandbox")
if(EULER_INCLUDE_TESTS)
	add_subdirectory("src/tests")
endif()
