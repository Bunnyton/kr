#pragma once
#define KLIENT1_HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8081

#define ID 1

#define MAXLINE 1024
#define CTX ((char) 2)
#define ETX ((char) 3)

enum _bool { false = 0, true = 1};

typedef enum _bool bool;