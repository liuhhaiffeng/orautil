#include <gtest/gtest.h>
#include <tchar.h>
#include <orautil/orautil.hpp>
#include <limits>       // std::numeric_limits
#include <stdlib.h>		// _countof
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

#ifdef _DEBUG
#pragma comment(lib, "gtest-vc100-md-sd.lib")
#else
#pragma  comment(lib, "gtest-vc100-md-s.lib")
#endif

//
// ocilib::Date类 测试
//

// 采用RAII手法实现"自动初始化" 示例: 
static asp::OciEnvInit ociEnvInit;

// ***************************************************************************** 
//   OpenPool Test
// ***************************************************************************** 
TEST(ocilib_Date, default_ctor)
{
	ocilib::Date dt;
	EXPECT_TRUE(dt.IsNull());

	ocilib::Date dt2 = ocilib::Date::SysDate();
	EXPECT_FALSE(dt2.IsNull());
}

TEST(ocilib_Date, str_ctor)
{
	const std::string testDateStr = "2016-12-17 11:53:44";
	ocilib::Date dt(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	EXPECT_FALSE(dt.IsNull());

	ocilib::Date dt2(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	EXPECT_FALSE(dt2.IsNull());
}

TEST(ocilib_Date, copy_ctor)
{
	ocilib::Date dt;
	ocilib::Date dtCopy(dt);
	EXPECT_TRUE(dtCopy.IsNull());

	const std::string testDateStr = "2016-12-17 11:53:44";
	ocilib::Date dt2(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	ocilib::Date dt2Copy(dt2);
	EXPECT_FALSE(dt2Copy.IsNull());
}

TEST(ocilib_Date, copy_operator)
{
	ocilib::Date dt;
	ocilib::Date dtCopy(dt);
	EXPECT_TRUE(dtCopy.IsNull());

	ocilib::Date dt2("2016-12-17 11:53:44", "yyyy-mm-dd hh24:mi:ss");
	ocilib::Date dt2Copy(dt2);
	EXPECT_FALSE(dt2Copy.IsNull());
}

TEST(ocilib_Date, SysDate)
{
	EXPECT_EQ(ocilib::Date::SysDate(), asp::Time2Date(time(NULL)));	
}

TEST(ocilib_Date, FromString)
{
	const std::string testDateStr = "2016-12-17 11:53:44";

	ocilib::Date dt;
	EXPECT_TRUE(dt.IsNull());
	EXPECT_ANY_THROW(dt.FromString(testDateStr, "yyyy-mm-dd hh24:mi:ss"));

	ocilib::Date dt2(testDateStr, "yyyy-mm-dd hh24:mi:ss");
	EXPECT_FALSE(dt2.IsNull());
	EXPECT_NO_THROW(dt2.FromString(testDateStr, "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_EQ(dt2.ToString("yyyy-mm-dd hh24:mi:ss"), testDateStr);
}

TEST(ocilib_Date, ToString)
{
	ocilib::Date dt("2016/12/27", "yyyy/mm/dd");
	EXPECT_EQ(dt.ToString("yyyy/mm/dd"), std::string("2016/12/27"));

	ocilib::Date dt2("2016-12-17 13:42:05", "yyyy-mm-dd hh24:mi:ss");
	EXPECT_EQ(dt2.ToString("yyyy-mm-dd hh24:mi:ss"), std::string("2016-12-17 13:42:05"));

	ocilib::Date dtNull;
	EXPECT_TRUE(dtNull.IsNull());
	EXPECT_EQ(dtNull.ToString("yyyy-mm-dd hh24:mi:ss"), OCI_STRING_NULL);
}

TEST(ocilib_Date, operator_plus_sub)
{
	ocilib::Date dt("2016-12-27 13:37:59", "yyyy-mm-dd hh24:mi:ss");

	asp::AddSeconds(dt, 1);
	EXPECT_EQ(dt.ToString("yyyy-mm-dd hh24:mi:ss"), std::string("2016-12-27 13:38:00"));

	asp::AddSeconds(dt, -1);
	EXPECT_EQ(dt.ToString("yyyy-mm-dd hh24:mi:ss"), std::string("2016-12-27 13:37:59"));
}

TEST(ocilib_Date, operator_bool)
{
	EXPECT_TRUE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 		
		== ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_FALSE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 
		!= ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_FALSE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 
		> ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_FALSE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 
		< ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_TRUE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 
		>= ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
	EXPECT_TRUE(ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss") 
		<= ocilib::Date("2016-12-27 13:35:49", "yyyy-mm-dd hh24:mi:ss"));
}

int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}