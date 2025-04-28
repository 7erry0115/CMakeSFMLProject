cmake_minimum_required(VERSION 3.7.2)
project(CMakeSFMLProject LANGUAGES CXX)

# 設定編譯後的執行檔目錄
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# 是否建立動態庫 (這裡預設 OFF)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# 使用 FetchContent 抓取 SFML
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM
)
FetchContent_MakeAvailable(SFML)

# 抓取所有 src 下的 cpp 和 hpp 檔案
file(GLOB_RECURSE SOURCES
        src/*.cpp
        src/*.hpp
)

# 建立執行檔
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)

# 複製 assets 資料夾到執行檔目錄
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
