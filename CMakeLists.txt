cmake_minimum_required(VERSION 3.14)
project(MiniSTL)

set(CMAKE_CXX_STANDARD 11)

add_executable(MiniSTL main.cpp iterator.h type_traits.h vector.h exception.h util.h construct.h allocator.h algobase.h uninitialized.h memory.h test/t_vector.h)