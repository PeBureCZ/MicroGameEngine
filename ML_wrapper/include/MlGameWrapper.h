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

using LAYER = std::vector<std::shared_ptr<MlVerticesObject>>;
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
        void drawGuiVertices(const std::shared_ptr<MlVerticesObject>& drawableObject) const;
        void drawText(const MlText& textToDraw);

        void drawVertices() const;
        void drawSprites() const;

        std::deque<MlEventType>& getMlEvents();
        void clearEvents();
        void resetMainWindow();
        bool removeMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        std::optional<size_t> getLayerOfMlVerticesObject(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        [[nodiscard]] const FPoint& getCursorWorldPosition() const noexcept;
        [[nodiscard]] const IPoint& getCursorGuiPosition() const noexcept;
        [[nodiscard]] std::optional<MlText> createText(std::string newText, unsigned int charSize_pxls = 30, FPoint position = FPoint()) noexcept;
        [[nodiscard]] ImageHolder& getImageHolder() noexcept;
        [[nodiscard]] bool addMlVerticesObject(const std::shared_ptr<MlVerticesObject>& newWidget);

        void setMlVerticesColor(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, mgeType::Color_RGBA newColor, std::optional<size_t> knownLayer = std::nullopt);
        std::optional<std::shared_ptr<MlVerticesObject>> replaceVerticesWithNewObject(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> removeObj,
            size_t newLayer, std::optional<size_t> knownLayer = std::nullopt);

        void setMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        void moveMlVerticesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, const FPoint& newPos, std::optional<size_t> knownLayer = std::nullopt);
        [[nodiscard]] std::optional<FPoint> getVericesPosition(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, std::optional<size_t> knownLayer = std::nullopt);

        void setMlVerticesRotation(std::variant<VerticesId, const std::shared_ptr<MlVerticesObject>> vertices, float newRotation, std::optional<size_t> knownLayer = std::nullopt);

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

        FPoint cursorWorldPosition;
        IPoint cursorGuiPosition;

        // Scale factor management - ensure correct scaling of ML elements on different resolutions / view sizes
        double zoom = 1.0;
		double maxZoom = 50.0;
		double minZoom = 0.01;
    };  

    std::shared_ptr<ML_wrapper::MlGameWrapper> getGlobalGameWrapper();
}

namespace ml
{
    [[nodiscard]] bool removeVertices(const std::shared_ptr<MlVerticesObject>& vertices);
    [[nodiscard]] bool  addVertices(const std::shared_ptr<MlVerticesObject>& vertices);
    void setVerticesColor(const std::shared_ptr<MlVerticesObject> vertices, const mgeType::Color_RGBA& newColor);
    void setVerticesPosition(const std::shared_ptr<MlVerticesObject> vertices, const FPoint& newPos);
    [[nodiscard]] FPoint getCursorPosition() noexcept;
}

                                                                      