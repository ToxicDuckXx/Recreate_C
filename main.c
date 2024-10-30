#include <winsock2.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

typedef struct {

    const char* headers;
    int delay;

}Http_Args;

char* get_content(char* in) {
    char *ptr = strstr(in, "\r\n\r\n");
    return ptr + 4;
}

int get_status_code(char* in) {
    char sub[4];
    strncpy(sub, in + 9, 3);
    sub[4] = '\0';
    return atoi(sub);
}

int get_response_headers(char* in, char* out) {
    char *ptr = strstr(in, "\r\n\r\n");
    if (ptr - in > strlen(out) - 1) {
        return -1;
    }
    strncpy(out, in, ptr - in);
    out[ptr - in] = '\0';
    return 0;
}

int wget_https(const char* server, char* buffer, int buff_size, Http_Args args) {
    
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    SSL_CTX *ctx = SSL_CTX_new(TLS_method());
    struct sockaddr_in addr;
    SSL *ssl;
    WSADATA wsa;
    int sock;
    char request[500];

    char hostname[100];
    char *ptr = strchr(server, '/');
    
    if (ptr == NULL) {
        ptr = "/";
    }
    
    sprintf(hostname, "%.*s", (int)(ptr - server), server);
    if (args.headers) {
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n%s\r\n\r\n", ptr, hostname, args.headers);
    } else {
        sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", ptr, hostname);
    }
   // printf(request);
    
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        return -1;
    }
    
    if (!ctx) {
        return -1;
    }
    
    ssl = SSL_new(ctx);
    if (!ssl) {
        SSL_CTX_free(ctx);
        return -1;
    }
    
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(443);
    
    struct hostent *host = gethostbyname(hostname);
    addr.sin_addr = *((struct in_addr *)host->h_addr);
    
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        return -1;
    }

    if (!args.delay) {args.delay = 2000;}
    DWORD timeout = args.delay; // Timeout in milliseconds
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    SSL_set_fd(ssl, sock);
    
    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closesocket(sock);
        return -1;
    }

    

    SSL_write(ssl, request, strlen(request));
    int bytes;
    int total_bytes = 0;

    while ((total_bytes < buff_size - 1 && (bytes = SSL_read(ssl, buffer + total_bytes, buff_size - total_bytes - 1))) > 0) {
       // fwrite(buffer, 1, bytes, stdout);
        total_bytes += bytes;
        if (bytes < 0) {
            
            // Handle error (you may want to log this or return an error code)
            break; // Exit the loop on error
        } else if (bytes == 0) {
        // No more data from the server
            break; // Exit the loop
        }
    }
    buffer[total_bytes] = '\0';

    SSL_free(ssl);
    SSL_CTX_free(ctx);
    closesocket(sock);

    return 0;
}

