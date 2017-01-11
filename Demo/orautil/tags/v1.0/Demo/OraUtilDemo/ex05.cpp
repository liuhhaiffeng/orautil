// asppro.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
#include <atlbase.h>
#include <atlstr.h>
#include <assert.h>
#include <fstream>
#include <array>
#include <stdio.h>
#include <vector>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ExecuteTransaction(), Rollback(), Commit()��ʹ��

#define local static

local bool CreateTable();
local bool InsertOneRow();
local int InsertRows();
local bool DropTable();
local void PrintRowData(unsigned int printrowcout = 10);

int main()
{
	try
	{
		//OracleUtil::OpenDB("���ݿ������", "�û���", "����");
		OracleUtil::OpenDB("mybk106", "test123", "test123");
		cout << "�����ݿ�ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�����ݿ�ʧ��:" << ex.what() << endl;
		return 0;
	}

	if (OracleUtil::IsConnected())
	{
		cout << "���ݿ�������" << endl;
	}
	else
	{
		cout << "���ݿ��ѶϿ�" << endl;
	}

	DropTable();	
	CreateTable();
	InsertOneRow();
	int insertcount = InsertRows();
	if (insertcount > 0)
	{
		PrintRowData(insertcount);
	}

	OracleUtil ora;

	ora.ExecuteTransaction("update b_kkqp_jgcl set jlbh = '000'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set hphm = '����'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set jgsj = sysdate");
	cout << endl;
	cout << "���� jlbh, hphm, jgsj��ֵ���ѯ���" << endl;
	PrintRowData(insertcount);

	ora.Rollback();
	cout << endl;
	cout << "�ع�֮��Ĳ�ѯ���" << endl;
	PrintRowData(insertcount);

	ora.ExecuteTransaction("update b_kkqp_jgcl set jlbh = '000'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set hphm = '����'");
	ora.ExecuteTransaction("update b_kkqp_jgcl set jgsj = sysdate");
	ora.Commit();
	cout << endl;
	cout << "�ٴθ��²��ύ���ѯ���" << endl;
	PrintRowData(insertcount);

	getchar();

	return 0;
}

bool CreateTable()
{
	ifstream readFile("create_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "�ļ� create_t_kk_jgcl ��ʧ��" << endl;
		return false;
	}

	string sql;

	while (!readFile.eof())
	{
		std::array<char , 256> line = {0};
		readFile.getline(line.data(), line.size());
		sql += line.data();
	}

	readFile.close();

	OracleUtil ora;

	bool created = false;
	try
	{
		ora.Execute(sql);
		created = true;
		cout << "�������ݿ���ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "�������ݿ��ʧ��:" << ex.what() << endl; 
	}

	return created;
}

bool InsertOneRow()
{
	string insert1 = "insert into b_kkqp_jgcl (JLBH, HPHM, HPZL, JGSJ, CLSD, ZGXS, ZDXS, CWKC, HPYS, CLLX, JLLX, HDSJ, BZWZDM, SBBH, CDBH, FXBH, TPSL, CLBJ, SHBJ, ZFBJ, SJLY, CCLX, BZ, TJBJ, CSYS, JLLB, XSFX, CSBJ, YZYS, TPLX, XRSJ, URL) values ('000000000000000000000000000', '��CT8283', 0, to_date('10-08-2016 00:03:53', 'dd-mm-yyyy hh24:mi:ss'), 22, 80, 80, 3.00, '0', '2', '0', 610300, '610300001045', '610300001045', 2, 'EW', 1, null, '0', '0', '127.0.0.1', 1, null, null, 0, 0, 1, 1, 1, 999, to_date('10-08-2016 00:44:40', 'dd-mm-yyyy hh24:mi:ss'), '/201605/25/08/12345678901-20160525083919-13-1.jpg;')";

	OracleUtil ora;

	bool inserted = false;
	try
	{
		ora.Execute(insert1);
		inserted = true;
		cout << "����һ�����ݳɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "����һ������ʧ��:" << ex.what() << endl;
	}

	return inserted;
}

int InsertRows()
{
	ifstream readFile("insert_b_kkqp_jgcl.sql");
	if (!readFile.is_open())
	{
		cout << "�ļ� insert_b_kkqp_jgcl ��ʧ��" << endl;
		return -1;
	}

	string text;
	while (!readFile.eof())
	{
		std::array<char , 1024> line = {0};
		readFile.getline(line.data(), line.size());
		text += line.data();
	}

	vector<string> sqlList;

	string::size_type pos = string::npos;
	string::size_type pos2 = string::npos;

	do 
	{
		pos = text.find("insert into");
		if (pos == string::npos)
		{
			break;
		}

		pos2 = text.find("insert into", pos+1);
		if (pos2 != string::npos)
		{
			sqlList.push_back(text.substr(pos, pos2-pos));
			text = text.substr(pos2);
		}
		else
		{
			sqlList.push_back(text);
			break;
		}

	} while (true);

	readFile.close();


	const int count = sqlList.size();
	if (count == 0)
	{
		cout << "��ȡ��0���Ϸ���sql���, insert_b_kkqp_jgcl �ļ�Ϊ�ջ����ݷǷ�" << endl;
		return -1;
	}
	else
	{
		cout << "�� insert_b_kkqp_jgcl �ж�ȡ�� " << count << " ��sql �����¼" << endl;
	}

	OracleUtil ora;

	string sql;
	pos = string::npos;
	int insertedCount = 0;
	for (int i = 0; i < count; ++i)
	{
		sql = sqlList[i];
		pos = sql.find_last_of(";");
		if (pos != string::npos)
		{
			sql = sql.substr(0, pos);
		}

		try
		{
			ora.Execute(sql);
			cout << "����һ����¼�ɹ�" << endl;
			++insertedCount;
		}
		catch (std::exception &ex)
		{
			cout << "����һ����¼ʧ��:" << ex.what() << endl; 
			break;
		}
	}

	cout << "�ɹ����� " << insertedCount << " ����¼" << endl;

	return insertedCount;
}

void PrintRowData(unsigned int printrowcout /* = 1 */)
{
	OracleUtil ora;

	std::array<char, 256> szSql = {0};
	sprintf_s(szSql.data(), szSql.size(), "select jlbh, hphm ,jgsj from b_kkqp_jgcl where rownum <= %d", printrowcout);

	try
	{
		ora.Select(string(szSql.data()));
	}
	catch (std::exception &ex)
	{
		cout << "��ѯ���ݳ���:" << ex.what() << endl;
		return;
	}

	cout << "���\t" << "��¼���\t" << "���ƺ���\t" << "����ʱ��" << endl;

	int idx = 0;
	while (ora.Next())
	{
		try
		{
			string jlbl = ora.Get<DTStr>(1);
			string hphm = ora.Get<DTStr>(2);
			ocilib::Date jgsj = ora.Get<DTDate>(3);

			cout << ++idx << ",\t" << jlbl << ",\t" << hphm << ",\t" << jgsj.ToString("yyyy-mm-dd hh24:mi:ss") << endl;
		}
		catch (std::exception &ex)
		{
			cout << "��ȡ�ֶδ���:" << ex.what() << endl;
			break;
		}
	}

}

bool DropTable()
{
	string sql = "drop table b_kkqp_jgcl";

	OracleUtil ora;

	bool droped = false;
	try
	{
		ora.Execute(sql);
		droped = true;
		cout << "ɾ�����ݿ�ɹ�" << endl;
	}
	catch (std::exception &ex)
	{
		cout << "ɾ����ʧ��:" << ex.what() << endl;
	}

	return droped;
}


