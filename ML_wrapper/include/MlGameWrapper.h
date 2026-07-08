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

class MlVerticesObject;

using LAYER = std::unordered_map<VerticesId, std::unique_ptr<MlVerticesObject>>;
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
        void drawGuiVertices(VerticesId objectID) const;
        void drawText(const MlText& textToDraw);

        void drawVertices() const;
        void drawSprites() const;

        std::deque<MlEventType>& getMlEvents();
        void clearEvents();
        void resetMainWindow();
        void removeMlVerticesObject(VerticesId objectID);

        std::optional<size_t> getLayerOfMlVerticesObject(VerticesId objectId, std::optional<size_t> knownLayer = std::nullopt);

        [[nodiscard]] const DPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        [[nodiscard]] std::optional<MlText> createText(std::string newText, unsigned int charSize_pxls = 30, FPoint position = FPoint()) noexcept;
        [[nodiscard]] ImageHolder& getImageHolder() noexcept;
        [[nodiscard]] VerticesId addMlVerticesObject(size_t layer, std::unique_ptr<MlVerticesObject> newWidget);

        void setMlVerticesColor(VerticesId id, tsmType::Color_RGBA newColor, std::optional<size_t> knownLayer = std::nullopt);
        void setMlVerticesLayer(VerticesId id, std::optional<size_t> knownLayer = std::nullopt);
        void setMlVerticesPosition(VerticesId id, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        void moveMlVerticesPosition(VerticesId id, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        [[nodiscard]] std::optional<FPoint> getVericesPosition(VerticesId id, std::optional<size_t> knownLayer = std::nullopt);

        void setMlVerticesRotation(VerticesId id, float newRotation, std::optional<size_t> knownLayer = std::nullopt) noexcept;
        void setSpriteRotation(VerticesId id, float newRotation, std::optional<size_t> knownLayer = std::nullopt) noexcept;

        [[nodiscard]] std::optional<LAYER*> getMlGuiObjects() const noexcept;
        [[nodiscard]] tsmType::Size<int> getScreenSize() const noexcept;
		void moveScreenOffset(FPoint offset) noexcept;
		void setMaxScreenZoom(double zoomFactor) noexcept;
		void setMinScreenZoom(double zoomFactor) noexcept;
		void setZoom(double zoomFactor) noexcept;
        [[nodiscard]] double getZoom() const noexcept;

        void setDrawToGuiView() const noexcept;
        void setDrawToWorldView() const noexcept;

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


                                                                      