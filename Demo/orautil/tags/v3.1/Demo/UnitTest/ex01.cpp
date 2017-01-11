#include "tool.h"
#include <gtest/gtest.h>
#include <tchar.h>
#include <orautil/orautil.hpp>
#include <limits>       // std::numeric_limits
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

#ifdef _DEBUG
#pragma comment(lib, "gtest-vc100-md-sd.lib")
#else
#pragma  comment(lib, "gtest-vc100-md-s.lib")
#endif

// 1. OpenDB Test
// 2. SelectOne Test
// 3. Execute (Insert, Rollback, Commit) Test
// 4. CloseDB Test

// 采用RAII手法实现"自动初始化" 示例: 
static asp::OciEnvInit ociEnvInit;

static ocilib::Connection aspCon;


// ***************************************************************************** 
//   OpenDB Test
// ***************************************************************************** 
TEST(OpenDB, open)
{
	EXPECT_NO_THROW(aspCon.Open("mybk106", "test123", "test123"));
}

TEST(OpenDB, IsConnected)
{
	EXPECT_NO_THROW(asp::IsConnected(aspCon));
	EXPECT_TRUE(asp::IsConnected(aspCon));
}

// ***************************************************************************** 
//   SelectOne Test
// ***************************************************************************** 
TEST(SelectOne, short)
{
	short numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<short>(aspCon, "select 32767 from dual"));
	EXPECT_EQ(numMax, 32767);

	short numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<short>(aspCon, "select -32768 from dual"));
	EXPECT_EQ(numMin, -32768);

	EXPECT_ANY_THROW(asp::SelectOne<short>(aspCon, "select 32767 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<short>(aspCon, "select -32768 - 1 from dual"));
}

TEST(SelectOne, ushort)
{
	unsigned short numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<unsigned short>(aspCon, "select 65535 from dual"));
	EXPECT_EQ(numMax, 65535);

	unsigned short numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<unsigned short>(aspCon, "select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(asp::SelectOne<unsigned short>(aspCon, "select 65535 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<unsigned short>(aspCon, "select 0 - 1 from dual"));
}

#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
TEST(SelectOne, int)
{
	int numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<int>(aspCon,"select 2147483647 from dual"));
	EXPECT_EQ(numMax, 2147483647);

	int numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<int>(aspCon,"select -2147483648 from dual"));
	// 下面代码为错误写法:
	// vs2010 导致: error C4146: 一元负运算符应用于无符号类型，结果仍为无符号类型
	// vs2013编译器直接判定为错误
	// 参考: http://www.hankcs.com/program/cpp/error-c4146-%E4%B8%80%E5%85%83%E8%B4%9F%E8%BF%90%E7%AE%97%E7%AC%A6%E5%BA%94%E7%94%A8%E4%BA%8E%E6%97%A0%E7%AC%A6%E5%8F%B7%E7%B1%BB%E5%9E%8B%EF%BC%8C%E7%BB%93%E6%9E%9C%E4%BB%8D%E4%B8%BA%E6%97%A0.html
	//EXPECT_EQ(numMin, -2147483648);		// 错误写法
	EXPECT_EQ(numMin, INT_MIN);				// 正确写法

	EXPECT_ANY_THROW(asp::SelectOne<int>(aspCon,"select 2147483647 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<int>(aspCon,"select -2147483648 - 1 from dual"));	
}

TEST(SelectOne, uint)
{
	unsigned int numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<unsigned int>(aspCon, "select 4294967295 from dual"));
	EXPECT_EQ(numMax, 4294967295);

	unsigned int numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<unsigned int>(aspCon, "select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(asp::SelectOne<unsigned int>(aspCon, "select 4294967295 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<unsigned int>(aspCon, "select 0 - 1 from dual"));
}

TEST(SelectOne, float)
{
	float num = 0.0f;
	EXPECT_NO_THROW(num = asp::SelectOne<float>(aspCon, "select 3.14f from dual"));
	EXPECT_EQ(num, 3.14f);
}

TEST(SelectOne, double)
{
	double num = 0.0;
	EXPECT_NO_THROW(num = asp::SelectOne<double>(aspCon, "select 3.1415926 from dual"));
	EXPECT_EQ(num, 3.1415926);
}

TEST(SelectOne, Date)
{
	const std::string cmp("2016-12-26 19:21:55");
	std::string dt;
	EXPECT_NO_THROW(
		dt = (asp::SelectOne<ocilib::Date>(aspCon, 
		"select to_date('2016-12-26 19:21:55', 'yyyy-mm-dd hh24:mi:ss') from dual")).
		ToString("yyyy-mm-dd hh24:mi:ss"));
	EXPECT_EQ(dt, cmp);

	EXPECT_ANY_THROW((asp::SelectOne<ocilib::Date>(aspCon, 
		"select to_date('2016-02-44 19:21:55', 'yyyy-mm-dd hh24:mi:ss') from dual")).
		ToString());
}

#define LONGLONG_MIN     (-9223372036854775807 - 1) /* minimum (signed) long long value */
TEST(SelectOne, long_long)
{
	long long numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<long long>(aspCon, "select 9223372036854775807 from dual"));
	EXPECT_EQ(numMax, 9223372036854775807);

	long long numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<long long>(aspCon, "select -9223372036854775808 from dual"));
	// EXPECT_EQ(numMin, -9223372036854775808);		// 错误写法:
	EXPECT_EQ(numMin, LONGLONG_MIN);				// 正确写法

	EXPECT_ANY_THROW(asp::SelectOne<long long>(aspCon, "select 9223372036854775807 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<long long>(aspCon, "select -9223372036854775808 - 1 from dual"));
}

TEST(SelectOne, ulong_long)
{
	unsigned long long numMax = 0;
	EXPECT_NO_THROW(numMax = asp::SelectOne<unsigned long long>(aspCon, "select 18446744073709551615 from dual"));
	EXPECT_EQ(numMax, 18446744073709551615);

	unsigned long long numMin = 0;
	EXPECT_NO_THROW(numMin = asp::SelectOne<unsigned long long>(aspCon, "select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(asp::SelectOne<unsigned long long>(aspCon, "select 18446744073709551615 + 1 from dual"));
	EXPECT_ANY_THROW(asp::SelectOne<unsigned long long>(aspCon, "select 0 - 1 from dual"));	
}

// ***************************************************************************** 
//   Select Test
// ***************************************************************************** 
TEST(Select, selectSingleRow)
{
	ocilib::Statement st(aspCon);
	EXPECT_NO_THROW(st.Execute("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum = 1"));
	ocilib::Resultset rs = st.GetResultset();
	EXPECT_TRUE(rs.Next());
	EXPECT_NO_THROW(rs.Get<std::string>(1));
	EXPECT_NO_THROW(rs.Get<std::string>(2));
	EXPECT_NO_THROW(rs.Get<ocilib::Date>(3));
	EXPECT_NO_THROW(rs.Get<double>(4));
	EXPECT_NO_THROW(rs.Get<std::string>(5));
}

TEST(Select, selectMultiRow)
{
	ocilib::Statement st(aspCon);
	EXPECT_NO_THROW(st.Execute("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30"));
	ocilib::Resultset rs = st.GetResultset();
	while (rs.Next())
	{
		EXPECT_TRUE(rs.Next());
		EXPECT_NO_THROW(rs.Get<std::string>(1));
		EXPECT_NO_THROW(rs.Get<std::string>(2));
		EXPECT_NO_THROW(rs.Get<ocilib::Date>(3));
		EXPECT_NO_THROW(rs.Get<double>(4));
		EXPECT_NO_THROW(rs.Get<std::string>(5));
	}
}

// ***************************************************************************** 
//   Execute Test
// *****************************************************************************
TEST(Execute, execute)
{
	const std::string jlbh = NewGUID();		// 记录编号
	const std::string sql = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, "
		"CWKC, HPYS, CLLX, JLLX, HDSJ, BZWZDM, SBBH, CDBH, FXBH, TPSL, CLBJ, SHBJ, ZFBJ, "
		" SJLY, CCLX, BZ, TJBJ, CSYS, JLLB, XSFX, CSBJ, YZYS, TPLX, XRSJ, URL) values "
		" ('"
		+ jlbh +		
		"', '陕CT8283', 0, to_date('10-08-2016 00:03:53', "
		"'dd-mm-yyyy hh24:mi:ss'), 22, 80, 80, 3.00, '0', '2', '0', 610300, '610300001045',"
		"'610300001045', 2, 'EW', 1, null, '0', '0', '127.0.0.1', 1, null, null, 0, 0, 1, "
		"1, 1, 999, to_date('10-08-2016 00:44:40', 'dd-mm-yyyy hh24:mi:ss'), "
		"'/201605/25/08/12345678901-20160525083919-13-1.jpg;')";

	ocilib::Statement st(aspCon);

	// Rollback测试
	// ++++++++++++
	EXPECT_NO_THROW(st.Execute(sql));

	const std::string sqlSel = "select jlbh from b_kkqp_jgcl where jlbh = '" + jlbh + "'";
	EXPECT_NO_THROW(aspCon.Rollback());
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(asp::SelectOne<std::string>(aspCon, sqlSel).empty());
	// -------------

	// Commit测试
	// ++++++++++++++
	EXPECT_NO_THROW(st.Execute(sql));
	EXPECT_NO_THROW(aspCon.Commit());
	// --------------

	// Execute 删除测试
	// ++++++++++++++++
	const std::string sql3 = "delete from b_kkqp_jgcl where jlbh = '" + jlbh + "'";
	EXPECT_NO_THROW(st.Execute(sql3));
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(asp::SelectOne<std::string>(aspCon, sqlSel).empty());

	// 未提交之前, 使用其他连接会话查询, 应该还能查询出此"记录编号"
	// +++
	ocilib::Connection tmpCon;
	tmpCon.Open("mybk106", "test123", "test123");
	std::string tmp = asp::SelectOne<std::string>(tmpCon, sqlSel);
	EXPECT_EQ(tmp, jlbh);
	tmpCon.Close();
	// ---

	// 删除提交
	EXPECT_NO_THROW(aspCon.Commit());
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(asp::SelectOne<std::string>(aspCon, sqlSel).empty());
	// ----------------
}

// ***************************************************************************** 
//   CloseDB Test
// ***************************************************************************** 
TEST(CloseDB, close)
{
	EXPECT_NO_THROW(aspCon.Close());
}

int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}