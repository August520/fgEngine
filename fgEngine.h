
#ifdef _MSC_VER
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#endif

typedef long long int64;

#include "utility.h"
#include "LuaScript.h"
#include "string.h"

#include "diag.h"

#include "math/math.h"
#include "math/mathUtility.h"

#include "Interfaces.h"
#include "platform/Interfaces.h"
#include "audio/Interfaces.h"
#include "particles/Interfaces.h"
#include "resources/Interfaces.h"
#include "input/Interfaces.h"
#include "objects2D/Interfaces.h"
#include "objects3D/Interfaces.h"

#include "render/OrderedCollection.h"
#include "render/Interfaces.h"

#include "resources/ResourceFactory.h"
#include "resources/ResourceManager.h"

#include "render/ShaderConst.h"
#include "render/TransparentDrawer.h"
#include "render/RenderSupport.h"
#include "render/DefaultRender.h"

#include "input/InputManager.h"
#include "pointers.h"

#ifdef FG_RESOURCE_LONGNAME
#define FG_SIMPLE_SHADER "bin/simpleModel.shader"
#define FG_IFACE_SHADER = "bin/displayObject.shader"
#else
#define FG_SIMPLE_SHADER "simpleModel.shader"
#define FG_IFACE_SHADER "displayObject.shader"
#endif

namespace fg {
    enum class HorizontalAnchor {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum class VerticalAnchor {
        TOP,
        MIDDLE,
        BOTTOM,
    };

    struct LogicalCoordSystem {
        float dpi = 1;
        float width = 0;
        float height = 0;
        HorizontalAnchor coordStartX = HorizontalAnchor::LEFT;
        VerticalAnchor coordStartY = VerticalAnchor::TOP;
    };

    class Engine {
        friend struct GameAPI;

    public:
        Engine(diag::LogInterface &ilog, platform::EnginePlatformInterface &iplatform, resources::EngineResourceManagerInterface &iresMan, render::EngineRenderSupportInterface &irenderSupport, input::EngineInputManagerInterface &iinput);
        virtual ~Engine();
        
        // loading/resuming
        bool  init(const platform::InitParams &initParams, render::RenderInterface &render, const LogicalCoordSystem &coordSystem, const fg::string &binaryFolder = fg::string("bin"));

        // unloading/uninitialize
        void  destroy();

        // call updates and render
        void  updateAndDraw();

        // device orientation update
        void  orientationChanged();

        // uwp window size changed
        void  sizeChanged(float width, float height);

        // user input
        void  pointerPressed(unsigned pointID, float pointX, float pointY);
        void  pointerMoved(unsigned pointID, float pointX, float pointY);
        void  pointerReleased(unsigned pointID, float pointX, float pointY);

        void  keyDown(unsigned vkeyCode);
        void  keyUp(unsigned vkeyCode);
        void  wheelRoll(int sign);

        // user handlers
        void  setInitHandler(const callback <void ()> &cb);
        void  setUpdateHandler(const callback <void (float)> &cb);
        void  setDestroyHandler(const callback <void ()> &cb);

        template <typename F, typename ...ARGS> void postponedDispatch(F func, ARGS... args) {
            _postponed.emplace_back(std::bind(func, args...));
        }
        
        float getCoordSystemWidth() const;
        float getCoordSystemHeight() const;
        float getCoordSystemDPIFactorX() const;
        float getCoordSystemDPIFactorY() const;

    protected:
        fg::diag::LogInterface                      &_log;
        platform::EnginePlatformInterface           &_platform;
        resources::EngineResourceManagerInterface   &_resMan;
        render::EngineRenderSupportInterface        &_renderSupport;
        render::RenderInterface                     *_render;
        input::EngineInputManagerInterface          &_input;
        object2d::DisplayObjectInterface            *_root2D;
        object3d::RenderObjectInterface             *_root3D;
        render::CameraInterface                     *_gameCamera;
        render::EngineSceneCompositionInterface     *_sceneComposition;
        
        callback  <void ()> _initHandler;
        callback  <void (float)> _updateHandler;
        callback  <void ()> _destroyHandler;
        
        int64  _lastFrameTimeStamp;
        bool   _isBaseResourcesLoaded;
        float  _screenPixelsPerCoordSystemPixelsX = 1;
        float  _screenPixelsPerCoordSystemPixelsY = 1;
        float  _systemDpiPerCoordSystemDpi;
        
        LogicalCoordSystem _coordSystem;
        std::list <std::function<void()>> _postponed;

        void   _binaryResourcesLoadedCallback();
        void   _scalePos(math::p2d &target);
        void   _updateCoordSystem();
        
    private:
        Engine(const Engine &);
        Engine &operator =(const Engine &);
    };
}

#include "GameAPI.h"