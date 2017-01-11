// asppro.cpp : 定义控制台应用程序的入口点。
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <assert.h>
#include <array>
#include <stdio.h>
#include <windows.h>
#include <process.h>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// 多线程并发操作, WaitForMultipleObjects()等待线程完成

#define local static
#define USE_CSTYLE_THREAD	1

local CRITICAL_SECTION cs; 
local const int TotalNum = 60;
local HANDLE events[TotalNum] = {NULL};
local DWORD threadsID[TotalNum] = {0};

#if USE_CSTYLE_THREAD
local void MutilThreadInset(void* pThreadNum);
#else
local DWORD WINAPI MutilThreadInset(void* pThreadNum);
#endif

int main()
{
	try
	{
		//OracleUtil::OpenDB("数据库服务名", "用户名", "密码");
		OracleUtil::OpenDB("mybk106", "test123", "test123", 1, 10);
		cout << "打开数据库成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "打开数据库失败:" << ex.what() << endl;
		return 0;
	}

	OracleUtil ora;

	try
	{
		ora.Execute("drop table b_mutltest");
		cout << "删除表成功" << endl;
	}
	catch (std::exception) 
	{ }

	try
	{
		ora.Execute(
			"create table b_mutltest "
			"("
				"jlbh varchar2(32) not null,"
				"xrsj date default sysdate,"
				"threadidx number(6)"
			")");
		cout << "创建表成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "创建表失败:" << ex.what() << endl;
		return 0;
	}

	InitializeCriticalSection(&cs);

	bool createEventGroupSucc = true;
	for (int i = 0; i < TotalNum; ++i)
	{
		events[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (events[i] == NULL)
		{
			createEventGroupSucc = false;
			cout << "创建事件组失败" << endl;
			break;
		}
	}

	if (!createEventGroupSucc)
	{
		for (int i = 0; i < TotalNum; ++i)
		{
			if (events[i] != NULL)
			{
				CloseHandle(events);
			}
		}

		return 0;
	}

	// 创建100个线程	
	int threadNum[TotalNum] = {0};
	for (int i  = 0; i < TotalNum; ++i)
	{
		threadNum[i] = i;
#if USE_CSTYLE_THREAD
		_beginthread(MutilThreadInset, 0, &(threadNum[i]));
#else
		CreateThread(NULL, 0, MutilThreadInset, &(threadNum[i]), 0, &(threadsID[i]));
#endif
	}
	cout << "创建线程池成功" << endl;	

	// MAXIMUM_WAIT_OBJECTS = 64
	static_assert(TotalNum <= MAXIMUM_WAIT_OBJECTS, 
		"The max-number of WaitForMultipleObjects() to wait for "
		"event-object is only MAXIMUM_WAIT_OBJECTS");
	int idx = 0;
	while (WaitForMultipleObjects(TotalNum, events, TRUE, 1000) == WAIT_TIMEOUT)
	{
		cout << ++idx << " >>>>>>>>>>>>>>>>>>>>>>>>等待任务完成..." << endl;
	}
	DWORD lasterr = GetLastError();
	cout << "所有任务完成, LastError:" << lasterr << endl;

	DeleteCriticalSection(&cs);

	for (int i = 0; i < TotalNum; ++i)
	{
		CloseHandle(events[i]);
	}

	std::unique_ptr<Dataset> dt = ora.Select2(
		"SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 "
		"FROM b_mutltest");

	ora.Select2("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 "
		"FROM b_mutltest");

	dt->Next();
	int count = dt->Get<DTInt>(1);
	DTDate maxXRSJ = dt->Get<DTDate>(2);
	DTDate minXRSJ = dt->Get<DTDate>(3);
	int secodes = dt->Get<DTInt>(4);
	cout << "插入总数\t" << "最大写入时间\t" << "最小写入时间\t" << "写入耗时" << endl;
	cout << count << ",\t" << maxXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
		<< ",\t" << minXRSJ.ToString("yyyy-mm-dd hh24:mi:ss")
		<< ",\t" << secodes << endl;

	getchar();	

	return 0;
}

#if USE_CSTYLE_THREAD
void MutilThreadInset(void* pThreadNum)
#else
DWORD WINAPI MutilThreadInset(void* pThreadNum)
#endif
{
	int id = * (static_cast<int*>(pThreadNum));

	OracleUtil ora;
	static int counter = 0;
	std::array<char, 50> szJLBH = {0};
	std::array<char, 256> szSql = {0};

	while (true)
	{
		EnterCriticalSection(&cs);
		counter += 1;
		LeaveCriticalSection(&cs);		

		sprintf_s(szJLBH.data(), szJLBH.size(), "%04d%08d", id, counter);

		time_t tt = time(NULL);

		sprintf_s(szSql.data(), szSql.size(), 
			"insert into b_mutltest(jlbh, xrsj, threadidx) values (%s, to_date('%s', 'yyyy-mm-dd hh24:mi:ss'), %d)",
			szJLBH.data(), Time2Str(tt, "yyyy-mm-dd hh24:mi:ss").c_str(), id);

		try
		{
			ora.Execute(string(szSql.data()));	
		}
		catch (std::exception &ex)
		{
			cout << "插入数据错误, 线程idx:" << id << ", 详情:" << ex.what() << endl;
			break;
		}

		if (counter == 1000)
		{
			EnterCriticalSection(&cs);
			cout << "线程:" << id << " 任务执行完毕" << endl;
			LeaveCriticalSection(&cs);	
			break;
		}
	}

	EnterCriticalSection(&cs);
	BOOL ret = SetEvent(events[id]);
	assert(ret);
	LeaveCriticalSection(&cs);	

#if !USE_CSTYLE_THREAD
	return 0;
#endif
}



