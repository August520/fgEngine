
#include "pch.h"

#include "luaScript.cpp"
#include "diag.cpp"
#include "math/math.cpp"
#include "math/mathUtility.cpp"
#include "string.cpp"

#include "tools/tools.h"
#include "tools/tools.cpp"

#include "resources/resource.h"
#include "resources/textResource.h"
#include "resources/shaderResource.h"
#include "resources/modelResource.h"
#include "resources/texture2DResource.h"
#include "resources/fontResource.h"
#include "resources/materialResource.h"
#include "resources/clipSetResource.h"
#include "resources/animationResource.h"
#include "resources/soundResource.h"

#include "resources/resource.cpp"
#include "resources/textResource.cpp"
#include "resources/shaderResource.cpp"
#include "resources/modelResource.cpp"
#include "resources/texture2DResource.cpp"
#include "resources/fontResource.cpp"
#include "resources/materialResource.cpp"
#include "resources/clipSetResource.cpp"
#include "resources/animationResource.cpp"
#include "resources/soundResource.cpp"

#include "resources/resourceFactory.cpp"
#include "resources/resourceManager.cpp"

#include "render/camera.h"

#include "render/displayObjectIterator.h"
#include "render/displayObjectIterator.cpp"
#include "render/renderObjectIterator.h"
#include "render/renderObjectIterator.cpp"
#include "render/renderSupport.cpp"
#include "render/camera.cpp"
#include "render/defaultRender.cpp"

#include "input/InputManager.cpp"

#include "objects2D/DisplayObject.h"
#include "objects2D/Sprite2D.h"
#include "objects2D/TextField.h"
#include "objects2D/Particles.h"
#include "objects2D/Panel.h"

#include "objects2D/DisplayObject.cpp"
#include "objects2D/Sprite2D.cpp"
#include "objects2D/TextField.cpp"
#include "objects2D/Particles.h"
#include "objects2D/Panel.h"

#include "objects3D/RenderObject.h"
#include "objects3D/Model.h"
#include "objects3D/Particles.h"
#include "objects3D/Billboard.h"

#include "objects3D/RenderObject.cpp"
#include "objects3D/Model.cpp"
#include "objects3D/Particles.cpp"
#include "objects3D/Billboard.cpp"

#include "iterators.cpp"
#include "gameAPI.cpp"

const char *_binaryResources = "\
bin/simpleModel.shader\n\
bin/simpleSkin.shader\n\
bin/displayObject.shader\n\
bin/lightedModel.shader\n\
bin/lightedNormalModel.shader\n\
bin/lightedSkin.shader\n\
bin/lightedNormalSkin.shader\n\
bin/lightedTexturedModel.shader\n\
bin/lightedTexturedNormalModel.shader\n\
bin/lightedTexturedNormalSkin.shader\n\
bin/texturedScreenQuad.shader\n\
bin/texturedScreenQuadFilter.shader\n\
bin/arial.ttf\n\
"; //


namespace fg {
    Engine::Engine(fg::diag::LogInterface &ilog, platform::EnginePlatformInterface &iplatform, resources::EngineResourceManagerInterface &iresMan, render::EngineRenderSupportInterface &irenderSupport, input::EngineInputManagerInterface &iinput) :
        _log(ilog), 
        _platform(iplatform), 
        _resMan(iresMan), 
        _renderSupport(irenderSupport),
        _input(iinput),
        _root3D(nullptr), 
        _root2D(nullptr) 
    {
        _isBaseResourcesLoaded = false;
        _lastFrameTimeStamp = _platform.getTimeMs();
    }

    Engine::~Engine() {

    }
    
    bool Engine::init(platform::InitParams &initParams, render::RenderInterface &render) {
        _log.msgInfo("init..");
        _appWidth = initParams.scrWidth;
        _appHeight = initParams.scrHeight;

        _render = &render;

        if(_platform.init(initParams) == false) {
            _log.msgError("platform init error");
            return false;
        }

        const char *renderResourceList = _render->getRenderResourceList();
        
        _log.msgInfo("loading resources..");
        _resMan.init();
        _resMan.loadResourcesList(_binaryResources, nullptr, false);
        _resMan.loadResourcesList(renderResourceList, callback <void ()> (this, &Engine::_binaryResourcesLoadedCallback), false);
    
        int64 curTime = _platform.getTimeMs();
        _lastFrameTimeStamp = curTime;

        _root2D = new object2d::DisplayObject ();
        _root3D = new object3d::RenderObject();
        _gameCamera = new render::Camera (_platform);

        return true;
    }

    void Engine::destroy() {
        if(_destroyHandler.isBinded()) {
            _destroyHandler();
        }
        
        _render->destroy();
        _resMan.destroy();
        _renderSupport.destroy();
        _platform.destroy();

        _isBaseResourcesLoaded = false;

        delete _root2D;
        delete _root3D;
        delete _gameCamera;

        _root2D = nullptr;
        _root3D = nullptr;
        _gameCamera = nullptr;
    }

    void Engine::updateAndDraw() {
        int64 curTime = _platform.getTimeMs();
        float frameTimeMs = float(curTime - _lastFrameTimeStamp);

        _lastFrameTimeStamp = curTime;
        _resMan.update(frameTimeMs);

        if(_isBaseResourcesLoaded) {
            if(_updateHandler.isBinded()) {
                _updateHandler(frameTimeMs);
            }

            _render->update(frameTimeMs, render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera));
            _renderSupport.getCamera().set(*_gameCamera);

            _renderSupport.frameInit3D(frameTimeMs);            
            _render->draw3D(object3d::RenderObjectIterator(_root3D, _platform, _resMan, frameTimeMs), render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera));

            _renderSupport.frameInit2D(frameTimeMs);
            _render->draw2D(object2d::DisplayObjectIterator(_root2D, _platform, _resMan, frameTimeMs), render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera));
        }

        _platform.rdPresent();
    }

    void Engine::orientationChanged() {
        _platform.updateOrientation();
    }

    //---

    void Engine::_binaryResourcesLoadedCallback() {
        _log.msgInfo("binary resources loaded"); 
        _renderSupport.init(_platform, _resMan);
        _render->init(render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera));

        if(_initHandler.isBinded()) {
            _initHandler();
        }

        _isBaseResourcesLoaded = true;
    }

    //---

    void Engine::pointerPressed(unsigned pointID, float pointX, float pointY) {
        _input.genPointerEvent(input::PointerEvent::PRESS, input::PointerEventArgs(pointID, pointX, pointY));
    }

    void Engine::pointerMoved(unsigned pointID, float pointX, float pointY) {
        _input.genPointerEvent(input::PointerEvent::MOVE, input::PointerEventArgs(pointID, pointX, pointY));
    }

    void Engine::pointerReleased(unsigned pointID, float pointX, float pointY) {
        _input.genPointerEvent(input::PointerEvent::RELEASE, input::PointerEventArgs(pointID, pointX, pointY));
    }

    void Engine::keyDown(unsigned vkeyCode) {
        _input.genKeyboardEvent(input::KeyboardEvent::DOWN, input::KeyboardEventArgs(vkeyCode));
    }

    void Engine::keyUp(unsigned vkeyCode) {
        _input.genKeyboardEvent(input::KeyboardEvent::UP, input::KeyboardEventArgs(vkeyCode));
    }

    void Engine::wheelRoll(int sign) {
        _input.genMiscEvent(input::MiscEvent::MOUSE_WHEEL, sign);
    }
    
    void Engine::setInitHandler(const callback <void()> &cb) {
        _initHandler = cb;
    }

    void Engine::setUpdateHandler(const callback <void(float)> &cb) {
        _updateHandler = cb;
    }

    void Engine::setDestroyHandler(const callback <void()> &cb) {
        _destroyHandler = cb;
    }
}


//