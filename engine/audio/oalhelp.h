#pragma once

#include "AL/al.h"
#include "AL/alc.h"

#include <utility/logger/logger.h>

#include <string>

namespace al
{
    static bool CheckError()
    {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR)
        {
            switch (error)
            {
            case AL_INVALID_NAME:
                log_error("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
                break;
            case AL_INVALID_ENUM:
                log_error("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
                break;
            case AL_INVALID_VALUE:
                log_error("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
                break;
            case AL_INVALID_OPERATION:
                log_error("AL_INVALID_OPERATION: the requested operation is not valid");
                break;
            case AL_OUT_OF_MEMORY:
                log_error("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
                break;
            default:
                log_error("UNKNOWN AL ERROR: {}", error);
            }
            return false;
        }
        return true;
    }

    template<class _Func, class... _Args>
    static auto alCallImpl(_Func&& functor, _Args&& ...args)
        -> typename std::enable_if_t<!std::is_same_v<void, decltype(functor(args...))>, decltype(functor(args...))>
    {
        auto ret = functor(std::forward<_Args>(args)...);
        CheckError();
        return ret;
    }

    template<class _Func, class... _Args>
    static auto CallImpl(_Func&& functor, _Args&& ...args)
        -> typename std::enable_if_t<std::is_same_v<void, decltype(functor(args...))>, bool>
    {
        functor(std::forward<_Args>(args)...);
        return CheckError();
    }
}

#define alCall(functor, ...) al::CallImpl(functor, __VA_ARGS__)


namespace alc
{
    static bool CheckError(ALCdevice* device)
    {
        ALCenum error = alcGetError(device);
        if (error != ALC_NO_ERROR)
        {
            switch (error)
            {
            case ALC_INVALID_VALUE:
                log_error("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function");
                break;
            case ALC_INVALID_DEVICE:
                log_error("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function");
                break;
            case ALC_INVALID_CONTEXT:
                log_error("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function");
                break;
            case ALC_INVALID_ENUM:
                log_error("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function");
                break;
            case ALC_OUT_OF_MEMORY:
                log_error("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function");
                break;
            default:
                log_error("UNKNOWN ALC ERROR:{} ", error);
            }
            __debugbreak();
            return false;
        }
        return true;
    }

    template<class _Func, class... _Args>
    static auto CallImpl(_Func&& functor, ALCdevice* device, _Args&& ...args)
        -> typename std::enable_if_t<std::is_same_v<void, decltype(functor(args...))>, bool>
    {
        functor(std::forward<_Args>(args)...);
        return CheckError(device);
    }

    template<class _Func, class _ReturnType, class... _Args>
    static auto CallImpl(_Func&& functor, _ReturnType& returnValue, ALCdevice* device, _Args&& ...args)
        -> typename std::enable_if_t<!std::is_same_v<void, decltype(functor(args...))>, bool>
    {
        returnValue = functor(std::forward<_Args>(args)...);
        return CheckError(device);
    }
}

#define alcCall(functor, device, ...) alc::CallImpl(functor, device, __VA_ARGS__)