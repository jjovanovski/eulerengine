add_executable(
	EulerModel
	main.cpp
)

target_link_libraries(
	EulerModel
	PUBLIC
	EulerCore
	assimp
)

target_include_directories(
	EulerModel
	PUBLIC
	"${PROJECT_SOURCE_DIR}/src/core"
	"${PROJECT_SOURCE_DIR}/thirdparty/assimp/include"
)