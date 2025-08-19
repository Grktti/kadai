cmake_minimum_required(VERSION 3.26)
project(intern0)
set(CMAKE_CXX_STANDARD 20)

# 課題ごとにターゲットを作る（mainが複数あってもOK）
add_executable(kadai_1A kadai_1A.cpp)
add_executable(kadai_1B kadai_1B.cpp)
add_executable(kadai_1C kadai_1C.cpp)
add_executable(kadai_2A kadai_2A.cpp)
add_executable(kadai_2B kadai_2B.cpp)
add_executable(kadai_2C kadai_2C.cpp)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 共通リンクをまとめて適用
if (WIN32)
    foreach(tgt IN ITEMS kadai_1A kadai_1B kadai_1C kadai_2A kadai_2B kadai_2C)
        target_link_directories(${tgt} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib/MSVC)
        target_link_libraries(${tgt} PRIVATE ${OPENGL_LIBRARIES} glfw3)
    endforeach()
endif()

add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
