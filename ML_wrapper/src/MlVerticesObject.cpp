#include "MlVerticesObject.h"

#include "ShapeInterpreter.h"

MlVerticesObject::MlVerticesObject(size_t layer)
    : m_layer(layer)
{
    // Initialize the rectangle (triangle strip)
    m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
}

MlVerticesObject::MlVerticesObject(sf::VertexArray batch, sf::RenderStates state, FPoint absolutePositionOffset, float rotation, size_t layer)
    : m_batch(batch), m_state(state), m_absolutePositionOffset(absolutePositionOffset), m_rotation(rotation), m_layer(layer)
{
}

void MlVerticesObject::addObjects(std::vector <DrawableObject<float>> content)
{
    sf::VertexArray convertedShapes = ShapeInterpreter::convertRectangleToVertices(content);
    for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
        m_batch.append(convertedShapes[i]);
}

void MlVerticesObject::addObjects(sf::VertexArray content)
{
    for (size_t i = 0; i < content.getVertexCount(); ++i)
        m_batch.append(content[i]);
}

VerticesId MlVerticesObject::getUniqueId() const noexcept
{
    return reinterpret_cast<VerticesId>(this);
}

void MlVerticesObject::setRotation(float newRotation) noexcept
{
    m_rotation = newRotation;

    m_state.transform = sf::Transform::Identity;
    //move to world position
    m_state.transform.translate(sf::Vector2f(m_absolutePositionOffset.x, m_absolutePositionOffset.y));
    //rotate around origin (0,0)
    m_state.transform.rotate(sf::degrees(m_rotation));
}

float MlVerticesObject::getRotation() const noexcept
{
    return m_rotation;
}

void MlVerticesObject::setPosition(const FPoint& newPosition)
{
    moveAbsolutePosition(newPosition - m_absolutePositionOffset);
}

FPoint MlVerticesObject::getPosition() const noexcept
{
    return m_absolutePositionOffset;
}

const sf::VertexArray& MlVerticesObject::getVertices() const noexcept
{
    return m_batch;
}

void MlVerticesObject::setColor(mgeType::Color_RGBA newColor)
{
    sf::Color color(newColor.r, newColor.g, newColor.b, newColor.a);
    for (size_t i = 0; i < m_batch.getVertexCount(); ++i)
        m_batch[i].color = color;
}

mgeType::Color_RGBA MlVerticesObject::getColor() noexcept
{
    if (m_batch.getVertexCount() > 0)
        return mgeType::Color_RGBA(m_batch[0].color.r, m_batch[0].color.g, m_batch[0].color.b, m_batch[0].color.a);
    return mgeType::Color_RGBA();
}

void MlVerticesObject::moveAbsolutePosition(const FPoint& newPos)
{
    m_absolutePositionOffset += newPos;
    m_state.transform.translate(sf::Vector2f(newPos.x, newPos.y));
}

const sf::RenderStates& MlVerticesObject::getRenderState() const noexcept
{
    return m_state;
}

std::shared_ptr<MlVerticesObject> MlVerticesObject::createCopyWithNewLayer(size_t newLayer) const noexcept
{
    return std::make_shared<MlVerticesObject>(m_batch, m_state, m_absolutePositionOffset, m_rotation, newLayer);
}

size_t MlVerticesObject::getLayer() const noexcept
{
    return m_layer;
}
