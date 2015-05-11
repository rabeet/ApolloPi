#include "web_socket_frame.h"

#include <iostream>
#include <limits>
#include <cstring>
#include <cassert>

WebSocketFrame::WebSocketFrame(bool fin,
                               bool rsv1, 
                               bool rsv2,
                               bool rsv3,
                               uint8_t opcode,
                               bool is_masked,
                               char mask[],
                               uint64_t payload_len,
                               char *payload)
    : fin_(fin), rsv1_(rsv1), rsv2_(rsv2), rsv3_(rsv3), opcode_(opcode), is_masked_(is_masked),
        payload_len_(
            (payload_len > std::numeric_limits<signed char>::max())
            ? (payload_len > std::numeric_limits<uint16_t>::max())
                ? EXT_PAYLOAD_LONG
                : EXT_PAYLOAD_SHORT
            : payload_len),
        ext_payload_len_(payload_len)
{
    int payload_offset = 2; // first two bytes are mandatory
    
    if (payload_len_ == EXT_PAYLOAD_LONG)
    {
        payload_offset += 8;
    }
    else if (payload_len_ == EXT_PAYLOAD_SHORT)
    {
        payload_offset += 2;
    }
    
    if (is_masked)
    {
        payload_offset += 4;
    }
    
    frame_len_ = payload_offset + payload_len;
    raw_frame_ = new char[frame_len_ + 1];
    
    std::memset(raw_frame_, 0, frame_len_ + 1);
    
    payload_ = raw_frame_ + payload_offset; // offset the payload_ pointer
        
    /* copy payload into the raw frame */
    std::strncpy(payload_, payload, ext_payload_len_);
    
    raw_frame_[0] = fin  << 7 // is this standard?
                  | rsv1 << 6
                  | rsv2 << 5
                  | rsv3 << 4
                  | opcode;

    raw_frame_[1] = is_masked << 7
                  | payload_len;
                  
    if (payload_len_ == EXT_PAYLOAD_LONG)
    {
        // populate the next 8 bytes with payload_len
        *((uint64_t *)(raw_frame_ + 2)) = payload_len;
    }
    else if (payload_len_ == EXT_PAYLOAD_SHORT)
    {
        // populate the next 8 bytes with payload_len
        *((uint16_t *)(raw_frame_ + 2)) = payload_len;
    }

    if (is_masked)
    {
        apply_mask(mask);
    }
}

WebSocketFrame::~WebSocketFrame()
{
    delete [] raw_frame_;
    // anything else to do?
}

WebSocketFrame *WebSocketFrame::make_frame(char *payload, uint64_t len)
{
    return new WebSocketFrame(true, false, false, false, TEXT_FRAME, false, NULL, len, payload);
}

WebSocketFrame *WebSocketFrame::proc_frame(char *data, int n)
{
    assert( n >= 2 ); // TODO handle special cases
    
    bool fin  = data[0] & (1 << 7); // TODO Macro these
    bool rsv1 = data[0] & (1 << 6);
    bool rsv2 = data[0] & (1 << 5);
    bool rsv3 = data[0] & (1 << 4);
    
    uint8_t opcode = data[0] & 0xF; // 4 LSBs
    bool is_masked = data[1] & (1 << 7);
    uint64_t len = data[1] & 0x7F;  // 7 LSBs
    
    int payload_offset = 2; // first two bytes mandatory

    if (len == 126)
    {
        len = *((uint16_t *)(data + 2));
        payload_offset += 2;
    }
    else if (len == 127)
    {
        len = *((uint64_t *)(data + 2));
        payload_offset += 8;

    }

    char mask[4];
    if (is_masked)
    {
        std::strncpy(mask, data + payload_offset, 4);
        payload_offset += 4;
    }
    
    return new WebSocketFrame(fin, rsv1, rsv2, rsv3, opcode, is_masked, mask, len, data + payload_offset);
}

/* Private methods */

// See section 5.3 for description of this algorithm
void WebSocketFrame::apply_mask(char mask[])
{
    int j;
    
    for (int i = 0; i < ext_payload_len_; ++i)
    {
        j = i % 4;
        payload_[i] ^= mask[j];
    }
}
