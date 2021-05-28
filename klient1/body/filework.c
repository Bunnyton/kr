#include "../headers/filework.h"

in_addr_t *find_addr(uint16_t id) {
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL)
        return NULL;

    char *str = (char *) malloc(sizeof(char) * MAXLINE);
    if (str == NULL)
        goto error1;

    in_addr_t *addr = (in_addr_t *) malloc(sizeof(in_addr_t));
    if (addr == NULL)
        goto error2;

    while (read_to(file, ' ', str) != EOF) {
        if (str_to_uint16_t(str) == id) {

            if (read_to(file, ' ', str) == EOF)
                goto error3;//пропуск имени
            if (read_to(file, '\n', str) == EOF)
                goto error3;//чтение адреса


            if (inet_aton(str, (struct in_addr*) addr) == 0)
                goto error3;

            fclose(file);
            free(str);
            return addr;
        }
        if (read_to(file, '\n', str) == EOF)
            goto error3;
    }

    error3:
    free(addr);
    error2:
    free(str);
    error1:
    fclose(file);
    return NULL;
}

uint16_t find_id(in_addr_t addr)
{
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL) return UNKNOWN;

    char *str = (char *) malloc(sizeof(char) * MAXLINE);
    if (str == NULL)
        goto error1;

    in_addr_t *current_addr = (in_addr_t *) malloc(sizeof(in_addr_t));
    if (current_addr == NULL)
        goto error2;

    while (read_to(file, ' ', str) != EOF) {
        uint16_t current_id = str_to_uint16_t(str);

        if (read_to(file, ' ', str) == EOF)
            goto error3;//пропуск имени
        if (read_to(file, '\n', str) == EOF)
            goto error3;//чтение адреса

        if (inet_aton(str, (struct in_addr*) current_addr) == 0)
            goto error3;

        if (*current_addr == addr)
            return current_id;
    }

    error3:
    free(current_addr);
    error2:
    free(str);
    error1:
    fclose(file);
    return UNKNOWN;
}

char* find_nick(uint16_t id)
{
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL)
        return NULL;

    char *str = (char *) malloc(sizeof(char) * MAXLINE);
    if (str == NULL)
        goto error1;

    while (read_to(file, ' ', str) != EOF) {
        if (str_to_uint16_t(str) == id) {

            if (read_to(file, ' ', str) == EOF)
                goto error2;//чтение имени

            fclose(file);
            return str;
        }
    }

    error2:
    free(str);
    error1:
    fclose(file);
    return NULL;
}

bool save_in(char *buffer, char *filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL)
        return false;

    fprintf(file, "%s\n", buffer);

    fclose(file);

    return true;
}

bool save_msg(char *buffer, uint16_t id) {
    char *filename = uint16_t_to_str(id);
    if (filename == NULL) {
        perror("can't allocate memory");
        return false;
    }
    filename = realloc(filename, strlen(filename) + 4);
    stradd(filename, ".txt", strlen(filename));

    bool result = true;

    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("file error");
        result = false;
    }

    fprintf(file, "%c%s%c\n", CTX, buffer, ETX);

    free(file);
    free(filename);

    return result;
}

uint16_t make_id() {
    FILE *file = fopen(USER_LIST_FILE, "r");
    if (file == NULL) return UNKNOWN;

    uint16_t id = -1;

    char *buffer = malloc(sizeof(char) * MAXLINE);
    if (buffer == NULL)
        goto error;

    while (read_to(file, ' ', buffer) != EOF) {
        id = str_to_uint16_t(buffer);

        if (read_to(file, '\n', buffer) == EOF)//Пролистывание ненужных столбцов
            break;
    }

    free(buffer);
    error:
    fclose(file);
    return id + 1;//если id == -1, то вернется 0 соответствующий UNKNOWN
}
