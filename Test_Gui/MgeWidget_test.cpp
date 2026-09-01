#include "gtest/gtest.h"

#include "MgeWidget_test.h"
TEST(Widgets, ptr)
{
	auto widget = std::make_shared<MgeWidget_test>();
	mgeGui::addWidgetToGui(widget);
	bool testCase = widget->mock_getSelfPtr().lock() ? true : false;
	EXPECT_TRUE(testCase);

	EXPECT_EQ(widget->getAbsolutePosition(), startTestPosition);
}

TEST(Widgets, basicLocation)
{
	auto widget = std::make_shared<MgeWidget_test>();
	EXPECT_EQ(widget->getAbsolutePosition(), startTestPosition);

	constexpr FPoint testLocation1(77.f,44.f);
	widget->setRelativePosition(testLocation1);
	EXPECT_EQ(widget->getAbsolutePosition(), testLocation1);
	EXPECT_EQ(widget->getRelativePosition(), testLocation1);
}
TEST(Widgets, parentLocation)
{
	auto widget_child = std::make_shared<MgeWidget_test>();
	auto widget_parent = std::make_shared<MgeWidget_test>();


	constexpr FPoint testLocation(10.f, 10.f);
	widget_parent->setRelativePosition(testLocation);
	widget_child->setRelativePosition(testLocation);

	mgeGui::addWidgetToGui(widget_parent);
	mgeGui::addWidgetToGui(widget_parent, widget_child);


	EXPECT_EQ(widget_parent->getAbsolutePosition(), testLocation);
	EXPECT_EQ(widget_parent->getRelativePosition(), testLocation);

	auto newLocation = testLocation * 2;
	EXPECT_NE(newLocation, testLocation);
	EXPECT_EQ(widget_child->getAbsolutePosition(), newLocation);
	EXPECT_EQ(widget_child->getRelativePosition(), testLocation);
}