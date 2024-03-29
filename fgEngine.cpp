
#include "pch.h"

#include <unordered_set>
#include <regex>

#include "LuaScript.cpp"
#include "diag.cpp"
#include "math/math.cpp"
#include "math/mathUtility.cpp"
#include "string.cpp"

#include "tools/tools.h"
#include "tools/tools.cpp"

#include "particles/Modifier.h"
#include "particles/ParticleEmitter.h"

#include "resources/Resource.h"
#include "resources/TextResource.h"
#include "resources/ShaderResource.h"
#include "resources/ModelResource.h"
#include "resources/Texture2DResource.h"
#include "resources/TextureCubeResource.h"
#include "resources/FontResource.h"
#include "resources/MaterialResource.h"
#include "resources/ClipSetResource.h"
#include "resources/AnimationResource.h"
#include "resources/SoundResource.h"
#include "resources/ParticleResource.h"

#include "resources/Resource.cpp"
#include "resources/TextResource.cpp"
#include "resources/ShaderResource.cpp"
#include "resources/ModelResource.cpp"
#include "resources/Texture2DResource.cpp"
#include "resources/TextureCubeResource.cpp"
#include "resources/FontResource.cpp"
#include "resources/MaterialResource.cpp"
#include "resources/ClipSetResource.cpp"
#include "resources/AnimationResource.cpp"
#include "resources/SoundResource.cpp"
#include "resources/ParticleResource.cpp"

#include "resources/ResourceFactory.cpp"
#include "resources/ResourceManager.cpp"

#include "render/Camera.h"

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
#include "objects3D/PointLight.h"

#include "objects3D/RenderObject.cpp"
#include "objects3D/Model.cpp"
#include "objects3D/Particles.cpp"
#include "objects3D/PointLight.cpp"

#include "render/SceneComposition.h"
#include "render/SceneComposition.cpp"

#include "GameAPI.cpp"

const char *_binaryResources = "\
$/displayObject.shader\n\
$/simpleModel.shader\n\
$/simpleSkin.shader\n\
$/texturedModel.shader\n\
$/texturedSkin.shader\n\
$/lightedModel.shader\n\
$/lightedSkin.shader\n\
$/lightedNormalModel.shader\n\
$/lightedNormalSkin.shader\n\
$/lightedTexturedModel.shader\n\
$/lightedTexturedSkin.shader\n\
$/lightedTexturedNormalModel.shader\n\
$/lightedTexturedNormalSkin.shader\n\
$/texturedScreenQuad.shader\n\
$/texturedScreenQuadFilter.shader\n\
$/defaultIrradiance.cubemap\n\
$/defaultEnvironment0.cubemap\n\
$/defaultEnvironment1.cubemap\n\
$/defaultEnvironment2.cubemap\n\
$/defaultEnvironment3.cubemap\n\
$/defaultEnvironment4.cubemap\n\
$/defaultEnvironment5.cubemap\n\
$/arial.ttf\n\
"; //


namespace fg {
    Engine::Engine(fg::diag::LogInterface &ilog, platform::EnginePlatformInterface &iplatform, resources::EngineResourceManagerInterface &iresMan, render::EngineRenderSupportInterface &irenderSupport, input::EngineInputManagerInterface &iinput) :
        _log(ilog), 
        _platform(iplatform), 
        _resMan(iresMan), 
        _renderSupport(irenderSupport),
        _input(iinput),
        _root3D(nullptr), 
        _root2D(nullptr),
        _gameCamera(nullptr),
        _sceneComposition(nullptr)
    {
        _isBaseResourcesLoaded = false;
        _lastFrameTimeStamp = _platform.getTimeMs();
    }

    Engine::~Engine() {

    }
    
    bool Engine::init(const platform::InitParams &initParams, render::RenderInterface &render, const LogicalCoordSystem &coordSystem, const fg::string &binaryFolder) {
        _log.msgInfo("init..");
        _render = &render;

        if(_platform.init(initParams) == false) {
            _log.msgError("platform init error");
            return false;
        }

        _gameCamera = new render::Camera(_platform);
        _sceneComposition = new render::SceneComposition(_platform, _resMan, _gameCamera);
        _coordSystem = coordSystem;
        _systemDpiPerCoordSystemDpi = initParams.dpi / coordSystem.dpi;
        _updateCoordSystem();
        
        const char *renderResourceList = _render->getRenderResourceList();
        std::string binaryResources = _binaryResources;        
        binaryResources = std::regex_replace(binaryResources, std::regex("\\$"), binaryFolder.data());
        
        _log.msgInfo("loading resources..");
        _resMan.init();
        _resMan.loadResourcesList(fg::string(binaryResources.c_str(), binaryResources.length()), nullptr, false);
        _resMan.loadResourcesList(renderResourceList, callback <void ()> (this, &Engine::_binaryResourcesLoadedCallback), false);
    
        int64 curTime = _platform.getTimeMs();
        _lastFrameTimeStamp = curTime;

        _root2D = new object2d::DisplayObject (_sceneComposition);
        _root3D = new object3d::RenderObject (_sceneComposition);
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
        delete _sceneComposition;

        _root2D = nullptr;
        _root3D = nullptr;
        _gameCamera = nullptr;
        _sceneComposition = nullptr;
    }

    void Engine::updateAndDraw() {
        int64 curTime = _platform.getTimeMs();
        int64 curDelta = curTime - _lastFrameTimeStamp;
        float frameTimeMs = std::min(float(curDelta), 40.0f);

        _lastFrameTimeStamp = curTime;
        _resMan.update(frameTimeMs);

        if(_isBaseResourcesLoaded) {
            while(_postponed.empty() == false) {
                _postponed.front()();
                _postponed.pop_front();
            }
            
            if(_updateHandler.isBinded() && curDelta > 0) {
                _updateHandler(frameTimeMs);
            }

            render::RenderAPI &&api = render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera);

            _sceneComposition->update(frameTimeMs);
            _render->update(frameTimeMs, api);            
            _renderSupport.getCamera().set(*_gameCamera);

            _renderSupport.frameInit3D(frameTimeMs);                        
            _render->draw3D(*_sceneComposition, api);            

            _renderSupport.frameInit2D(frameTimeMs, _screenPixelsPerCoordSystemPixelsX, _screenPixelsPerCoordSystemPixelsY, _systemDpiPerCoordSystemDpi);
            _render->draw2D(*_sceneComposition, api);
        }

        _platform.rdPresent();
    }

    void Engine::orientationChanged() {
        _platform.updateOrientation();
        _updateCoordSystem();
    }

    void Engine::sizeChanged(float width, float height) {
        _platform.resize(width, height);
        _updateCoordSystem();
    }

    //---

    void Engine::_binaryResourcesLoadedCallback() {
        _log.msgInfo("binary resources loaded"); 
        _renderSupport.init(_platform, _resMan);
        
        render::RenderAPI &&api = render::RenderAPI(_platform, _resMan, _renderSupport, *_gameCamera);
        _render->init(api);

        if(_initHandler.isBinded()) {
            _initHandler();
        }

        _isBaseResourcesLoaded = true;
    }

    //---

    void Engine::_scalePos(math::p2d &target) {
        if(_screenPixelsPerCoordSystemPixelsX < 0.0f) {
            target.x = _platform.getScreenWidth() - target.x;
        }
        if(_screenPixelsPerCoordSystemPixelsY < 0.0f) {
            target.y = _platform.getScreenHeight() - target.y;
        }

        target.x = target.x / fabs(_screenPixelsPerCoordSystemPixelsX);
        target.y = target.y / fabs(_screenPixelsPerCoordSystemPixelsY);
    }

    void Engine::_updateCoordSystem() {
        _screenPixelsPerCoordSystemPixelsX = _platform.getScreenWidth() / _coordSystem.width * (_coordSystem.coordStartX == HorizontalAnchor::RIGHT ? -1.0f : 1.0f);
        _screenPixelsPerCoordSystemPixelsY = _platform.getScreenHeight() / _coordSystem.height * (_coordSystem.coordStartY == VerticalAnchor::TOP ? 1.0f : -1.0f);
        _gameCamera->updateMatrix();
    }

    void Engine::pointerPressed(unsigned pointID, float pointX, float pointY) {
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _scalePos(pos);
        _input.genPointerEvent(input::PointerEvent::PRESS, input::PointerEventArgs(pointID, pos.x, pos.y));
    }

    void Engine::pointerMoved(unsigned pointID, float pointX, float pointY) {
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _scalePos(pos);
        _input.genPointerEvent(input::PointerEvent::MOVE, input::PointerEventArgs(pointID, pos.x, pos.y));
    }

    void Engine::pointerReleased(unsigned pointID, float pointX, float pointY) {
        math::p2d pos = math::p2d(pointX, pointY).transform(_platform.getInputTransform(), true);
        _scalePos(pos);
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
    
    float Engine::getCoordSystemWidth() const {
        return _coordSystem.width;
    }

    float Engine::getCoordSystemHeight() const {
        return _coordSystem.height;
    }

    float Engine::getCoordSystemDPIFactorX() const {
        return _systemDpiPerCoordSystemDpi / _screenPixelsPerCoordSystemPixelsX;
    }

    float Engine::getCoordSystemDPIFactorY() const {
        return _systemDpiPerCoordSystemDpi / _screenPixelsPerCoordSystemPixelsY;
    }
}


//