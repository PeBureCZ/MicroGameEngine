#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <variant>  
#include "graphicdependencies.h"
#include "BasicShapes.h"

class MlVerticesObject
{
public:
private:
    sf::VertexArray batch;
    sf::RenderStates state;
    FPoint absolutePositionOffset;
    float rotation = 0.0f;
    size_t layer = GraphicItemLayer::DEFAULT_LAYER;

public:
    MlVerticesObject();

    void addObjects(std::vector <DrawableObject<float>> content) noexcept;
    void addObjects(sf::VertexArray content) noexcept;
    uintptr_t getUniqueId() const noexcept;
    void setRotation(float newRotation) noexcept;
    float getRotation() const noexcept;

    const sf::VertexArray& getVertices() const noexcept;
    void setColor(tsmType::Color_RGBA newColor) noexcept;
    tsmType::Color_RGBA getColor() noexcept;
    void moveAbsolutePosition(FPoint& newPos) noexcept;

    [[nodiscard]] const sf::RenderStates& getRenderState() const noexcept;
};

