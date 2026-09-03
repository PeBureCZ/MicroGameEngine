#pragma once
#include <memory>
#include <unordered_map>
#include <deque>
#include "MlEvent.h"
#include <variant>
#include <optional>
#include <vector>
#include <mutex>

#include "BasicTypes.h"
#include "GraphicDependencies.h"
#include "layerDefinition.h"

namespace sf
{
    class Font;
	class View;
    class Sprite;
	class Text;
    class Texture;
    class RenderWindow;
}

struct MlVerticesObject;
class MgeImage;
class MgeText;

namespace ML_wrapper
{
    class MlWrapper 
    {
    public:
        MlWrapper();

        void pollMlEvents();
        void clear();
        void displayActualFrame();
        void drawContent();
        void resetMainWindow();

        void removeMgeLayerObject(const std::shared_ptr<MgeLayerObject> removedObject);
        void changeLayerOfMgeObject(std::shared_ptr<MgeLayerObject> mgeObject, size_t newLayer);

        [[nodiscard]] const FPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        void addMgeLayerObject(const std::shared_ptr<MgeLayerObject> newObject);

        [[nodiscard]] mgeType::Size<int> getScreenSize() const noexcept;
		void moveScreenOffset(FPoint offset) noexcept;
		void setMaxScreenZoom(double zoomFactor) noexcept;
		void setMinScreenZoom(double zoomFactor) noexcept;
		void setZoom(double zoomFactor);
        [[nodiscard]] double getZoom() const noexcept;

        void setDrawToGuiView() const;
        void setDrawToWorldView() const;

        MlWrapper(const MlWrapper&) = delete;
        MlWrapper(MlWrapper&&) = delete;
        MlWrapper operator=(const MlWrapper&) = delete;
        MlWrapper operator=(MlWrapper&&) = delete;

        void sortLayerByZIndex_delayed(size_t layer);

        const sf::Texture& appendTexture(const MgeImage& image);
        [[nodiscard]] mgeType::Size<int> getTextureSize(TextureId id);

        ~MlWrapper();

    private:

        void removeTexture(TextureId textureId);
        void doSortLayerIfNeeded(MgeLayer& layer);
        void createLayerIfNeeded(size_t layer);

        template <typename Event>
        void sendEvent(Event&& ev)
        {
            getEventSystem().pushEvent(std::move(ev));
        }

        std::mutex delayedSortMutex;
        std::vector<size_t> delayedSort; //used to store "do sort" during next app tick

		MGE_LAYERS layers;

        using TEXTURE_COUNT = size_t;
        using TEXTURE_OBJECT = std::pair<TEXTURE_COUNT, sf::Texture>;
        using TEXTURES = std::unordered_map<TextureId, TEXTURE_OBJECT>;
        TEXTURES textures;

        sf::View worldView;
        sf::View guiView;

        FPoint cursorWorldPosition;
        IPoint cursorGuiPosition;

        // Scale factor management - ensure correct scaling of ML elements on different resolutions / view sizes
        double zoom = 1.0;
		double maxZoom = 50.0;
		double minZoom = 0.01;
    };  

    std::shared_ptr<ML_wrapper::MlWrapper> getGlobalMlWrapper();
}

namespace ml
{
    [[nodiscard]] FPoint getCursorPosition() noexcept;
    [[nodiscard]] std::shared_ptr<sf::RenderWindow> getRenderWindow() noexcept;
} //namespace ml end

                                                                      