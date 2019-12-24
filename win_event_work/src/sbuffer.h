#ifndef sbuffer_h
#define sbuffer_h

#include <vector>

#include <assert.h>
#include <string.h>

class sbuffer 
{
    public:
      sbuffer(int size){
          pdata_ = new char[size];
          size_ = size;
          cur_pos = 0;
          end_pos = size;
          cur_read_pos = 0;
      }
      ~sbuffer(){
          if (pdata_){
              delete[] pdata_;
          }
      }

      int get_left_length() { return end_pos - cur_pos; }
      void write_int(int data){
          int length = sizeof(data);
          if (length > get_left_length()){
              return;
          }
          memcpy(get_begin_data(), &data, length);
          cur_pos += length;
      }

      void write_data(const char *pdata, int length){
          if (NULL == pdata){
              return;
          }
          if (length < 1 || length > get_left_length()){
              return;
          }
          memcpy(get_begin_data(), pdata, length);
          cur_pos += length;
      }
      
      char* get_begin_data() { return pdata_ + cur_pos; }
      void add_length(int length){
          cur_pos += length;
      }
      int get_data_length(){
          return cur_pos;
      }
      char *get_data() { return pdata_; }
      virtual void reset() { 
          cur_pos = 0;
          cur_read_pos = 0;
      }

      void read_int(int& value){
          int len = sizeof(value);
          memcpy(&value, pdata_ + cur_read_pos, len);
          cur_read_pos += len;
      }
      char *get_read_data() { return pdata_ + cur_read_pos; }
      int left_can_read() { return cur_pos - cur_read_pos; }

    private:
      char *pdata_;
      int size_;
      int cur_pos;
      int end_pos;

      int cur_read_pos;
};
#endif // !1