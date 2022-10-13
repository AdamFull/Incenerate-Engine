#pragma once

#include <SDL.h>

namespace engine
{
    namespace system
    {
        namespace input
        {
            enum class EKeyState
            {
                ePress = SDL_PRESSED,
                eRelease = SDL_RELEASED,
                ePressed,
                eReleased
            };

            enum class EKeyCode
            {
                eUnknown,

                //Keyboard input
                eKeyA = SDL_SCANCODE_A,
                eKeyB = SDL_SCANCODE_B,
                eKeyC = SDL_SCANCODE_C,
                eKeyD = SDL_SCANCODE_D,
                eKeyE = SDL_SCANCODE_E,
                eKeyF = SDL_SCANCODE_F,
                eKeyG = SDL_SCANCODE_G,
                eKeyH = SDL_SCANCODE_H,
                eKeyI = SDL_SCANCODE_I,
                eKeyJ = SDL_SCANCODE_J,
                eKeyK = SDL_SCANCODE_K,
                eKeyL = SDL_SCANCODE_L,
                eKeyM = SDL_SCANCODE_M,
                eKeyN = SDL_SCANCODE_N,
                eKeyO = SDL_SCANCODE_O,
                eKeyP = SDL_SCANCODE_P,
                eKeyQ = SDL_SCANCODE_Q,
                eKeyR = SDL_SCANCODE_R,
                eKeyS = SDL_SCANCODE_S,
                eKeyT = SDL_SCANCODE_T,
                eKeyU = SDL_SCANCODE_U,
                eKeyV = SDL_SCANCODE_V,
                eKeyW = SDL_SCANCODE_W,
                eKeyX = SDL_SCANCODE_X,
                eKeyY = SDL_SCANCODE_Y,
                eKeyZ = SDL_SCANCODE_Z,

                eKey1 = SDL_SCANCODE_1,
                eKey2 = SDL_SCANCODE_2,
                eKey3 = SDL_SCANCODE_3,
                eKey4 = SDL_SCANCODE_4,
                eKey5 = SDL_SCANCODE_5,
                eKey6 = SDL_SCANCODE_6,
                eKey7 = SDL_SCANCODE_7,
                eKey8 = SDL_SCANCODE_8,
                eKey9 = SDL_SCANCODE_9,
                eKey0 = SDL_SCANCODE_0,

                eKyeReturn = SDL_SCANCODE_RETURN,
                eKyeEscape = SDL_SCANCODE_ESCAPE,
                eKyeBackspace = SDL_SCANCODE_BACKSPACE,
                eKyeTab = SDL_SCANCODE_TAB,
                eKeySpace = SDL_SCANCODE_SPACE,

                eKeyMinus = SDL_SCANCODE_MINUS,
                eKeyEquals = SDL_SCANCODE_EQUALS,
                eKeyLeftBracket = SDL_SCANCODE_LEFTBRACKET,
                eKeyRightBracket = SDL_SCANCODE_RIGHTBRACKET,
                eKeyBackslash = SDL_SCANCODE_BACKSLASH,
                eKeyNonuslash = SDL_SCANCODE_NONUSHASH,
                eKeySemicolon = SDL_SCANCODE_SEMICOLON,
                eKeyApostrophe = SDL_SCANCODE_APOSTROPHE,
                eKeyGrave = SDL_SCANCODE_GRAVE,
                eKeyComma = SDL_SCANCODE_COMMA,
                eKeyPeriod = SDL_SCANCODE_PERIOD,
                eKeySlash = SDL_SCANCODE_SLASH,
                eKeyCapslock = SDL_SCANCODE_CAPSLOCK,

                eKeyF1 = SDL_SCANCODE_F1,
                eKeyF2 = SDL_SCANCODE_F2,
                eKeyF3 = SDL_SCANCODE_F3,
                eKeyF4 = SDL_SCANCODE_F4,
                eKeyF5 = SDL_SCANCODE_F5,
                eKeyF6 = SDL_SCANCODE_F6,
                eKeyF7 = SDL_SCANCODE_F7,
                eKeyF8 = SDL_SCANCODE_F8,
                eKeyF9 = SDL_SCANCODE_F9,
                eKeyF10 = SDL_SCANCODE_F10,
                eKeyF11 = SDL_SCANCODE_F11,
                eKeyF12 = SDL_SCANCODE_F12,

                eKeyPrintscreen = SDL_SCANCODE_PRINTSCREEN,
                eKeyScrollLock = SDL_SCANCODE_SCROLLLOCK,
                eKeyPause = SDL_SCANCODE_PAUSE,
                eKeyInsert = SDL_SCANCODE_INSERT,

                eKeyHome = SDL_SCANCODE_HOME,
                eKeyPageUp = SDL_SCANCODE_PAGEUP,
                eKeyDelete = SDL_SCANCODE_DELETE,
                eKeyEnd = SDL_SCANCODE_END,
                eKeyPageDown = SDL_SCANCODE_PAGEDOWN,
                eKeyRight = SDL_SCANCODE_RIGHT,
                eKeyLeft = SDL_SCANCODE_LEFT,
                eKeyDown = SDL_SCANCODE_DOWN,
                eKeyUp = SDL_SCANCODE_UP,

                eKeyNumlockClear = SDL_SCANCODE_NUMLOCKCLEAR,

                eKeyNLDivide = SDL_SCANCODE_KP_DIVIDE,
                eKeyNLMultiply = SDL_SCANCODE_KP_MULTIPLY,
                eKeyNLMinus = SDL_SCANCODE_KP_MINUS,
                eKeyNLPlus = SDL_SCANCODE_KP_PLUS,
                eKeyNLEnter = SDL_SCANCODE_KP_ENTER,
                eKeyNL1 = SDL_SCANCODE_KP_1,
                eKeyNL2 = SDL_SCANCODE_KP_2,
                eKeyNL3 = SDL_SCANCODE_KP_3,
                eKeyNL4 = SDL_SCANCODE_KP_4,
                eKeyNL5 = SDL_SCANCODE_KP_5,
                eKeyNL6 = SDL_SCANCODE_KP_6,
                eKeyNL7 = SDL_SCANCODE_KP_7,
                eKeyNL8 = SDL_SCANCODE_KP_8,
                eKeyNL9 = SDL_SCANCODE_KP_9,
                eKeyNL0 = SDL_SCANCODE_KP_0,
                eKeyNLPeriod = SDL_SCANCODE_KP_PERIOD,
                eKeyNLEquals = SDL_SCANCODE_KP_EQUALS,

                eKeyNonusBackslash = SDL_SCANCODE_NONUSBACKSLASH,

                eKeyApplication = SDL_SCANCODE_APPLICATION,
                eKeyPower = SDL_SCANCODE_POWER,

                eKeyF13 = SDL_SCANCODE_F13,
                eKeyF14 = SDL_SCANCODE_F14,
                eKeyF15 = SDL_SCANCODE_F15,
                eKeyF16 = SDL_SCANCODE_F16,
                eKeyF17 = SDL_SCANCODE_F17,
                eKeyF18 = SDL_SCANCODE_F18,
                eKeyF19 = SDL_SCANCODE_F19,
                eKeyF20 = SDL_SCANCODE_F20,
                eKeyF21 = SDL_SCANCODE_F21,
                eKeyF22 = SDL_SCANCODE_F22,
                eKeyF23 = SDL_SCANCODE_F23,
                eKeyF24 = SDL_SCANCODE_F24,

                eKeyExecute = SDL_SCANCODE_EXECUTE,
                eKeyHelp = SDL_SCANCODE_HELP,
                eKeyMenu = SDL_SCANCODE_MENU,
                eKeySelect = SDL_SCANCODE_SELECT,
                eKeyStop = SDL_SCANCODE_STOP,
                eKeyAgain = SDL_SCANCODE_AGAIN,
                eKeyUndo = SDL_SCANCODE_UNDO,
                eKeyCut = SDL_SCANCODE_CUT,
                eKeyCopy = SDL_SCANCODE_COPY,
                eKeyPaste = SDL_SCANCODE_PASTE,
                eKeyFind = SDL_SCANCODE_FIND,
                eKeyMute = SDL_SCANCODE_MUTE,
                eKeyVolumeUp = SDL_SCANCODE_VOLUMEUP,
                eKeyVolumeDown = SDL_SCANCODE_VOLUMEDOWN,

                eKeyKPComma = SDL_SCANCODE_KP_COMMA,
                eKeyKPEqualSAS400 = SDL_SCANCODE_KP_EQUALSAS400,

                eKeyInternational1 = SDL_SCANCODE_INTERNATIONAL1,
                eKeyInternational2 = SDL_SCANCODE_INTERNATIONAL2,
                eKeyInternational3 = SDL_SCANCODE_INTERNATIONAL3,
                eKeyInternational4 = SDL_SCANCODE_INTERNATIONAL4,
                eKeyInternational5 = SDL_SCANCODE_INTERNATIONAL5,
                eKeyInternational6 = SDL_SCANCODE_INTERNATIONAL6,
                eKeyInternational7 = SDL_SCANCODE_INTERNATIONAL7,
                eKeyInternational8 = SDL_SCANCODE_INTERNATIONAL8,
                eKeyInternational9 = SDL_SCANCODE_INTERNATIONAL9,

                eKeyLang1 = SDL_SCANCODE_LANG1,
                eKeyLang2 = SDL_SCANCODE_LANG2,
                eKeyLang3 = SDL_SCANCODE_LANG3,
                eKeyLang4 = SDL_SCANCODE_LANG4,
                eKeyLang5 = SDL_SCANCODE_LANG5,
                eKeyLang6 = SDL_SCANCODE_LANG6,
                eKeyLang7 = SDL_SCANCODE_LANG7,
                eKeyLang8 = SDL_SCANCODE_LANG8,
                eKeyLang9 = SDL_SCANCODE_LANG9,

                eKeyAlterase = SDL_SCANCODE_ALTERASE,
                eKeySysReq = SDL_SCANCODE_SYSREQ,
                eKeyCancel = SDL_SCANCODE_CANCEL,
                eKeyClear = SDL_SCANCODE_CLEAR,
                eKeyPrior = SDL_SCANCODE_PRIOR,
                eKeyReturn2 = SDL_SCANCODE_RETURN2,
                eKeySeparator = SDL_SCANCODE_SEPARATOR,
                eKeyOut = SDL_SCANCODE_OUT,
                eKeyOper = SDL_SCANCODE_OPER,
                eKeyClearAgain = SDL_SCANCODE_CLEARAGAIN,
                eKeyCrSel = SDL_SCANCODE_CRSEL,
                eKeyExSel = SDL_SCANCODE_EXSEL,

                eKeyKP00 = SDL_SCANCODE_KP_00,
                eKeyKP000 = SDL_SCANCODE_KP_000,
                eKeyThousousSeparator = SDL_SCANCODE_THOUSANDSSEPARATOR,
                eKeyDecimalSeparator = SDL_SCANCODE_DECIMALSEPARATOR,
                eKeyCurrencyUnit = SDL_SCANCODE_CURRENCYUNIT,
                eKeyCurrencySubUnit = SDL_SCANCODE_CURRENCYSUBUNIT,
                eKeyKPLeftParen = SDL_SCANCODE_KP_LEFTPAREN,
                eKeyKPRightParen = SDL_SCANCODE_KP_RIGHTPAREN,
                eKeyKPLeftBrace = SDL_SCANCODE_KP_LEFTBRACE,
                eKeyKPRightBrace = SDL_SCANCODE_KP_RIGHTBRACE,
                eKeyKPTab = SDL_SCANCODE_KP_TAB,
                eKeyKPBackspace = SDL_SCANCODE_KP_BACKSPACE,
                eKeyKPA = SDL_SCANCODE_KP_A,
                eKeyKPB = SDL_SCANCODE_KP_B,
                eKeyKPC = SDL_SCANCODE_KP_C,
                eKeyKPD = SDL_SCANCODE_KP_D,
                eKeyKPE = SDL_SCANCODE_KP_E,
                eKeyKPF = SDL_SCANCODE_KP_F,
                eKeyKPXOR = SDL_SCANCODE_KP_XOR,
                eKeyKPPower = SDL_SCANCODE_KP_POWER,
                eKeyKPPercent = SDL_SCANCODE_KP_PERCENT,
                eKeyKPLess = SDL_SCANCODE_KP_LESS,
                eKeyKPGreater = SDL_SCANCODE_KP_GREATER,
                eKeyKPAmpersand = SDL_SCANCODE_KP_AMPERSAND,
                eKeyKPDBLAmpersand = SDL_SCANCODE_KP_DBLAMPERSAND,
                eKeyKPVerticalBar = SDL_SCANCODE_KP_VERTICALBAR,
                eKeyKPDBLVerticalBar = SDL_SCANCODE_KP_DBLVERTICALBAR,
                eKeyKPColon = SDL_SCANCODE_KP_COLON,
                eKeyKPHash = SDL_SCANCODE_KP_HASH,
                eKeyKPSpace = SDL_SCANCODE_KP_SPACE,
                eKeyKPAt = SDL_SCANCODE_KP_AT,
                eKeyKPExclam = SDL_SCANCODE_KP_EXCLAM,
                eKeyKPMemStore = SDL_SCANCODE_KP_MEMSTORE,
                eKeyKPMemRecall = SDL_SCANCODE_KP_MEMRECALL,
                eKeyKPMemClear = SDL_SCANCODE_KP_MEMCLEAR,
                eKeyKPMemAdd = SDL_SCANCODE_KP_MEMADD,
                eKeyKPMemSubstract = SDL_SCANCODE_KP_MEMSUBTRACT,
                eKeyKPMemMultoply = SDL_SCANCODE_KP_MEMMULTIPLY,
                eKeyKPMemDevide = SDL_SCANCODE_KP_MEMDIVIDE,
                eKeyKPPlusMinus = SDL_SCANCODE_KP_PLUSMINUS,
                eKeyKPClear = SDL_SCANCODE_KP_CLEAR,
                eKeyKPClearEntry = SDL_SCANCODE_KP_CLEARENTRY,
                eKeyKPBinary = SDL_SCANCODE_KP_BINARY,
                eKeyKPOctal = SDL_SCANCODE_KP_OCTAL,
                eKeyKPDecimal = SDL_SCANCODE_KP_DECIMAL,
                eKeyKPHexDecimal = SDL_SCANCODE_KP_HEXADECIMAL,

                eKeyLCtrl = SDL_SCANCODE_LCTRL,
                eKeyLShift = SDL_SCANCODE_LSHIFT,
                eKeyLAlt = SDL_SCANCODE_LALT,
                eKeyLGui = SDL_SCANCODE_LGUI,
                eKeyRCtrl = SDL_SCANCODE_RCTRL,
                eKeyRShift = SDL_SCANCODE_RSHIFT,
                eKeyRAlt = SDL_SCANCODE_RALT,
                eKeyRGui = SDL_SCANCODE_RGUI,
                eKeyMode = SDL_SCANCODE_MODE,

                eKeyAudioNext = SDL_SCANCODE_AUDIONEXT,
                eKeyAudioPrev = SDL_SCANCODE_AUDIOPREV,
                eKeyAudioStop = SDL_SCANCODE_AUDIOSTOP,
                eKeyAudioPlay = SDL_SCANCODE_AUDIOPLAY,
                eKeyAudioMute = SDL_SCANCODE_AUDIOMUTE,
                eKeyMediaSelect = SDL_SCANCODE_MEDIASELECT,
                eKeyWWW = SDL_SCANCODE_WWW,
                eKeyMail = SDL_SCANCODE_MAIL,
                eKeyCalculator = SDL_SCANCODE_CALCULATOR,
                eKeyComputer = SDL_SCANCODE_COMPUTER,
                eKeyACSearch = SDL_SCANCODE_AC_SEARCH,
                eKeyACHome = SDL_SCANCODE_AC_HOME,
                eKeyACBack = SDL_SCANCODE_AC_BACK,
                eKeyACForward = SDL_SCANCODE_AC_FORWARD,
                eKeyACStop = SDL_SCANCODE_AC_STOP,
                eKeyACRefresh = SDL_SCANCODE_AC_REFRESH,
                eKeyACBookmarks = SDL_SCANCODE_AC_BOOKMARKS,

                eKeyBrightnessDown = SDL_SCANCODE_BRIGHTNESSDOWN,
                eKeyBrightnessUp = SDL_SCANCODE_BRIGHTNESSUP,
                eKeyDisplaySwitch = SDL_SCANCODE_DISPLAYSWITCH,

                eKeyKBDillumToggle = SDL_SCANCODE_KBDILLUMTOGGLE,
                eKeyKBDillumDown = SDL_SCANCODE_KBDILLUMDOWN,
                eKeyKBDillumUp = SDL_SCANCODE_KBDILLUMUP,

                eKeyEject = SDL_SCANCODE_EJECT,
                eKeySleep = SDL_SCANCODE_SLEEP,

                eKeyApp1 = SDL_SCANCODE_APP1,
                eKeyApp2 = SDL_SCANCODE_APP2,

                eKeyAudioRewind = SDL_SCANCODE_AUDIOREWIND,
                eKeyAudioFastForward = SDL_SCANCODE_AUDIOFASTFORWARD,

                eKeySoftLeft = SDL_SCANCODE_SOFTLEFT,
                eKeySoftRight = SDL_SCANCODE_SOFTRIGHT,
                eKeyCall = SDL_SCANCODE_CALL,
                eKeyEndCall = SDL_SCANCODE_ENDCALL,

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

                eCursorPosition,
                eCursorMove
            };

            struct FKeycodeHelper
            {
                static EKeyCode KeyboardCodeToEngineCode(SDL_Scancode code)
                {
                    return static_cast<EKeyCode>(code);
                }

                static EKeyCode GamepadCodeToEngineCode(SDL_GameControllerButton code)
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

                static EKeyCode MouseCodeToEngineCode(int32_t code)
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
            };
        }
    }
}