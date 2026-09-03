#include "MlWrapper.h"

#include <windows.h>
#include <algorithm>

#include "GlobalFunctions.h"
#include "MlEvent.h"
#include "MgeDrawable.h"
#include "MgeText.h"
#include "MgeImage.h"
#include "GlobalEvents.h"

std::shared_ptr<sf::RenderWindow> mainWindow;

namespace ML_wrapper
{
    MlWrapper::MlWrapper()
    {
        if (!mainWindow)
        {
            //#define USE_FULLSCREEN //uncomment to enable fullscreen mode in debug (for testing purposes)
#if defined(NDEBUG) || defined(USE_FULLSCREEN)
            // Fullscreen mode with current desktop resolution
            auto dm = sf::VideoMode::getDesktopMode();
            mainWindow = std::make_shared<sf::RenderWindow>(dm, "Train siding manager", sf::Style::Default, sf::State::Fullscreen);
#else
            sf::Vector2u windowSize{ 1280, 720 };
            mainWindow = std::make_shared<sf::RenderWindow>(sf::VideoMode(windowSize), "Train siding manager", sf::Style::Default, sf::State::Windowed);
#endif 
            auto size = mainWindow->getSize();

            // World view (zoomable)
            worldView.setSize(sf::Vector2f((float)size.x, (float)size.y));
            worldView.setCenter(worldView.getSize() / 2.f);

            // GUI view (pixel perfect, no zoom)
            guiView = mainWindow->getDefaultView();
        }
        else
        {
            _ASSERT(false); //trying to run game duplicately
        }
    }

    const FPoint& MlWrapper::getCursorWorldPosition() const noexcept
    {
        return cursorWorldPosition;
    }

    const IPoint& MlWrapper::getCursorGuiPosition() const noexcept
    {
        return cursorGuiPosition;
    }


    void MlWrapper::addMgeLayerObject(const std::shared_ptr<MgeLayerObject> newObject)
    {
        MAIN_THREAD_GUARD;
        _ASSERT(newObject);
        if (!newObject)
            return;

        createLayerIfNeeded(newObject->m_layer);
        auto it_layer = std::find_if(layers.begin(), layers.end(), [this, newObject](auto& layer)
            { return layer.m_layerNumber == newObject->m_layer; });


        _ASSERT(it_layer != layers.end());
        if (it_layer == layers.end())
            return;

        it_layer->m_layerObjects.push_back(newObject);
    }

    mgeType::Size<int> MlWrapper::getScreenSize() const noexcept
    {
        return mgeType::Size<int>((int)guiView.getSize().x, (int)guiView.getSize().y);
    }

    void MlWrapper::moveScreenOffset(FPoint offset) noexcept
    {
		worldView.move(sf::Vector2f(static_cast<float>(offset.x), static_cast<float>(offset.y)));
    }

    void MlWrapper::setMaxScreenZoom(double zoomFactor) noexcept
    {
        if (zoomFactor > 0.0)
			maxZoom = zoomFactor;
    }

    void MlWrapper::setMinScreenZoom(double zoomFactor) noexcept
    {
        if (zoomFactor > 0.0 && zoomFactor < maxZoom)
			minZoom = zoomFactor;
    }

    void MlWrapper::setZoom(double zoomFactor)
    {
        if (zoomFactor > 0.0 && zoomFactor <= maxZoom && zoomFactor >= minZoom)
        {
            double factor = zoomFactor / zoom;
            zoom = zoomFactor;
            worldView.zoom(static_cast<float>(factor));
		}
    }

    [[nodiscard]] double MlWrapper::getZoom() const noexcept
    {
        return zoom;
	}

    void MlWrapper::pollMlEvents()
    {
        //cursor position
        sf::Vector2i mousePixel = sf::Mouse::getPosition(*mainWindow);
        sf::Vector2f mouseWorld = mainWindow->mapPixelToCoords(mousePixel, worldView);
        sf::Vector2f mouseGui = mainWindow->mapPixelToCoords(mousePixel, guiView);
        cursorWorldPosition = FPoint
        (
            static_cast<float>(std::lround(mouseWorld.x)),
            static_cast<float>(std::lround(mouseWorld.y))
		);
        cursorGuiPosition = IPoint
            (
                static_cast<int>(std::lround(mouseGui.x)),
                static_cast<int>(std::lround(mouseGui.y))
            );

        auto toWorldPoint = [&](const sf::Vector2i& pixel)
            {
                sf::Vector2f world = mainWindow->mapPixelToCoords(pixel, mainWindow->getView());
                return IPoint
                (
                    static_cast<int>(std::lround(world.x)),
                    static_cast<int>(std::lround(world.y))
                );
            };

        while (const std::optional event = mainWindow->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                sendEvent(std::move(AppTermination{}));
                return;
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                IPoint pos = toWorldPoint(mousePressed->position);

                if (mousePressed->button == sf::Mouse::Button::Left)
                    sendEvent(MouseClick{ pos, PRESSED, MlEventTypeEnum::MouseLeftClick });
                else if (mousePressed->button == sf::Mouse::Button::Right)
                    sendEvent(MouseClick{ pos, PRESSED, MlEventTypeEnum::MouseRightClick });
                else if (mousePressed->button == sf::Mouse::Button::Middle)
                    sendEvent(MouseClick{ pos, PRESSED, MlEventTypeEnum::WheelMiddleButton });
            }
            else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                IPoint pos = toWorldPoint(mouseReleased->position);

                if (mouseReleased->button == sf::Mouse::Button::Left)
                    sendEvent(MouseClick{ pos, RELEASED, MlEventTypeEnum::MouseLeftClick });
                else if (mouseReleased->button == sf::Mouse::Button::Right)
                    sendEvent(MouseClick{ pos, RELEASED, MlEventTypeEnum::MouseRightClick });
				else if (mouseReleased->button == sf::Mouse::Button::Middle)
                    sendEvent(MouseClick{ pos, RELEASED, MlEventTypeEnum::WheelMiddleButton });
            }
            else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
                sendEvent(WheelScroll{wheel->delta > 0 ? 1 : -1 });
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                const auto size = mainWindow->getSize();

                sf::Vector2f newSize
                    (
                        static_cast<float>(size.x),
                        static_cast<float>(size.y)
                    );

                // --- GUI VIEW ---
                guiView.setSize(newSize);
                guiView.setCenter(newSize / 2.f);

                // --- WORLD VIEW ---
                // Keep zoom, adapt view size to window
                worldView.setSize(newSize);
                worldView.setCenter(newSize / 2.f);
                worldView.zoom((float)zoom);

                sendEvent(std::move(resizeWindowEvent{}));
            }
        }

        _ASSERT(mainWindow);
        if (!mainWindow)
            return;
    }

    void MlWrapper::setDrawToGuiView() const
    {
        _ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->setView(guiView);
    }

    void MlWrapper::setDrawToWorldView() const
    {
        _ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->setView(worldView);
    }

    void MlWrapper::clear()
    {
		_ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->clear();
    }

    void MlWrapper::displayActualFrame()
    {
        //window->setFramerateLimit(60); //set specific FPS, disabled for now - due to cause lags
        mainWindow->display();
    }

    void MlWrapper::drawContent()
    {
        setDrawToWorldView();
        for (auto& layer : layers)
        {
            if (layer.m_layerNumber == GraphicItemLayer::UNRENDERED_LAYER)
                continue;

            doSortLayerIfNeeded(layer);

            if (layer.m_layerNumber == GraphicItemLayer::GUI_LAYER)
                setDrawToGuiView();

            const auto currentView = mainWindow->getView();
            auto x = currentView.getCenter().x;
            auto y = currentView.getCenter().y;

            for (auto& objectInLayer : layer.m_layerObjects)
            {
                _ASSERT(objectInLayer);
                if (objectInLayer)
                {
                    if (std::holds_alternative<MlVerticesObject>(objectInLayer->data))
                    {
                        auto& vertices = std::get<MlVerticesObject>(objectInLayer->data);
                        mainWindow->draw(vertices.m_batch, vertices.m_state);
                    }
                    else if (std::holds_alternative<sf::Sprite>(objectInLayer->data))
                    {
                        auto& sprite = std::get<sf::Sprite>(objectInLayer->data);
                        mainWindow->draw(sprite);
                    }
                    else if (std::holds_alternative<sf::Text>(objectInLayer->data))
                    {
                        auto& text = std::get<sf::Text>(objectInLayer->data);
                        mainWindow->draw(text);
                    }
                    else
                        _ASSERT(false); //unknown type
                        
                }
            }

			//back to world view for next layer
            if (layer.m_layerNumber == GraphicItemLayer::GUI_LAYER)
                setDrawToWorldView();
        }
    }

    MlWrapper::~MlWrapper()
    {
#ifdef _DEBUG
        for (const auto& layer : layers)
        {
            for (auto& objectInLayer : layer.m_layerObjects)
            {
                _ASSERT(objectInLayer);
                if (objectInLayer)
                    _ASSERT(objectInLayer.use_count() == 1); //correct behavior = remove last instance
            }
        }
#endif
        layers.clear();
    }

    void MlWrapper::resetMainWindow()
    {
		mainWindow.reset();
    }

    void MlWrapper::removeMgeLayerObject(const std::shared_ptr<MgeLayerObject> removedObject)
    {
        _ASSERT(removedObject);
        if (!removedObject)
            return;

        MAIN_THREAD_GUARD;

        auto it_layer = std::find_if(layers.begin(), layers.end(), [this, removedObject] (auto& layer)
            { return layer.m_layerNumber == removedObject->m_layer; });

        if (it_layer != layers.end())
        {
            auto it_obj = std::find(it_layer->m_layerObjects.begin(), it_layer->m_layerObjects.end(), removedObject);
            _ASSERT(it_obj != it_layer->m_layerObjects.end());
            if (it_obj != it_layer->m_layerObjects.end())
            {
                it_layer->m_layerObjects.erase(it_obj);
                if (it_layer->m_layerObjects.size() == 0)
                    layers.erase(it_layer);
            }
        }
        else
            _ASSERT(false);
    }

    void MlWrapper::changeLayerOfMgeObject(std::shared_ptr<MgeLayerObject> mgeObject, size_t newLayer)
    {
        _ASSERT(mgeObject);
        if (!mgeObject)
            return;

        removeMgeLayerObject(mgeObject);
        mgeObject->m_layer = newLayer;
        addMgeLayerObject(mgeObject);
    }

    void MlWrapper::removeTexture(TextureId textureId)
    {
        // decrease reference count
        auto texIt = textures.find(textureId);
        if (texIt != textures.end())
        {
            texIt->second.first--;

            if (texIt->second.first == 0) // nobody uses this texture anymore → remove it
                textures.erase(texIt);
        }
        else
            _ASSERT(false); // texture id does not exist
    }

    void MlWrapper::createLayerIfNeeded(size_t layer)
    {
        auto it_layer = std::find_if(layers.begin(), layers.end(), [layer](auto& layerElement)
            {
                return layerElement.m_layerNumber == layer;
            });

        if (it_layer != layers.end())
            return;

        MgeLayer newLayer;
        newLayer.m_layerNumber = layer;
        layers.push_back(std::move(newLayer));

        std::sort(layers.begin(), layers.end(), [] (MgeLayer& elA, MgeLayer& elB)
            { return elA.m_layerNumber < elB.m_layerNumber; });
    }

    void MlWrapper::doSortLayerIfNeeded(MgeLayer& layer)
    {
        MAIN_THREAD_GUARD;
        std::lock_guard<std::mutex> lock(delayedSortMutex);
        auto it = std::find(delayedSort.begin(), delayedSort.end(), layer.m_layerNumber);
        if (it != delayedSort.end())
            std::sort(layer.m_layerObjects.begin(), layer.m_layerObjects.end(),
                [](auto& a, const auto& b)
                    { 
                        if (a && b)
                            return a->zPosition < b->zPosition; 
                        return false;
                    });
    }

    const sf::Texture& MlWrapper::appendTexture(const MgeImage& image)
    {
        auto textureId = image.getTextureID();

        auto texture_it = std::find_if(textures.begin(), textures.end(), [textureId] (const auto& entry)
            { return textureId == entry.first;  });

        if (texture_it != textures.end())
        {
            texture_it->second.first++; //increase counter
            return texture_it->second.second;
        }
        else
        {
            sf::Texture texture;
            if (!texture.loadFromFile(textureId.path))
                _ASSERT(false); //file not found
            else
            {
                textures.insert({ textureId, std::make_pair(1, std::move(texture)) });
                return textures[textureId].second;
            }
        }

        static sf::Texture empty;
        return empty;
    }

    [[nodiscard]] mgeType::Size<int> MlWrapper::getTextureSize(TextureId id)
    {
        for (const auto& texture : textures)
        {
            if (texture.first.path == id.path)
            {
                sf::Vector2u texSize = texture.second.second.getSize();
                static mgeType::Size<int> size;
                size.width = static_cast<int>(texSize.x);
                size.height = static_cast<int>(texSize.y);
                return size;
            }
        }
        _ASSERT(false); //texture not found - maybe not loaded yet
        return mgeType::Size<int>();
    }

    void MlWrapper::sortLayerByZIndex_delayed(size_t layer)
    {
        std::lock_guard<std::mutex> lock(delayedSortMutex);
        auto it = std::find(delayedSort.begin(), delayedSort.end(), layer);
        if (it == delayedSort.end())
            delayedSort.push_back(layer);
        //else -> contained
        _ASSERT(delayedSort.size() <= 100); //need optimise?
    }

    static std::shared_ptr<ML_wrapper::MlWrapper> g_mlWrapper;
    std::shared_ptr<ML_wrapper::MlWrapper> getGlobalMlWrapper()
    {
        if (!g_mlWrapper)
            g_mlWrapper = std::make_shared<ML_wrapper::MlWrapper>();
        return g_mlWrapper;
    }

} //ML_wrapper namespace end

FPoint ml::getCursorPosition() noexcept
{
    return ML_wrapper::getGlobalMlWrapper()->getCursorWorldPosition();
}

std::shared_ptr<sf::RenderWindow> ml::getRenderWindow() noexcept
{
    return mainWindow;
}

