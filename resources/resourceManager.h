
// TODO: 
// part unloading
// part loading
// resFont code safe

#include <list>
#include <atomic>
#include <future>
#include <thread>
#include <memory>

//#define FG_RESOURCE_LONGNAME // def for complex projects
//#define FG_RESOURCE_UNLOADABLE // def for complex projects 

static const unsigned FG_UNLOAD_TIME      = 1000;
static const unsigned FG_RESOURCES_MAX    = 64;
static const unsigned FG_RES_LOADING_MAX  = 4;
static const unsigned FG_RES_UPDATING_MAX = 4;


namespace fg {
    namespace resources {
        class ResourceManager : public EngineResourceManagerInterface {
        public:
            ResourceManager(const diag::LogInterface &log, platform::EnginePlatformInterface &platform);
            ~ResourceManager() override;

            void loadResourcesDir(const fg::string &dir, const callback <void ()> &completeCb, bool unloadable) override;
            void loadResourcesList(const fg::string &resList, const callback <void ()> &completeCb, bool unloadable) override;

            void unloadResourcesDir(const fg::string &dir) override;
            void unloadResourcesList(const fg::string &resList) override;

            ResourcePtr createResource(const fg::string &fullpath) override;
            ResourcePtr getResource(const fg::string &path) const override;

            void init() override;
            void update(float frameTimeMs) override;
            void destroy() override;

        protected:
            const diag::LogInterface  &_log;
            platform::EnginePlatformInterface  &_platform;
            
            struct ResListFuture {
            public:
                std::future  <std::string *> future;
                std::string  content;
                bool isUnloadable;

                ResListFuture(bool unloadable) : isUnloadable(unloadable) {}
                ResListFuture(ResListFuture &&r) {
                    future = std::move(r.future);
                    content = std::move(r.content);
                    isUnloadable = r.isUnloadable;
                }

            private:
                ResListFuture(const ResListFuture &);
            };

            ResourceFactory _factory;

            std::list       <ResListFuture> _resListFutures;
            std::list       <std::future <ManagedResourceInterface *>> _resourceLoadingFutures;
            std::list       <std::future <ManagedResourceInterface *>> _resourceSavingFutures;

            callback        <void ()> _loadingCompleteCb;
            unsigned        _resListReady;
            unsigned        _elementInProgress;

            volatile bool   _allowLoadingResources;         
            std::atomic     <int> _futuresCount;
            fg::StaticHash  <FG_RESOURCES_MAX, ManagedResourceInterface *> _resources;
                        
            void _resListReadyCallback(const char *resList, bool unloadable);
            static int _getStringsFromPath(const char *path, char *outName, char *outExtension);
        };
    }
}

