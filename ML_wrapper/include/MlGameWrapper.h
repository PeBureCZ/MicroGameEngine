#pragma once
#include <memory>
#include <unordered_map>
#include <deque>
#include "MlEvent.h"
#include <variant>
#include <optional>
#include <vector>

#include "ImageHolder.h"
#include "MlText.h"

#include "BasicTypes.h"
#include "GraphicDependencies.h"

namespace sf
{
    class Font;
	class View;
}

class MgeDrawable;

using LAYER = std::vector<std::shared_ptr<MgeDrawable>>;
using VERTICES_LAYERS = std::vector<std::pair<size_t, std::unique_ptr<LAYER>>>; //size_t = layer number, LAYER = map of objects in this layer

namespace ML_wrapper
{
    class MlGameWrapper 
    {
    public:
        MlGameWrapper();

        void pollMlEvents();

        void clear();
        void displayActualFrame();

        void drawGuiSprite(SPRITE_ID id) const;
        void drawGuiVertices(const std::shared_ptr<MgeDrawable>& drawableObject) const;
        void drawText(const MlText& textToDraw);

        void drawVertices() const;
        void drawSprites() const;

        std::deque<MlEventType>& getMlEvents();
        void clearEvents();
        void resetMainWindow();
        void removeMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        std::optional<size_t> getLayerOfMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        [[nodiscard]] const DPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        [[nodiscard]] std::optional<MlText> createText(std::string newText, unsigned int charSize_pxls = 30, FPoint position = FPoint()) noexcept;
        [[nodiscard]] ImageHolder& getImageHolder() noexcept;
        [[nodiscard]] bool addMlVerticesObject(size_t layer, const std::shared_ptr<MgeDrawable>& newWidget);

        void setMlVerticesColor(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, mgeType::Color_RGBA newColor, std::optional<size_t> knownLayer = std::nullopt);
        void setMlVerticesLayer(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices,
            size_t newLayer, std::optional<size_t> knownLayer = std::nullopt);

        void setMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        void moveMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        [[nodiscard]] std::optional<FPoint> getVericesPosition(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        void setMlVerticesRotation(std::variant<VerticesId, const std::shared_ptr<MgeDrawable>> vertices, float newRotation, std::optional<size_t> knownLayer = std::nullopt);

        [[nodiscard]] std::optional<LAYER*> getMlGuiObjects() const noexcept;
        [[nodiscard]] mgeType::Size<int> getScreenSize() const noexcept;
		void moveScreenOffset(FPoint offset) noexcept;
		void setMaxScreenZoom(double zoomFactor) noexcept;
		void setMinScreenZoom(double zoomFactor) noexcept;
		void setZoom(double zoomFactor);
        [[nodiscard]] double getZoom() const noexcept;

        void setDrawToGuiView() const;
        void setDrawToWorldView() const;

        MlGameWrapper(const MlGameWrapper&) = delete;
        MlGameWrapper(MlGameWrapper&&) = delete;
        MlGameWrapper operator=(const MlGameWrapper&) = delete;
        MlGameWrapper operator=(MlGameWrapper&&) = delete;

        ~MlGameWrapper();

    private:
        ImageHolder imageHolder;
        std::deque<MlEventType> events;

		VERTICES_LAYERS mlVerticesLayers;

        sf::View worldView;
        sf::View guiView;

        std::shared_ptr<sf::Font> defaultFont;

        DPoint cursorWorldPosition;
        IPoint cursorGuiPosition;

        // Scale factor management - ensure correct scaling of ML elements on different resolutions / view sizes
        double zoom = 1.0;
		double maxZoom = 50.0;
		double minZoom = 0.01;
    };  

    std::shared_ptr<ML_wrapper::MlGameWrapper> getGlobalGameWrapper();
}


                                                                      