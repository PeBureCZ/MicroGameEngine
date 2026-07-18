#include "MlGameWrapper.h"

#include <windows.h>
#include "GlobalFunctions.h"
#include "SFML\Graphics.hpp"

#include "MlVerticesObject.h"
#include "MlText.h"
#include "MlEvent.h"

std::shared_ptr<sf::RenderWindow> mainWindow;

struct VerticesMatcher
{
    std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices;
    bool operator()(const std::shared_ptr<MlVerticesObject>& vert) const
    {
        if (!vert)
            return false;

        if (std::holds_alternative<VerticesId>(vertices))
            return vert->getUniqueId() == std::get<VerticesId>(vertices);

        return vert == std::get<const std::shared_ptr<MlVerticesObject>>(vertices);
    }
};

static std::shared_ptr<MlVerticesObject> checkLayerIfMatch(VERTICES_LAYERS& mlVerticesLayers, std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer)
{
    for (const auto& [layerNum, layer] : mlVerticesLayers)
    {
        _ASSERT(layer);

        if (!layer)
            continue;

        if (knownLayer.has_value() && layerNum != *knownLayer)
            continue;

        auto it = std::find_if(layer->begin(), layer->end(), VerticesMatcher(vertices));

        if (it != layer->end() && *it)
            return (*it);
        else if (knownLayer.has_value())
        {
            _ASSERT(false); //wrong vertices id/ptr
        }
    }
    _ASSERT(false);  //wrong it management
    return std::make_shared<MlVerticesObject>();
}

namespace ML_wrapper
{
    void MlGameWrapper::clearEvents()
    {
        events.clear();
	}

    std::deque<MlEventType>& MlGameWrapper::getMlEvents()
    {
        return events;
    }

    void MlGameWrapper::setMlVerticesColor(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, mgeType::Color_RGBA newColor,
        std::optional<size_t> knownLayer)
    {
        auto checkedVertices = checkLayerIfMatch(mlVerticesLayers, vertices, knownLayer);
        if (checkedVertices)
            checkedVertices->setColor(newColor);
        else
        {
            _ASSERT(false);
        }
    }

    void MlGameWrapper::setMlVerticesLayer(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices,
        size_t newLayer, std::optional<size_t> knownLayer)
    {
        std::shared_ptr<MlVerticesObject> object;
        bool checkLayerNum = (knownLayer.has_value()) ? true : false;
        for (auto& [layerNum, layer_uniquePtr] : mlVerticesLayers)
        {
            _ASSERT(layer_uniquePtr);
            if (!layer_uniquePtr || (checkLayerNum && layerNum != knownLayer.value()))
                continue;

            auto it = std::find_if(layer_uniquePtr->begin(), layer_uniquePtr->end(), VerticesMatcher(vertices));
            if (it != layer_uniquePtr->end() && *it)
            {
                object = *it;
                layer_uniquePtr->erase(it);
            }
        }

        if (object)
        {
            [[maybe_unused]] auto correctlyAdd = addMlVerticesObject(newLayer, std::move(object));
        }
        else
        {
            _ASSERT(false);  //wrong it management
        }
    }

    void MlGameWrapper::setMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer)
    {
        auto checkedVertices = checkLayerIfMatch(mlVerticesLayers, vertices, knownLayer);
        if (checkedVertices)
            checkedVertices->setPosition(newPos);
        else
        {
            _ASSERT(false);
        }
    }

    void MlGameWrapper::moveMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer)
    {
        auto checkedVertices = checkLayerIfMatch(mlVerticesLayers, vertices, knownLayer);
        if (checkedVertices)
            checkedVertices->moveAbsolutePosition(newPos);
        else
        {
            _ASSERT(false);
        }
    }

    std::optional<FPoint> MlGameWrapper::getVericesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer)
    {
        auto checkedVertices = checkLayerIfMatch(mlVerticesLayers, vertices, knownLayer);
        if (checkedVertices)
            return checkedVertices->getPosition();
        else
        {
            _ASSERT(false);
        }
        return std::nullopt;
    }

    void MlGameWrapper::setMlVerticesRotation(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, float newRotation, std::optional<size_t> knownLayer)
    {
        auto checkedVertices = checkLayerIfMatch(mlVerticesLayers, vertices, knownLayer);
        if (checkedVertices)
            checkedVertices->setRotation(newRotation);
        else
        {
            _ASSERT(false);
        }
    }

    std::optional<size_t> MlGameWrapper::getLayerOfMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer)
    {
        bool checkLayerNum = (knownLayer.has_value()) ? true : false;
        for (const auto& [layerNum, layer] : mlVerticesLayers)
        {
            if (checkLayerNum && layerNum != knownLayer.value())
                continue;

            auto it = std::find_if(layer->begin(), layer->end(), VerticesMatcher(vertices));
            if (it != layer->end())
                return layerNum;
		}
		return std::nullopt;
    }

    const DPoint& MlGameWrapper::getCursorWorldPosition() const noexcept
    {
        return cursorWorldPosition;
    }

    const IPoint& MlGameWrapper::getCursorGuiPosition() const noexcept
    {
        return cursorGuiPosition;
    }

    std::optional<LAYER*> MlGameWrapper::getMlGuiObjects() const noexcept
    {
        for (auto& [layerNum, layer] : mlVerticesLayers)
        {
            if (layerNum == GraphicItemLayer::GUI_LAYER)
				return layer.get();
        }
		return std::nullopt;
    }

    mgeType::Size<int> MlGameWrapper::getScreenSize() const noexcept
    {
        return mgeType::Size<int>((int)guiView.getSize().x, (int)guiView.getSize().y);
    }

    void MlGameWrapper::moveScreenOffset(FPoint offset) noexcept
    {
		worldView.move(sf::Vector2f(static_cast<float>(offset.x), static_cast<float>(offset.y)));
    }

    void MlGameWrapper::setMaxScreenZoom(double zoomFactor) noexcept
    {
        if (zoomFactor > 0.0)
			maxZoom = zoomFactor;
    }

    void MlGameWrapper::setMinScreenZoom(double zoomFactor) noexcept
    {
        if (zoomFactor > 0.0 && zoomFactor < maxZoom)
			minZoom = zoomFactor;
    }

    void MlGameWrapper::setZoom(double zoomFactor)
    {
        if (zoomFactor > 0.0 && zoomFactor <= maxZoom && zoomFactor >= minZoom)
        {
            double factor = zoomFactor / zoom;
            zoom = zoomFactor;
            worldView.zoom(static_cast<float>(factor));
		}
    }

    [[nodiscard]] double MlGameWrapper::getZoom() const noexcept
    {
        return zoom;
	}

    void MlGameWrapper::pollMlEvents()
    {
        //cursor position
        sf::Vector2i mousePixel = sf::Mouse::getPosition(*mainWindow);
        sf::Vector2f mouseWorld = mainWindow->mapPixelToCoords(mousePixel, worldView);
        sf::Vector2f mouseGui = mainWindow->mapPixelToCoords(mousePixel, guiView);
        cursorWorldPosition = DPoint
        (
            static_cast<double>(std::lround(mouseWorld.x)),
            static_cast<double>(std::lround(mouseWorld.y))
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

    void MlGameWrapper::setDrawToGuiView() const
    {
        _ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->setView(guiView);
    }

    void MlGameWrapper::setDrawToWorldView() const
    {
        _ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->setView(worldView);
    }

    void MlGameWrapper::clear()
    {
		_ASSERT(mainWindow);
        if (mainWindow)
            mainWindow->clear();
    }

    void MlGameWrapper::displayActualFrame()
    {
        //window->setFramerateLimit(60); //set specific FPS, disabled for now - due to cause lags
        mainWindow->display();
    }

    void MlGameWrapper::drawGuiSprite(SPRITE_ID id) const
    {
		auto guiSprites_opt = imageHolder.getGuiSprites();
        if (guiSprites_opt.has_value() && guiSprites_opt.value()) //opt_ptr -> raw ptr!
         {
			const auto* guiSprites = guiSprites_opt.value();
            auto it = guiSprites->find(id);
            if (it != guiSprites->end())
                mainWindow->draw(it->second);
        }
        else
        {
            _ASSERT(false); //gui layer not found
        }
    }

    void MlGameWrapper::drawGuiVertices(const std::shared_ptr<MlVerticesObject>& drawableObject) const
    {
		const auto mlGuiVerticesObjects_opt = getMlGuiObjects();
        if (mlGuiVerticesObjects_opt.has_value() && mlGuiVerticesObjects_opt.value()) //opt_ptr -> raw ptr!
        {
			const auto* mlGuiVerticesObjects = mlGuiVerticesObjects_opt.value();
            auto it = std::find(mlGuiVerticesObjects->begin(), mlGuiVerticesObjects->end(), drawableObject);
            if (it != mlGuiVerticesObjects->end() && *it)
                mainWindow->draw((*it)->getVertices(), (*it)->getRenderState());
        }
        else
        {
			_ASSERT(false); //gui layer not found
        }
    }

    void MlGameWrapper::drawText(const MlText& textToDraw)
    {
        if (mainWindow && textToDraw.getTextObject())
            mainWindow->draw(*textToDraw.getTextObject());
    }

    void MlGameWrapper::drawVertices() const
    {
        setDrawToWorldView();
        for (const auto& [layerNum, layer] : mlVerticesLayers)
        {
            if (layerNum == GraphicItemLayer::UNRENDERED_LAYER || layerNum == GraphicItemLayer::GUI_LAYER)
                continue;
            for (const auto& mlVerticesObject : *layer.get())
                mainWindow->draw(mlVerticesObject->getVertices(), mlVerticesObject->getRenderState());
        }
    }

    void MlGameWrapper::drawSprites() const
    {
        for (const auto& [layerId, spriteLayer] : imageHolder.getSprites())
        {
            if (layerId == GraphicItemLayer::UNRENDERED_LAYER || layerId == GraphicItemLayer::GUI_LAYER)
			    continue;
            for (const auto& sprite : spriteLayer)
				mainWindow->draw(sprite.second);
        }
    }

    std::optional<MlText> MlGameWrapper::createText(std::string newText, unsigned int charSize_pxls, FPoint position) noexcept
    {
        if (defaultFont)
		    return MlText(*defaultFont, std::move(newText), charSize_pxls, position);
		_ASSERT(false); //font not loaded
		return std::nullopt;
    }

    MlGameWrapper::MlGameWrapper()
    {
		defaultFont = std::make_shared<sf::Font>();

		const std::string fontPath = tsmCore::getExecutablePath() + "\\Fonts\\NotoSerifGeorgian-Regular.ttf";

        if (!defaultFont->openFromFile(fontPath))
            {_ASSERT(false); } //font loading failed

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

    MlGameWrapper::~MlGameWrapper()
    {
    }

    void MlGameWrapper::resetMainWindow()
    {
		mainWindow.reset();
    }

    void MlGameWrapper::removeMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer)
    {
        for (const auto& [layerNum, layer] : mlVerticesLayers)
        {
            bool checkLayerNum = knownLayer.has_value();
            _ASSERT(layer);
            if (!layer || (checkLayerNum && layerNum != knownLayer.value()))
                continue;

            auto it = std::find_if(layer->begin(), layer->end(), VerticesMatcher(vertices));
            if (it != layer->end())
                layer->erase(it);
        }
    }

    ImageHolder& MlGameWrapper::getImageHolder() noexcept
    {
        return imageHolder;
    }

    [[nodiscard]] bool MlGameWrapper::addMlVerticesObject(size_t layer, const std::shared_ptr<MlVerticesObject>& newWidget)
    {
        if (!newWidget)
        {
            _ASSERT(false); //invalid widget
            return false;
		}

        auto it = std::lower_bound(mlVerticesLayers.begin(),mlVerticesLayers.end(),layer,[](const auto& lhs, size_t value)
            {return lhs.first < value;});

        // If layer doesn't exist -> create it
        if (it == mlVerticesLayers.end() || it->first != layer)
        {
            auto newLayer = std::make_unique<LAYER>();
            it = mlVerticesLayers.insert(it, { layer, std::move(newLayer) });
        }

        // Insert object into the layer 
        if (it->second)
            it->second->push_back(newWidget);
        return true;
    }

    static std::shared_ptr<ML_wrapper::MlGameWrapper> g_mlGameWrapper;
    std::shared_ptr<ML_wrapper::MlGameWrapper> getGlobalGameWrapper()
    {
        if (!g_mlGameWrapper)
        {
            g_mlGameWrapper = std::make_shared<ML_wrapper::MlGameWrapper>();
        }
	    return g_mlGameWrapper;
    }

} //ML_wrapper namespace end


