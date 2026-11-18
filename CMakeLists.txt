cmake_minimum_required(VERSION 3.25)
project(Lab8Variant12 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(Lab8Variant12
    src/main.cpp
    src/database.cpp
    src/camera_repository.cpp
    src/event_repository.cpp
    src/scenarios.cpp
    src/raw_sqlite_demo.cpp
)

target_include_directories(Lab8Variant12 PRIVATE include)

target_link_libraries(Lab8Variant12 PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)

if(MSVC)
  target_compile_options(Lab8Variant12 PRIVATE /W4 /permissive- /utf-8)
endif()