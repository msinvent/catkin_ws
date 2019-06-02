#include <ros/ros.h>
#include <gtest/gtest.h>
#include "classy_lib.hpp"

TEST(TestSuite, equalityTest)
{
	EXPECT_EQ(1+2,3);
}

TEST(TestSuite, unequalityTest)
{
	EXPECT_EQ(1+2,4);
}

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc,argv);
	
	ros::init(argc,argv,"testing");
	ros::NodeHandle nh;
	ros::shutdown();
	
	return RUN_ALL_TESTS();
}
	

