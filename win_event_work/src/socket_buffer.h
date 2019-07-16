#ifndef socket_buffer_h
#define socket_buffer_h
#include "sbuffer.h"

class socket_buffer : public sbuffer
{
    public:
        socket_buffer() : sbuffer(1024)
        , msg_len_(0)
        , is_header_decoded_(false) {

        }
        bool is_header_decoded() { return is_header_decoded_; }
        int get_msg_len() { return msg_len_; }

        virtual void reset(){
            sbuffer::reset();
            msg_len_ = 0;
            is_header_decoded_ = false;
        }

        void decode_msg_len(){
            read_int(msg_len_);
            is_header_decoded_ = true;
        }
      private:
        int msg_len_;
        bool is_header_decoded_;
};

#endif // !socket_buffer_h