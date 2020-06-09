//
// Created by hhhxiao on 5/16/20.
//

#ifndef ANET_DATABUFFER_H
#define ANET_DATABUFFER_H


#include <cstring>

class DataBuffer {
private:
    typedef char *iterator;
    char *_begin;
    size_t capacity = 0;
    char *_end;

    void incCapacity() {
        //alloc memory
        capacity *= 2; //add capacity
        char *newBuffer = new char[capacity];
        memset(newBuffer, 0, capacity);
        auto len = _end - _begin;
        for (int i = 0; i < len; ++i)
            newBuffer[i] = _begin[i];
        delete[] _begin;
        //reset _begin and _end
        _begin = newBuffer;
        _end = newBuffer + len;
    }

public:
    DataBuffer() : _begin(nullptr), _end(nullptr) {}

    ~DataBuffer() {
        delete[] _begin;
    }

    iterator begin() {
        return this->_begin;
    }

    void push_back(const char c) {
        //the first push_back
        if (!_begin) {
            this->_begin = new char[1024];
            this->capacity = 1024;
            *_begin = c;
            this->_end = _begin + 1;
        } else {
            //not the first push_back
            if (size() == capacity) {
                printf("size is %zu\n", size());
                //inc capacity
                incCapacity();
            }
            *_end = c;
            ++_end;
        }
    }

    iterator end() {
        return this->_end;
    }

    size_t cap() const {
        return capacity;
    }

    size_t size() {
        return _begin && _end ? _end - _begin : 0;
    }

    void clear() {
        memset(_begin, 0, size());
        _end = _begin;
    }

    std::string to_string() {
        return std::string(this->_begin);
    }

};


#endif //ANET_DATABUFFER_H
