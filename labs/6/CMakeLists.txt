cmake_minimum_required(VERSION 3.20)
project(xyrs LANGUAGES CXX)

add_executable(xyrs xyrs.cpp calculations.cpp)
target_compile_features(xyrs PRIVATE cxx_std_17)

if(MINGW)
    target_link_options(xyrs PRIVATE -static -static-libgcc -static-libstdc++)
endif()
