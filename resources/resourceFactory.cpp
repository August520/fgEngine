
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
        }

        ManagedResourceInterface *ResourceFactory::createResource(const fg::string &ext, const char *path, bool unloadable) const {
            auto creator = _creators.get(ext);

            if(creator) {
                return creator(path, unloadable);
            }
            return nullptr;
        }

        template <typename DOWNTYPE> ResourcePtr::operator const DOWNTYPE *() {
            return static_cast <const DOWNTYPE *> (static_cast <const ResourceInterfaceTable <DOWNTYPE> ::type *> (_resource));
        }

        template ResourcePtr::operator const TextResourceInterface *();
        template ResourcePtr::operator const ShaderResourceInterface *();
        template ResourcePtr::operator const ModelResourceInterface *();
        template ResourcePtr::operator const Texture2DResourceInterface *();
        template ResourcePtr::operator const FontResourceInterface *();
        template ResourcePtr::operator const MaterialResourceInterface *();
        template ResourcePtr::operator const ClipSetResourceInterface *();
        template ResourcePtr::operator const AnimationResourceInterface *();
        template ResourcePtr::operator const SoundResourceInterface *();

    }
}

