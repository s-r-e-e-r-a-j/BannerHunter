// Developer: Sreeraj
// GitHub: https://github.com/s-r-e-e-r-a-j

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define BUFFER_SIZE 4096
#define TIMEOUT_SEC 5

// Initialize SSL
void init_ssl() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

// Set socket timeout
void set_timeout(int sock) {
    struct timeval tv;
    tv.tv_sec = TIMEOUT_SEC;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

// Connect TCP socket
int tcp_connect(char *host, int port) {
    int sock;
    struct sockaddr_in server;
    struct hostent *target = gethostbyname(host);
    if (!target) return -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr = *((struct in_addr *)target->h_addr);
    memset(&(server.sin_zero), 0, 8);

    set_timeout(sock);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        return -1;
    }
    return sock;
}

// Send HTTP HEAD request with HTTP/1.1 -> fallback HTTP/1.0
int send_http_head(int sock, SSL *ssl, char *host, int use_ssl) {
    char buffer[BUFFER_SIZE];
    char request[512];
    int bytes;

    // Try HTTP/1.1 first
    snprintf(request, sizeof(request),
             "HEAD / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", host);

    if (use_ssl)
        SSL_write(ssl, request, strlen(request));
    else
        send(sock, request, strlen(request), 0);

    if (use_ssl)
        bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);
    else
        bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[+] Banner (HTTP/1.1):\n%s\n", buffer);
        return 1; // success
    }

    // Fallback to HTTP/1.0
    snprintf(request, sizeof(request),
             "HEAD / HTTP/1.0\r\nHost: %s\r\n\r\n", host);

    if (use_ssl)
        SSL_write(ssl, request, strlen(request));
    else
        send(sock, request, strlen(request), 0);

    if (use_ssl)
        bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);
    else
        bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[+] Banner (HTTP/1.0):\n%s\n", buffer);
        return 1;
    }

    return 0; // failed both
}

// Grab banner over plain TCP (send \r\n if needed)
int grab_plain_tcp(int sock, char *host, int port) {
    char buffer[BUFFER_SIZE];
    int bytes = recv(sock, buffer, sizeof(buffer)-1, 0);

    // If no banner received, send \r\n
    if (bytes <= 0) {
        const char *newline = "\r\n";
        send(sock, newline, strlen(newline), 0);
        bytes = recv(sock, buffer, sizeof(buffer)-1, 0);
    }

    // If HTTP port, send HEAD request with version fallback
    if (port == 80 || port == 8080 || port == 8000) {
        if (send_http_head(sock, NULL, host, 0)) {
            return 1;
        }
    }

    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[+] Banner (TCP):\n%s\n", buffer);
        return 1;
    }
    return 0;
}

// Grab banner over SSL/TLS
int grab_ssl(int sock, char *host, int port) {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    if (SSL_connect(ssl) <= 0) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 0; // SSL failed
    }

    char buffer[BUFFER_SIZE];
    int bytes;

    // If HTTPS port, send HEAD request with version fallback
    if (port == 443 || port == 8443) {
        if (send_http_head(sock, ssl, host, 1)) {
            SSL_shutdown(ssl);
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            return 1;
        }
    }

    // Try reading any banner
    bytes = SSL_read(ssl, buffer, sizeof(buffer)-1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[+] Banner (SSL/TLS):\n%s\n", buffer);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 1;
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}

// Grab banner with SSL detection and \r\n request
void grab_banner(char *host, int port) {
    int sock = tcp_connect(host, port);
    if (sock < 0) {
        printf("[!] Could not connect to %s:%d\n", host, port);
        return;
    }

    printf("[+] Connected to %s:%d\n", host, port);

    // Try SSL first
    if (!grab_ssl(sock, host, port)) {
        // Retry plain TCP
        close(sock);
        sock = tcp_connect(host, port);
        if (sock >= 0) {
            if (!grab_plain_tcp(sock, host, port)) {
                printf("[!] No banner received\n");
            }
            close(sock);
        }
    } else {
        close(sock);
    }

    printf("---------------------------\n");
}

// Scan multiple ports
void scan_ports(char *host, int *ports, int count) {
    for (int i = 0; i < count; i++) {
        grab_banner(host, ports[i]);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <host> <port1> [port2 ...]\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    int port_count = argc - 2;
    int ports[port_count];

    for (int i = 0; i < port_count; i++) {
        ports[i] = atoi(argv[i+2]);
    }

    init_ssl();
    scan_ports(host, ports, port_count);

    return 0;
}
