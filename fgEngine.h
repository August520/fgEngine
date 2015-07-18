
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new( _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

typedef long long int64;

#include "LuaScript.h"
#include "utility.h"
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
#include "render/Interfaces.h"

#include "resources/ResourceFactory.h"
#include "resources/ResourceManager.h"

#include "render/ShaderConst.h"
#include "render/TransparentDrawer.h"
#include "render/RenderSupport.h"
#include "render/DefaultRender.h"

#include "input/InputManager.h"

#include "Iterators.h"
#include "GameAPI.h"

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

    struct Logical2DCoordSystem {
        float width;
        float height;
        HorizontalAnchor coordStartX;
        VerticalAnchor coordStartY;
    };

    class Engine {
        friend struct GameAPI;

    public:
        Engine(diag::LogInterface &ilog, platform::EnginePlatformInterface &iplatform, resources::EngineResourceManagerInterface &iresMan, render::EngineRenderSupportInterface &irenderSupport, input::EngineInputManagerInterface &iinput);
        virtual ~Engine();
        
        // loading/resuming
        bool init(const platform::InitParams &initParams, render::RenderInterface &render, const Logical2DCoordSystem &coordSystem);

        // unloading/uninitialize
        void destroy();

        // call updates and render
        void updateAndDraw();

        // device orientation update
        void orientationChanged();

        // user input
        void pointerPressed(unsigned pointID, float pointX, float pointY);
        void pointerMoved(unsigned pointID, float pointX, float pointY);
        void pointerReleased(unsigned pointID, float pointX, float pointY);

        void keyDown(unsigned vkeyCode);
        void keyUp(unsigned vkeyCode);
        void wheelRoll(int sign);

        // user handlers
        void setInitHandler(const callback <void ()> &cb);
        void setUpdateHandler(const callback <void (float)> &cb);
        void setDestroyHandler(const callback <void ()> &cb);
        
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
        
        callback  <void ()> _initHandler;
        callback  <void (float)> _updateHandler;
        callback  <void ()> _destroyHandler;

        float  _appWidth;
        float  _appHeight;
        int64  _lastFrameTimeStamp;
        bool   _isBaseResourcesLoaded;
        float  _logicalScreenScaleFactorX = 1;
        float  _logicalScreenScaleFactorY = 1;

        void   _binaryResourcesLoadedCallback();
        void   _scalePos(math::p2d &target);
        
    private:
        Engine(const Engine &);
        Engine &operator =(const Engine &);
    };
}
