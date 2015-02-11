
namespace fg {
    namespace input {
        enum class PointerEvent {
            PRESS    = 0,
            MOVE     = 1,
            RELEASE  = 2,
        };

        enum class KeyboardEvent {
            DOWN  = 0,
            UP    = 1,
        };

        enum class MiscEvent {
            MOUSE_WHEEL = 5,        
        };

        struct PointerEventArgs {
            unsigned   id;
            math::p2d  point;

            PointerEventArgs(unsigned pointID, float pointX, float pointY) : id(pointID), point(pointX, pointY) {}
        };

        struct KeyboardEventArgs {
            unsigned   virtualKey;

            KeyboardEventArgs(unsigned vkey) : virtualKey(vkey) {}
        };        
        
        class InputManagerInterface {
        public:
            virtual ~InputManagerInterface() {}

            virtual void addPointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) = 0;
            virtual void addKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) = 0;
            virtual void addMiscHandler(MiscEvent type, const callback <bool (int)> &cb) = 0;

            virtual void removePointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) = 0;
            virtual void removeKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) = 0;
            virtual void removeMiscHandler(MiscEvent type, const callback <bool (int)> &cb) = 0;
        };

        class EngineInputManagerInterface : public InputManagerInterface {
        public:
            virtual ~EngineInputManagerInterface() {}
        
            virtual void genPointerEvent(PointerEvent type, const PointerEventArgs &args) = 0;
            virtual void genKeyboardEvent(KeyboardEvent type, const KeyboardEventArgs &args) = 0;
            virtual void genMiscEvent(MiscEvent type, int arg) = 0;
        };
    }
}



//