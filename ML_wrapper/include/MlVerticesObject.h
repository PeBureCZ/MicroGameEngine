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

    void addObjects(std::vector <DrawableObject<float>> content);
    void addObjects(sf::VertexArray content);
    [[nodiscard]] VerticesId getUniqueId() const noexcept;
    void setRotation(float newRotation) noexcept;
    [[nodiscard]] float getRotation() const noexcept;

    void setPosition(const FPoint& newPosition);
    [[nodiscard]] FPoint getPosition() const noexcept; //absolute

    [[nodiscard]] const sf::VertexArray& getVertices() const noexcept;
    void setColor(mgeType::Color_RGBA newColor);
    [[nodiscard]] mgeType::Color_RGBA getColor() noexcept;
    void moveAbsolutePosition(const FPoint& newPos);

    [[nodiscard]] const sf::RenderStates& getRenderState() const noexcept;
};

