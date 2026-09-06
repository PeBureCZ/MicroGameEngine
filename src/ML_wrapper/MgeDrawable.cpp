#include "MgeDrawable.h"

#include "ShapeInterpreter.h"
#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include "MgeImage.h"

MgeDrawable::MgeDrawable(size_t layer)
{
    MlVerticesObject vertices;
    vertices.m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
    auto newDrawable = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), layer, std::move(vertices)});
    ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newDrawable);
    m_vertices = newDrawable;
}

MgeDrawable::MgeDrawable(const MgeVertices<float>& drawable, FPoint absolutePositionOffset, float rotation, size_t layer)
{
    sf::VertexArray convertedShapes;
    ShapeInterpreter::convertRectangleToVertices(drawable, convertedShapes);
    frameObject = drawable;
    MlVerticesObject vertices;
    vertices.m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
    vertices.m_batch = convertedShapes;
    auto newDrawable = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), layer, std::move(vertices) });
    ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newDrawable);
    m_vertices = newDrawable;
    moveAbsolutePosition(absolutePositionOffset);
}

MgeDrawable::MgeDrawable(MgeDrawable&& other) noexcept
{
    if (auto drawable = other.m_vertices.lock())
    {
        m_color = other.m_color;
        m_vertices = drawable;
        frameObject = std::move(other.frameObject);
        m_absolutePositionOffset = other.m_absolutePositionOffset;
        m_rotation = other.m_rotation;
        m_alpha = other.m_alpha;
        m_isVisible = other.m_isVisible;

        other.m_vertices.reset(); //to eliminate double free in layer system
    }
}

void MgeDrawable::addObjects(const MgeVertices<float> content)
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        sf::VertexArray convertedShapes;
        ShapeInterpreter::convertRectangleToVertices(content, convertedShapes);

        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            auto& sfVertices = std::get<MlVerticesObject>(vertices->data);
            for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
                sfVertices.m_batch.append(convertedShapes[i]);
        }
        else
            {  _ASSERT(false); } //wrong type
    }
}

void MgeDrawable::addObjects(std::vector <MgeVertices<float>> content)
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        sf::VertexArray convertedShapes = ShapeInterpreter::convertRectanglesToVertices(content);
        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            auto& sfVertices = std::get<MlVerticesObject>(vertices->data);
            for (size_t i = 0; i < convertedShapes.getVertexCount(); ++i)
                sfVertices.m_batch.append(convertedShapes[i]);
        }
        else
        {
            _ASSERT(false);
        } //wrong type
    }
}

void MgeDrawable::addObjects(sf::VertexArray content)
{
    content.setPrimitiveType(sf::PrimitiveType::Triangles);
    MAIN_THREAD_GUARD;
    if (m_vertices.expired())
    {
        MlVerticesObject vertices;
        vertices.m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
        auto newDrawable = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), getLayer(), std::move(vertices)});
        ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newDrawable);
        m_vertices = newDrawable;
    }

    auto drawable = m_vertices.lock();
    if (drawable && std::holds_alternative<MlVerticesObject>(drawable->data))
    {
        auto& mlVertices = std::get<MlVerticesObject>(drawable->data);
        for (size_t i = 0; i < content.getVertexCount(); ++i)
            mlVertices.m_batch.append(std::move(content[i]));;
    }
}

VerticesId MgeDrawable::getUniqueId() const noexcept
{
    return reinterpret_cast<VerticesId>(this);
}

void MgeDrawable::setRotation(float newRotation) noexcept
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        m_rotation = newRotation;

        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            auto& sfVertices = std::get<MlVerticesObject>(vertices->data);
            sfVertices.m_state.transform = sf::Transform::Identity;
            sfVertices.m_state.transform.translate(sf::Vector2f(m_absolutePositionOffset.x, m_absolutePositionOffset.y)); //move to world position
            sfVertices.m_state.transform.rotate(sf::degrees(m_rotation)); //rotate around origin (0,0)
        }
    }
}

float MgeDrawable::getRotation() const noexcept
{
    _ASSERT(!m_vertices.expired());
    return m_rotation;
}

void MgeDrawable::setPosition(const FPoint& newPosition)
{
    _ASSERT(!m_vertices.expired());
    moveAbsolutePosition(newPosition - m_absolutePositionOffset);
}

FPoint MgeDrawable::getPosition() const noexcept
{
    _ASSERT(!m_vertices.expired());
    return m_absolutePositionOffset;
}

const std::shared_ptr<MgeLayerObject> MgeDrawable::getVertices() const noexcept
{
    return m_vertices.lock();
}

void MgeDrawable::setColor(const mgeType::Color_RGBA& newColor)
{
    m_color = newColor;
    _ASSERT(!m_vertices.expired());
    if (!m_vertices.expired())
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
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            m_absolutePositionOffset += newPos;
            std::get<MlVerticesObject>(vertices->data).m_state.transform.translate(sf::Vector2f(newPos.x, newPos.y));
        }
        else
        {
            _ASSERT(false);
        }

    }
}

void MgeDrawable::rescale(float scaleX, float scaleY) noexcept
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            std::get<MlVerticesObject>(vertices->data).rescale(scaleX, scaleY);
            m_absolutePositionOffset.x *= scaleX;
            m_absolutePositionOffset.y *= scaleY;
        }
        else if (std::holds_alternative<sf::Sprite>(vertices->data))
        {
			_ASSERT(false); //not implemented yet
        }
        else if (std::holds_alternative<sf::Text>(vertices->data))
        {
			//no rescale for sf::Text
        }
        else
		{ // unknown type
			_ASSERT(false); //invalid rescale request for this type of object
        }
	}
}


void MgeDrawable::setVerticesColor()
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        if (std::holds_alternative<MlVerticesObject>(vertices->data))
        {
            auto& mlVertices = std::get<MlVerticesObject>(vertices->data);
            auto currentColor = getColor();
            currentColor.a = (m_isVisible) ? m_alpha : 0;
            sf::Color color(currentColor.r, currentColor.g, currentColor.b, currentColor.a);

            for (size_t i = 0; i < mlVertices.m_batch.getVertexCount(); ++i)
                mlVertices.m_batch[i].color = color;
        }
        else
        {
            _ASSERT(false);
        }
    }
}

size_t MgeDrawable::getLayer() const noexcept
{
    _ASSERT(!m_vertices.expired());
    if (auto sprite = m_vertices.lock())
        sprite->m_layer;
    return 0;
}

void MgeDrawable::setLayer(size_t newLayer)
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
        ML_wrapper::getGlobalMlWrapper()->changeLayerOfMgeObject(vertices, newLayer);
}

void MgeDrawable::setZPosition(const int64_t newZPosition) noexcept
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
    {
        vertices->zPosition = newZPosition;
        ML_wrapper::getGlobalMlWrapper()->sortLayerByZIndex_delayed(getLayer());
    }
}

int64_t MgeDrawable::getZPosition() const noexcept
{
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
        return vertices->zPosition;
    return 0;
}

MgeDrawable::~MgeDrawable()
{
    if (auto vertices = m_vertices.lock())
    {
        ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(vertices);
        m_vertices.reset();
        _ASSERT(vertices.use_count() == 1); //correct = last instance is local
    }
}

