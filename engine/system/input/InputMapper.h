#pragma once

#include "KeycodeHelper.hpp"

#include <utility/ufunction.hpp>

#include <map>
#include <vector>
#include <string>

namespace engine
{
    namespace system
    {
        namespace input
        {
            struct FInputAction
            {
                EKeyState eState;
                std::vector<utl::function<void(EKeyCode, EKeyState)>> vListeners;
            };

            struct FInputAxis
            {
                std::vector<utl::function<void(float, float)>> vListeners;
            };

            class CInputMapper
            {
            public:
                CInputMapper();
                ~CInputMapper();

                /**
                 * @brief Creates binding with KeyCode and action name. After creating action, you can bind your function to this name.
                 *
                 * @tparam Args Variadic arguments. In this case should contain EActionKey type
                 * @param srActionName New action name
                 * @param args Action keys
                 */
                template<class ..._Args>
                void createAction(const std::string& srActionName, _Args &&...args)
                {
                    std::array<EKeyCode, sizeof...(_Args)> aKeys = { { args... } };
                    for (auto key : aKeys)
                        mInputDescriptor.emplace(std::make_pair(key, srActionName));
                }

                template<class ..._Args>
                void bindAction(const std::string& srActionName, EKeyState eState, _Args &&...args)
                {
                    auto it = mInputActions.find(srActionName);
                    if (it != mInputActions.end())
                    {
                        auto range = mInputActions.equal_range(srActionName);
                        for (auto range_it = range.first; range_it != range.second; ++range_it)
                        {
                            if (range_it->second.eState == eState)
                            {
                                range_it->second.vListeners.emplace_back(utl::function<void(EKeyCode, EKeyState)>(std::forward<_Args>(args)...));
                                return;
                            }
                        }
                    }
                    mInputActions.emplace(srActionName, makeBindAction(eState, utl::function<void(EKeyCode, EKeyState)>(std::forward<_Args>(args)...)));
                }

                template<class ..._Args>
                void bindAxis(const std::string& srAxisName, _Args &&...args)
                {
                    auto it = mInputAxis.find(srAxisName);
                    if (it != mInputAxis.end())
                    {
                        auto range = mInputAxis.equal_range(srAxisName);
                        for (auto range_it = range.first; range_it != range.second; ++range_it)
                        {
                            range_it->second.vListeners.emplace_back(utl::function<void(float, float)>(std::forward<_Args>(args)...));
                        }
                    }
                    mInputAxis.emplace(srAxisName, makeBindAxis(utl::function<void(float, float)>(std::forward<_Args>(args)...)));
                }

                void update(float fDeltaTime);
            private:
                void keyInput(EKeyCode code, EKeyState state);
                void cursorInput(float xpos, float ypos);
                void cursorMove(float xpos, float ypos);

                void handleActions(const std::string& srActionName, EKeyCode eKey, const EKeyState& eKeyState);
                void handleAxis(const std::string& srAxisName, const glm::vec2& fValue);

                FInputAction makeBindAction(EKeyState eState, utl::function<void(EKeyCode, EKeyState)>&& dCallback);
                FInputAxis makeBindAxis(utl::function<void(float, float)>&& dCallback);

            private:
                std::multimap<EKeyCode, std::string> mInputDescriptor;

                std::multimap<std::string, FInputAction> mInputActions;
                std::multimap<std::string, FInputAxis> mInputAxis;

                std::map<EKeyCode, EKeyState> mKeyStates;
                std::map<EKeyCode, glm::vec2> mAxisStates;

                glm::vec2 fPosOld{ 0.f, 0.f };
                float fDeltaTime{ 0 };
            };
        }
    }
}