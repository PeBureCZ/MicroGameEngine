#include "gtest/gtest.h"

#include "Frame_test.h"
TEST(Frames, layeredPosition)
{
	auto testFrame = std::make_shared<Frame_test>();
	//testFrame->layout();
	auto startPosition_rel = testFrame->getAbsolutePosition();
	auto savedLastLayeredPos_rel = testFrame->mock_getLastLayoutAbsolutePosition();
	EXPECT_EQ(savedLastLayeredPos_rel, startPosition_rel);

	constexpr IPoint newPos_rel(45, 999);
	testFrame->setRelativePosition(newPos_rel);
	savedLastLayeredPos_rel = testFrame->mock_getLastLayoutAbsolutePosition();
	EXPECT_NE(savedLastLayeredPos_rel, startPosition_rel);

	testFrame->layout();
	EXPECT_EQ(testFrame->getAbsolutePosition(), testFrame->mock_getLastLayoutAbsolutePosition());
	EXPECT_NE(startPosition_rel, testFrame->getAbsolutePosition());
}

TEST(Frames, common)
{
	EXPECT_EQ(1, 1);
}