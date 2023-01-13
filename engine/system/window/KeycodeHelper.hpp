#pragma once

#include <SDL.h>

#include <bitset>

namespace engine
{
    namespace system
    {
        namespace window
        {
            enum class EKeyState
            {
                ePress = SDL_PRESSED,
                eRelease = SDL_RELEASED,
                ePressed,
                eReleased
            };

            enum class EKeyCode : size_t
            {
                eUnknown,

                //Keyboard input
                eKeyA,
                eKeyB,
                eKeyC,
                eKeyD,
                eKeyE,
                eKeyF,
                eKeyG,
                eKeyH,
                eKeyI,
                eKeyJ,
                eKeyK,
                eKeyL,
                eKeyM,
                eKeyN,
                eKeyO,
                eKeyP,
                eKeyQ,
                eKeyR,
                eKeyS,
                eKeyT,
                eKeyU,
                eKeyV,
                eKeyW,
                eKeyX,
                eKeyY,
                eKeyZ,

                eKey1,
                eKey2,
                eKey3,
                eKey4,
                eKey5,
                eKey6,
                eKey7,
                eKey8,
                eKey9,
                eKey0,

                eKyeReturn,
                eKyeEscape,
                eKyeBackspace,
                eKyeTab,
                eKeySpace,

                eKeyMinus,
                eKeyEquals,
                eKeyLeftBracket,
                eKeyRightBracket,
                eKeyBackslash,
                eKeyNonuslash,
                eKeySemicolon,
                eKeyApostrophe,
                eKeyGrave,
                eKeyComma,
                eKeyPeriod,
                eKeySlash,
                eKeyCapslock,

                eKeyF1,
                eKeyF2,
                eKeyF3,
                eKeyF4,
                eKeyF5,
                eKeyF6,
                eKeyF7,
                eKeyF8,
                eKeyF9,
                eKeyF10,
                eKeyF11,
                eKeyF12,

                eKeyPrintscreen,
                eKeyScrollLock,
                eKeyPause,
                eKeyInsert,

                eKeyHome,
                eKeyPageUp,
                eKeyDelete,
                eKeyEnd,
                eKeyPageDown,
                eKeyRight,
                eKeyLeft,
                eKeyDown,
                eKeyUp,

                //Mouse/touch input
                eMouseLeft,
                eMouseMiddle,
                eMouseRight,
                eMouseX1,
                eMouseX2,

                //Game controller
                eButtonA,
                eButtonB,
                eButtonX,
                eButtonY,
                eButtonBack,
                eButtonGuide,
                eButtonStart,
                eButtonLeftStick,
                eButtonRightStick,
                eButtonLeftShoulder,
                eButtonRightShoulder,
                eButtonDPADUp,
                eButtonDPADDown,
                eButtonDPADLeft,
                eButtonDPADRight,
                eButtonMisc1,
                eButtonPaddle1,
                eButtonPaddle2,
                eButtonPaddle3,
                eButtonPaddle4,
                eButtonTouchpad,

                eCount
            };

            static EKeyCode keyboardCodeToEngineCode(SDL_Scancode code)
            {
                switch (code)
                {
                    case SDL_SCANCODE_A: return EKeyCode::eKeyA;
                    case SDL_SCANCODE_B: return EKeyCode::eKeyB;
                    case SDL_SCANCODE_C: return EKeyCode::eKeyC;
                    case SDL_SCANCODE_D: return EKeyCode::eKeyD;
                    case SDL_SCANCODE_E: return EKeyCode::eKeyE;
                    case SDL_SCANCODE_F: return EKeyCode::eKeyF;
                    case SDL_SCANCODE_G: return EKeyCode::eKeyG;
                    case SDL_SCANCODE_H: return EKeyCode::eKeyH;
                    case SDL_SCANCODE_I: return EKeyCode::eKeyI;
                    case SDL_SCANCODE_J: return EKeyCode::eKeyJ;
                    case SDL_SCANCODE_K: return EKeyCode::eKeyK;
                    case SDL_SCANCODE_L: return EKeyCode::eKeyL;
                    case SDL_SCANCODE_M: return EKeyCode::eKeyM;
                    case SDL_SCANCODE_N: return EKeyCode::eKeyN;
                    case SDL_SCANCODE_O: return EKeyCode::eKeyO;
                    case SDL_SCANCODE_P: return EKeyCode::eKeyP;
                    case SDL_SCANCODE_Q: return EKeyCode::eKeyQ;
                    case SDL_SCANCODE_R: return EKeyCode::eKeyR;
                    case SDL_SCANCODE_S: return EKeyCode::eKeyS;
                    case SDL_SCANCODE_T: return EKeyCode::eKeyT;
                    case SDL_SCANCODE_U: return EKeyCode::eKeyU;
                    case SDL_SCANCODE_V: return EKeyCode::eKeyV;
                    case SDL_SCANCODE_W: return EKeyCode::eKeyW;
                    case SDL_SCANCODE_X: return EKeyCode::eKeyX;
                    case SDL_SCANCODE_Y: return EKeyCode::eKeyY;
                    case SDL_SCANCODE_Z: return EKeyCode::eKeyZ;

                    case SDL_SCANCODE_1: return EKeyCode::eKey1;
                    case SDL_SCANCODE_2: return EKeyCode::eKey2;
                    case SDL_SCANCODE_3: return EKeyCode::eKey3;
                    case SDL_SCANCODE_4: return EKeyCode::eKey4;
                    case SDL_SCANCODE_5: return EKeyCode::eKey5;
                    case SDL_SCANCODE_6: return EKeyCode::eKey6;
                    case SDL_SCANCODE_7: return EKeyCode::eKey7;
                    case SDL_SCANCODE_8: return EKeyCode::eKey8;
                    case SDL_SCANCODE_9: return EKeyCode::eKey9;
                    case SDL_SCANCODE_0: return EKeyCode::eKey0;

                    case SDL_SCANCODE_RETURN: return EKeyCode::eKyeReturn;
                    case SDL_SCANCODE_ESCAPE: return EKeyCode::eKyeEscape;
                    case SDL_SCANCODE_BACKSPACE: return EKeyCode::eKyeBackspace;
                    case SDL_SCANCODE_TAB: return EKeyCode::eKyeTab;
                    case SDL_SCANCODE_SPACE: return EKeyCode::eKeySpace;

                    case SDL_SCANCODE_MINUS: return EKeyCode::eKeyMinus;
                    case SDL_SCANCODE_EQUALS: return EKeyCode::eKeyEquals;
                    case SDL_SCANCODE_LEFTBRACKET: return EKeyCode::eKeyLeftBracket;
                    case SDL_SCANCODE_RIGHTBRACKET: return EKeyCode::eKeyRightBracket;
                    case SDL_SCANCODE_BACKSLASH: return EKeyCode::eKeyBackslash;
                    case SDL_SCANCODE_NONUSHASH: return EKeyCode::eKeyNonuslash;
                    case SDL_SCANCODE_SEMICOLON: return EKeyCode::eKeySemicolon;
                    case SDL_SCANCODE_APOSTROPHE: return EKeyCode::eKeyApostrophe;
                    case SDL_SCANCODE_GRAVE: return EKeyCode::eKeyGrave;
                    case SDL_SCANCODE_COMMA: return EKeyCode::eKeyComma;
                    case SDL_SCANCODE_PERIOD: return EKeyCode::eKeyPeriod;
                    case SDL_SCANCODE_SLASH: return EKeyCode::eKeySlash;
                    case SDL_SCANCODE_CAPSLOCK: return EKeyCode::eKeyCapslock;

                    case SDL_SCANCODE_F1: return EKeyCode::eKeyF1;
                    case SDL_SCANCODE_F2: return EKeyCode::eKeyF2;
                    case SDL_SCANCODE_F3: return EKeyCode::eKeyF3;
                    case SDL_SCANCODE_F4: return EKeyCode::eKeyF4;
                    case SDL_SCANCODE_F5: return EKeyCode::eKeyF5;
                    case SDL_SCANCODE_F6: return EKeyCode::eKeyF6;
                    case SDL_SCANCODE_F7: return EKeyCode::eKeyF7;
                    case SDL_SCANCODE_F8: return EKeyCode::eKeyF8;
                    case SDL_SCANCODE_F9: return EKeyCode::eKeyF9;
                    case SDL_SCANCODE_F10: return EKeyCode::eKeyF10;
                    case SDL_SCANCODE_F11: return EKeyCode::eKeyF11;
                    case SDL_SCANCODE_F12: return EKeyCode::eKeyF12;

                    case SDL_SCANCODE_PRINTSCREEN: return EKeyCode::eKeyPrintscreen;
                    case SDL_SCANCODE_SCROLLLOCK: return EKeyCode::eKeyScrollLock;
                    case SDL_SCANCODE_PAUSE: return EKeyCode::eKeyPause;
                    case SDL_SCANCODE_INSERT: return EKeyCode::eKeyInsert;

                    case SDL_SCANCODE_HOME: return EKeyCode::eKeyHome;
                    case SDL_SCANCODE_PAGEUP: return EKeyCode::eKeyPageUp;
                    case SDL_SCANCODE_DELETE: return EKeyCode::eKeyDelete;
                    case SDL_SCANCODE_END: return EKeyCode::eKeyEnd;
                    case SDL_SCANCODE_PAGEDOWN: return EKeyCode::eKeyPageDown;
                    case SDL_SCANCODE_RIGHT: return EKeyCode::eKeyRight;
                    case SDL_SCANCODE_LEFT: return EKeyCode::eKeyLeft;
                    case SDL_SCANCODE_DOWN: return EKeyCode::eKeyDown;
                    case SDL_SCANCODE_UP: return EKeyCode::eKeyUp;
                    default: return EKeyCode::eUnknown;
                }
            }

            static EKeyCode gamepadCodeToEngineCode(SDL_GameControllerButton code)
            {
                switch (code)
                {
                case SDL_CONTROLLER_BUTTON_A: return EKeyCode::eButtonA; break;
                case SDL_CONTROLLER_BUTTON_B: return EKeyCode::eButtonB; break;
                case SDL_CONTROLLER_BUTTON_X: return EKeyCode::eButtonX; break;
                case SDL_CONTROLLER_BUTTON_Y: return EKeyCode::eButtonY; break;
                case SDL_CONTROLLER_BUTTON_BACK: return EKeyCode::eButtonBack; break;
                case SDL_CONTROLLER_BUTTON_GUIDE: return EKeyCode::eButtonGuide; break;
                case SDL_CONTROLLER_BUTTON_START: return EKeyCode::eButtonStart; break;
                case SDL_CONTROLLER_BUTTON_LEFTSTICK: return EKeyCode::eButtonLeftStick; break;
                case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return EKeyCode::eButtonRightStick; break;
                case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return EKeyCode::eButtonLeftStick; break;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return EKeyCode::eButtonRightShoulder; break;
                case SDL_CONTROLLER_BUTTON_DPAD_UP: return EKeyCode::eButtonDPADUp; break;
                case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return EKeyCode::eButtonDPADDown; break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return EKeyCode::eButtonDPADLeft; break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return EKeyCode::eButtonDPADRight; break;
                case SDL_CONTROLLER_BUTTON_MISC1: return EKeyCode::eButtonMisc1; break;
                case SDL_CONTROLLER_BUTTON_PADDLE1: return EKeyCode::eButtonPaddle1; break;
                case SDL_CONTROLLER_BUTTON_PADDLE2: return EKeyCode::eButtonPaddle2; break;
                case SDL_CONTROLLER_BUTTON_PADDLE3: return EKeyCode::eButtonPaddle3; break;
                case SDL_CONTROLLER_BUTTON_PADDLE4: return EKeyCode::eButtonPaddle4; break;
                case SDL_CONTROLLER_BUTTON_TOUCHPAD: return EKeyCode::eButtonTouchpad; break;
                }

                return EKeyCode::eUnknown;
            }

            static EKeyCode mouseCodeToEngineCode(int32_t code)
            {
                switch (code)
                {
                case SDL_BUTTON_LEFT: return EKeyCode::eMouseLeft; break;
                case SDL_BUTTON_MIDDLE: return EKeyCode::eMouseMiddle; break;
                case SDL_BUTTON_RIGHT: return EKeyCode::eMouseRight; break;
                case SDL_BUTTON_X1: return EKeyCode::eMouseX1; break;
                case SDL_BUTTON_X2: return EKeyCode::eMouseX2; break;
                }

                return EKeyCode::eUnknown;
            }

            using keystate = std::bitset<static_cast<size_t>(EKeyCode::eCount)>;

            class CKeyDecoder
            {
            public:
                bool update(SDL_KeyboardEvent key)
                {
                    return update(keyboardCodeToEngineCode(key.keysym.scancode), key.state);
                }

                bool update(SDL_ControllerButtonEvent cbutton)
                {
                    return update(gamepadCodeToEngineCode((SDL_GameControllerButton)cbutton.button), cbutton.state);
                }

                bool update(SDL_MouseButtonEvent button)
                {
                    return update(mouseCodeToEngineCode(button.button), button.state);
                }

                bool update(EKeyCode eCode, bool bPressed)
                {
                    auto code = static_cast<size_t>(eCode);

                    if (bPressed != mStates.test(code))
                    {
                        if (bPressed)
                            mStates.set(static_cast<size_t>(eCode));
                        else
                            mStates.reset(static_cast<size_t>(eCode));

                        return true;
                    }

                    return false;
                }

                bool test(EKeyCode eCode)
                {
                    return mStates.test(static_cast<size_t>(eCode));
                }
            private:
                keystate mStates;
            };
        }
    }
}