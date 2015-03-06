
// TODO: unicode?

namespace fg {
    namespace resources {
        template <typename IFACE> struct ResourceInterfaceTable {
        };
        template <> struct ResourceInterfaceTable <TextResourceInterface> {
            typedef TextResource type;
        };
        template <> struct ResourceInterfaceTable <ShaderResourceInterface> {
            typedef ShaderResource type;
        };
        template <> struct ResourceInterfaceTable <ModelResourceInterface> {
            typedef ModelResource type;
        };
        template <> struct ResourceInterfaceTable <Texture2DResourceInterface> {
            typedef Texture2DResource type;
        };
        template <> struct ResourceInterfaceTable <FontResourceInterface> {
            typedef FontResource type;
        };
        template <> struct ResourceInterfaceTable <MaterialResourceInterface> {
            typedef MaterialResource type;
        };
        template <> struct ResourceInterfaceTable <ClipSetResourceInterface> {
            typedef ClipSetResource type;
        };
        template <> struct ResourceInterfaceTable <AnimationResourceInterface> {
            typedef AnimationResource type;
        };
        template <> struct ResourceInterfaceTable <SoundResourceInterface> {
            typedef SoundResource type;
        };
        template <> struct ResourceInterfaceTable <ParticleResourceInterface> {
            typedef ParticleResource type;
        };

        ResourceFactory::ResourceFactory() {
            _creators.add("lua", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new TextResource(path, unloadable));
            });
            _creators.add("desc", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new TextResource(path, unloadable));
            });
            _creators.add("txt", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new TextResource(path, unloadable));
            });
            _creators.add("shader", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new ShaderResource(path, unloadable));
            });
            _creators.add("mdl", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new ModelResource(path, unloadable));
            });
            _creators.add("tex", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new Texture2DResource(path, unloadable));
            });
            _creators.add("png", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new Texture2DResource(path, unloadable));
            });
            _creators.add("ttf", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new FontResource(path, unloadable));
            });
            _creators.add("mt", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new MaterialResource(path, unloadable));
            });
            _creators.add("clipset", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new ClipSetResource(path, unloadable));
            });
            _creators.add("man", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new AnimationResource(path, unloadable));
            });
            _creators.add("ogg", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new SoundResource(path, unloadable));
            });
            _creators.add("ptc", [](const char *path, bool unloadable) {
                return static_cast <ManagedResourceInterface *> (new ParticleResource(path, unloadable));
            });
        }

        ManagedResourceInterface *ResourceFactory::createResource(const fg::string &ext, const char *path, bool unloadable) const {
            auto creator = _creators.get(ext);

            if(creator) {
                return creator(path, unloadable);
            }
            return nullptr;
        }

        template <typename DOWNTYPE> ResourcePtr::operator DOWNTYPE *() const {
            return static_cast <DOWNTYPE *> (static_cast <ResourceInterfaceTable <std::remove_const <DOWNTYPE>::type> ::type *> (_resource));
        }

        template ResourcePtr::operator const TextResourceInterface *() const;
        template ResourcePtr::operator const ShaderResourceInterface *() const;
        template ResourcePtr::operator const ModelResourceInterface *() const;
        template ResourcePtr::operator const Texture2DResourceInterface *() const;
        template ResourcePtr::operator const FontResourceInterface *() const;
        template ResourcePtr::operator const MaterialResourceInterface *() const;
        template ResourcePtr::operator const ClipSetResourceInterface *() const;
        template ResourcePtr::operator const AnimationResourceInterface *() const;
        template ResourcePtr::operator const SoundResourceInterface *() const;
        template ResourcePtr::operator const ParticleResourceInterface *() const;
        
        template ResourcePtr::operator TextResourceInterface *() const;
        template ResourcePtr::operator ShaderResourceInterface *() const;
        template ResourcePtr::operator ModelResourceInterface *() const;
        template ResourcePtr::operator Texture2DResourceInterface *() const;
        template ResourcePtr::operator FontResourceInterface *() const;
        template ResourcePtr::operator MaterialResourceInterface *() const;
        template ResourcePtr::operator ClipSetResourceInterface *() const;
        template ResourcePtr::operator AnimationResourceInterface *() const;
        template ResourcePtr::operator SoundResourceInterface *() const;
        template ResourcePtr::operator ParticleResourceInterface *() const;
    }
}

