#pragma once
#include "BasicShapes.h"
#include "BasicTypes.h"
#include "MgeDrawable.h"

inline sf::Vector2f rotatePoint(sf::Vector2f p, float rad)
{
    float c = cosf(rad);
    float s = sinf(rad);
    return {
        p.x * c - p.y * s,
        p.x * s + p.y * c
    };
}

class ShapeInterpreter
{
public:

    static void convertRectangleToVertices(const MgeVertices<float>& object, sf::VertexArray& batch)
    {
        auto addRect = [&](sf::Vector2f pos, sf::Vector2f size, float rotationDeg, sf::Color color)
            {
                batch.setPrimitiveType(sf::PrimitiveType::Triangles);
                float rad = rotationDeg * 3.14159265f / 180.f;

                sf::Vector2f half = size * 0.5f;
                sf::Vector2f center = pos/* + half*/;

                sf::Vector2f corners[4] =
                {
                    { 0.f, 0.f },
                    { size.x, 0.f },
                    { size.x, size.y },
                    { 0.f, size.y }
                };

                sf::Vector2f world[4];
                for (int i = 0; i < 4; i++)
                {
                    sf::Vector2f r = rotatePoint(corners[i], rad);
                    world[i] = center + r;
                }

                batch.append({ world[0], color });
                batch.append({ world[1], color });
                batch.append({ world[2], color });

                batch.append({ world[0], color });
                batch.append({ world[2], color });
                batch.append({ world[3], color });
            };

        auto& shape = object.getShape();
        auto color = object.getColor();
        if (std::holds_alternative<mgeShape::Rectangle<float>>(shape))
        {
            mgeShape::Rectangle<float> rectanglesFloat = std::get<mgeShape::Rectangle<float>>(shape);
            addRect
            (
                sf::Vector2f{ rectanglesFloat.getPosition().x, rectanglesFloat.getPosition().y },
                sf::Vector2f{ rectanglesFloat.getSize().width, rectanglesFloat.getSize().height },
                rectanglesFloat.getRotation(),
                sf::Color(color.r, color.g, color.b, color.a)

            );
        }
        else if (std::holds_alternative<mgeShape::Circle<float>>(shape))
        {
            //not yet implemented
        }
        else
            _ASSERT(false); // Unsupported shape type
    }

    static sf::VertexArray convertRectanglesToVertices(std::vector<MgeVertices<float>>& shapesToConvert)
    {
        sf::VertexArray batch;
        for (auto& object : shapesToConvert)
            convertRectangleToVertices(object, batch);
        return batch;
    }
};

        