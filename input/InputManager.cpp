
namespace fg {
    namespace input {
        InputManager::InputManager() {
        
        }

        InputManager::~InputManager() {
        
        }

        void InputManager::addPointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) {
            if(type == PointerEvent::PRESS) {
                _pointerPressCallbacks.push_front(cb);
            }
            if(type == PointerEvent::RELEASE) {
                _pointerReleaseCallbacks.push_front(cb);
            }
            if(type == PointerEvent::MOVE) {
                _pointerMoveCallbacks.push_front(cb);
            }
        }

        void InputManager::addKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) {
            if(type == KeyboardEvent::DOWN) {
                _keyDownCallbacks.push_front(cb);
            }
            if(type == KeyboardEvent::UP) {
                _keyUpCallbacks.push_front(cb);
            }
        }

        void InputManager::addMiscHandler(MiscEvent type, const callback <bool (int)> &cb) {
            if(type == MiscEvent::MOUSE_WHEEL) {
                _mouseWheelCallbacks.push_front(cb);
            }
        }

        void InputManager::removePointerHandler(PointerEvent type, const callback <bool (const PointerEventArgs &)> &cb) {
            if(type == PointerEvent::PRESS) {
                _removeCallbackFromList(_pointerPressCallbacks, cb);
            }
            if(type == PointerEvent::RELEASE) {
                _removeCallbackFromList(_pointerReleaseCallbacks, cb);
            }
            if(type == PointerEvent::MOVE) {
                _removeCallbackFromList(_pointerMoveCallbacks, cb);
            }
        }

        void InputManager::removeKeyboardHandler(KeyboardEvent type, const callback <bool (const KeyboardEventArgs &)> &cb) {
            if(type == KeyboardEvent::DOWN) {
                _removeCallbackFromList(_keyDownCallbacks, cb);
            }
            if(type == KeyboardEvent::UP) {
                _removeCallbackFromList(_keyUpCallbacks, cb);
            }
        }

        void InputManager::removeMiscHandler(MiscEvent type, const callback <bool (int)> &cb) {
            if(type == MiscEvent::MOUSE_WHEEL) {
                _removeCallbackFromList(_mouseWheelCallbacks, cb);
            }
        }

        void InputManager::genPointerEvent(PointerEvent type, const PointerEventArgs &args) {
            if(type == PointerEvent::PRESS) {
                _call(_pointerPressCallbacks, args);
            }
            if(type == PointerEvent::RELEASE) {
                _call(_pointerReleaseCallbacks, args);
            }
            if(type == PointerEvent::MOVE) {
                _call(_pointerMoveCallbacks, args);
            }
        }

        void InputManager::genKeyboardEvent(KeyboardEvent type, const KeyboardEventArgs &args) {
            if(type == KeyboardEvent::DOWN) {
                _call(_keyDownCallbacks, args);
            }
            if(type == KeyboardEvent::UP) {
                _call(_keyUpCallbacks, args);
            }
        }

        void InputManager::genMiscEvent(MiscEvent type, int arg) {
            if(type == MiscEvent::MOUSE_WHEEL) {
                _call(_mouseWheelCallbacks, arg);
            }
        }
    }
}