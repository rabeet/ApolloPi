/**
 * Implementation of a WebSocket server compliant to the RFC 6455.
 *
 * Author : Jim Carroll
 * Last modified on 2013-3-5
 */

#include "WebSocketServer.h"
#include <climits>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>

WebSocketServer::WebSocketServer(int port)
{
    init(port);
}

WebSocketServer::~WebSocketServer()
{
    // nothing to delete... yet
    // other cleanup?
}

bool WebSocketServer::accept()
{
    /* wait for a client to connect -- blocks while waiting */
    socklen_t client_len = sizeof(client_addr);
    client_fd = ::accept(socket_fd, (sockaddr *) &client_addr, &client_len);

    if (client_fd < 0)
    {
        std::cerr << "ERROR: Could not accept new client." << std::endl;
        return false;
    }

    std::cout << "Sending handshake...";
    if (handshake())
    {
        return true;
    }

    return false;
}

/// frames the data per section 5.2
bool WebSocketServer::send(std::string data, int length)
{
	int      packet_size = 2 + length;
    char *   packet = new char[packet_size];

    
    // start setting up frame
    std::memset(packet, 0, packet_size);

    /* FIN and opcode */
    packet[0] = 0x81; // FIN = 1, opcode = 1 (text frame) 

    /* packet length */
    packet[1] = (char) length;
    packet[1] &= 0x7f; // turn off mask bit
	
	std::strcpy(packet + 2, data.c_str());
	
	int n = ::send(client_fd, packet, packet_size, 0);
	
	std::cout << "Sent " << n << " bytes to client: " << (char *)(packet+2) << std::endl;
	free(packet);
	return true;
}

/**
 * Echo on receive.
 */
char * WebSocketServer::receive()
{
    char * buffer = new char[BUF_SIZE];
    std::memset(buffer, 0, BUF_SIZE);
	
	int n = recv(client_fd, buffer, BUF_SIZE - 1, 0);
	
	if (n == -1) {
		std::cout << "***ERROR RECEIVING***" << std::endl;
	} else if (n == 0) {
		std::cout << "recv() returned a zero." << std::endl;
	} else {
		std::cout << "Read a packet. " << n << " bytes." << std::endl << "Frame:" << std::endl;
		/* draw line */
		std::cout << std::setw(20) << std::setfill('-') << "" << std::endl;
		/* reset fill */
		std::cout << std::setfill(' ');
	}

    char * data = process_frame(buffer, n);

    return data;
}

char * WebSocketServer::base64(const unsigned char * input, int length)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;
    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    char * buff = (char *)malloc(bptr->length+1);
    memcpy(buff, bptr->data, bptr->length);
    buff[bptr->length] = 0;

    BIO_free_all(b64);

    return buff;
}

void WebSocketServer::init(int port)
{
    /* create an internet TCP socket with default protocol */
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    /* zero out address structures */
    std::memset((char *) &server_addr, 0, sizeof(server_addr));

    /* initialize server_addr structure */
    server_addr.sin_family      = AF_INET;
    server_addr.sin_port        = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* attempt to bind to a port -- must be super user if port < 1024 */
    std::cout << "Binding...";

    if (bind(socket_fd, (sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "ERROR: Could not bind to port " << port
            << ". Try using sudo." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Success!" << std::endl;

    /* listen on socket with backlog queue length of 0 */
    listen(socket_fd, 0);
}

bool WebSocketServer::handshake()
{
    /*
     * Now, we need to validate the WebSocket connection with client
     * per the RFC 6455.
     */
    std::string header(receive_header());
    std::cout << std::endl << "HTTP Header:" << std::endl << header << std::endl;
    
    /* parse the header */

    // FIXME All this does is find the Sec-WebSocket-Key value...
    
    // this next part is definitely hacky and error-prone
    int key_idx = header.find("Sec-WebSocket-Key:") + 19;
    std::string str_key = header.substr(key_idx, 24);

    std::cout << "Sec-WebSocket-Key: " << str_key << "END" << std::endl;

    // sha-1 key+GUID
    unsigned char hash[24];
    std::memset(hash, 0, 24);

    str_key.append(GUID);

    SHA1(reinterpret_cast<const unsigned char*>(str_key.c_str()),
            str_key.length(), hash);
    hash[20] = '\0';
    char * accept_b64 = base64(hash, 20);

    std::cout << "Sec-WebSocket-Accept: " << accept_b64 << std::endl;

    // send back an Upgrade/Accept header

    std::string accept_hdr(
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: " + std::string(accept_b64) + "\r\n");

    // send accept header
    if (::send(client_fd, accept_hdr.c_str(), accept_hdr.length(), 0))
    {
        std::cout << "Sent accept header." << std::endl;
        return true;
    }
    else
    {
        return false;
    }
}

char * WebSocketServer::receive_header()
{
    char * buffer = new char[BUF_SIZE];
    std::memset(buffer, 0, BUF_SIZE);
	
    int n = recv(client_fd, buffer, BUF_SIZE - 1, 0);
    
    if (n < 0)
    {
        std::cerr << "ERROR: Could not obtain header." << std::endl;
        // exit? exception?
    }

    std::cout << "Processing header..." << std::endl;

    return buffer;

}

void WebSocketServer::close()
{
    // send status code 1002 (protocol error)
    // then clean up
}

char * WebSocketServer::process_frame(const char * frame, int n)
{
	bool fin  = frame[0] & 0x80; // bit 7
	bool rsv1 = frame[0] & 0x40; // bit 6
	bool rsv2 = frame[0] & 0x20; // bit 5
	bool rsv3 = frame[0] & 0x10; // bit 4
	int  opc  = frame[0] & 0x0F; // bits 3, 2, 1, and 0
	bool is_masked = frame[1] & 1 << 7;
    long payload   = frame[1] & 0x7F; // b01111111
	
	int  payload_offset = 2;
	if (payload == 126) // payload is a 16-bit integer
	{
		payload = *( (uint16_t *)(frame + 2));
        payload_offset += 2; // offset past extended payload length
	}
    else if (payload == 127) // payload is a 64-bit integer
	{
		payload = *( (uint64_t *)(frame + 2) );
        // This next line needs to ensure MSB is 0, and FAIL if it is not
        std::cout << "MSB of payload length is "
		<< std::boolalpha << ( payload & (uint64_t)1<<63) << std::endl;
        payload_offset += 8;
	}
	
	char mask[4]; // mask is 4 octets
	
    if (is_masked)
	{
        mask[0] = frame[payload_offset];
        mask[1] = frame[payload_offset + 1];
        mask[2] = frame[payload_offset + 2];
        mask[3] = frame[payload_offset + 3];
        payload_offset += 4;     // 4 bytes for mask
	}
	
	std::cout << std::setw(10) << std::left << "Name"
	<< std::setw(10) << std::left << "Value" << std::endl;
	std::cout << std::setw(10) << std::left << "FIN"
	<< std::setw(10) << std::left << fin << std::endl;
	std::cout << std::setw(10) << std::left << "RSV1"
	<< std::setw(10) << std::left << rsv1 << std::endl;
	std::cout << std::setw(10) << std::left << "RSV2"
	<< std::setw(10) << std::left << rsv2 << std::endl;
	std::cout << std::setw(10) << std::left << "RSV3"
	<< std::setw(10) << std::left << rsv3 << std::endl;
	std::cout << std::setw(10) << std::left << "OPCODE"
	<< std::setw(10) << std::left << std::hex << "0x" << opc << std::endl;
	std::cout << std::setw(10) << std::left << "Payload"
	<< std::setw(10) << std::left << std::dec << payload << std::endl;
	std::cout << std::setw(10) << std::left << "Is masked"
	<< std::setw(10) << std::left << std::boolalpha << is_masked <<std::endl;
	if (is_masked) {
		std::cout << std::setw(10) << std::left << "Mask"
		<< std::setw(10) << std::left << std::hex << "0x" << (*( (uint32_t *)(mask))) << std::endl;
	}
	
	std::cout << std::endl << "OPCODE INFO: ";
    switch(opc) // TODO #define all of these
    {
        case 0x0:  // this frame is a continuation
            std::cout << "Continuation frame." << std::endl;
            /* how do we handle continued data streams? */
            break;
        case 0x1:  // denotes a text frame
			std::cout << "Text frame." << std::endl;
            break;
        case 0x2:  // denotes a binary frame
			std::cout << "Binary frame." << std::endl;
            break;
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:  // reserved for further non-control frames
			std::cout << "Unknown (control frame)." << std::endl;
            break;
        case 0x8:  // denotes a connection close
			std::cout << "Close frame." << std::endl;
			close();
			break;
        case 0x9:  // denotes a ping
			std::cout << "Ping." << std::endl;
            break;
        case 0xA:  // denotes a pong
			std::cout << "Ping." << std::endl;
            break;
        case 0xB:
        //...
        case 0xF:  // reserved for further control frames
			std::cout << "Unknown (control frame)." << std::endl;
            break;
    }

	
	std::cout << std::dec << std::endl << std::setw(20) << std::setfill('-') << "" << std::endl;
	std::cout << std::setfill(' ');
	
    char * data = (char *)(frame + payload_offset);

    if (is_masked)
    {
        apply_mask(data, payload, mask);
    }

    std::cout << std::dec << std::endl;

    return data;
}

void WebSocketServer::apply_mask(char * data, int length, char * mask)
{
    // see section 5.3 for description of this algorithm
    for (int i = 0; i < length; ++i)
    {
        int j = i % 4;
        data[i] = data[i] xor mask[j];
    }

}

int main()
{
    WebSocketServer client; // default to port 80
    /* block and wait for client */
    client.accept();

    bool connected = true;
    while (connected)
    {
        char * data = client.receive();
		std::cout << "Received: " << std::endl << data << std::endl;

		std::string str_data(data);
		std::string response;
	
		if (str_data == "name") {
			response.assign("Apollo Pi");
		} else if (str_data == "time") {
			time_t rawtime;
			struct tm * timeinfo;
			
			std::time (&rawtime);
			timeinfo = std::localtime (&rawtime);
			response.assign( asctime(timeinfo) );
		} else if (str_data == "hi") {
			response.assign("Hello!");
		} else if (str_data == "ON") {
			response.assign("Light on.");
			// add hardware code here
		} else if (str_data == "OFF") {
			response.assign("Light off.");
		} else {
			response.assign("Huh?");
		}
	
		std::cout << "Sending back: " << response << std::endl;
		client.send(response, response.length());
    }
}
