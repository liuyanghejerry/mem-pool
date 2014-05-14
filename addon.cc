#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "SharedMemory.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
    MemorySlice::Init(exports);
    SharedMemory::Init(exports);
}

NODE_MODULE(addon, InitAll)
