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
        void set_msg_len(int value) { msg_len_ = value;
            is_header_decoded_ = true;
        }

        virtual void reset(){
            sbuffer::reset();
            msg_len_ = 0;
            is_header_decoded_ = false;
        }
      private:
        int msg_len_;
        bool is_header_decoded_;
};

#endif // !socket_buffer_h