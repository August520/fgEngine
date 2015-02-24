
namespace fg {
    namespace particles {
        enum class ParticleType {
            BILL = 0,
            AXISBILL = 1,
            PPVELOCITY = 2,
        };

        enum class ModifierFunction {
            CONSTANT  = 0,
            LINEUP    = 1,
            LINEDOWN  = 2,
            DIAGRAM   = 3,
        };

        enum class ParticleParamType {
            VELOCITY            = 0,
            COLOR_R             = 1,
            COLOR_G             = 2,
            COLOR_B             = 3,
            COLOR_A             = 4,
            ANGLE_VELOCITY      = 5,
            SIZE                = 6,
            STRETCH             = 7,
            ANGLE               = 8,
            TORSION             = 9, 
            _count              = 10,
        };

        enum class EmitterParamType {
            PARTICLES_PER_SEC   = 0,
            VELOCITY_MIN        = 1,
            VELOCITY_MAX        = 2,
            LIFETIME_MIN        = 3,
            LIFETIME_MAX        = 4,
            CONE_ANGLE_MIN      = 5,
            CONE_ANGLE_MAX      = 6,
            PARTICLE_SIZE_MIN   = 7,
            PARTICLE_SIZE_MAX   = 8,
            ANGLE_MIN           = 9,
            ANGLE_MAX           = 10,
            ANGLE_VELOCITY_MIN  = 11,
            ANGLE_VELOCITY_MAX  = 12,
            TORSION_MIN         = 13,
            TORSION_MAX         = 14,
            _count              = 15,
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

            virtual float getMinimum() const = 0;
            virtual float getMaximum() const = 0;
        };

        class ParticleEmitterInterface {
        public:
            virtual ~ParticleEmitterInterface() {}

            virtual void  build() = 0;
            virtual void  setTimeStamp(float timeMs) = 0;
            virtual bool  getNextParticleData(math::m4x4 &outTransform, fg::color &outColor) const = 0;

            virtual ModifierInterface *createEmitterModifier(EmitterParamType type) = 0;
            virtual ModifierInterface *createParticleModifier(ParticleParamType type) = 0;

            virtual void  removeEmitterModifier(EmitterParamType type) = 0;
            virtual void  removeParticleModifier(ParticleParamType type) = 0;

            virtual void  setTorsionAxis(const math::p3d &axis) = 0;
            virtual void  setParam(EmitterParamType param, float value) = 0;
            virtual void  setFps(float framesPerSecond) = 0;
            virtual void  setLifeTime(float lifeTimeMs) = 0;
            virtual void  setCycled(bool cycled) = 0;

            virtual float getParam(EmitterParamType param) const = 0;
            virtual float getFps() const = 0;
            virtual float getLifeTime() const = 0;
            
            virtual unsigned getMaxParticles() const = 0;

            virtual bool  isCycled() const = 0;
        };
    }
}
