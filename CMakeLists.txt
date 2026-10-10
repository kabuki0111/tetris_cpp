cmake_minimum_required(VERSION 3.16)
project(Tetris C CXX)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON  CACHE BOOL "" FORCE)
FetchContent_Declare(SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG release-3.4.18)
FetchContent_MakeAvailable(SDL3)

# src フォルダの .cpp をすべてビルドする（ファイルを足したときも自動で拾う）
file(GLOB SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(tetris ${SOURCES})
target_link_libraries(tetris PRIVATE SDL3::SDL3-static)

# ソースは UTF-8（MSVC は指定しないと Shift-JIS として読む）
if(MSVC)
  target_compile_options(tetris PRIVATE /utf-8)
endif()

# assets フォルダ（サウンドなど）を実行ファイルの横にフォルダごとコピーする
add_custom_command(TARGET tetris POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:tetris>/assets)
