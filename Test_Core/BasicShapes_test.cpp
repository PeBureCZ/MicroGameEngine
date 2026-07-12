#include "pch.h"

#include "..\MGE_core\include\BasicShapes.h"

#include <cmath>

TEST(BasicShapes, Default)
{
	mgeShape::Shape<int> shape;
	constexpr float epsilon = 0.00001f;
	EXPECT_TRUE(std::fabs(shape.getRotation()) < epsilon);
	EXPECT_EQ(shape.getType(), SHAPE_TYPE::UNDEFINED);

	mgeShape::Circle<float> circle(FPoint(), 10.f);
	EXPECT_TRUE(std::fabs(circle.getRotation()) - 10.f < epsilon);
	EXPECT_EQ(circle.getType(), SHAPE_TYPE::circle);
}
