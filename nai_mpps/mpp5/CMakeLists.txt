cmake_minimum_required(VERSION 3.5)

project(MPP5 VERSION 1.0.0 LANGUAGES CXX)

add_executable(Main
    ./src/Main.cpp)

target_compile_features(Main PRIVATE cxx_std_20)

