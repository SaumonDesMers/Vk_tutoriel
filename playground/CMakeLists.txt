cmake_minimum_required(VERSION 3.15)

project(play
	VERSION 0.0.1
	DESCRIPTION "Playground for testing c++ vulkan API features"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-g
)

target_sources(${PROJECT_NAME}
	PRIVATE
		src/main.cpp
		src/logger.cpp
		src/application.cpp
)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		cppVulkanAPI
)