#include "MgeDrawable.h"

#include "ShapeInterpreter.h"
#include "MlWrapper.h"
#include "GlobalFunctions.h"
#include "MgeImage.h"

MgeDrawable::MgeDrawable(size_t layer)
{
    MAIN_THREAD_GUARD;
    MlVerticesObject vertices;
    vertices.m_batch.setPrimitiveType(sf::PrimitiveType::Triangles);
    auto newDrawable = std::make_shared<MgeLayerObject>(MgeLayerObject{ mgeCore::getDefaultZPosition(), layer, std::move(vertices)});
    ML_wrapper::getGlobalMlWrapper()->addMgeLayerObject(newDrawable);
    m_vertices = newDrawable;
}

MgeDrawable::MgeDrawable(const MgeVertices<float>& drawable, FPoint absolutePositionOffset, float rotation, size_t layer)
{
    MAIN_THREAD_GUARD;
    sf::VertexArray convertedShapes;
    ShapeInterpreter::convertRectangleToVertices(drawable, convertedShapes);
    frameObject = drawable;
    m_alpha = drawable.getColor().a;
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
    MAIN_THREAD_GUARD;
    removeVertices();
    if (auto drawable = other.m_vertices.lock())
    {
        m_vertices = drawable;
        frameObject = std::move(other.frameObject);
        m_absolutePositionOffset = other.m_absolutePositionOffset;
        m_rotation = other.m_rotation;
        m_alpha = other.m_alpha;
        m_isVisible = other.m_isVisible;

        other.m_vertices.reset(); //to eliminate double free in layer system
    }
}

MgeDrawable& MgeDrawable::operator= (MgeDrawable&& other) noexcept
{
    MAIN_THREAD_GUARD;
    removeVertices();
    if (auto drawable = other.m_vertices.lock())
    {
        m_vertices = drawable;
        frameObject = std::move(other.frameObject);
        m_absolutePositionOffset = other.m_absolutePositionOffset;
        m_rotation = other.m_rotation;
        m_alpha = other.m_alpha;
        m_isVisible = other.m_isVisible;

        other.m_vertices.reset(); //to eliminate double free in layer system
    }
    return *this;
}

void MgeDrawable::addObjects(const MgeVertices<float> content)
{
    MAIN_THREAD_GUARD;
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
    MAIN_THREAD_GUARD;
    content.setPrimitiveType(sf::PrimitiveType::Triangles);
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
    MAIN_THREAD_GUARD;
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
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    return m_rotation;
}

[[nodiscard]] bool MgeDrawable::isValid() const noexcept
{
    return !m_vertices.expired();
}

void MgeDrawable::setPosition(const FPoint& newPosition)
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    moveAbsolutePosition(newPosition - m_absolutePositionOffset);
}

FPoint MgeDrawable::getPosition() const noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    return m_absolutePositionOffset;
}

const std::shared_ptr<MgeLayerObject> MgeDrawable::getVertices() const noexcept
{
    MAIN_THREAD_GUARD;
    return m_vertices.lock();
}

void MgeDrawable::setColor(const MgeColor& newColor)
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (!m_vertices.expired())
    {
        m_alpha = newColor.a;
        setVerticesColor(newColor);
    }
}

std::optional<MgeColor> MgeDrawable::getColor() const noexcept
{
    MAIN_THREAD_GUARD;
    return getVerticesColor();
}

void MgeDrawable::setIsVisible(bool visible) noexcept
{
    MAIN_THREAD_GUARD;
    m_isVisible = visible;
    if (!m_vertices.expired())
        setVerticesVisible(visible);
}

void MgeDrawable::moveAbsolutePosition(const FPoint& newPos)
{
    MAIN_THREAD_GUARD;
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
    MAIN_THREAD_GUARD;
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

void MgeDrawable::setVerticesColor(const MgeColor& color) noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());

    try
    {
        if (auto vertices = m_vertices.lock())
        {
            if (std::holds_alternative<MlVerticesObject>(vertices->data))
            {
                auto& mlVertices = std::get<MlVerticesObject>(vertices->data);

                for (size_t i = 0; i < mlVertices.m_batch.getVertexCount(); ++i)
                    mlVertices.m_batch[i].color = sf::Color(color.r, color.g, color.b, (m_isVisible) ? color.a : 0);
            }
            else
            {
                _ASSERT(false);
            }
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
}

std::optional<MgeColor> MgeDrawable::getVerticesColor() const noexcept
{
    try
    {
        if (auto vertices = m_vertices.lock())
        {
            if (std::holds_alternative<MlVerticesObject>(vertices->data))
            {
                const auto& mlVertices = std::get<MlVerticesObject>(vertices->data);
                size_t count = mlVertices.m_batch.getVertexCount();
                if (count > 0)
                {
                    const auto& firstVertexColor = mlVertices.m_batch[0].color;
#ifdef _DEBUG
                    for (size_t i = 0; i < count; ++i)
                    {
                        const auto& vertexColor = mlVertices.m_batch[i].color;
                        _ASSERT(vertexColor.r == firstVertexColor.r && vertexColor.g == firstVertexColor.g
                            && vertexColor.b == firstVertexColor.b); // different vertex color is not supported yet
                    }
#endif
                    //alpha is the logical one: hidden objects must not report alpha 0,
                    //otherwise setColor(getColor()) would make them permanently invisible
                    return MgeColor{ firstVertexColor.r, firstVertexColor.g, firstVertexColor.b, m_alpha };
                }
            }
            else
            {
                _ASSERT(false);
            }
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
    return std::nullopt;
}

void MgeDrawable::setVerticesVisible(bool visible) noexcept
{
    //todo: need to re-do the current approach to not draw vertices!
    MAIN_THREAD_GUARD;
    try
    {
        if (auto vertices = m_vertices.lock())
        {
            if (std::holds_alternative<MlVerticesObject>(vertices->data))
            {
                auto& mlVertices = std::get<MlVerticesObject>(vertices->data);

                for (size_t i = 0; i < mlVertices.m_batch.getVertexCount(); ++i)
                    mlVertices.m_batch[i].color.a = (visible) ? m_alpha : 0;
            }
            else
            {
                _ASSERT(false);
            }
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
}

void MgeDrawable::removeVertices() noexcept
{
    MAIN_THREAD_GUARD;
    try
    {
        if (auto vertices = m_vertices.lock())
        {
            ML_wrapper::getGlobalMlWrapper()->removeMgeLayerObject(vertices);
            m_vertices.reset();
            _ASSERT(vertices.use_count() == 1); //correct = last instance is local
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
}

size_t MgeDrawable::getLayer() const noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
        return vertices->m_layer;
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
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    try
    {
        if (auto vertices = m_vertices.lock())
        {
            vertices->zPosition = newZPosition;
            ML_wrapper::getGlobalMlWrapper()->sortLayerByGroupAndZ_delayed(getLayer());
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
}

int64_t MgeDrawable::getZPosition() const noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
        return vertices->zPosition;
    return 0;
}

void MgeDrawable::setDrawGroup(const int64_t newDrawGroup) noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    try
    {
        if (auto vertices = m_vertices.lock())
        {
            if (vertices->drawGroup == newDrawGroup)
                return;
            vertices->drawGroup = newDrawGroup;
            ML_wrapper::getGlobalMlWrapper()->sortLayerByGroupAndZ_delayed(vertices->m_layer);
        }
    }
    catch (...)
    {
        _ASSERT(false);
    }
}

int64_t MgeDrawable::getDrawGroup() const noexcept
{
    MAIN_THREAD_GUARD;
    _ASSERT(!m_vertices.expired());
    if (auto vertices = m_vertices.lock())
        return vertices->drawGroup;
    return 0;
}

MgeDrawable::~MgeDrawable()
{
    MAIN_THREAD_GUARD;
    removeVertices();
}

