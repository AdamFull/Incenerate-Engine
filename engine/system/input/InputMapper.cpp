#include "InputMapper.h"

#include "window/WindowCallback.h"
#include "window/WindowHandle.h"

using namespace engine::system::window;
using namespace engine::system::input;


template<class _Ty>
_Ty rangeToRange(_Ty input, _Ty in_min, _Ty in_max, _Ty out_min, _Ty out_max)
{
    return (input - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

CInputMapper::CInputMapper()
{
    FWindowCallback::SubscribeKeyStateChange(this, &CInputMapper::keyInput);
    FWindowCallback::SubscribeCursorInput(this, &CInputMapper::cursorInput);
    FWindowCallback::SubscribeCursorMove(this, &CInputMapper::cursorMove);
}

CInputMapper::~CInputMapper()
{

}

void CInputMapper::update(float fDeltaTime)
{
    this->fDeltaTime = fDeltaTime;

    for (auto &[key, value] : mInputDescriptor)
    {
        auto keyState = mKeyStates.find(key);
        if (keyState != mKeyStates.end())
        {
            // If we have multiple bindings to this key
            auto range = mInputDescriptor.equal_range(key);
            for (auto range_it = range.first; range_it != range.second; ++range_it)
                handleActions(range_it->second, key, keyState->second);

            if (keyState->second == EKeyState::ePress)
                keyState->second = EKeyState::ePressed;
            else if (keyState->second == EKeyState::eRelease)
                keyState->second = EKeyState::eReleased;
        }

        auto keyAxis = mAxisStates.find(key);
        if (keyAxis != mAxisStates.end())
        {
            // If we have multiple bindings to this key
            auto range = mInputDescriptor.equal_range(key);
            for (auto range_it = range.first; range_it != range.second; ++range_it)
                handleAxis(range_it->second, keyAxis->second);

            if (key != EKeyCode::eCursorOriginal)
                mAxisStates[key] = {};
        }
    }
}

void CInputMapper::keyInput(EKeyCode code, EKeyState state)
{
    mKeyStates[code] = state;
}

void CInputMapper::cursorInput(float xpos, float ypos)
{
    
}

void CInputMapper::cursorMove(float xpos, float ypos)
{
    float xmax = static_cast<float>(CWindowHandle::iWidth);
    float ymax = static_cast<float>(CWindowHandle::iHeight);

    mAxisStates[EKeyCode::eCursorOriginal] = { xpos, ypos };

    fPosOld = mAxisStates[EKeyCode::eCursorPosition];
    mAxisStates[EKeyCode::eCursorPosition] =
    {
        rangeToRange<float>(xpos, 0.0, xmax, -1.0, 1.0),
        rangeToRange<float>(ypos, 0.0, ymax, -1.0, 1.0)
    };

    mAxisStates[EKeyCode::eCursorDelta] = (mAxisStates[EKeyCode::eCursorPosition] - fPosOld) * fDeltaTime;
}

void CInputMapper::handleActions(const std::string& srActionName, EKeyCode eKey, const EKeyState& eKeyState)
{
    auto it = mInputActions.find(srActionName);
    if (it != mInputActions.end())
    {
        auto range = mInputActions.equal_range(srActionName);
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            if (range_it->second.eState == eKeyState)
            {
                auto &vListeners = range_it->second.vListeners;
                for (auto &listener : vListeners)
                    listener(eKey, eKeyState);
            }
        }
    }
}

void CInputMapper::handleAxis(const std::string& srAxisName, const glm::vec2& fValue)
{
    auto it = mInputAxis.find(srAxisName);
    if (it != mInputAxis.end())
    {
        auto range = mInputAxis.equal_range(srAxisName);
        for (auto range_it = range.first; range_it != range.second; ++range_it)
        {
            auto &vListeners = range_it->second.vListeners;
            for (auto &listener : vListeners)
                listener(fValue.x, fValue.y);
        }
    }
}

FInputAction CInputMapper::makeBindAction(EKeyState eState, utl::function<void(EKeyCode, EKeyState)>&& dCallback)
{
    FInputAction newAction;
    newAction.eState = eState;
    newAction.vListeners.emplace_back(std::move(dCallback));
    return newAction;
}

FInputAxis CInputMapper::makeBindAxis(utl::function<void(float, float)>&& dCallback)
{
    FInputAxis newAxis;
    newAxis.vListeners.emplace_back(std::move(dCallback));
    return newAxis;
}
