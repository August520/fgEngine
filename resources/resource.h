
// TODO: unicode?

namespace fg {
    namespace resources {
        class Resource : public ManagedResourceInterface {
        public:
            Resource(const char *path, bool unloadable);
            ~Resource() override;

            bool              valid() const override;
            bool              unloadable() const override;
            const fg::string  &getFilePath() const override;
            ResourceState     getState() const override;
            unsigned          getUnusedTimeMs() const override;
            void              setUnusedTimeMs(unsigned value) override;
            void              setState(ResourceState state) override;
            void              setBinary(void *binaryData, unsigned binarySize) override;
            void              freeBinary() override;

        protected:
            mutable unsigned  _unusedTime;
            ResourceState     _state;
            fg::string        _loadPath;
            bool              _unloadable;
            unsigned          _binarySize;
            void              *_binaryData;

        private:
            Resource(const Resource &);
            Resource &operator = (const Resource &);
        };
    }
}

