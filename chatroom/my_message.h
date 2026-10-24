//
// Created by 李玉辉 on 2026/10/24.
//

#ifndef RASPI_MY_MESSAGE_H
#define RASPI_MY_MESSAGE_H

#endif //RASPI_MY_MESSAGE_H
#include <cstdio>
#include <cstdlib>
#include <cstring>
class my_message
{
    public:
    static constexpr std::size_t header_length = 4;
    static constexpr std::size_t max_body_length = 512;
    my_message():body_length_(0)
    {

    }
    const char* data() const
    {
        return data_;
    }
    char* data()
    {
        return data_;
    }

    std::size_t length() const
    {
        return header_length + body_length_;
    }

    std::size_t body_length() const
    {
        return body_length_;
    }

    void body_length(std::size_t new_length){
        body_length_=new_length;
        if (body_length_>max_body_length)
            body_length_=max_body_length;
    }

    const char* body() const{
        return data_ + header_length;
    }

    char* body(){
        return data_ + header_length;
    }
    bool decode_header()
    {
        char buffer[header_length+1]="";
        std::strncpy(buffer, data_, header_length);
        body_length_=std::atoi(buffer);
        if (body_length_ > max_body_length)
        {
            body_length_ =0;
            return false;
        }
        return true;
    }

    void encode_header()
    {
        char buffer[header_length+1]="";
        std::snprintf(buffer, header_length+1, "%4d", body_length_);
        memcpy(data_, buffer, header_length);
    }
private:
    char data_[header_length+max_body_length];
    std::size_t body_length_;
};