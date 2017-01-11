// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <array>
#include <stdio.h>
#include <windows.h>
#include <process.h>
namespace asp = asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ocilib::Date 没有提供针对"小时", "分钟"和"秒"级别的加减操作, 
// 提供三个辅助函数 AddHours(), AddMintues(), AddSeconds()

#define USE_CSTYLE_THREAD	1

static CRITICAL_SECTION cs; 
static const int TotalNum = 60;
static HANDLE events[TotalNum] = {NULL};
static DWORD threadsID[TotalNum] = {0};

#if USE_CSTYLE_THREAD
static void MutilThreadInset(void* pThreadNum);
#else
static DWORD WINAPI MutilThreadInset(void* pThreadNum);
#endif

// 采用RAII手法实现"自动初始化" 示例: 
static asp::OciEnvInit ociEnvInit;

static ocilib::Pool aspPool;

int main()
{
	ocilib::Connection con;

	try
	{
		con.Open("mybk106", "test123", "test123");
		std::cout << "打开数据库成功\n";
	}
	catch (std::exception &ex)
	{
		std::cout << "打开数据库失败:" << ex.what() << '\n';
		return 0;
	}

	try
	{
		ocilib::Date dt = asp::SelectOne<ocilib::Date>(con, "select sysdate from dual");
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddSeconds(dt, 20);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddSeconds(dt, -20);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddMintues(dt, 12);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddMintues(dt, -12);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddHours(dt, 1);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

		asp::AddHours(dt, -1);
		std::cout << dt.ToString("yyyy-mm-dd hh24:mi:ss") << '\n';

	}
	catch (std::exception &ex)
	{
		std::cout << ex.what() << '\n';
	}

	getchar();	

	return 0;
}





