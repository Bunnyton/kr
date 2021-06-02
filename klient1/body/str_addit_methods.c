#include "../headers/str_addit_methods.h"

char* uint16_t_to_str(uint16_t number)
{
    char *rev_str = (char *) malloc(sizeof(char) * 5);//Максимальное количество символов в числе
    if (rev_str == NULL) return NULL;

    int num_len = 0;
    while (number != 0) {
        rev_str[num_len] = (char)(number % 10 + (int) '0');
        number /= 10;
        ++num_len;
    }

    char *str = (char *)malloc(sizeof(char) * (num_len + 1));
    if (str == NULL) return NULL;

    for (int i = 0; i < num_len; ++i)
        str[i] = rev_str[num_len - 1 - i];
    str[num_len] = '\0';

    return str;
}

uint16_t str_to_uint16_t(char *str)
{
    uint16_t val = 0;
    for (int i = 0; i < strlen(str); ++i)
        val = val * 10 + (uint16_t) str[i] - (uint16_t) '0';
    return val;
}

int read_to(FILE *file, char end_sym, char *str)
{
    int len = 0;

    char current_value;
    while (fscanf(file, "%c", &current_value) != EOF && current_value != end_sym){
        ++len;
        str[len - 1] = current_value;
    }

    str[len] = '\0';

    if (current_value == end_sym)
        return len;
    else
        return EOF;
}
