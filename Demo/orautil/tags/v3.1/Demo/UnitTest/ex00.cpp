#include <gtest/gtest.h>
#include <tchar.h>
#include <orautil/orautil.hpp>

#ifdef _DEBUG
#pragma comment(lib, "gtest-vc100-md-sd.lib")
#else
#pragma  comment(lib, "gtest-vc100-md-s.lib")
#endif

// Google Test 用法简单示例

// ***************************************************************************** 
//   MyTestCase1
// ***************************************************************************** 
TEST(MyTestCase1, MyCaseName1)
{
	EXPECT_FALSE(2 != 1);			// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}

TEST(MyTestCase1, MyCaseName2)
{
	EXPECT_FALSE(2 != 1);			// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}

TEST(MyTestCase1, MyCaseName3)
{
	EXPECT_FALSE(2 != 1);		// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}


// ***************************************************************************** 
//   MyTestCase2
// ***************************************************************************** 
TEST(MyTestCase2, MyCaseName1)
{
	EXPECT_FALSE(2 != 1);		// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}

TEST(MyTestCase2, MyCaseName2)
{
	EXPECT_FALSE(2 != 1);		// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}

TEST(MyTestCase2, MyCaseName3)
{
	EXPECT_FALSE(2 != 1);		// Test failed
	EXPECT_TRUE(2 == 2);
	EXPECT_EQ(2, 2);
}

int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}