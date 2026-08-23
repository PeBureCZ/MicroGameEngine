#pragma once
#include <memory>
#include <unordered_map>
#include <deque>
#include "MlEvent.h"
#include <variant>
#include <optional>
#include <vector>

#include "SFML/Graphics.hpp"

#include "BasicTypes.h"
#include "GraphicDependencies.h"

namespace sf
{
    class Font;
	class View;
    class Sprite;
	class Text;
    class Texture;
}

struct MlVerticesObject;
class MgeImage;
class MgeText;

using MGE_VERTICES = std::shared_ptr<MlVerticesObject>;
using MGE_IMAGE = std::shared_ptr<sf::Sprite>;
using MGE_TEXT = std::shared_ptr<sf::Text>;

using LAYERED_OBJECT = std::variant<MGE_VERTICES, MGE_IMAGE, MGE_TEXT>;
using LAYERED_OBJECTS = std::vector<LAYERED_OBJECT>;
using BASE_LAYER = std::unique_ptr<LAYERED_OBJECTS>;
using LAYER_NUMBER = size_t;

using LAYER = std::pair<LAYER_NUMBER, BASE_LAYER>;
using MGE_LAYERS = std::vector<LAYER>;

namespace ML_wrapper
{
    class MlWrapper 
    {
    public:
        MlWrapper();

        void pollMlEvents();

        void clear();
        void displayActualFrame();

        void drawContent() const;

        std::deque<MlEventType>& getMlEvents();
        void clearEvents();
        void resetMainWindow();
        bool removeMlVerticesObject(const MGE_VERTICES& vertices, std::optional<size_t> knownLayer = std::nullopt);

        [[nodiscard]] const FPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        void addMlVerticesObject(const MGE_VERTICES& newWidget, size_t layer);

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

        void removeText(const MgeText& txt);
        void appendText(const MgeText& txt);

        void appendImage(const MgeImage& image);
        void removeImage(MgeImage& image);
        void changeSpriteLayer(MGE_IMAGE& image, size_t newLayer, size_t oldLayer);
        const sf::Texture& appendTexture(const MgeImage& image);
        [[nodiscard]] mgeType::Size<int> getTextureSize(TextureId id);

        ~MlWrapper();

    private:

        void removeSprite(MGE_IMAGE& img, size_t layer);
        void appendSprite(const MGE_IMAGE& image, size_t layer);

        void removeTexture(TextureId textureId);

        [[nodiscard]] bool createLayerIfNeeded(MGE_LAYERS::iterator& layer_it, size_t layer);

        std::deque<MlEventType> events;

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
} //namespace ml end

                                                                      