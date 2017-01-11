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

// 多线程并发操作

#define local static

local CRITICAL_SECTION cs; 

local void MutilThreadInset(void* pThreadIdx);

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
		ora.Execute("create table b_mutltest (jlbh varchar2(32) not null, xrsj date default sysdate, threadidx number(6))");
		cout << "创建表成功" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "创建表失败:" << ex.what() << endl;
		return 0;
	}

	InitializeCriticalSection(&cs);

	// 创建100个线程
	const int threadNum = 100;
	int threadIdx[threadNum] = {0};
	for (int i  = 0; i < threadNum; ++i)
	{
		threadIdx[i] = i + 1;
		_beginthread(MutilThreadInset, 0, &(threadIdx[i]));
	}
	cout << "创建线程池成功" << endl;	

	getchar();		// 等待任务完成

	DeleteCriticalSection(&cs);

	std::unique_ptr<Dataset> dt = ora.Select2("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 FROM b_mutltest");

	ora.Select2("SELECT count(*), MAX(xrsj), MIN(xrsj), (MAX(xrsj) - MIN(xrsj)) * 24 * 3600 FROM b_mutltest");

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

void MutilThreadInset(void* pThradIdx)
{
	int id = * (static_cast<int*>(pThradIdx));

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

		if (counter == 10000)
		{
			cout << "线程:" << id << " 任务执行完毕" << endl;
			break;
		}
	}
}



