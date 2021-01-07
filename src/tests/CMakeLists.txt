add_executable(
	Tests 
	main.cpp
	MathTests.cpp
)

target_link_libraries(Tests PUBLIC 
	EulerCore
	gtest
	gtest_main
)

target_include_directories(Tests PUBLIC
	"${PROJECT_SOURCE_DIR}/src/core"
	"${PROJECT_SOURCE_DIR}/thirdparty/googletest/googletest/include"
)

set_target_properties( Tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY 
    "${PROJECT_SOURCE_DIR}/out"
)
set_target_properties( gtest PROPERTIES LIBRARY_OUTPUT_DIRECTORY 
    "${PROJECT_SOURCE_DIR}/out"
)