#include "umemory.hpp"

using namespace utl;

std::map<uintptr_t, FAllocInfo> FAllocationInfoHolder::_allocInfos{};
size_t FAllocationInfoHolder::_allocationId{0};