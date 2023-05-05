#include "mimalloc/include/mimalloc.h"
#include <exception>
#include <new>

//void* operator new(std::size_t sz) noexcept(false)
//{
//    if (void* ptr = mi_malloc(sz))
//        return ptr;
//
//    throw std::bad_alloc{};
//}
//
//void operator delete(void* ptr) throw()
//{
//    mi_free(ptr);
//}
//
//void* operator new[](std::size_t sz) noexcept(false)
//{
//    if (void* ptr = mi_malloc(sz))
//        return ptr;
//
//    throw std::bad_alloc{};
//}
//
//void operator delete[](void* ptr) throw()
//{
//    mi_free(ptr);
//}