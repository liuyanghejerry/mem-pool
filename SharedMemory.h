#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <node.h>
#include <node_buffer.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <iostream>
#include "MemorySlice.h"
#include "common.h"

using namespace boost::interprocess;
using namespace v8;
using node::Buffer;

class SharedMemory : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit SharedMemory(v8::Local<String> name,
                          v8::Local<Integer> open_mode,
                          v8::Local<Integer> operation_mode);
    ~SharedMemory();
    void truncate(v8::Local<Number> len);
    std::size_t size() const;
    void recycle();
    MemorySlice *slice(offset_t ofst, size_t si,
                       common::ACCESS_MODE mo=common::READ);

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> Truncate(const v8::Arguments& args);
    static v8::Handle<v8::Value> Whole(const v8::Arguments& args);
    static v8::Handle<v8::Value> Recycle(const v8::Arguments& args);
    static v8::Handle<v8::Value> Slice(const v8::Arguments &args);
    static v8::Handle<v8::Value> Size(const v8::Arguments &args);
    static v8::Persistent<v8::Function> constructor;
    shared_memory_object * shm_;
    friend class MemorySlice;
};

#endif
