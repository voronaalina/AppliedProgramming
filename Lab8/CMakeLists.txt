cmake_minimum_required(VERSION 3.21)

project(Lab8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(Lab8
    src/main.cpp
    src/SQLiteUserRepository.cpp
    src/PrintDocumentDataAccess.cpp
    src/TestData.cpp
)

target_include_directories(Lab8
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Lab8
    PRIVATE
        sqlite_orm::sqlite_orm
        unofficial::sqlite3::sqlite3
)