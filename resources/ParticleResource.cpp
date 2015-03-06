
namespace fg {
    namespace resources {
        struct ParamTypeConstTable {
            StaticHash <unsigned(particles::EmitterParamType::_count), particles::EmitterParamType> emitterParamConstants;
            StaticHash <unsigned(particles::ParticleParamType::_count), particles::ParticleParamType> particleParamConstants;
            StaticHash <unsigned(particles::ModifierFunction::_count), particles::ModifierFunction> modifierFunctionConstants;

            ParamTypeConstTable() {
                emitterParamConstants.add("PARTICLES_PER_SEC", particles::EmitterParamType::PARTICLES_PER_SEC);
                emitterParamConstants.add("VELOCITY_MIN", particles::EmitterParamType::VELOCITY_MIN);
                emitterParamConstants.add("VELOCITY_MAX", particles::EmitterParamType::VELOCITY_MAX);
                emitterParamConstants.add("LIFETIME_MIN", particles::EmitterParamType::LIFETIME_MIN);
                emitterParamConstants.add("LIFETIME_MAX", particles::EmitterParamType::LIFETIME_MAX);
                emitterParamConstants.add("CONE_ANGLE_MIN", particles::EmitterParamType::CONE_ANGLE_MIN);
                emitterParamConstants.add("CONE_ANGLE_MAX", particles::EmitterParamType::CONE_ANGLE_MAX);
                emitterParamConstants.add("PARTICLE_SIZE_MIN", particles::EmitterParamType::PARTICLE_SIZE_MIN);
                emitterParamConstants.add("PARTICLE_SIZE_MAX", particles::EmitterParamType::PARTICLE_SIZE_MAX);
                emitterParamConstants.add("ANGLE_MIN", particles::EmitterParamType::ANGLE_MIN);
                emitterParamConstants.add("ANGLE_MAX", particles::EmitterParamType::ANGLE_MAX);
                emitterParamConstants.add("ANGLE_VELOCITY_MIN", particles::EmitterParamType::ANGLE_VELOCITY_MIN);
                emitterParamConstants.add("ANGLE_VELOCITY_MAX", particles::EmitterParamType::ANGLE_VELOCITY_MAX);
                emitterParamConstants.add("TORSION_MIN", particles::EmitterParamType::TORSION_MIN);
                emitterParamConstants.add("TORSION_MAX", particles::EmitterParamType::TORSION_MAX);

                particleParamConstants.add("VELOCITY", particles::ParticleParamType::VELOCITY);
                particleParamConstants.add("COLOR_R", particles::ParticleParamType::COLOR_R);
                particleParamConstants.add("COLOR_G", particles::ParticleParamType::COLOR_G);
                particleParamConstants.add("COLOR_B", particles::ParticleParamType::COLOR_B);
                particleParamConstants.add("COLOR_A", particles::ParticleParamType::COLOR_A);
                particleParamConstants.add("ANGLE_VELOCITY", particles::ParticleParamType::ANGLE_VELOCITY);
                particleParamConstants.add("SIZE", particles::ParticleParamType::SIZE);
                particleParamConstants.add("STRETCH", particles::ParticleParamType::STRETCH);
                particleParamConstants.add("ANGLE", particles::ParticleParamType::ANGLE);
                particleParamConstants.add("TORSION", particles::ParticleParamType::TORSION);

                modifierFunctionConstants.add("CONSTANT", particles::ModifierFunction::CONSTANT);
                modifierFunctionConstants.add("LINEUP", particles::ModifierFunction::LINEUP);
                modifierFunctionConstants.add("LINEDOWN", particles::ModifierFunction::LINEDOWN);
                modifierFunctionConstants.add("DIAGRAM", particles::ModifierFunction::DIAGRAM);
            }

            bool getConstant(const char *name, particles::EmitterParamType &out) {
                return emitterParamConstants.tryGet(name, out);
            }

            bool getConstant(const char *name, particles::ParticleParamType &out) {
                return particleParamConstants.tryGet(name, out);
            }

            bool getConstant(const char *name, particles::ModifierFunction &out) {
                return modifierFunctionConstants.tryGet(name, out);
            }

        } __paramTypeConstTable;

        //---

        ParticleResource::~ParticleResource() {
            _emitters.foreach([](const fg::string &, particles::Emitter *ptr) {
                delete ptr;
            });
        }

        void ParticleResource::loaded(const diag::LogInterface &log) {
            luaScript cfgSource;

            if(cfgSource.execLuaChunk((char *)_binaryData, _binarySize)) {
                luaObj cfg;
                cfgSource.getGlobalVar("particle", cfg);

                cfg.foreach([this, &log](const char *emitterName, const luaObj &emitterTable) {
                    if(emitterName[0] && emitterTable.type() == LUATYPE_TABLE) {
                        particles::Emitter *emitter = new particles::Emitter ();
                        const char *shaderPath = emitterTable.get("shader");

                        emitter->setLifeTime(math::clamp(emitterTable.get("lifeTime"), 0, 60000));
                        emitter->setFps(math::clamp(emitterTable.get("framesPerSecond"), 1, 1000));
                        emitter->setCycled(emitterTable.get("isCycled"));
                        emitter->setShader(shaderPath);

                        emitterTable.get("textureBinds").foreach([emitter](const luaObj &, const luaObj &textureBind){
                            const char *texturePath = textureBind;
                            emitter->addTextureBind(texturePath);
                            return true; // continue cycle
                        });

                        luaObj &axisCoord = emitterTable.get("torsionAxis");
                        emitter->setTorsionAxis(math::p3d(axisCoord.get(1), axisCoord.get(2), axisCoord.get(3)));

                        emitterTable.get("baseParams").foreach([emitter](const char *paramName, const luaObj &value) {
                            particles::EmitterParamType paramType;

                            if(value.type() == LUATYPE_NUMBER && __paramTypeConstTable.getConstant(paramName, paramType)) {
                                emitter->setParam(paramType, value);
                            }

                            return true; // continue cycle
                        });

                        emitterTable.get("emitterModifiers").foreach([emitter](const char *paramName, const luaObj &modifierParams) {
                            particles::EmitterParamType paramType;
                            particles::ModifierFunction modifierFuncType;

                            if(modifierParams.type() == LUATYPE_TABLE && __paramTypeConstTable.getConstant(paramName, paramType)) {
                                particles::ModifierInterface *mdf = emitter->createEmitterModifier(paramType);
                                
                                if(__paramTypeConstTable.getConstant(modifierParams.get("func"), modifierFuncType)) {
                                    mdf->setFunction(modifierFuncType);
                                    mdf->setYAxisLimit(modifierParams.get("minValue"), modifierParams.get("maxValue"));
                                }
                            }

                            return true; // continue cycle
                        });

                        emitterTable.get("particleModifiers").foreach([emitter](const char *paramName, const luaObj &modifierParams) {
                            particles::ParticleParamType paramType;
                            particles::ModifierFunction  modifierFuncType;

                            if(modifierParams.type() == LUATYPE_TABLE && __paramTypeConstTable.getConstant(paramName, paramType)) {
                                particles::ModifierInterface *mdf = emitter->createParticleModifier(paramType);

                                if(__paramTypeConstTable.getConstant(modifierParams.get("func"), modifierFuncType)) {
                                    mdf->setFunction(modifierFuncType);
                                    mdf->setYAxisLimit(modifierParams.get("minValue"), modifierParams.get("maxValue"));
                                }
                            }

                            return true; // continue cycle
                        });

                        emitter->build();
                        _emitters.add(emitterName, emitter);
                    }
                    else {
                        log.msgError("ParticleResource::loaded %s bad emitter format", _loadPath.data());
                    }

                    return true; // continue cycle
                });
            }
            else {
                log.msgError("ParticleResource::loaded %s - %s", _loadPath.data(), cfgSource.getLastError());
                _loadingState = ResourceLoadingState::INVALID;
            }
        }

        bool ParticleResource::constructed(const diag::LogInterface &log, platform::PlatformInterface &api) {
            
            return false;
        }

        void ParticleResource::unloaded() {
            _emitters.foreach([](const fg::string &, particles::Emitter *ptr) {
                delete ptr;
            });

            _emitters.clear();
        }
        
        void ParticleResource::getEmitters(std::vector <particles::EmitterInterface *> &out) const {
            out.clear();

            _emitters.foreach([&out](const fg::string &, particles::Emitter *ptr) {
                out.push_back(ptr);
            });
        }

        particles::EmitterInterface *ParticleResource::getEmitter(const fg::string &name) const {
            return _emitters.get(name);
        }
    }
}


