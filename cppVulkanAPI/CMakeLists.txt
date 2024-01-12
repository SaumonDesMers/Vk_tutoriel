cmake_minimum_required(VERSION 3.15)

project(cppVulkanAPI
	VERSION 0.1
	DESCRIPTION "Wrapper for c++ vulkan API"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} SHARED)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-g
		-Wall
		-Wextra
		-Werror
		-Wpedantic
)

target_sources(${PROJECT_NAME}
	PUBLIC
		src/window_manager.cpp
		src/window.cpp
		src/instance.cpp
		src/debug.cpp
		src/physical_device.cpp
		src/device.cpp
		src/queue.cpp
		src/swapchain.cpp
		src/image/image.cpp
		src/image/image_view.cpp
		src/pipeline/graphic_pipeline.cpp
		src/pipeline/shader_module.cpp
		src/pipeline/pipeline_layout.cpp
		src/pipeline/render_pass.cpp
		src/pipeline/descriptor_layout.cpp
		src/pipeline/descriptor_pool.cpp
		src/pipeline/descriptor_set.cpp
		src/frame_buffer.cpp
		src/command_pool.cpp
		src/command_buffer.cpp
		src/sync_object.cpp
		src/buffer.cpp
		src/device_memory.cpp
)

target_link_libraries(${PROJECT_NAME} glfw vulkan dl pthread X11 Xxf86vm Xrandr Xi)

target_include_directories(${PROJECT_NAME}
	INTERFACE
		${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE
		# ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
		${CMAKE_CURRENT_SOURCE_DIR}/src
)
