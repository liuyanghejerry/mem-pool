#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_buffer.h>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include "MemorySlice.h"
#include "SharedMemory.h"

using namespace v8;
using node::Buffer;

Persistent<Function> MemorySlice::constructor;

void MemorySlice::Init(v8::Handle<v8::Object> exports)
{
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("MemorySlice"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("buffer"),
                                  FunctionTemplate::New(GetBuffer)->GetFunction());

    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("MemorySlice"), constructor);
}

Handle<Value> MemorySlice::New(const Arguments& args) {
  HandleScope scope;

  if (args.IsConstructCall()) {
    // Invoked as constructor: `new MyObject(...)`
    Handle<Object> shared_memory_js = args[0]->ToObject();
    SharedMemory* shared_memory = ObjectWrap::Unwrap<SharedMemory>(shared_memory_js);
    if(!shared_memory){
        throw;
    }

    offset_t offset_arg = args[1]->IsUndefined() ? 0 :  args[0]->ToNumber()->ToInteger()->Value();
    size_t size_arg = args[2]->IsUndefined() ? shared_memory->size() :  args[1]->ToNumber()->ToInteger()->Value();
    common::ACCESS_MODE mode_arg = args[3]->IsUndefined() ? common::READ :  common::ACCESS_MODE(args[3]->ToNumber()->ToInteger()->Value());

    MemorySlice* obj = shared_memory->slice(offset_arg, size_arg, mode_arg);

    obj->Wrap(args.This());
    return args.This();
  } else {
    // Invoked as plain function `MyObject(...)`, turn into construct call.
    const int argc = 4;
    Local<Value> argv[argc] = { args[0], args[1], args[2], args[3] };
    return scope.Close(constructor->NewInstance(argc, argv));
  }
}

MemorySlice::MemorySlice(shared_memory_object &shm,
                         std::size_t offset,
                         std::size_t length,
                         common::ACCESS_MODE mode):
    region_(new mapped_region(shm,
                              common::toAccessMode(mode),
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

static void dummy_free_callback(char *, void *)
{
    return;
}

Handle<Value> MemorySlice::GetBuffer(const Arguments &args)
{
    HandleScope scope;
    MemorySlice* obj = ObjectWrap::Unwrap<MemorySlice>(args.This());
    return scope.Close(obj->buffer());
}

Local<Object> MemorySlice::buffer()
{
    Buffer* buf = Buffer::New((char*)getAddress(), size(), dummy_free_callback, NULL);
    return toJsBuffer(buf);
}

Local<v8::Object> MemorySlice::object(v8::Local<v8::Object> handle)
{
    // FIXME: result in empty Object
    this->Wrap(handle);
    return handle;
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
