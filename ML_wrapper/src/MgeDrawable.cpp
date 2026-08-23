#include "MgeDrawable.h"

#include "ShapeInterpreter.h"
#include "MlWrapper.h"
#include "GlobalFunctions.h"

MgeDrawable::MgeDrawable(size_t layer)
    : m_layer(layer)
{
    // Initialize the rectangle (triangle strip)
	m_vertices = std::make_shared<MlVerticesObject>();
    m_vertices->m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
    ML_wrapper::getGlobalMlWrapper()->addMlVerticesObject(m_vertices, m_layer);
}

MgeDrawable::MgeDrawable(const DrawableObject<float>& drawable, FPoint absolutePositionOffset, float rotation, size_t layer)
{
    sf::VertexArray convertedShapes;
    ShapeInterpreter::convertRectangleToVertices(drawable, convertedShapes);
    m_vertices = std::make_shared<MlVerticesObject>();
	m_vertices->m_batch = convertedShapes;
    frameObject = drawable;
    moveAbsolutePosition(absolutePositionOffset);
    m_layer = layer;
    ML_wrapper::getGlobalMlWrapper()->addMlVerticesObject(m_vertices, m_layer);
}

MgeDrawable::MgeDrawable(MgeDrawable&& other) noexcept
{
    if (other.m_vertices)
    {
        m_vertices = other.m_vertices; //shared smart ptr deployed in layer system
        frameObject = std::move(other.frameObject);
        m_absolutePositionOffset = other.m_absolutePositionOffset;
        m_layer = other.m_layer;
        ML_wrapper::getGlobalMlWrapper()->removeMlVerticesObject(other.m_vertices, other.m_layer);
        other.m_vertices.reset();
        ML_wrapper::getGlobalMlWrapper()->addMlVerticesObject(m_vertices, m_layer);
    }
}

MgeDrawable::~MgeDrawable()
{
    if (m_vertices)
    {
        ML_wrapper::getGlobalMlWrapper()->removeMlVerticesObject(m_vertices, m_layer);
        _ASSERT(m_vertices.use_count() == 1); //correct = vertices live in MgeDrawable only, not in any layer or another object
        m_vertices.reset();
    }
}

void MgeDrawable::addObjects(const DrawableObject<float> content)
{
    _ASSERT(m_vertices);
    if (m_vertices)
    {
        sf::VertexArray convertedShapes;
        ShapeInterpreter::convertRectangleToVertices(content, convertedShapes);
        for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
            m_vertices->m_batch.append(convertedShapes[i]);
    }
}

void MgeDrawable::addObjects(std::vector <DrawableObject<float>> content)
{
    MAIN_THREAD_GUARD;
    _ASSERT(m_vertices);
    if (m_vertices)
    {
        sf::VertexArray convertedShapes = ShapeInterpreter::convertRectanglesToVertices(content);
        for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
            m_vertices->m_batch.append(convertedShapes[i]);
    }
}

void MgeDrawable::addObjects(sf::VertexArray content)
{
    content.setPrimitiveType(sf::PrimitiveType::Triangles);
    MAIN_THREAD_GUARD;
    if (!m_vertices)
    {
        m_vertices = std::make_shared<MlVerticesObject>();
        ML_wrapper::getGlobalMlWrapper()->addMlVerticesObject(m_vertices, m_layer);
    }

    for (size_t i = 0; i < content.getVertexCount(); ++i)
        m_vertices->m_batch.append(std::move(content[i]));
}

VerticesId MgeDrawable::getUniqueId() const noexcept
{
    return reinterpret_cast<VerticesId>(this);
}

void MgeDrawable::setRotation(float newRotation) noexcept
{
    _ASSERT(m_vertices);
    if (m_vertices)
    {
        m_rotation = newRotation;
        m_vertices->m_state.transform = sf::Transform::Identity;
        m_vertices->m_state.transform.translate(sf::Vector2f(m_absolutePositionOffset.x, m_absolutePositionOffset.y)); //move to world position
        m_vertices->m_state.transform.rotate(sf::degrees(m_rotation)); //rotate around origin (0,0)
    }
}

float MgeDrawable::getRotation() const noexcept
{
    _ASSERT(m_vertices);
    return m_rotation;
}

void MgeDrawable::setPosition(const FPoint& newPosition)
{
    _ASSERT(m_vertices);
    moveAbsolutePosition(newPosition - m_absolutePositionOffset);
}

FPoint MgeDrawable::getPosition() const noexcept
{
    _ASSERT(m_vertices);
    return m_absolutePositionOffset;
}

const sf::VertexArray& MgeDrawable::getVertices() const noexcept
{
    _ASSERT(m_vertices);
    if (m_vertices)
        return m_vertices->m_batch;
    static sf::VertexArray empty;
    return empty;
}

void MgeDrawable::setColor(const mgeType::Color_RGBA& newColor)
{
    m_color = newColor;
    _ASSERT(m_vertices);
    if (m_vertices)
    {
        m_alpha = newColor.a;
        setVerticesColor();
    }
}

mgeType::Color_RGBA MgeDrawable::getColor() const noexcept
{
    return m_color;
}

void MgeDrawable::setIsVisible(bool visible) noexcept
{
    m_isVisible = visible;
    setVerticesColor();
}

void MgeDrawable::moveAbsolutePosition(const FPoint& newPos)
{
    _ASSERT(m_vertices);
    if (m_vertices)
    {
        m_absolutePositionOffset += newPos;
        m_vertices->m_state.transform.translate(sf::Vector2f(newPos.x, newPos.y));
    }
}

const sf::RenderStates& MgeDrawable::getRenderState() const noexcept
{
    if (m_vertices)
        return m_vertices->m_state;
    static sf::RenderStates empty;
    return empty;
}

void MgeDrawable::setVerticesColor()
{
    auto currentColor = getColor();
    currentColor.a = (m_isVisible) ? m_alpha : 0;
    sf::Color color(currentColor.r, currentColor.g, currentColor.b, currentColor.a);
    for (size_t i = 0; i < m_vertices->m_batch.getVertexCount(); ++i)
        m_vertices->m_batch[i].color = color;
}

size_t MgeDrawable::getLayer() const noexcept
{
    return m_layer;
}

void MgeDrawable::setLayer(size_t newLayer)
{
    MAIN_THREAD_GUARD;
    if (m_vertices)
    {
        ML_wrapper::getGlobalMlWrapper()->removeMlVerticesObject(m_vertices, m_layer);
        ML_wrapper::getGlobalMlWrapper()->addMlVerticesObject(m_vertices, newLayer);
    }
    m_layer = newLayer;
}
