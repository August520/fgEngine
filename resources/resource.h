
// TODO: unicode?

namespace fg {
    namespace resources {
        class Resource : public ManagedResourceInterface {
        public:
            Resource(const char *path, bool unloadable);
            ~Resource() override;

            bool  valid() const override;
            bool  unloadable() const override;
            
            const fg::string     &getFilePath() const override;
            ResourceLoadingState getLoadingState() const override;
            ResourceSavingState  getSavingState() const override;
            
            unsigned getUnusedTimeMs() const override;

            void  setUnusedTimeMs(unsigned value) override;
            void  setLoadingState(ResourceLoadingState state) override;
            void  setSavingState(ResourceSavingState state) override;
            void  setBinary(void *binaryData, unsigned binarySize) override;
            void  freeBinary() override;

            bool  commit() override;

        protected:
            mutable unsigned     _unusedTime;
            ResourceLoadingState _loadingState;
            ResourceSavingState  _savingState;

            fg::string  _loadPath;
            bool        _unloadable;
            unsigned    _binarySize;
            void        *_binaryData;

        private:
            Resource(const Resource &);
            Resource &operator = (const Resource &);
        };
    }
}

