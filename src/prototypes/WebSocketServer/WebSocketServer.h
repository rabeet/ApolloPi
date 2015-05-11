#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

/**
 * An implementation of the HTML5 WebSocket protocol compliant
 * to RFC 6455.
 *
 * Author : Jim Carroll
 * Last modified on
 */

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* DEFINITIONS */

#define HTTP_PORT 80  // WebSockets are created via HTTP
#define BUF_SIZE  1024
#define GUID      "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

class WebSocketServer
{   public:
        WebSocketServer(int port = 80);
        ~WebSocketServer();

        bool   accept();
		bool   send(std::string data, int length);
        char * receive();

    private:
        int         socket_fd;
		int         client_fd;
        sockaddr_in server_addr;
        sockaddr_in client_addr;

        char * base64(const unsigned char * input, int length);
        void   init(int port);
        bool   handshake();
        char * receive_header();
        void   close();
        char * process_frame(const char * frame, int n);
        void   apply_mask(char * data, int length, char * mask);
};

#endif // WEBSOCKETSERVER_H
