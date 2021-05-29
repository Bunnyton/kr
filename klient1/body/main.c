#include <pthread.h>

#include "../headers/network.h"
#include "../headers/system.h"
#include "../headers/str_addit_methods.h"


int main() {

    NetworkContext *ctx = system_init();
    if (!ctx) {
        printf("Ошибка инициализации");
        return 1;
    }
    system_start(ctx);

    return 0;
}
