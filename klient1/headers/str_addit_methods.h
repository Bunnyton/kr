#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* uint16_t_to_str(uint16_t number);

uint16_t str_to_uint16_t(char *str);

void stradd(char *dest, char *src, ulong start);

int read_to(FILE *file, char end_sym, char *str);
