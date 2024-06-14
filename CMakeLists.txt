cmake_minimum_required(VERSION 3.28)
project(Proyecto1)

set(CMAKE_CXX_STANDARD 17)

add_executable(IMP imp_test.cpp imp.cpp imp_parser.cpp imp_value.cpp imp_printer.cpp imp_interpreter.cpp)

add_executable(Compiler imp_compiler.cpp imp.cpp imp_parser.cpp imp_printer.cpp imp_typechecker.cpp imp_value.cpp imp_interpreter.cpp imp_codegen.cpp)

add_executable(SVM svm_run.cpp svm_parser.cpp svm.cpp)

