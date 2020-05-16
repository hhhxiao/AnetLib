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
        char *newBuffer = new char[capacity * 2];
        memset(newBuffer, 0, capacity * 2);
        for (int i = 0; i < capacity; ++i)
            newBuffer[i] = _begin[i];
        auto len = size();
        delete _begin;
        _begin = newBuffer;
        _end = newBuffer + len;
    }

public:
    DataBuffer() : _begin(nullptr), _end(nullptr) {}

    iterator begin() {
        return this->_begin;
    }

    void push_back(const char c) {
        if (!_begin) {
            this->_begin = new char[64];
            this->capacity = 64;
            *_begin = c;
            this->_end = _begin + 1;
        } else {
            if (size() == capacity) {
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
