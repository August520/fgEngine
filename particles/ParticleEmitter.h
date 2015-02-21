
namespace fg {
    namespace particles {
        class ParticleAnimation final {
        public:
            ParticleAnimation();
            ~ParticleAnimation();

            void init(float bornTimeMs, float lifeTimeMs, float frameTimeMs, const math::p3d &startPos);
            void initFrame(unsigned frameIndex, const Modifier *modifiers);

            void getTransform(float animKoeff, math::m4x4 &out) const;

            unsigned getFrameCount() const;
            float    getLifeTimeMs() const;
            float    getBornTimeMs() const;

        private:
            struct Frame {
                math::quat  localRotation;
                math::p3d   localScale;
                math::p3d   localPosition;

                Frame() : localScale(0.1f, 0.1f, 0.1f) {
                }
            };

            float      _bornTimeMs;
            float      _lifeTimeMs;
            unsigned   _frameCount;
            math::p3d  _startPos;
            Frame      *_animationFrames;

        private:
            ParticleAnimation(const ParticleAnimation &);
            ParticleAnimation &operator =(const ParticleAnimation &);
        };

        //---------------------------------------------------------------------

        class ParticleEmitter final {
        public:
            ParticleEmitter();
            ~ParticleEmitter();

            void  build();
            void  setTimeStamp(float timeMs);
            bool  getNextParticleData(math::m4x4 &trfm);

            Modifier &addEmitterModifier(EmitterModifierType type);
            Modifier &addParticleModifier(EmitterModifierType type);

            void  setFps(float framesPerSecond);
            void  setLifeTime(float lifeTimeMs);
            void  setCycled(bool cycled);

            float getFps();
            float getLifeTime();
            bool  isCycled();

        private:
            struct ParticleModifier {
                ParticleModifierType  type;
                Modifier  modifier;
            };

            struct EmitterModifier {
                EmitterModifierType  type;
                Modifier  modifier;
            };

            std::vector <EmitterModifier> _emitterModifiers;
            std::vector <ParticleModifier> _particleModifiers;

            unsigned  _maxParticles;
            bool      _cycled;
            float     _nrmLifePeriodMs;           // cycle period multiple of min born period
            float     _lifeTimeMs;                // active time of emitter (cycle time)
            float     _frameTimeMs;

            float     _dynamicParams[EmitterModifierType::_count];

            float     _curTimeMs;                 // current global time
            float     _curNrmTimeMs;              // current global time normalized to '_nrmLifePeriodMs' range
            unsigned  _curYoungestParticleIndex;  // current youngest particle index in '_particles'
            unsigned  _curParticleIndex;          // current index for 'getNextParticleData'

            ParticleAnimation *_particles;

        private:
            ParticleEmitter(const ParticleEmitter &);
            ParticleEmitter &operator =(const ParticleEmitter &);
        };
    }
}