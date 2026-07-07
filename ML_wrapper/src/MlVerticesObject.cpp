#include "MlVerticesObject.h"

#include "ShapeInterpreter.h"

MlVerticesObject::MlVerticesObject()
{
    // Initialize the rectangle (triangle strip)
    batch.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void MlVerticesObject::addObjects(std::vector <DrawableObject<float>> content) noexcept
{
    sf::VertexArray convertedShapes = ShapeInterpreter::convertRectangleToVertices(content);
    for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
        batch.append(convertedShapes[i]);
}

void MlVerticesObject::addObjects(sf::VertexArray content) noexcept
{
    for (size_t i = 0; i < content.getVertexCount(); ++i)
		batch.append(content[i]);
}

VerticesId MlVerticesObject::getUniqueId() const noexcept
{
    return reinterpret_cast<VerticesId>(this);
}

void MlVerticesObject::setRotation(float newRotation) noexcept
{
    rotation = newRotation;

    state.transform = sf::Transform::Identity;
    //move to world position
    state.transform.translate(sf::Vector2f(absolutePositionOffset.x,absolutePositionOffset.y));
    //rotate around origin (0,0)
    state.transform.rotate(sf::degrees(rotation));
}

float MlVerticesObject::getRotation() const noexcept
{
    return rotation;
}

const sf::VertexArray& MlVerticesObject::getVertices() const noexcept
{
    return batch;
}

void MlVerticesObject::setColor(tsmType::Color_RGBA newColor) noexcept
{
    sf::Color color(newColor.r, newColor.g, newColor.b, newColor.a);
    for (size_t i = 0; i < batch.getVertexCount(); ++i)
		batch[i].color = color;
}

tsmType::Color_RGBA MlVerticesObject::getColor() noexcept
{
    if (batch.getVertexCount() > 0)
        return tsmType::Color_RGBA(batch[0].color.r, batch[0].color.g, batch[0].color.b, batch[0].color.a);
    return tsmType::Color_RGBA();
}

void MlVerticesObject::moveAbsolutePosition(FPoint& newPos) noexcept
{
    absolutePositionOffset += newPos;
    state.transform.translate(sf::Vector2f(newPos.x, newPos.y));
}

const sf::RenderStates& MlVerticesObject::getRenderState() const noexcept
{
    return state;
}
