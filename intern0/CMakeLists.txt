cmake_minimum_required(VERSION 3.26)
project(intern0)
set(CMAKE_CXX_STANDARD 20)

# ---------- executables ----------
add_executable(kadai_1A kadai_1A.cpp)
add_executable(kadai_1B kadai_1B.cpp)
add_executable(kadai_1C kadai_1C.cpp)
add_executable(kadai_2A kadai_2A.cpp)
add_executable(kadai_2B kadai_2B.cpp)
add_executable(kadai_2C kadai_2C.cpp)

# すべての課題ターゲットを一括管理
set(ALL_TGTS kadai_1A kadai_1B kadai_1C kadai_2A kadai_2B kadai_2C)

# ---------- 共通: インクルードパス ----------
# プロジェクト同梱の include/ を全ターゲットに
foreach(tgt IN LISTS ALL_TGTS)
    target_include_directories(${tgt} PRIVATE
            ${CMAKE_SOURCE_DIR}/include    # ← ここに GLFW/ が入っている前提
    )
endforeach()

# ---------- OpenGL ----------
find_package(OpenGL REQUIRED)

# ---------- Windows ----------
if (WIN32)
    foreach(tgt IN LISTS ALL_TGTS)
        target_link_directories(${tgt} PRIVATE ${CMAKE_SOURCE_DIR}/lib/MSVC)
        target_link_libraries(${tgt} PRIVATE ${OPENGL_LIBRARIES} glfw3)
    endforeach()
endif()

# ---------- macOS ----------
if (APPLE)
    # 同梱の GLFW dylib を使用（lib/APPLE/ に置いてある想定）
    set(GLFW_DIR   "${CMAKE_SOURCE_DIR}/lib/APPLE")
    set(GLFW_DYLIB "${GLFW_DIR}/libglfw.3.3.dylib")   # 必要なら libglfw.3.3.dylib に変更

    if (NOT EXISTS "${GLFW_DYLIB}")
        message(FATAL_ERROR "GLFW dylib not found: ${GLFW_DYLIB}")
    endif()

    find_library(COCOA_FRAMEWORK     Cocoa     REQUIRED)
    find_library(IOKIT_FRAMEWORK     IOKit     REQUIRED)
    find_library(COREVIDEO_FRAMEWORK CoreVideo REQUIRED)

    foreach(tgt IN LISTS ALL_TGTS)
        target_compile_definitions(${tgt} PRIVATE GL_SILENCE_DEPRECATION)
        target_link_directories(${tgt} PRIVATE ${GLFW_DIR})
        target_link_libraries(${tgt} PRIVATE
                OpenGL::GL
                glfw
                ${COCOA_FRAMEWORK} ${IOKIT_FRAMEWORK} ${COREVIDEO_FRAMEWORK}
        )
#        # 実行時に dylib を見失わないよう、バイナリ横へコピー
#        add_custom_command(TARGET ${tgt} POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                "${GLFW_DYLIB}" "$<TARGET_FILE_DIR:${tgt}>/"
#                # 参照先を書き換え：絶対パス -> @executable_path
#                COMMAND install_name_tool -change
#                /opt/homebrew/opt/glfw/lib/libglfw.3.dylib
#                @executable_path/libglfw.3.dylib
#                $<TARGET_FILE:${tgt}>
#                VERBATIM
#        )


    endforeach()
endif()

# ---------- MSVC の文字コード（Windowsのみ） ----------
add_compile_options($<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
