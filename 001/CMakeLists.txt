cmake_minimum_required(VERSION 3.16)
project(basicpl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(src)

add_executable(${PROJECT_NAME}
    main.cpp
    src/lexer.cpp
    src/token.cpp
    src/exception.cpp
)
add_library(mylib
    src/lexer.cpp
    src/token.cpp
    src/exception.cpp
    src/lexer.h
    src/token.h
    src/exception.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE mylib)