cmake_minimum_required(VERSION 3.16)

project(cpp-algorithm-experiments LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#${CMAKE_SOURCE_DIR}/*.h
FILE(GLOB_RECURSE ALL_HEADERS "src/*.h" "src/*.hpp")
FILE(GLOB_RECURSE ALL_CPP "src/*.cpp" "src/*.c")

add_executable(${PROJECT_NAME} ${ALL_CPP} ${ALL_HEADERS}) # "main.cpp"

include(GNUInstallDirs)
install(TARGETS cpp-algorithm-experiments
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

