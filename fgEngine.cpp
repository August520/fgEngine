
#include "pch.h"

#include "LuaScript.cpp"
#include "diag.cpp"
#include "math/math.cpp"
#include "math/mathUtility.cpp"
#include "string.cpp"

#include "tools/tools.h"
#include "tools/tools.cpp"

#include "resources/Resource.h"
#include "resources/TextResource.h"
#include "resources/ShaderResource.h"
#include "resources/ModelResource.h"
#include "resources/Texture2DResource.h"
#include "resources/FontResource.h"
#include "resources/MaterialResource.h"
#include "resources/ClipSetResource.h"
#include "resources/AnimationResource.h"
#include "resources/SoundResource.h"

#include "resources/Resource.cpp"
#include "resources/TextResource.cpp"
#include "resources/ShaderResource.cpp"
#include "resources/ModelResource.cpp"
#include "resources/Texture2DResource.cpp"
#include "resources/FontResource.cpp"
#include "resources/MaterialResource.cpp"
#include "resources/ClipSetResource.cpp"
#include "resources/AnimationResource.cpp"
#include "resources/SoundResource.cpp"

#include "resources/ResourceFactory.cpp"
#include "resources/ResourceManager.cpp"

#include "render/Camera.h"

#include "render/DisplayObjectIterator.h"
#include "render/DisplayObjectIterator.cpp"
#include "render/RenderObjectIterator.h"
#include "render/RenderObjectIterator.cpp"
#include "render/RenderSupport.cpp"
#include "render/Camera.cpp"
#include "render/DefaultRender.cpp"

#include "input/InputManager.cpp"



#include "particles/Modifier.cpp"
#include "particles/ParticleEmitter.cpp"

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

#include "Iterators.cpp"
#include "GameAPI.cpp"

const char *_binaryResources = "\
bin/simpleModel.shader\n\
bin/simpleSkin.shader\n\
bin/displayObject.shader\n\
bin/texturedModel.shader\n\
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
        int64 curDelta = curTime - _lastFrameTimeStamp;
        float frameTimeMs = float(curDelta);

        _lastFrameTimeStamp = curTime;
        _resMan.update(frameTimeMs);

        if(_isBaseResourcesLoaded) {
            if(_updateHandler.isBinded() && curDelta > 0) {
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
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _input.genPointerEvent(input::PointerEvent::PRESS, input::PointerEventArgs(pointID, pos.x, pos.y));
    }

    void Engine::pointerMoved(unsigned pointID, float pointX, float pointY) {
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _input.genPointerEvent(input::PointerEvent::MOVE, input::PointerEventArgs(pointID, pos.x, pos.y));
    }

    void Engine::pointerReleased(unsigned pointID, float pointX, float pointY) {
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _input.genPointerEvent(input::PointerEvent::RELEASE, input::PointerEventArgs(pointID, pos.x, pos.y));
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