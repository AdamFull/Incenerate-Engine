#pragma once

#include "udetail.hpp"

#include <memory>
#include <source_location>
#include <map>
#include <iostream>
#include <format>
#include <iomanip>
#include <sstream>

namespace utl
{
    struct FAllocInfo
    {
        std::source_location _file;
        size_t _size{0ull};
        size_t _allocId{0ull};
        std::string_view _typename;
        void* _data{nullptr};
    };

    struct FAllocationInfoHolder
    {
        static std::map<uintptr_t, FAllocInfo> _allocInfos;
        static size_t _allocationId;
    };

    template<class _Ty>
    static inline std::pair<uintptr_t, FAllocInfo> make_allocation_info(_Ty* _ptr, const size_t _size, std::source_location&& location)
    {
        FAllocInfo info{};
        info._allocId = FAllocationInfoHolder::_allocationId;
        info._file = std::move(location);
        info._data = _ptr;
        info._typename = type_name<_Ty>();
        info._size = _size * sizeof(_Ty); 

        FAllocationInfoHolder::_allocationId++;

        return std::make_pair(reinterpret_cast<uintptr_t>(_ptr), std::move(info));
    }

    template<class _Ty>
    static void log_allocation(_Ty* _ptr, size_t _size, std::source_location&& location)
    {
#ifdef _DEBUG
        FAllocationInfoHolder::_allocInfos.emplace(make_allocation_info<_Ty>(_ptr, _size, std::move(location)));
#endif
    }

    template<class _Ty>
    static void log_deallocation(_Ty* _ptr)
    {
#ifdef _DEBUG
        auto it = FAllocationInfoHolder::_allocInfos.find(reinterpret_cast<uintptr_t>(_ptr));
        if (it != FAllocationInfoHolder::_allocInfos.end()) 
            FAllocationInfoHolder::_allocInfos.erase(it);
#endif
    }

    template<class _Ty, std::enable_if_t<std::is_array_v<_Ty> && std::extent_v<_Ty> == 0, int> = 0>
    static auto* allocate(const size_t _size, std::source_location&& location = std::source_location::current())
    {
        using _Elem = std::remove_extent_t<_Ty>;

        auto _ptr = new _Elem[_size]();
        log_allocation(_ptr, _size, std::move(location));
        return _ptr;
    }

    template<class _Ty, class... _Args, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
    static _Ty* allocate(std::tuple<_Args...>&& _args, std::source_location&& location = std::source_location::current())
    {
        auto _ptr = std::apply([](auto&&... args) { return new _Ty(args...); }, std::move(_args));
        log_allocation(_ptr, 1, std::move(location));
        return _ptr;
    }

    template<class _Ty, class... _Args, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
    static _Ty* allocate(std::tuple<_Args&&...>&& _args, std::source_location&& location = std::source_location::current())
    {
        auto _ptr = std::apply([](auto&&... args) { return new _Ty(args...); }, std::move(_args));
        log_allocation(_ptr, 1, std::move(location));
        return _ptr;
    }

    template<class _Ty, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
    _Ty* allocate(std::source_location&& location = std::source_location::current())
    {
        auto _ptr = new _Ty();
        log_allocation(_ptr, 1, std::move(location));
        return _ptr;
    }

    template<class _Ty, std::enable_if_t<std::is_array_v<_Ty> && std::extent_v<_Ty> == 0, int> = 0>
    static void deallocate(_Ty* _ptr)
    {
        log_deallocation(_ptr);

        if (_ptr)
        {
            delete[] _ptr;
            _ptr = nullptr;
        }
    }

    template<class _Ty, std::enable_if_t<!std::is_array_v<_Ty>, int> = 0>
    static void deallocate(_Ty* _ptr)
    {
        log_deallocation(_ptr);

        if (_ptr)
        {
            delete _ptr;
            _ptr = nullptr;
        }
    }

    static void collect_garbage()
    {
#ifdef _DEBUG
        if(!FAllocationInfoHolder::_allocInfos.empty())
        {
            size_t leakedSize{0};
            std::cout << "Detected memory leaks!" << std::endl;

            for(auto& [address, info] : FAllocationInfoHolder::_allocInfos)
            {
                std::stringstream ss{};

                for(size_t i = 0; i < (info._size < 16 ? info._size : 16); i++)
                {
                    auto sym = *(static_cast<char*>(info._data) + i);
                    ss << std::setfill('0') << std::setw(2) << std::hex << (uint32_t)sym << " ";
                }

                leakedSize += info._size;

                std::cout << std::format(
                    "----------------------------------------------------------\n"
                    "{}:({}, {}) in function \"{}\" \n"
                    "allocation: {}, type \"{}\" leak at {:#x}, {} bytes long. \n"
                    "Data: <{}> {}\n", 

                info._file.file_name(), info._file.line(), info._file.column(), info._file.function_name(), 
                info._allocId, info._typename, address, info._size, static_cast<const char*>(info._data), ss.str());

                delete info._data;
            }

            std::cout << std::format("Total leaked: {} bytes. \nGarbage cleared.\n", leakedSize);
        }
#endif
    }

    template<class _Ty>
    class allocator : public std::allocator<_Ty>
    {
    public:
        constexpr allocator() noexcept {}

        constexpr allocator(const allocator&) noexcept = default;

        template <class _Other>
        constexpr allocator(const allocator<_Other>&) noexcept {}

        ~allocator() = default;
        allocator& operator=(const allocator&) = default;

        constexpr void deallocate(_Ty* const _Ptr, const size_t _Count) 
        {
            auto it = FAllocationInfoHolder::_allocInfos.find(reinterpret_cast<uintptr_t>(_Ptr));
            if(it != FAllocationInfoHolder::_allocInfos.end())
                FAllocationInfoHolder::_allocInfos.erase(it);

            std::allocator<_Ty>::deallocate(_Ptr, _Count);
        }

        constexpr _Ty* allocate(const size_t _Count) 
        {
            auto _ptr = std::allocator<_Ty>::allocate(_Count);
            FAllocationInfoHolder::_allocInfos.emplace(make_allocation_info<_Ty>(_ptr, 1, std::move(std::source_location::current())));
            return _ptr;
        }
    };

    template<class _Ty>
    using scope_ptr = std::unique_ptr<_Ty>;

    template<class _Ty, class... _Args>
    scope_ptr<_Ty> make_scope(_Args&& ...args)
    {
        //return allocate_unique<_Ty, _Allocator>(_Allocator(), std::forward<_Args>(args)...);
        return std::make_unique<_Ty>(std::forward<_Args>(args)...);
    }

    template<class _Ty>
    using ref_ptr = std::shared_ptr<_Ty>;

    template<class _Ty>
    using weak_ptr = std::weak_ptr<_Ty>;

    template<class _Ty, class... _Args, class _Allocator = std::allocator<_Ty>>
    constexpr ref_ptr<_Ty> make_ref(_Args&& ...args)
    {
        return std::allocate_shared<_Ty>(_Allocator(), std::forward<_Args>(args)...);
    }
}