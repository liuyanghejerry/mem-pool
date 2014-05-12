#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <node.h>
#include <node_buffer.h>
#include <boost/interprocess/shared_memory_object.hpp>
//#include <boost/interprocess/mapped_region.hpp>
#include <iostream>

using namespace boost::interprocess;
using namespace v8;
using node::Buffer;

class MemorySlice {
public:
    MemorySlice(shared_memory_object& shm,
                std::size_t offset,
                std::size_t length);
    ~MemorySlice();
    void * getAddress() const;
    std::size_t size() const;
    v8::Local<v8::Object> buffer();
private:
    mapped_region* region_;
    Buffer* toBuffer();
    v8::Local<v8::Object> toJsBuffer(Buffer * slowBuffer);
};

class SharedMemory : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);

private:
    explicit SharedMemory(Local<String> name, Local<Boolean> no_create);
    ~SharedMemory();
    void truncate(Local<Number> len);
    std::size_t size() const;
    void recycle();

    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static Handle<v8::Value> Truncate(const v8::Arguments& args);
    static v8::Handle<v8::Value> Whole(const v8::Arguments& args);
    static v8::Handle<v8::Value> Recycle(const v8::Arguments& args);
    static v8::Persistent<v8::Function> constructor;
    shared_memory_object * shm_;
};

#endif
