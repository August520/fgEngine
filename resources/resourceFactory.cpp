
// TODO: unicode?

namespace fg {
    namespace resources {
        template <typename IFACE> struct ResourceInterfaceTable {
            typedef void *type;
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
            _creators.add("dds", [](const char *path, bool unloadable) {
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

        template <typename DOWNTYPE> DOWNTYPE *ResourcePtr::_to() const {
            return static_cast <DOWNTYPE *> (static_cast <typename ResourceInterfaceTable <typename std::remove_const <DOWNTYPE>::type> ::type *> (_resource));
        }

        template TextResourceInterface *ResourcePtr::_to <TextResourceInterface>() const;
        template ShaderResourceInterface *ResourcePtr::_to <ShaderResourceInterface>() const;
        template ModelResourceInterface *ResourcePtr::_to <ModelResourceInterface>() const;
        template Texture2DResourceInterface *ResourcePtr::_to <Texture2DResourceInterface>() const;
        template FontResourceInterface *ResourcePtr::_to <FontResourceInterface>() const;
        template MaterialResourceInterface *ResourcePtr::_to <MaterialResourceInterface>() const;
        template ClipSetResourceInterface *ResourcePtr::_to <ClipSetResourceInterface>() const;
        template AnimationResourceInterface *ResourcePtr::_to <AnimationResourceInterface>() const;
        template SoundResourceInterface *ResourcePtr::_to <SoundResourceInterface>() const;
        template ParticleResourceInterface *ResourcePtr::_to <ParticleResourceInterface>() const;

        template const TextResourceInterface *ResourcePtr::_to <const TextResourceInterface>() const;
        template const ShaderResourceInterface *ResourcePtr::_to <const ShaderResourceInterface>() const;
        template const ModelResourceInterface *ResourcePtr::_to <const ModelResourceInterface>() const;
        template const Texture2DResourceInterface *ResourcePtr::_to <const Texture2DResourceInterface>() const;
        template const FontResourceInterface *ResourcePtr::_to <const FontResourceInterface>() const;
        template const MaterialResourceInterface *ResourcePtr::_to <const MaterialResourceInterface>() const;
        template const ClipSetResourceInterface *ResourcePtr::_to <const ClipSetResourceInterface>() const;
        template const AnimationResourceInterface *ResourcePtr::_to <const AnimationResourceInterface>() const;
        template const SoundResourceInterface *ResourcePtr::_to <const SoundResourceInterface>() const;
        template const ParticleResourceInterface *ResourcePtr::_to <const ParticleResourceInterface>() const;

    }
}

