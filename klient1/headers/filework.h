#pragma once

#include "../headers/header.h"
#include "../headers/str_addit_methods.h"
#include "../headers/network.h"

#define USER_LIST_FILE "user_list.txt"

in_addr_t* find_addr(uint16_t id);
uint16_t find_id(in_addr_t addr);
char* find_nick(uint16_t id);

bool save_in(char *buffer, char *filename);

bool save_msg(char *buffer, uint16_t id);

uint16_t make_id();
