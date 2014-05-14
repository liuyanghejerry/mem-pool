#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_buffer.h>
#include "SharedMemory.h"
#include "common.h"

using namespace v8;
using node::Buffer;

Persistent<Function> SharedMemory::constructor;

SharedMemory::SharedMemory(Local<String> name,
                           Local<Integer> open_mode,
                           Local<Integer> access_mode) :
    shm_(NULL)
{
    common::OPEN_MODE m = (common::OPEN_MODE)open_mode->Value();
    common::ACCESS_MODE m2 = (common::ACCESS_MODE)access_mode->Value();
    boost::interprocess::mode_t a_m = common::toAccessMode(m2);

    switch(m){
    default: // fall-through
    case common::OPEN_ONLY:
        shm_ = new shared_memory_object(open_only,
                                        *(String::Utf8Value(name)),
                                        a_m);
        break;
    case common::CREATE_ONLY:
        shm_ = new shared_memory_object(create_only,
                                        *(String::Utf8Value(name)),
                                        a_m);
        break;
    case common::OPEN_OR_CREATE:
        shm_ = new shared_memory_object(open_or_create,
                                        *(String::Utf8Value(name)),
                                        a_m);
        break;
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

MemorySlice* SharedMemory::slice(offset_t ofst, size_t si, common::ACCESS_MODE mo)
{
    return new MemorySlice(*shm_, ofst, si, mo);
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
    tpl->PrototypeTemplate()->Set(String::NewSymbol("slice"),
                                  FunctionTemplate::New(Slice)->GetFunction());
    tpl->PrototypeTemplate()->Set(String::NewSymbol("size"),
                                  FunctionTemplate::New(Size)->GetFunction());

    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("SharedMemory"), constructor);
}

Handle<Value> SharedMemory::New(const Arguments& args) {
    HandleScope scope;

    if (args.IsConstructCall()) {
        // Invoked as constructor: `new SharedMemory(...)`
        Local<String> name = args[0]->IsUndefined() ? String::New("shared_memory") : args[0]->ToString();
        Local<Integer> open_mode = args[1]->IsUndefined() ? Integer::New(0) :  Local<Integer>::New(args[1]->ToNumber()->ToInteger());
        Local<Integer> access_mode = args[2]->IsUndefined() ? Integer::New(0) :  Local<Integer>::New(args[2]->ToNumber()->ToInteger());

        SharedMemory* obj = new SharedMemory(name, open_mode, access_mode);

        obj->Wrap(args.This());
        return args.This();
    } else {
        // Invoked as plain function `SharedMemory(...)`, turn into construct call.
        const int argc = 3;
        Local<Value> argv[argc] = { args[0], args[1], args[2] };
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
    // FIXME: leaks
    MemorySlice* slice = obj->slice(0, obj->size(), common::READ_WRITE);
    return slice->buffer();
}

Handle<Value> SharedMemory::Recycle(const Arguments &args)
{
    HandleScope scope;
    SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(args.This());
    obj->recycle();
    return scope.Close(Undefined());
}

Handle<Value> SharedMemory::Slice(const Arguments &args)
{
    HandleScope scope;

    Handle<Value> constructorArgs[4] = { args.This(),
                                         args[0],
                                         args[1],
                                         args[2] };
    return scope.Close(MemorySlice::NewInstance(4, constructorArgs));
}

v8::Handle<v8::Value> SharedMemory::Size(const v8::Arguments &args)
{
    HandleScope scope;

    SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(args.This());
    return scope.Close(Number::New(obj->size()));
}
