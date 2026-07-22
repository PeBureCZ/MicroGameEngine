#pragma once

#include <vector>
#include <cmath>
#include <cstdlib>

#include <SFML/Graphics.hpp>
#include "BasicTypes.h"
#include "MlVerticesObject.h"


template<typename T>
T bezierPoint(float t, const T& P0, const T& P1, const T& P2, const T& P3)
{
    float u = 1.f - t;

    return u * u * u * P0 +
        3.f * u * u * t * P1 +
        3.f * u * t * t * P2 +
        t * t * t * P3;
}

// Arc-length sampling table
template<typename T>
struct BezierSample
{
    float t;
    float length;
};

template<typename T>
std::vector<BezierSample<T>> buildLengthTable
    (
        const T& P0, const T& P1, const T& P2, const T& P3,
        int samples, float& totalLength
    )
{
    std::vector<BezierSample<T>> table;
    table.reserve(samples + 1);

    totalLength = 0.0f;

    T prev = bezierPoint(0.f, P0, P1, P2, P3);
    table.push_back({ 0.f, 0.f });

    for (int i = 1; i <= samples; ++i)
    {
        float t = i / static_cast<float>(samples);
        T curr = bezierPoint(t, P0, P1, P2, P3);

        T diff = curr - prev;
        float segLen = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        totalLength += segLen;
        table.push_back({ t, totalLength });

        prev = curr;
    }

    return table;
}

// Find t for given arc length (linear search + interpolation)
template<typename T>
float findT(float targetLength, const std::vector<BezierSample<T>>& table)
{
    for (size_t i = 1; i < table.size(); ++i)
    {
        if (table[i].length >= targetLength)
        {
            float l1 = table[i - 1].length;
            float l2 = table[i].length;
            float t1 = table[i - 1].t;
            float t2 = table[i].t;

            float alpha = (targetLength - l1) / (l2 - l1);
            return t1 + alpha * (t2 - t1);
        }
    }
    return 1.0f;
}

template<typename T>
class MlCurve
{
public:
    MlCurve() = default;

    MlCurve(T P0, T P1, T P2, T P3, float thick = 1.f,
        mgeType::Color_RGBA color = mgeType::Color_RGBA(), float desiredSegmentLength = 20.f);

    [[nodiscard]] float getLength() const
    {
        return totalLength;
    }

    const sf::VertexArray& getVertices() const noexcept
    {
        return vertices.getVertices();
    }

    mgeType::Point<float> getPositionAt(float distance) const
    {
        if (sampledPoints.empty())
            return {};

		constexpr float epsilon = 0.0001f;

        if (distance <= 0.f || segmentLength <= epsilon)
        {
            _ASSERT(distance > -epsilon); //wrong distance
            auto& point = sampledPoints.front();
            return { point.x, point.y};
        }

        if (distance >= totalLength)
        {
            _ASSERT(distance == totalLength); //wrong distance
            auto& point = sampledPoints.back();
            return { point.x, point.y };
        }

        float segment = distance / segmentLength;
        size_t index = static_cast<size_t>(segment);
        index = (sampledPoints.size() > 1)
            ? (std::min)(index, sampledPoints.size() - 2)
            : 0;

        float t = segment - (float)index;

        if (index + 1 >= sampledPoints.size())
        {
            _ASSERT(false);
			auto& point = sampledPoints.back();
            return { point.x, point.y };
        }

        const auto& A = sampledPoints[index];
        const auto& B = sampledPoints[index + 1];
		const auto result = A + (B - A) * t;
        return { result.x, result.y};
    }

    mgeType::Point<float> pointAtPercent(float percent) const
    {
        float distance = percent * totalLength;
        return getPositionAt(distance);
    }

    size_t getSegmentCount() const noexcept
    {
		return segmentsCount;
    }

    mgeType::Point<float> pointAtSegment(size_t segmentIndex) const
    {
        if (segmentIndex >= segmentsCount)
        {
            _ASSERT(false);
            auto& point = sampledPoints.back();
            return { point.x, point.y };
        }

        const auto& findedSegment = sampledPoints[segmentIndex];
        return { findedSegment.x, findedSegment.y };
    }

private:
    std::vector<sf::Vector2f> sampledPoints;
    float totalLength = 0.f;

    float distanceFromLine(const T& P, const T& A, const T& B);

    void build(mgeType::Color_RGBA color, float desiredSegmentLength);

private:
    MlVerticesObject vertices = MlVerticesObject(GraphicItemLayer::DEFAULT_LAYER);
    size_t segmentsCount = 0;
    float segmentLength = 0.f;
    float thickness = 1.0f;
    T positions[4];
};


template<typename T>
MlCurve<T>::MlCurve(T P0, T P1, T P2, T P3, float thick, mgeType::Color_RGBA color, float desiredSegmentLength)
    : positions{ P0, P1, P2, P3 },
    thickness(thick)
{
    build(std::move(color), desiredSegmentLength);
}

template<typename T>
float MlCurve<T>::distanceFromLine(const T& P, const T& A, const T& B)
{
    T AB = B - A;
    T AP = P - A;
    float area = std::abs(AB.x * AP.y - AB.y * AP.x);
    float len = std::sqrt(AB.x * AB.x + AB.y * AB.y);

    return (len > 0.f) ? area / len : 0.f;
}

template<typename T>
void MlCurve<T>::build(mgeType::Color_RGBA color, float desiredSegmentLength)
{
    // Convert relative to absolute
    T P0 = positions[0];
    T P1 = P0 + positions[1];
    T P2 = P0 + positions[2];
    T P3 = P0 + positions[3];

    // Build arc-length table
    const int tableSamples = 100;
    auto table = buildLengthTable(P0, P1, P2, P3, tableSamples, totalLength);

    // Determine number of segments
    float d1 = distanceFromLine(P1, P0, P3);
    float d2 = distanceFromLine(P2, P0, P3);
    float curvature = (std::max)(d1, d2);

    constexpr float epsilon = 0.01f;

    if (curvature < epsilon)
        segmentsCount = 1;
    else
    {
        int lengthSegments = static_cast<int>(totalLength / desiredSegmentLength);
        int curvatureSegments = static_cast<int>(curvature * 0.5f); // tweak

        segmentsCount = (std::max)(1, (std::max)(lengthSegments, curvatureSegments));
    }
    segmentLength = totalLength / segmentsCount;

    sf::VertexArray va(sf::PrimitiveType::Triangles);
    sf::Color sfColor(color.r, color.g, color.b, color.a);

    // Compute positions along the curve
	sampledPoints.clear();
    sampledPoints.reserve(segmentsCount + 1);
    for (int i = 0; i <= segmentsCount; ++i)
    {
        float t = findT(i * segmentLength, table);
        T Px = bezierPoint(t, P0, P1, P2, P3);
        sampledPoints.emplace_back(Px.x, Px.y);
    }

    // Compute miter offsets
    std::vector<sf::Vector2f> offsets;
    offsets.reserve(segmentsCount + 1);

    for (int i = 0; i <= segmentsCount; ++i)
    {
        sf::Vector2f prevDir, nextDir;

        if (i == 0)
        {
            // Start point: pretend prevDir = nextDir
            nextDir = sampledPoints[1] - sampledPoints[0];
            prevDir = nextDir;
        }
        else if (i == segmentsCount)
        {
            // End point: pretend nextDir = prevDir
            prevDir = sampledPoints[i] - sampledPoints[i - 1];
            nextDir = prevDir;
        }
        else
        {
            prevDir = sampledPoints[i] - sampledPoints[i - 1];
            nextDir = sampledPoints[i + 1] - sampledPoints[i];
        }

        // Normalize directions
        float lenPrev = std::sqrt(prevDir.x * prevDir.x + prevDir.y * prevDir.y);
        float lenNext = std::sqrt(nextDir.x * nextDir.x + nextDir.y * nextDir.y);
        if (lenPrev != 0.f) prevDir /= lenPrev;
        if (lenNext != 0.f) nextDir /= lenNext;

        // Compute tangent and miter
        sf::Vector2f tangent = prevDir + nextDir;
        float tLen = std::sqrt(tangent.x * tangent.x + tangent.y * tangent.y);
        if (tLen == 0.f) tangent = sf::Vector2f(-prevDir.y, prevDir.x);
        else tangent /= tLen;

        sf::Vector2f miter(-tangent.y, tangent.x);
        float dot = prevDir.x * miter.x + prevDir.y * miter.y;
        offsets.push_back(miter * thickness);
    }

    // Precompute left/right vertices
    std::vector<sf::Vector2f> left, right;
    left.reserve(segmentsCount + 1);
    right.reserve(segmentsCount + 1);
    for (int i = 0; i <= segmentsCount; ++i)
    {
        left.push_back(sampledPoints[i] + offsets[i]);
        right.push_back(sampledPoints[i] - offsets[i]);
    }

    // Build triangles per segment
    for (int i = 0; i < segmentsCount; ++i)
    {
        // Triangle 1
        va.append(sf::Vertex(left[i], sfColor));
        va.append(sf::Vertex(right[i], sfColor));
        va.append(sf::Vertex(right[i + 1], sfColor));

        // Triangle 2
        va.append(sf::Vertex(left[i], sfColor));
        va.append(sf::Vertex(right[i + 1], sfColor));
        va.append(sf::Vertex(left[i + 1], sfColor));
    }

    vertices.addObjects(std::move(va));
}