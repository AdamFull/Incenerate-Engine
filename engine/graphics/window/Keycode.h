#pragma once

#include <bitset>

namespace engine
{
	namespace graphics
	{
        enum class EKeyState
        {
            ePress = 1,
            eRelease = 0,
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
            eKeyLCtrl,
            eKeyRCtrl,
            eKeyLShift,
            eKeyRShift,

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

        using keystate = std::bitset<static_cast<size_t>(EKeyCode::eCount)>;

        class CKeyDecoder
        {
        public:
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