
namespace fg {
    namespace resources {
        static const unsigned FG_RESTYPE_MAX = 16;

        template <typename IFACE> struct ResourceInterfaceTable {
            typedef void type;
        };

        class ResourceFactory final {
        public:
            ResourceFactory();
            ManagedResourceInterface *createResource(const fg::string &ext, const char *path, bool unloadable) const;

        private:
            fg::StaticHash <FG_RESTYPE_MAX, ManagedResourceInterface *(*)(const char *, bool)> _creators;

            ResourceFactory(const ResourceFactory &);
            ResourceFactory &operator =(const ResourceFactory &);
        };
    }
}

