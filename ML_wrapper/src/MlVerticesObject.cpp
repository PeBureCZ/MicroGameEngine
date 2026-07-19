#include "MgeDrawable.h"

#include "ShapeInterpreter.h"

MgeDrawable::MgeDrawable()
{
    // Initialize the rectangle (triangle strip)
    batch.setPrimitiveType(sf::PrimitiveType::Triangles);
}

void MgeDrawable::addObjects(std::vector <DrawableObject<float>> content)
{
    sf::VertexArray convertedShapes = ShapeInterpreter::convertRectangleToVertices(content);
    for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
        batch.append(convertedShapes[i]);
}

void MgeDrawable::addObjects(sf::VertexArray content)
{
    for (size_t i = 0; i < content.getVertexCount(); ++i)
		batch.append(content[i]);
}

[[nodiscard]] VerticesId MgeDrawable::getUniqueId() const noexcept
{
    return reinterpret_cast<VerticesId>(this);
}

void MgeDrawable::setRotation(float newRotation) noexcept
{
    rotation = newRotation;

    state.transform = sf::Transform::Identity;
    //move to world position
    state.transform.translate(sf::Vector2f(absolutePositionOffset.x,absolutePositionOffset.y));
    //rotate around origin (0,0)
    state.transform.rotate(sf::degrees(rotation));
}

[[nodiscard]] float MgeDrawable::getRotation() const noexcept
{
    return rotation;
}

void MgeDrawable::setPosition(const FPoint& newPosition)
{
    moveAbsolutePosition(newPosition - absolutePositionOffset);
}

[[nodiscard]] FPoint MgeDrawable::getPosition() const noexcept
{
    return absolutePositionOffset;
}

[[nodiscard]] const sf::VertexArray& MgeDrawable::getVertices() const noexcept
{
    return batch;
}

void MgeDrawable::setColor(mgeType::Color_RGBA newColor)
{
    sf::Color color(newColor.r, newColor.g, newColor.b, newColor.a);
    for (size_t i = 0; i < batch.getVertexCount(); ++i)
		batch[i].color = color;
}

[[nodiscard]] mgeType::Color_RGBA MgeDrawable::getColor() noexcept
{
    if (batch.getVertexCount() > 0)
        return mgeType::Color_RGBA(batch[0].color.r, batch[0].color.g, batch[0].color.b, batch[0].color.a);
    return mgeType::Color_RGBA();
}

void MgeDrawable::moveAbsolutePosition(const FPoint& newPos)
{
    absolutePositionOffset += newPos;
    state.transform.translate(sf::Vector2f(newPos.x, newPos.y));
}

[[nodiscard]] const sf::RenderStates& MgeDrawable::getRenderState() const noexcept
{
    return state;
}
