#include "MlWrapper.h"

#include <windows.h>

#include "GlobalFunctions.h"
#include "MlEvent.h"
#include "MgeDrawable.h"
#include "MgeText.h"
#include "MgeImage.h"

std::shared_ptr<sf::RenderWindow> mainWindow;

namespace ML_wrapper
{
    void MlWrapper::clearEvents()
    {
        events.clear();
	}

    std::deque<MlEventType>& MlWrapper::getMlEvents()
    {
        return events;
    }

    const FPoint& MlWrapper::getCursorWorldPosition() const noexcept
    {
        return cursorWorldPosition;
    }

    const IPoint& MlWrapper::getCursorGuiPosition() const noexcept
    {
        return cursorGuiPosition;
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
                AppTermination exitGameEvent;
                events.emplace_back(exitGameEvent);
                return;
            }
            else if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                IPoint pos = toWorldPoint(mousePressed->position);

                if (mousePressed->button == sf::Mouse::Button::Left)
                    events.emplace_back(MouseClick(pos, PRESSED, MlEventTypeEnum::MouseLeftClick));
                else if (mousePressed->button == sf::Mouse::Button::Right)
                    events.emplace_back(MouseClick(pos, PRESSED, MlEventTypeEnum::MouseRightClick));
                else if (mousePressed->button == sf::Mouse::Button::Middle)
					events.emplace_back(MouseClick(pos, PRESSED, MlEventTypeEnum::WheelMiddleButton));
            }
            else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>())
            {
                IPoint pos = toWorldPoint(mouseReleased->position);

                if (mouseReleased->button == sf::Mouse::Button::Left)
                    events.emplace_back(MouseClick(pos, RELEASED, MlEventTypeEnum::MouseLeftClick));
                else if (mouseReleased->button == sf::Mouse::Button::Right)
                    events.emplace_back(MouseClick(pos, RELEASED, MlEventTypeEnum::MouseRightClick));
				else if (mouseReleased->button == sf::Mouse::Button::Middle)
					events.emplace_back(MouseClick(pos, RELEASED, MlEventTypeEnum::WheelMiddleButton));
            }
            else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>())
                events.emplace_back(WheelScroll((wheel->delta > 0) ? 1 : -1));
            else if (const auto* resized = event->getIf<sf::Event::Resized>())
            {
                sf::Vector2f newSize
                    (
                        static_cast<float>(resized->size.x),
                        static_cast<float>(resized->size.y)
                    );

                // --- GUI VIEW ---
                guiView.setSize(newSize);
                guiView.setCenter(newSize / 2.f);

                // --- WORLD VIEW ---
                // Keep zoom, adapt view size to window
                worldView.setSize(newSize);
                worldView.setCenter(newSize / 2.f);
                worldView.zoom((float)zoom);
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

    void MlWrapper::drawContent() const
    {
        setDrawToWorldView();
        for (const auto& [layerNum, layer] : layers)
        {
            if (layerNum == GraphicItemLayer::UNRENDERED_LAYER)
                continue;

            if (layerNum == GraphicItemLayer::GUI_LAYER)
                setDrawToGuiView();

            for (const auto& objectInLayer : *layer.get())
            {
                if (std::holds_alternative<MGE_VERTICES>(objectInLayer))
                {
					const auto& verticesObject = std::get<MGE_VERTICES>(objectInLayer);
                    _ASSERT(verticesObject);
                    if (verticesObject)
                        mainWindow->draw(verticesObject->m_batch, verticesObject->m_state);
                }
                else if (std::holds_alternative<MGE_IMAGE>(objectInLayer))
                {
                    const auto& imageObject = std::get<MGE_IMAGE>(objectInLayer);
                    _ASSERT(imageObject);
                    if (imageObject)
                        mainWindow->draw(*imageObject);
				}
                else if (std::holds_alternative<MGE_TEXT>(objectInLayer))
                {
                    const auto& textObject = std::get<MGE_TEXT>(objectInLayer);
                    _ASSERT(textObject);
                    if (textObject)
                        mainWindow->draw(*textObject);
				}
                else 
                    _ASSERT(false); //unknown object type
            }

			//back to world view for next layer
            if (layerNum == GraphicItemLayer::GUI_LAYER)
                setDrawToWorldView();
        }
    }

    MlWrapper::MlWrapper()
    {
        if (!mainWindow)
        {
            // Fullscreen mode with current desktop resolution
            auto dm = sf::VideoMode::getDesktopMode();

//#define USE_FULLSCREEN //uncomment to enable fullscreen mode in debug (for testing purposes)
#if defined(NDEBUG) || defined(USE_FULLSCREEN)
            mainWindow = std::make_shared<sf::RenderWindow>(dm, "Train siding manager", sf::Style::Default, sf::State::Fullscreen);
#else
            mainWindow = std::make_shared<sf::RenderWindow>(dm, "Train siding manager", sf::Style::Default, sf::State::Windowed);
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

    MlWrapper::~MlWrapper()
    {
#ifdef _DEBUG
        for (const auto& [layerId, layer] : layers)
        {
            _ASSERT(layer);
            if (!layer)
                continue;
            for (const auto& drawableItem : *layer.get())
            {
                //if more than one object exists, it mean´s there is an issue with drawable item management
                if (std::holds_alternative<MGE_VERTICES>(drawableItem))
                {
                    const auto& verticesObject = std::get<MGE_VERTICES>(drawableItem);
                    _ASSERT(verticesObject);
                    if (verticesObject)
                        { _ASSERT(verticesObject.use_count() == 1); }
                }
                else if (std::holds_alternative<MGE_IMAGE>(drawableItem))
                {
                    const auto& imageObject = std::get<MGE_IMAGE>(drawableItem);
                    _ASSERT(imageObject);
                    if (imageObject)
                        { _ASSERT(imageObject.use_count() == 1); }
                }
                else if (std::holds_alternative<MGE_TEXT>(drawableItem))
                {
                    const auto& textObject = std::get<MGE_TEXT>(drawableItem);
                    _ASSERT(textObject);
                    if (textObject)
                        { _ASSERT(textObject.use_count() == 1); }
                }
                else
                {
					_ASSERT(false); //unknown object type
                }
            }
        }
#endif
    }

    void MlWrapper::resetMainWindow()
    {
		mainWindow.reset();
    }

    bool MlWrapper::removeMlVerticesObject(const MGE_VERTICES& vertices, std::optional<size_t> knownLayer)
    {
        for (const auto& [layerNum, layer] : layers)
        {
            bool checkLayerNum = knownLayer.has_value();
            _ASSERT(layer);
            if (!layer || (checkLayerNum && layerNum != knownLayer.value()))
                continue;

            auto it = std::find_if(layer->begin(), layer->end(), [&vertices](const LAYERED_OBJECT& obj)
                {return std::holds_alternative<MGE_VERTICES>(obj) && std::get<MGE_VERTICES>(obj) == vertices;});

            if (it != layer->end())
            {
                layer->erase(it);
                return true;
            }
        }
        _ASSERT(false);
        return false;
    }

    void MlWrapper::addMlVerticesObject(const MGE_VERTICES& newWidget, size_t layer)
    {
        if (!newWidget)
        {
            _ASSERT(false); //invalid widget
            return;
		}

        auto layer_it = std::lower_bound(layers.begin(),layers.end(), layer, [](const auto& lhs, size_t value)
            {return lhs.first < value;});

        if (!createLayerIfNeeded(layer_it, layer))
            return;

        // Insert object into the layer 
        if (layer_it->second)
            layer_it->second->push_back(newWidget);
        return;
    }

    static std::shared_ptr<ML_wrapper::MlWrapper> g_mlWrapper;
    std::shared_ptr<ML_wrapper::MlWrapper> getGlobalMlWrapper()
    {
        if (!g_mlWrapper)
        {
            g_mlWrapper = std::make_shared<ML_wrapper::MlWrapper>();
        }
	    return g_mlWrapper;
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

    bool MlWrapper::createLayerIfNeeded(MGE_LAYERS::iterator& layer_it, size_t layer)
    {
        if (layer_it == layers.end() || layer_it->first != layer)
        {
            BASE_LAYER newBaseLayer = std::make_unique<LAYERED_OBJECTS>();
            layer_it = layers.insert(layer_it, LAYER{ layer, std::move(newBaseLayer) });
            if (layer_it == layers.end())
            {
                _ASSERT(false);
                return false;
            }
        }
        return true;
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

    void MlWrapper::changeSpriteLayer(MGE_IMAGE& image, size_t newLayer, size_t oldLayer)
    {
        removeSprite(image, oldLayer);
        appendSprite(image, newLayer);
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

    void MlWrapper::removeText(const MgeText& txt)
    {
        auto layer = txt.getLayer();
        auto it = std::find_if(layers.begin(), layers.end(), [layer](const auto& pair) { return pair.first == layer; });
        if (it != layers.end())
        {
            auto& objects = it->second;
            if (objects)
            {
                auto txtObj = txt.getTextObject();
                auto text_it = std::find_if(objects->begin(), objects->end(),
                    [&txtObj](const LAYERED_OBJECT& lobj) { return std::holds_alternative<MGE_TEXT>(lobj) && std::get<MGE_TEXT>(lobj) == txtObj;  });

                if (text_it != objects->end())
                    objects->erase(text_it);
                else
                    _ASSERT(false); //image not found in the specified layer
            }
            else
                _ASSERT(false); //layer exists but has no objects
        }
        else
            _ASSERT(false); //layer not found
    }

    void MlWrapper::appendText(const MgeText& txt)
    {
        _ASSERT(txt.getTextObject());
        size_t layer = txt.getLayer();
        auto layer_it = std::lower_bound(layers.begin(), layers.end(), layer, [](const auto& lhs, size_t value)
            {return lhs.first < value; });

        if (!createLayerIfNeeded(layer_it, layer))
            return;

        // Insert object into the layer 
        if (layer_it->second)
            layer_it->second->push_back(txt.getTextObject());
    }

    void MlWrapper::appendImage(const MgeImage& image)
    {
        auto textureId = image.getTextureID();
        auto texture_it = textures.find(textureId);

        if (texture_it != textures.end())
            texture_it->second.first++; //increase counter
        else
        {
            sf::Texture texture;
            if (!texture.loadFromFile(textureId.path))
            {
                _ASSERT(false); //file not found
            }
            else
                textures.insert({ textureId, std::make_pair(1, texture) });
        }

        appendSprite(image.getSprite(), image.getLayer());
    }

    void MlWrapper::appendSprite(const MGE_IMAGE& image, size_t layer)
    {
        auto layer_it = std::lower_bound(layers.begin(), layers.end(), layer, [](const auto& lhs, size_t value)
            {return lhs.first < value; });

        if (!createLayerIfNeeded(layer_it, layer))
            return;

        // Insert object into the layer 
        if (layer_it->second)
            layer_it->second->push_back(image);
    }

    void MlWrapper::removeImage(MgeImage& image)
    {
        auto textureId = image.getTextureID();
        _ASSERT(textureId.getPath() != UNDEFINED_TEXTURE_PATH);
        auto texture_it = textures.find(textureId);

        if (texture_it != textures.end())
        {
            auto sprite = image.getSprite();
            if (sprite)
            {
				removeSprite(sprite, image.getLayer());
                removeTexture(std::move(textureId));
            }
            else
                { _ASSERT(false); } //sprite not found
        }
        else
            { _ASSERT(false); } // texture id does not exist
    } 

    void MlWrapper::removeSprite(MGE_IMAGE& img, size_t layer)
    {
		auto it = std::find_if(layers.begin(), layers.end(), [layer](const auto& pair) { return pair.first == layer; });
        if (it != layers.end())
        {
            auto& sprites = it->second;
            if (sprites)
            {
                auto spriteIt = std::find_if(sprites->begin(), sprites->end(), 
                    [&img](const LAYERED_OBJECT& lobj) { return std::holds_alternative<MGE_IMAGE>(lobj) && std::get<MGE_IMAGE>(lobj) == img;  });

                if (spriteIt != sprites->end())
                    sprites->erase(spriteIt);
                else
                    { _ASSERT(false); } //sprite not found in the specified layer
            }
            else
                { _ASSERT(false); } //layer exists but has no sprites
        }
        else
            { _ASSERT(false); } //layer not found
    }
} //ML_wrapper namespace end

FPoint ml::getCursorPosition() noexcept
{
    return ML_wrapper::getGlobalMlWrapper()->getCursorWorldPosition();
}

