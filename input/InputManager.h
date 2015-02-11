
#include <list>

namespace fg {
    namespace input {
        class InputManager : public EngineInputManagerInterface {
        public:
            InputManager();
            ~InputManager() override;

            void addPointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) override;
            void addKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) override;
            void addMiscHandler(MiscEvent type, const callback <bool (int)> &cb) override;

            void removePointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) override;
            void removeKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) override;
            void removeMiscHandler(MiscEvent type, const callback <bool (int)> &cb) override;

            void genPointerEvent(PointerEvent type, const PointerEventArgs &args) override;
            void genKeyboardEvent(KeyboardEvent type, const KeyboardEventArgs &args) override;
            void genMiscEvent(MiscEvent type, int arg) override;

        protected:
            std::list <callback <bool (const PointerEventArgs &)>> _pointerPressCallbacks;
            std::list <callback <bool (const PointerEventArgs &)>> _pointerMoveCallbacks;
            std::list <callback <bool (const PointerEventArgs &)>> _pointerReleaseCallbacks;
            std::list <callback <bool (const KeyboardEventArgs &)>> _keyDownCallbacks;
            std::list <callback <bool (const KeyboardEventArgs &)>> _keyUpCallbacks;
            std::list <callback <bool (int)>> _mouseWheelCallbacks;

            template <typename T> void _removeCallbackFromList(std::list <callback <T>> &lst, const callback <T> &cb) {
                for(auto index = lst.begin(); index != lst.end(); ++index) {
                    if(*index == cb) {
                        lst.erase(index);
                        break;
                    }
                }
            }

            template <typename T, typename ARGT> void _call(std::list <callback <T>> &lst, const ARGT &args) {
                for(auto index = lst.begin(); index != lst.end(); ++index) {
                    if((*index)(args)) {
                        break;
                    }
                }
            }

        private:
            InputManager(const InputManager &);
            InputManager &operator =(const InputManager &);
        };

    }
}