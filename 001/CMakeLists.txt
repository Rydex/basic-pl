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
    src/position.cpp
    src/parser.cpp
    src/interpreter.cpp
    src/context.cpp
    src/nodes.cpp
)
add_library(mylib
    src/lexer.cpp
    src/token.cpp
    src/exception.cpp
    src/position.cpp
    src/parser.cpp
    src/interpreter.cpp
    src/context.cpp
    src/nodes.cpp
    src/token.h
    src/exception.h
    src/position.h
    src/parser.h
    src/interpreter.h
    src/context.h
    src/nodes.h
)
target_link_libraries(${PROJECT_NAME} PRIVATE mylib)