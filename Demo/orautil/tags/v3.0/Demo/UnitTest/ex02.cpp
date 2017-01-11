#include "tool.h"
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
// 数据库连接池Pool测试
//

// 采用RAII手法实现"自动初始化" 示例: 
class ASPAutoInit
{
public:
	ASPAutoInit() { ASPInitialize(); }
	~ASPAutoInit() { ASPCleanup(); }
};
static ASPAutoInit aspAutoInit;	

static ASPPool aspPool;
static ASPConnection aspCon;


// ***************************************************************************** 
//   OpenPool Test
// ***************************************************************************** 
TEST(OpenPool, open)
{
	EXPECT_NO_THROW(aspPool.Open("mybk106", "test123", "test123", 1, 10));
}

// ***************************************************************************** 
//   GetConnection Test
// ***************************************************************************** 
TEST(GetConnection, get)
{
	EXPECT_NO_THROW(aspCon = aspPool.GetConnection());
}

// ***************************************************************************** 
//   OpenDB Test
// ***************************************************************************** 
TEST(OpenDB, IsConnected)
{
	EXPECT_NO_THROW(aspCon.IsConnected());
	EXPECT_TRUE(aspCon.IsConnected());
}

// ***************************************************************************** 
//   SelectOne Test
// ***************************************************************************** 
TEST(SelectOne, short)
{
	short numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<short>("select 32767 from dual"));
	EXPECT_EQ(numMax, 32767);

	short numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<short>("select -32768 from dual"));
	EXPECT_EQ(numMin, -32768);

	EXPECT_ANY_THROW(aspCon.SelectOne<short>("select 32767 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<short>("select -32768 - 1 from dual"));
}

TEST(SelectOne, ushort)
{
	unsigned short numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<unsigned short>("select 65535 from dual"));
	EXPECT_EQ(numMax, 65535);

	unsigned short numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<unsigned short>("select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned short>("select 65535 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned short>("select 0 - 1 from dual"));
}

#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
TEST(SelectOne, int)
{
	int numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<int>("select 2147483647 from dual"));
	EXPECT_EQ(numMax, 2147483647);

	int numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<int>("select -2147483648 from dual"));
	// 下面代码为错误写法:
	// vs2010 导致: error C4146: 一元负运算符应用于无符号类型，结果仍为无符号类型
	// vs2013编译器直接判定为错误
	// 参考: http://www.hankcs.com/program/cpp/error-c4146-%E4%B8%80%E5%85%83%E8%B4%9F%E8%BF%90%E7%AE%97%E7%AC%A6%E5%BA%94%E7%94%A8%E4%BA%8E%E6%97%A0%E7%AC%A6%E5%8F%B7%E7%B1%BB%E5%9E%8B%EF%BC%8C%E7%BB%93%E6%9E%9C%E4%BB%8D%E4%B8%BA%E6%97%A0.html
	//EXPECT_EQ(numMin, -2147483648);		// 错误写法
	EXPECT_EQ(numMin, INT_MIN);				// 正确写法

	EXPECT_ANY_THROW(aspCon.SelectOne<int>("select 2147483647 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<int>("select -2147483648 - 1 from dual"));	
}

TEST(SelectOne, uint)
{
	unsigned int numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<unsigned int>("select 4294967295 from dual"));
	EXPECT_EQ(numMax, 4294967295);

	unsigned int numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<unsigned int>("select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned int>("select 4294967295 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned int>("select 0 - 1 from dual"));
}

TEST(SelectOne, float)
{
	float num = 0.0f;
	EXPECT_NO_THROW(num = aspCon.SelectOne<float>("select 3.14f from dual"));
	EXPECT_EQ(num, 3.14f);
}

TEST(SelectOne, double)
{
	double num = 0.0;
	EXPECT_NO_THROW(num = aspCon.SelectOne<double>("select 3.1415926 from dual"));
	EXPECT_EQ(num, 3.1415926);
}

TEST(SelectOne, Date)
{
	const string cmp("2016-12-26 19:21:55");
	string dt;
	EXPECT_NO_THROW(
		dt = (aspCon.SelectOne<ASPDate>(
		"select to_date('2016-12-26 19:21:55', 'yyyy-mm-dd hh24:mi:ss') from dual")).
		ToString("yyyy-mm-dd hh24:mi:ss"));
	EXPECT_EQ(dt, cmp);

	EXPECT_NO_THROW(dt = (aspCon.SelectOne<ASPDate>(
		"select to_date('2016-12-26 19:21:55', 'yyyy-mm-dd hh24:mi:ss') from dual")).
		ToString());
	EXPECT_EQ(dt, cmp);

	EXPECT_ANY_THROW((aspCon.SelectOne<ASPDate>(
		"select to_date('2016-02-44 19:21:55', 'yyyy-mm-dd hh24:mi:ss') from dual")).
		ToString());
}

#define LONGLONG_MIN     (-9223372036854775807 - 1) /* minimum (signed) long long value */
TEST(SelectOne, long_long)
{
	long long numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<long long>("select 9223372036854775807 from dual"));
	EXPECT_EQ(numMax, 9223372036854775807);

	long long numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<long long>("select -9223372036854775808 from dual"));
	// EXPECT_EQ(numMin, -9223372036854775808);		// 错误写法:
	EXPECT_EQ(numMin, LONGLONG_MIN);				// 正确写法

	EXPECT_ANY_THROW(aspCon.SelectOne<long long>("select 9223372036854775807 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<long long>("select -9223372036854775808 - 1 from dual"));
}

TEST(SelectOne, ulong_long)
{
	unsigned long long numMax = 0;
	EXPECT_NO_THROW(numMax = aspCon.SelectOne<unsigned long long>("select 18446744073709551615 from dual"));
	EXPECT_EQ(numMax, 18446744073709551615);

	unsigned long long numMin = 0;
	EXPECT_NO_THROW(numMin = aspCon.SelectOne<unsigned long long>("select 0 from dual"));
	EXPECT_EQ(numMin, 0);

	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned long long>("select 18446744073709551615 + 1 from dual"));
	EXPECT_ANY_THROW(aspCon.SelectOne<unsigned long long>("select 0 - 1 from dual"));	
}

// ***************************************************************************** 
//   Select Test
// ***************************************************************************** 
TEST(Select, selectSingleRow)
{
	ASPDataset dt;
	EXPECT_NO_THROW(dt = aspCon.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum = 1"));
	EXPECT_TRUE(dt.Next());
	EXPECT_NO_THROW(dt.Get<string>(1));
	EXPECT_NO_THROW(dt.Get<string>(2));
	EXPECT_NO_THROW(dt.Get<ASPDate>(3));
	EXPECT_NO_THROW(dt.Get<double>(4));
	EXPECT_NO_THROW(dt.Get<string>(5));
}

TEST(Select, selectMultiRow)
{
	ASPDataset dt;
	EXPECT_NO_THROW(dt = aspCon.Select("select jlbh, hphm, jgsj, cwkc, sbbh from b_kkqp_jgcl where rownum <= 30"));
	while (dt.Next())
	{
		EXPECT_TRUE(dt.Next());
		EXPECT_NO_THROW(dt.Get<string>(1));
		EXPECT_NO_THROW(dt.Get<string>(2));
		EXPECT_NO_THROW(dt.Get<ASPDate>(3));
		EXPECT_NO_THROW(dt.Get<double>(4));
		EXPECT_NO_THROW(dt.Get<string>(5));
	}
}

// ***************************************************************************** 
//   Execute Test
// *****************************************************************************
TEST(Execute, execute)
{
	const string jlbh = NewGUID();		// 记录编号
	const string sql = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, "
		"CWKC, HPYS, CLLX, JLLX, HDSJ, BZWZDM, SBBH, CDBH, FXBH, TPSL, CLBJ, SHBJ, ZFBJ, "
		" SJLY, CCLX, BZ, TJBJ, CSYS, JLLB, XSFX, CSBJ, YZYS, TPLX, XRSJ, URL) values "
		" ('"
		+ jlbh +		
		"', '陕CT8283', 0, to_date('10-08-2016 00:03:53', "
		"'dd-mm-yyyy hh24:mi:ss'), 22, 80, 80, 3.00, '0', '2', '0', 610300, '610300001045',"
		"'610300001045', 2, 'EW', 1, null, '0', '0', '127.0.0.1', 1, null, null, 0, 0, 1, "
		"1, 1, 999, to_date('10-08-2016 00:44:40', 'dd-mm-yyyy hh24:mi:ss'), "
		"'/201605/25/08/12345678901-20160525083919-13-1.jpg;')";

	// Rollback测试
	// ++++++++++++
	EXPECT_NO_THROW(aspCon.Execute(sql));

	const string sqlSel = "select jlbh from b_kkqp_jgcl where jlbh = '" + jlbh + "'";
	EXPECT_NO_THROW(aspCon.Rollback());
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(aspCon.SelectOne<string>(sqlSel).empty());
	// -------------

	// Commit测试
	// ++++++++++++++
	EXPECT_NO_THROW(aspCon.Execute(sql));
	EXPECT_NO_THROW(aspCon.Commit());
	// --------------

	// Execute 删除测试
	// ++++++++++++++++
	const string sql3 = "delete from b_kkqp_jgcl where jlbh = '" + jlbh + "'";
	EXPECT_NO_THROW(aspCon.Execute(sql3));
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(aspCon.SelectOne<string>(sqlSel).empty());

	// 未提交之前, 使用其他连接会话查询, 应该还能查询出此"记录编号"
	// +++
	ASPConnection tmpCon;
	tmpCon.Open("mybk106", "test123", "test123");
	string tmp = tmpCon.SelectOne<string>(sqlSel);
	EXPECT_EQ(tmp, jlbh);
	tmpCon.Close();
	// ---

	// 删除提交
	EXPECT_NO_THROW(aspCon.Commit());
	// 插入记录后又回滚, 再查询此记录, 查询结果应该为空
	EXPECT_TRUE(aspCon.SelectOne<string>(sqlSel).empty());
	// ----------------
}

// ***************************************************************************** 
//   CloseDB Test
// ***************************************************************************** 
TEST(CloseDB, close)
{
	EXPECT_NO_THROW(aspCon.Close());
}

// ***************************************************************************** 
//   ClosePool Test
// ***************************************************************************** 
TEST(ClosePool, open)
{
	EXPECT_NO_THROW(aspPool.Close());
}

int main(int argc, TCHAR* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}