#include "gtest/gtest.h"

#include "MgeFrame_test.h"
#include "Trigger.h"

TEST(Triggers, rescaleIntBoxKeepsFractionalScale)
{
	auto sizeOf = [](const Trigger<int>& trigger)
		{ return std::get<mgeShape::Rectangle<int>>(trigger.getShape()).getSize(); };

	Trigger<int> trigger(true, mgeShape::Rectangle<int>(mgeType::Point<int>(0, 0), mgeType::Size<int>(90, 60)));
	trigger.rescaleShape(1.f, 0.5f); //90x60 -> 90x30 (was truncated to 0)
	EXPECT_EQ(sizeOf(trigger).width, 90);
	EXPECT_EQ(sizeOf(trigger).height, 30);
	EXPECT_TRUE(trigger.isPointInside(mgeType::Point<int>(45, 15)));

	trigger.rescaleShape(1.5f, 2.f); //90x30 -> 135x60 (1.5 was truncated to 1)
	EXPECT_EQ(sizeOf(trigger).width, 135);
	EXPECT_EQ(sizeOf(trigger).height, 60);
}
TEST(Frames, layeredPosition)
{
	//todo: Test throw asserts due to lack of real APP! The whole GUI unit-testing approach needs to be reworked.
	
	//auto testFrame = std::make_shared<MgeFrame_test>();
	//auto startPosition_rel = testFrame->getAbsolutePosition();
	//auto savedLastLayeredPos_rel = testFrame->mock_getLastLayoutAbsolutePosition();
	//EXPECT_EQ(savedLastLayeredPos_rel, startPosition_rel);

	//constexpr FPoint newPos_rel(45.f, 999.f);
	//testFrame->setRelativePosition(newPos_rel);
	//savedLastLayeredPos_rel = testFrame->mock_getLastLayoutAbsolutePosition();
	//EXPECT_GT(std::abs(savedLastLayeredPos_rel.x - startPosition_rel.x), 0.001f);
	//EXPECT_GT(std::abs(savedLastLayeredPos_rel.y - startPosition_rel.y), 0.001f);

	//testFrame->layout();
	//EXPECT_NEAR(startPosition_rel.x, testFrame->getAbsolutePosition().x, 0.001f);
	//EXPECT_NEAR(startPosition_rel.y, testFrame->getAbsolutePosition().y, 0.001f);
}