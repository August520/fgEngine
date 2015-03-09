
namespace fg {
    namespace particles {
        enum class ParticleType {
            BILL       = 0,         // face to camera
            AXISBILL   = 1,         // face to camera around axis
            PPVELOCITY = 2,         // face to velocity dir
            _count,
        };

        enum class ModifierFunction {
            CONSTANT  = 0,
            LINEUP    = 1,
            LINEDOWN  = 2,
            EXPUP     = 3,
            EXPDOWN   = 4,
            LOGUP     = 5,
            LOGDOWN   = 6,
            DIAGRAM   = 7,
            _count,
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
            TORSION             = 8, 
            _count,
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
            _count,
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

        class EmitterInterface {
        public:
            virtual ~EmitterInterface() {}

            virtual void  setTimeStamp(float timeMs) = 0;
            virtual bool  getNextParticleData(math::m4x4 &outTransform, fg::color &outColor, float &outLifeTimeMs) const = 0;

            virtual ModifierInterface *createEmitterModifier(EmitterParamType type) = 0;
            virtual ModifierInterface *createParticleModifier(ParticleParamType type) = 0;

            virtual void  removeEmitterModifier(EmitterParamType type) = 0;
            virtual void  removeParticleModifier(ParticleParamType type) = 0;

            virtual void  setShader(const fg::string &shaderPath) = 0;
            virtual void  addTextureBind(const fg::string &texturePath) = 0;
            virtual void  clearTextureBinds() = 0;
            
            virtual void  setType(ParticleType type) = 0;
            virtual void  setTorsionAxis(const math::p3d &axis) = 0;
            virtual void  setParam(EmitterParamType param, float value) = 0;
            virtual void  setFps(float framesPerSecond) = 0;
            virtual void  setLifeTime(float lifeTimeMs) = 0;
            virtual void  setCycled(bool cycled) = 0;
            virtual void  setWorldSpace(bool worldSpace) = 0;

            virtual float getParam(EmitterParamType param) const = 0;
            virtual float getFps() const = 0;
            virtual float getLifeTime() const = 0;
            virtual float getMaxParticleLifeTime() const = 0;
            
            virtual ParticleType getType() const = 0;

            virtual const fg::string &getShader() const = 0;
            virtual const fg::string &getTextureBind(unsigned index) const = 0;

            virtual unsigned getTextureBindCount() const = 0;
            virtual unsigned getMaxParticleCount() const = 0;

            virtual bool  isWorldSpace() const = 0;
            virtual bool  isCycled() const = 0;
        };
    }
}
