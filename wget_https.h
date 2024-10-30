#include <stdio.h>

typedef struct {

    const char* headers;
    int delay;

}Http_Args;

int get_status_code(char* in);
char* get_content(char* in);
int get_response_headers(char* in, char* out);
int wget_https(const char* server, char* buffer, int buff_size, Http_Args args);
