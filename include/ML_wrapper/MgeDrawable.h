#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <variant>  
#include <optional>
#include "graphicdependencies.h"
#include "BasicShapes.h"
#include "MgeComponents.h"

struct MgeLayerObject;

class MgeDrawable : public MgeBasicComponent
{
public:
    MgeDrawable() = default;
    explicit MgeDrawable(size_t layer);
    MgeDrawable(const MgeVertices<float>& drawable, FPoint absolutePositionOffset, float rotation, size_t layer);

    MgeDrawable(const MgeDrawable& other) = delete;
    MgeDrawable(MgeDrawable&& other) noexcept;
	MgeDrawable& operator= (const MgeDrawable& other) = delete; 
    MgeDrawable& operator= (MgeDrawable&& other) noexcept;

    ~MgeDrawable();

    void addObjects(std::vector <MgeVertices<float>> content);
    void addObjects(sf::VertexArray content);
    void addObjects(const MgeVertices<float> content);
    [[nodiscard]] VerticesId getUniqueId() const noexcept;
    void setRotation(float newRotation) noexcept;
    [[nodiscard]] float getRotation() const noexcept;
    [[nodiscard]] bool isValid() const noexcept;

    void setPosition(const FPoint& newPosition); //absolute coordination
    [[nodiscard]] FPoint getPosition() const noexcept; //absolute coordination

    [[nodiscard]] const std::shared_ptr<MgeLayerObject> getVertices() const noexcept;
    void setColor(const MgeColor& newColor);
    //logical color (alpha is the set alpha even when the object is hidden)
    [[nodiscard]] std::optional<MgeColor> getColor() const noexcept;
    void setIsVisible(bool visible) noexcept;
    void moveAbsolutePosition(const FPoint& movedPosBy);

	void rescale(float scaleX, float scaleY) noexcept;

    [[nodiscard]] size_t getLayer() const noexcept;
    void setLayer(size_t newLayer);
    void setZPosition(const int64_t newZPosition) noexcept;
    int64_t getZPosition() const noexcept;
    //draw group = primary sort key inside the layer (see MgeLayerObject::drawGroup)
    void setDrawGroup(const int64_t newDrawGroup) noexcept;
    [[nodiscard]] int64_t getDrawGroup() const noexcept;

private:
    std::weak_ptr<MgeLayerObject> m_vertices;
    MgeVertices<float> frameObject;
    FPoint m_absolutePositionOffset;
    float m_rotation = 0.0f;
    std::uint8_t m_alpha = 255;
    bool m_isVisible = true;

    void setVerticesColor(const MgeColor& color) noexcept;
    [[nodiscard]] std::optional<MgeColor> getVerticesColor() const noexcept;
    void setVerticesVisible(bool visible) noexcept;
    void removeVertices() noexcept;
};

