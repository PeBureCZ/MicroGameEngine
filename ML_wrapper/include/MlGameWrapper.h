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

namespace sf
{
    class Font;
	class View;
}

class MlVerticesObject;

using LAYER = std::unordered_map<uintptr_t, std::unique_ptr<MlVerticesObject>>;
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
        void drawGuiVertices(uintptr_t objectID) const;
        void drawText(const MlText& textToDraw);

        void drawVertices() const;
        void drawSprites() const;

        std::deque<MlEventType>& getMlEvents();
        void clearEvents();
        void resetMainWindow();
        void removeMlVerticesObject(uintptr_t objectID);

        std::optional<size_t> getLayerOfMlVerticesObject(uintptr_t objectId);

        [[nodiscard]] const DPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        [[nodiscard]] std::optional<MlText> createText(std::string newText, unsigned int charSize_pxls = 30, FPoint position = FPoint()) noexcept;
        [[nodiscard]] ImageHolder& getImageHolder() noexcept;
        [[nodiscard]] uintptr_t addMlVerticesObject(size_t layer, std::unique_ptr<MlVerticesObject> newWidget);

        //GUI
        void setMlVerticesColor(uintptr_t id, tsmType::Color_RGBA newColor);
        void setMlVerticesLayer(uintptr_t id, size_t newLayer);
        void moveMlVerticesPosition(uintptr_t id, FPoint newPos);
        void setMlVerticesRotation(uintptr_t id, float newRotation) noexcept;
        void setSpriteRotation(uintptr_t id, float newRotation) noexcept;

        [[nodiscard]] std::optional<LAYER*> getMlGuiObjects() const noexcept;
        [[nodiscard]] tsmType::Size<int> getScreenSize() const noexcept;
		void moveScreenOffset(FPoint offset) noexcept;
		void setMaxScreenZoom(double zoomFactor) noexcept;
		void setMinScreenZoom(double zoomFactor) noexcept;
		void setZoom(double zoomFactor) noexcept;

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


                                                                      