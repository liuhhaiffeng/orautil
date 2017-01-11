// asppro::oracle.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include <orautil/orautil.hpp>
#include <iostream>
using namespace std;
using namespace asppro::oracle;

#pragma comment(lib, "ociliba.lib")

// ��, �ر����ݿ� ��ʾ

int main()
{
	try
	{
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

	// ���Բ�����ʽ����, OracleUtil�����������Զ�����
	/*OracleUtil::CloseDB();
	cout << "�ر����ݿ�" << endl;*/

	return 0;
}
