#ifndef buffer_h
#define buffer_h

#include <vector>

#include <assert.h>
#include <string.h>

class buffer 
{
    public:
      buffer(int size){
          pdata_ = new char[size];
          size_ = size;
          cur_pos = 0;
          end_pos = size;
      }
      ~buffer(){
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
          memcpy(pdata_ + cur_pos, &data, length);
          cur_pos += length;
      }

      void write_data(char *pdata, int length){
          if (NULL == pdata){
              return;
          }
          if (length < 1 || length > get_left_length()){
              return;
          }
          memcpy(pdata_ + cur_pos, &pdata, length);
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

    private:
      char *pdata_;
      int size_;
      int cur_pos;
      int end_pos;
}
#endif // !1