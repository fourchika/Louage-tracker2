// src/config.h
#ifndef CONFIG_H
#define CONFIG_H

#ifdef _WIN32
    // Windows
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define CLOSE_SOCKET closesocket
    #define SOCKET_TYPE SOCKET
#else
    // Linux
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <cstring>
    #define CLOSE_SOCKET close
    #define SOCKET_TYPE int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
#endif

#endif // CONFIG_H