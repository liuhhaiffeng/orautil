#include <gtest/gtest.h>
#include <tchar.h>
#include <orautil/orautil.hpp>
#include <limits>       // std::numeric_limits
#include <stdlib.h>		// _countof
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

#ifdef _DEBUG
#pragma comment(lib, "gtest-vc100-md-sd.lib")
#else
#pragma  comment(lib, "gtest-vc100-md-s.lib")
#endif

//
// ASPDate类 测试
//

// 采用RAII手法实现"自动初始化" 示例: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

// ***************************************************************************** 
//   OpenPool Test
// ***************************************************************************** 
TEST(ASPDate, default_ctor)
{
	ASPDate dt;
	EXPECT_TRUE(dt.IsNull());

	ASPDate dt2 = ASPDate::SysDate();
	EXPECT_FALSE(dt2.IsNull());
}

TEST(ASPDate, str_ctor)
{
	const string testDateStr = "2016-12-17 11:53:44";
	ASPDate dt(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	EXPECT_FALSE(dt.IsNull());

	ASPDate dt2(testDateStr);
	EXPECT_FALSE(dt2.IsNull());
}

TEST(ASPDate, timet_ctor)
{
	time_t tt = time(NULL);
	ASPDate dt(tt);
	EXPECT_FALSE(dt.IsNull());
}

TEST(ASPDate, copy_ctor)
{
	ASPDate dt;
	ASPDate dtCopy(dt);
	EXPECT_TRUE(dtCopy.IsNull());

	const string testDateStr = "2016-12-17 11:53:44";
	ASPDate dt2(testDateStr);
	ASPDate dt2Copy(dt2);
	EXPECT_FALSE(dt2Copy.IsNull());
}

TEST(ASPDate, copy_operator)
{
	ASPDate dt;
	ASPDate dtCopy(dt);
	EXPECT_TRUE(dtCopy.IsNull());

	ASPDate dt2("2016-12-17 11:53:44", "yyyy-mm-dd hh24:mi:ss");
	ASPDate dt2Copy(dt2);
	EXPECT_FALSE(dt2Copy.IsNull());
}

TEST(ASPDate, SysDate)
{
	EXPECT_EQ((ASPDate::SysDate()).GetTimet(), time(NULL));	
}

TEST(ASPDate, FromString)
{
	const string testDateStr = "2016-12-17 11:53:44";

	ASPDate dt;
	dt.FromString(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	EXPECT_EQ(dt.ToString("yyyy-mm-dd hh24:mi:ss"), testDateStr);
	EXPECT_EQ(dt.ToString(), testDateStr);
}

TEST(ASPDate, FromTimet)
{
	const time_t tt = time(NULL);

	ASPDate dt;
	dt.FromTimet(tt);
	EXPECT_EQ(dt.GetTimet(), tt);
}

TEST(ASPDate, ToString)
{
	ASPDate dt("2016/12/27", "yyyy/mm/dd");
	EXPECT_EQ(dt.ToString("yyyy/mm/dd"), string("2016/12/27"));

	ASPDate dt2("2016-12-17 13:42:05", "yyyy-mm-dd hh24:mi:ss");
	EXPECT_EQ(dt2.ToString("yyyy-mm-dd hh24:mi:ss"), string("2016-12-17 13:42:05"));
	EXPECT_EQ(dt2.ToString(), string("2016-12-17 13:42:05"));

	ASPDate dtNull;
	EXPECT_ANY_THROW(dtNull.ToString("yyyy-mm-dd hh24:mi:ss"));
	EXPECT_ANY_THROW(dtNull.ToString());
}

TEST(ASPDate, GetTimet)
{
	const time_t tt = time(NULL);

	ASPDate dt(tt);
	EXPECT_EQ(dt.GetTimet(), tt);
}

TEST(ASPDate, operator_plus_sub)
{
	ASPDate dt("2016-12-27 13:37:59");

	++dt;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:38:00"));
	dt++;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:38:01"));

	--dt;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:38:00"));
	dt--;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:37:59"));

	dt += 120;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:39:59"));
	dt -= 120;
	EXPECT_EQ(dt.ToString(), string("2016-12-27 13:37:59"));

	ASPDate dt2("2016-12-27 13:37:59");
	ASPDate dtAdd = dt2 + 130;
	EXPECT_EQ(dtAdd.ToString(), string("2016-12-27 13:40:09"));
	ASPDate dtSub = dt2 - 130;
	EXPECT_EQ(dtSub.ToString(), string("2016-12-27 13:35:49"));
}

TEST(ASPDate, operator_bool)
{
	EXPECT_TRUE(ASPDate("2016-12-27 13:35:49") == ASPDate("2016-12-27 13:35:49"));
	EXPECT_FALSE(ASPDate("2016-12-27 13:35:49") != ASPDate("2016-12-27 13:35:49"));
	EXPECT_FALSE(ASPDate("2016-12-27 13:35:49") > ASPDate("2016-12-27 13:35:49"));
	EXPECT_FALSE(ASPDate("2016-12-27 13:35:49") < ASPDate("2016-12-27 13:35:49"));
	EXPECT_TRUE(ASPDate("2016-12-27 13:35:49") >= ASPDate("2016-12-27 13:35:49"));
	EXPECT_TRUE(ASPDate("2016-12-27 13:35:49") <= ASPDate("2016-12-27 13:35:49"));
}

int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}