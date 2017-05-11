#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

extern int rank, size;

const char *INFO = "INFO";
const char *WARN = "WARN";
const char *ERROR = "ERROR";

void logger(const char* tag, const char* message) {
    time_t now;
    time(&now);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = 0;   
    printf("%s [Processor %d of %d] [%s]: %s\n", timestamp, rank, size, tag, message);
}

char* logger_string(const char* tag) {
    char *buffer = malloc(sizeof(char) * 55);
    time_t now;
    time(&now);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp) - 1] = 0;
    sprintf(buffer, "%s [Processor %d of %d] [%s]:", timestamp, rank, size, tag);
    return buffer;
}

