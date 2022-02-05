##generate a static lib...
cmake_minimum_required(VERSION 3.16.3)

project(hundun)

set(SOURCES
    stc/*.cpp
    lib/lei/src/*.cpp
    main.cpp
)

target_include_directories(hundun
    PRIVATE 
        src/*.h
        lib/lei/src/*.h
)

add_executable(hundun ${SOURCES})

target_link_libraries(hundun
    PRIVATE
        pthread
        rocksdb
)

