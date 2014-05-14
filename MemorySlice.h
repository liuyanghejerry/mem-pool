#ifndef MEMORY_SLICE_H
#define MEMORY_SLICE_H

#include <node.h>
#include <node_buffer.h>
#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/mapped_region.hpp>
#include <iostream>
#include "common.h"

using namespace boost::interprocess;
using namespace v8;
using node::Buffer;

class MemorySlice: public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);
    static v8::Handle<v8::Value> NewInstance(const v8::Arguments& args);
    static v8::Handle<v8::Value> NewInstance(int constructorArgc,
                                             v8::Handle<v8::Value>* constructorArgv);
    MemorySlice(shared_memory_object& shm,
                std::size_t offset,
                std::size_t length,
                common::ACCESS_MODE mode);
    ~MemorySlice();
    void * getAddress() const;
    std::size_t size() const;
    v8::Local<v8::Object> buffer();
private:
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetBuffer(const v8::Arguments &args);
    static v8::Handle<v8::Value> Size(const v8::Arguments &args);
    v8::Local<v8::Object> toJsBuffer(Buffer * slowBuffer);
    static v8::Persistent<v8::Function> constructor;
    Buffer* toBuffer();
    mapped_region* region_;


};

#endif
