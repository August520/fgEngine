
#include <vector>
#include <unordered_map>

namespace fg {
    namespace particles {
        static const bool FG_PARTICLE_INTERPOLATION = true;

        struct ParticleBornParams {
            float velocity;
            float avelocity;
            float torsion;
            float angle;
            float size;
        };

        class ParticleAnimation final {
            friend class Emitter;

        public:
            void init(const math::p3d &startPos, const math::p3d &dir, const ParticleBornParams &bornParams);
            void initFrames(const std::unordered_map <ParticleParamType, Modifier *> &modifiers, const math::p3d &torsionAxis);

            void getData(float animKoeff, math::m4x4 &outTransform, fg::color &outColor) const;
            
            unsigned getFrameCount() const;
            float    getLifeTimeMs() const;
            float    getBornTimeMs() const;

        private:
            struct Frame {
                math::p3d  position; 
                fg::color  color;
                float      size;
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

        class Emitter : public EmitterInterface {
        public:
            Emitter();
            ~Emitter() override;

            void  build() override;
            void  setTimeStamp(float timeMs) override;
            bool  getNextParticleData(math::m4x4 &outTransform, fg::color &outColor) const override;

            ModifierInterface *createEmitterModifier(EmitterParamType type) override;
            ModifierInterface *createParticleModifier(ParticleParamType type) override;
            
            void  removeEmitterModifier(EmitterParamType type) override;
            void  removeParticleModifier(ParticleParamType type) override;
            
            void  setShader(const fg::string &shaderPath) override;
            void  addTextureBind(const fg::string &texturePath) override;
            void  clearTextureBinds() override;

            void  setTorsionAxis(const math::p3d &axis) override;
            void  setParam(EmitterParamType param, float value) override;
            void  setFps(float framesPerSecond) override;
            void  setLifeTime(float lifeTimeMs) override;
            void  setCycled(bool cycled) override;

            float getParam(EmitterParamType param) const override;
            float getFps() const override;
            float getLifeTime() const override;

            const fg::string &getShader() const override;
            const fg::string &getTextureBind(unsigned index) const override;

            unsigned getTextureBindCount() const override;
            unsigned getMaxParticleCount() const override;

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

            fg::string        _shaderPath;
            fg::string        _textureBinds[resources::FG_MATERIAL_TEXTURE_MAX];
            unsigned          _textureBindCount;

            float _getModifiedEmitterRandomParam(EmitterParamType paramTypeMin, EmitterParamType paramTypeMax, float koeff) const;
            float _getModifiedEmitterParam(EmitterParamType paramType, float koeff) const;
            float _getMaximumEmitterParam(EmitterParamType paramType) const;
            float _getMinimumEmitterParam(EmitterParamType paramType) const;
            void  _getConeRandomVectorAroundY(float maxAngle, math::p3d &out) const;

        private:
            Emitter(const Emitter &);
            Emitter &operator =(const Emitter &);
        };
    }
}