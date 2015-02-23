
namespace fg {
    namespace particles {
        class Modifier : public ModifierInterface {
        public:
            Modifier();
            ~Modifier() override;

            void setYAxisLimit(float minY, float maxY) override;
            void setFunction(ModifierFunction func) override;
            void setDiagramValue(unsigned id, float value) override;

            unsigned getDiagramValueCount() const override;
            unsigned addDiargamValue(float koeff, float value) override;

            ModifierFunction getFunction() const override;

            float getYAxisMin() const override;
            float getYAxisMax() const override;
            float getDiagramValue(unsigned id) const override;

            float getMinimum() const override;
            float getMaximum() const override;

            float modify(float koeff, float value) const;

        protected:
            static const unsigned DIAGRAM_VALUES_MAX = 16;

            struct DiagramValue {
                unsigned id;
                float koeff;
                float value;
            };

            float  _minY;
            float  _maxY;

            ModifierFunction  _func;
            DiagramValue      _diagram[DIAGRAM_VALUES_MAX];
            unsigned          _diagramValueCount;
            unsigned          _lastID;

            float  (Modifier::*_getValue)(float) const;
            float  _getConstValue(float koeff) const;
            float  _getLineUpValue(float koeff) const;
            float  _getLineDownValue(float koeff) const;
            float  _getDiagramValue(float koeff) const;

        private:
            Modifier(const Modifier &);
            Modifier &operator =(const Modifier &);
        };
    }
}