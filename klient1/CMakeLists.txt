cmake_minimum_required(VERSION 3.17)
project(klient1 C)

set(CMAKE_C_STANDARD 11)

add_executable(klient1 body/main.c headers/header.h body/pack.c headers/pack.h body/network.c headers/network.h body/receive_manager.c headers/receive_manager.h body/processing_manager.c headers/processing_manager.h body/filework.c headers/filework.h body/send_manager.c headers/send_manager.h body/str_addit_methods.c headers/str_addit_methods.h body/system.c headers/system.h)