
namespace fg {
    namespace particles {
        enum class ModifierFunction {
            CONSTANT  = 0,
            LINEUP    = 1,
            LINEDOWN  = 2,
            DIAGRAM   = 3,
        };

        enum class ParticleModifierType {
            VELOCITY             = 0,
            SIZE                 = 1,
            COLOR_R              = 2,
            COLOR_G              = 3,
            COLOR_B              = 4,
            COLOR_A              = 5,
            ANGLE_VELOCITY       = 6,
            SIZE_X               = 7,
            SIZE_Y               = 8,
            SIZE_Z               = 9, 
            TORSION              = 10, 
            _count               = 11,
        };

        enum class EmitterModifierType {
            BORN_PERIOD          = 0,
            VELOCITY_MIN         = 1,
            VELOCITY_MAX         = 2,
            LIFETIME_MIN         = 3,
            LIFETIME_MAX         = 4,
            CONE_ANGLE_MIN       = 5,
            CONE_ANGLE_MAX       = 6,
            PARTICLE_SIZE_MIN    = 7,
            PARTICLE_SIZE_MAX    = 8,
            ANGLE_MIN            = 9,
            ANGLE_MAX            = 10,
            ANGLE_VELOCITY_MIN   = 11,
            ANGLE_VELOCITY_MAX   = 12,
            TORSION_MIN          = 13,
            TORSION_MAX          = 14,
            _count               = 15,
        };

        class ModifierInterface {
        public:
            virtual ~ModifierInterface() {}

            virtual void setYAxisLimit(float minY, float maxY) = 0;
            virtual void setFunction(ModifierFunction func) = 0;
            virtual void setDiagramValue(unsigned id, float value) = 0;

            virtual unsigned getDiagramValueCount() const = 0;
            virtual unsigned addDiargamValue(float koeff, float value) = 0;

            virtual ModifierFunction getFunction() const = 0;

            virtual float getYAxisMin() const = 0;
            virtual float getYAxisMax() const = 0;
            virtual float getDiagramValue(unsigned id) const = 0;
        };
    }
}
