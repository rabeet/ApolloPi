#ifndef web_socket_frame_h
#define web_socket_frame_h

#include <cstdint>
/**
 * A rough draft of the WebSocket Data Frame class.
 * Many features and special cases are left unhandled.
 *
 */

#define TEXT_FRAME 0x01

#define EXT_PAYLOAD_SHORT 126
#define EXT_PAYLOAD_LONG  127

/*

RFC 6455 Data Frame

      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+

*/
class WebSocketFrame {
public:
    ~WebSocketFrame();
    
    /// Processes frame received over a Websocket
    static WebSocketFrame * proc_frame(char *data, int len);

    /// Frames data to be sent over a WebSocket
    static WebSocketFrame * make_frame(char *data, uint64_t len);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //             Getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /// Returns true if this frame is final
    bool is_fin() { return fin_; }
    
    /* not sure about these next three */
    bool rsv1() { return rsv1_; }
    bool rsv2() { return rsv2_; }
    bool rsv3() { return rsv3_; }
    
    /// Returns the opcode of this frame
    uint8_t get_opcode() { return opcode_; }
    uint64_t get_payload_len()
    {
        if (payload_len_ == EXT_PAYLOAD_SHORT || payload_len_ == EXT_PAYLOAD_LONG)
            return ext_payload_len_;
        else
            return payload_len_;
    }
    
    char * const get_payload() { return payload_; }
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //             Setters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void set_fin(bool fin)
    {
        fin_ = fin;
    }
    
    void set_rsv(bool rsv1, bool rsv2, bool rsv3)
    {
        rsv1_ = rsv1;
        rsv2_ = rsv2;
        rsv3_ = rsv3;
    }
    
    void set_opcode(uint8_t opcode)
    {
        opcode_ = opcode;
    }
    
    void set_payload(char * payload, uint64_t len)
    {
        // TODO
        // Does payload already exist? Deallocate it.
        // ...
    }
    
    char *get_frame() { return raw_frame_; }
    uint64_t get_len() { return frame_len_; }
    
private:
    /// should not be used directly. Use proc_frame() for receiving and make_frame() for sending.
    WebSocketFrame(bool fin, bool rsv1, bool rsv2, bool rsv3, uint8_t opcode, bool is_masked,
        char mask[], uint64_t payload_len, char *payload);
        
    bool fin_;
    bool rsv1_;
    bool rsv2_;
    bool rsv3_;
    uint8_t opcode_;
    bool is_masked_;
    uint64_t payload_len_;
    uint64_t ext_payload_len_;
    char *payload_;
    
    uint64_t frame_len_;
    char *raw_frame_;
    
    /// applies mask to payload
    void apply_mask(char mask[]);
};

#endif /* web_socket_frame_h */
