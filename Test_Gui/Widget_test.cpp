#include "gtest/gtest.h"

#include "Widget_test.h"
TEST(Widgets, ptr)
{
	auto widget = std::make_shared< Widget_test>();
	guiAdd::addWidgetToGui(widget);
	bool testCase = widget->mock_getSelfPtr().lock() ? true : false;
	EXPECT_TRUE(testCase);

	EXPECT_EQ(widget->getAbsolutePosition(), startTestPosition);
}

TEST(Widgets, basicLocation)
{
	auto widget = std::make_shared< Widget_test>();
	EXPECT_EQ(widget->getAbsolutePosition(), startTestPosition);

	constexpr IPoint testLocation1(77,44);
	widget->setRelativePosition(testLocation1);
	EXPECT_EQ(widget->getAbsolutePosition(), testLocation1);
	EXPECT_EQ(widget->getRelativePosition(), testLocation1);
}
TEST(Widgets, parentLocation)
{
	auto widget_child = std::make_shared< Widget_test>();
	auto widget_parent = std::make_shared< Widget_test>();


	constexpr IPoint testLocation(10, 10);
	widget_parent->setRelativePosition(testLocation);
	widget_child->setRelativePosition(testLocation);

	guiAdd::addWidgetToGui(widget_parent);
	guiAdd::addWidgetToGui(std::dynamic_pointer_cast<Widget>(widget_parent), std::dynamic_pointer_cast<Widget>(widget_child));


	EXPECT_EQ(widget_parent->getAbsolutePosition(), testLocation);
	EXPECT_EQ(widget_parent->getRelativePosition(), testLocation);

	EXPECT_EQ(widget_child->getAbsolutePosition(), testLocation * 2);
	EXPECT_EQ(widget_child->getRelativePosition(), testLocation);
}