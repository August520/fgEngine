
#include <vector>
#include <unordered_map>

namespace fg {
    namespace particles {
        struct ParticleBornParams {
            float velocity;
            float avelocity;
            float torsion;
            float angle;
            float size;
        };

        class ParticleAnimation final {
            friend class ParticleEmitter;

        public:
            void init(const math::p3d &startPos, const math::p3d &dir, const ParticleBornParams &bornParams);
            void initFrames(const std::unordered_map <ParticleParamType, Modifier *> &modifiers, const math::p3d &torsionAxis);

            void getTransform(float animKoeff, math::m4x4 &out) const;

            unsigned getFrameCount() const;
            float    getLifeTimeMs() const;
            float    getBornTimeMs() const;

        private:
            struct Frame {
                math::p3d position;  
                float     size;
            };

            float      _bornTimeMs;
            float      _lifeTimeMs;
            float      _frameTime;
            unsigned   _frameCount;
            
            float      _dynamicParams[ParticleParamType::_count];

            math::p3d  _startPos;
            math::p3d  _dir;
                        
            Frame      *_animationFrames;

            ParticleAnimation(float bornTimeMs, float lifeTimeMs, float frameTimeMs);
            ~ParticleAnimation();

            float _getModifiedParticleParam(const std::unordered_map <ParticleParamType, Modifier *> &modifiers, ParticleParamType paramType, float koeff) const;

        private:
            ParticleAnimation(const ParticleAnimation &);
            ParticleAnimation &operator =(const ParticleAnimation &);
        };

        //---------------------------------------------------------------------

        class ParticleEmitter : public ParticleEmitterInterface {
        public:
            ParticleEmitter();
            ~ParticleEmitter() override;

            void  build();
            void  setTimeStamp(float timeMs);
            bool  getNextParticleData(math::m4x4 &trfm) const;

            ModifierInterface *createEmitterModifier(EmitterParamType type);
            ModifierInterface *createParticleModifier(ParticleParamType type);

            void  removeEmitterModifier(EmitterParamType type);
            void  removeParticleModifier(ParticleParamType type);

            void  setTorsionAxis(const math::p3d &axis);
            void  setParam(EmitterParamType param, float value);
            void  setFps(float framesPerSecond);
            void  setLifeTime(float lifeTimeMs);
            void  setCycled(bool cycled);

            float getParam(EmitterParamType param) const;
            float getFps() const;
            float getLifeTime() const;
            bool  isCycled() const;

        protected:
            std::unordered_map <EmitterParamType, Modifier *> _emitterModifiers;
            std::unordered_map <ParticleParamType, Modifier *> _particleModifiers;

            bool   _cycled;
            float  _nrmLifePeriodMs;                 // cycle period multiple of min born period
            float  _lifeTimeMs;                      // active time of emitter (cycle time)
            float  _frameTimeMs;
            float  _dynamicParams[EmitterParamType::_count];

            float  _curTimeMs;                       // current global time
            float  _curNrmTimeMs;                    // current global time normalized to '_nrmLifePeriodMs' range
            
            mutable unsigned  _curParticleIndex;     // current index for 'getNextParticleData'
            math::p3d         _torsionAxis;
            std::vector       <ParticleAnimation *> _particles;

            float _getModifiedEmitterRandomParam(EmitterParamType paramTypeMin, EmitterParamType paramTypeMax, float koeff) const;
            float _getModifiedEmitterParam(EmitterParamType paramType, float koeff) const;
            float _getMaximumEmitterParam(EmitterParamType paramType) const;
            float _getMinimumEmitterParam(EmitterParamType paramType) const;
            void  _getConeRandomVectorAroundY(float maxAngle, math::p3d &out) const;

        private:
            ParticleEmitter(const ParticleEmitter &);
            ParticleEmitter &operator =(const ParticleEmitter &);
        };
    }
}