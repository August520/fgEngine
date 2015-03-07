
namespace fg {
    namespace particles {
        float Modifier::_getConstValue(float koeff) const {
            return _maxY;
        }

        float Modifier::_getLineUpValue(float koeff) const {
            return _minY + (_maxY - _minY) * koeff;
        }

        float Modifier::_getLineDownValue(float koeff) const {
            return _minY + (_maxY - _minY) * (1.0f - koeff);
        }

        float Modifier::_getExpUpValue(float koeff) const {
            return _minY + (_maxY - _minY) * (koeff * koeff);
        }

        float Modifier::_getExpDownValue(float koeff) const {
            return _minY + (_maxY - _minY) * ((1.0f - koeff) * (1.0f - koeff));        
        }

        float Modifier::_getLogUpValue(float koeff) const {
            return _minY + (_maxY - _minY) * (1.0f - ((1.0f - koeff) * (1.0f - koeff)));        
        }

        float Modifier::_getLogDownValue(float koeff) const {
            return _minY + (_maxY - _minY) * (1.0f - koeff * koeff);        
        }

        float Modifier::_getDiagramValue(float koeff) const {
            float leftValue = _maxY;
            float leftKoeff = -0.00001f;
            float rightKoeff = 1.00001f;
            float rightValue = _maxY;

            for(unsigned i = 0; i < _diagramValueCount; i++) {
                if(koeff >= _diagram[i].koeff) {
                    leftValue = _diagram[i].value;
                    leftKoeff = _diagram[i].koeff;
                }
            }

            for(int i = int(_diagramValueCount) - 1; i >= 0; i--) {
                if(koeff <= _diagram[i].koeff) {
                    rightValue = _diagram[i].value;
                    rightKoeff = _diagram[i].koeff;
                }
            }

            return leftValue + (koeff - leftKoeff) / (rightKoeff - leftKoeff) * (rightValue - leftValue);
        }

        //---

        Modifier::Modifier() {
            _diagramValueCount = 0;
            _minY = 0.0f;
            _maxY = 1.0f;
            _func = ModifierFunction::CONSTANT;
            _getValue = &Modifier::_getConstValue;
            _lastID = 0x10;
        }

        Modifier::~Modifier() {

        }

        void Modifier::setYAxisLimit(float minY, float maxY) {
            _minY = minY;
            _maxY = maxY;
        }

        void Modifier::setFunction(ModifierFunction func) {
            static float (Modifier::*_modifierEvaluateFunctions[ModifierFunction::_count])(float) const = {
                &Modifier::_getConstValue,
                &Modifier::_getLineUpValue,
                &Modifier::_getLineDownValue,
                &Modifier::_getExpUpValue,
                &Modifier::_getExpDownValue,
                &Modifier::_getLogUpValue,
                &Modifier::_getLogDownValue,
                &Modifier::_getDiagramValue,
            };

            _getValue = _modifierEvaluateFunctions[unsigned(func)];
            _func = func;
        }

        void Modifier::setDiagramValue(unsigned id, float value) {
            for(unsigned i = 0; i < _diagramValueCount; i++) {
                if(_diagram[i].id == id) {
                    _diagram[i].value = value;
                    break;
                }
            }
        }

        unsigned Modifier::getDiagramValueCount() const {
            return _diagramValueCount;
        }

        unsigned Modifier::addDiargamValue(float koeff, float value) {
            if(_diagramValueCount < DIAGRAM_VALUES_MAX) {
                int i = int(_diagramValueCount);

                for(; i > 0; i--) {
                    if(koeff < _diagram[i - 1].koeff) {
                        _diagram[i] = _diagram[i - 1];
                    }
                    else {
                        break;
                    }
                }

                _diagram[i].id = _lastID;
                _diagram[i].koeff = koeff;
                _diagram[i].value = value;
                _diagramValueCount++;
                return _lastID++;
            }
            return 0;
        }

        ModifierFunction Modifier::getFunction() const {
            return _func;
        }

        float Modifier::getYAxisMin() const {
            return _minY;
        }

        float Modifier::getYAxisMax() const {
            return _maxY;
        }

        float Modifier::getDiagramValue(unsigned id) const {
            for(unsigned i = 0; i < _diagramValueCount; i++) {
                if(_diagram[i].id == id) {
                    return _diagram[i].value;
                }
            }
            return 0.0f;
        }

        float Modifier::getMinimum() const {
            if(_func == ModifierFunction::DIAGRAM) {
                float r = _maxY;

                for(unsigned i = 0; i < _diagramValueCount; i++) {
                    if(_diagram[i].value < r) {
                        r = _diagram[i].value;
                    }
                }

                return r;
            }
            else if(_func == ModifierFunction::CONSTANT) {
                return _maxY;
            }
            
            return _minY;
        }

        float Modifier::getMaximum() const {
            if(_func == ModifierFunction::DIAGRAM) {
                float r = _maxY;

                for(unsigned i = 0; i < _diagramValueCount; i++) {
                    if(_diagram[i].value > r) {
                        r = _diagram[i].value;
                    }
                }

                return r;
            }

            return _maxY;
        }

        float Modifier::modify(float koeff, float value) const {
            return value * (this->*_getValue)(koeff);
        }
    }
}