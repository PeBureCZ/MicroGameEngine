#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <variant>  
#include "graphicdependencies.h"
#include "BasicShapes.h"
#include "MgeComponents.h"

class MlVerticesObject : public MgeBasicComponent
{
public:
    MlVerticesObject(size_t layer);
    MlVerticesObject(sf::VertexArray batch, sf::RenderStates state, FPoint absolutePositionOffset, float rotation, size_t layer);

    void addObjects(std::vector <DrawableObject<float>> content);
    void addObjects(sf::VertexArray content);
    [[nodiscard]] VerticesId getUniqueId() const noexcept;
    void setRotation(float newRotation) noexcept;
    [[nodiscard]] float getRotation() const noexcept;

    void setPosition(const FPoint& newPosition); //absolute coordination
    [[nodiscard]] FPoint getPosition() const noexcept; //absolute coordination

    [[nodiscard]] const sf::VertexArray& getVertices() const noexcept;
    void setColor(mgeType::Color_RGBA newColor);
    [[nodiscard]] mgeType::Color_RGBA getColor() noexcept;
    void moveAbsolutePosition(const FPoint& newPos);

    [[nodiscard]] size_t getLayer() const noexcept;

#ifdef _DEBUG
    void setLayer(size_t newLayer)
    {
        m_layer = newLayer;
    }
#endif


    [[nodiscard]] const sf::RenderStates& getRenderState() const noexcept;

    [[nodiscard]] std::shared_ptr<MlVerticesObject> createCopyWithNewLayer(size_t newLayer) const noexcept;

private:
    sf::VertexArray m_batch;
    sf::RenderStates m_state;
    FPoint m_absolutePositionOffset;
    float m_rotation = 0.0f;
    size_t m_layer = GraphicItemLayer::DEFAULT_LAYER;
};

