#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_buffer.h>
#include <boost/interprocess/mapped_region.hpp>
#include "SharedMemory.h"

using namespace v8;
using node::Buffer;

Persistent<Function> SharedMemory::constructor;

SharedMemory::SharedMemory(Local<String> name, Local<Boolean> no_create) :
    shm_(NULL)
{
    if(no_create->Value()) {
        shm_ = new shared_memory_object(open_only,
                                        *(String::Utf8Value(name)),
                                        read_write);
    } else {
//        shared_memory_object::remove(*(String::Utf8Value(name)));
        shm_ = new shared_memory_object(open_or_create,
                                        *(String::Utf8Value(name)),
                                        read_write);
    }

}

SharedMemory::~SharedMemory()
{
    delete shm_;
}

void SharedMemory::truncate(Local<Number> len)
{
    shm_->truncate(len->NumberValue());
}

std::size_t SharedMemory::size() const
{
    offset_t a = 0;
    shm_->get_size(a);
    return a;
}

void SharedMemory::recycle()
{
    shared_memory_object::remove(shm_->get_name());
}

void SharedMemory::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("SharedMemory"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("truncate"),
                                  FunctionTemplate::New(Truncate)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("whole"),
                                  FunctionTemplate::New(Whole)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("recycle"),
                                  FunctionTemplate::New(Recycle)->GetFunction());
    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("SharedMemory"), constructor);
}

Handle<Value> SharedMemory::New(const Arguments& args) {
    HandleScope scope;

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new SharedMemory(...)`
        Local<String> name = args[0]->IsUndefined() ? String::New("shared_memory") : args[0]->ToString();
        Local<Boolean> open_only = args[1]->IsUndefined() ? Local<Boolean>::New(True()) :  Local<Boolean>::New(args[1]->ToBoolean());
        SharedMemory* obj = new SharedMemory(name, open_only);
        obj->Wrap(args.This());
        return args.This();
    } else {
        // Invoked as plain function `SharedMemory(...)`, turn into construct call.
        const int argc = 1;
        Local<Value> argv[argc] = { args[0] };
        return scope.Close(constructor->NewInstance(argc, argv));
    }
}

Handle<Value> SharedMemory::Truncate(const Arguments &args)
{
    HandleScope scope;
    Local<Number> len = args[0]->IsUndefined() ? Number::New(0) : args[0]->ToNumber();
    SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(args.This());
    obj->truncate(len);
    return scope.Close(Number::New(obj->size()));
}

Handle<Value> SharedMemory::Whole(const Arguments &args)
{
    HandleScope scope;
    SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(args.This());
    MemorySlice* slice = new MemorySlice(*(obj->shm_), 0, obj->size());
    return slice->buffer();
}

Handle<Value> SharedMemory::Recycle(const Arguments &args)
{
    HandleScope scope;
    SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(args.This());
    obj->recycle();
    return scope.Close(Undefined());
}


MemorySlice::MemorySlice(shared_memory_object &shm,
                         std::size_t offset,
                         std::size_t length):
    region_(new mapped_region(shm,
                              read_write,
                              offset,
                              length))
{
}

MemorySlice::~MemorySlice()
{
    delete region_;
}

void *MemorySlice::getAddress() const
{
    return region_->get_address();
}

std::size_t MemorySlice::size() const
{
    return region_->get_size();
}

Local<Object> MemorySlice::buffer()
{
    return toJsBuffer(toBuffer());
}

void dummy_free_callback(char *, void *)
{
    return;
}

Buffer *MemorySlice::toBuffer()
{
    return Buffer::New((char*)getAddress(), size(), dummy_free_callback, NULL);
}

Local<Object> MemorySlice::toJsBuffer(Buffer *slowBuffer)
{
    HandleScope scope;
    Local<Object> globalObj = Context::GetCurrent()->Global();
    Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));

    Handle<Value> constructorArgs[3] = { slowBuffer->handle_, Integer::New(this->size()), Integer::New(0) };
    Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);
    return actualBuffer;
}
