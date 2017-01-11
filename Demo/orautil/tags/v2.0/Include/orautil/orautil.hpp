#ifndef ASPPRO_ORACLE_ORAUTIL_H_
#define ASPPRO_ORACLE_ORAUTIL_H_

#pragma once

#include <string>
#include <vector>
#include "detail/ocilib/ocilib.hpp"

namespace asppro {
namespace oracle {
	
// ****************************************************************************** 
// 封装库环境初始化与注销
// 注: 1. 程序中, 在使用asppro::oracle命名内的所有类型(如: ASPPool, ASPConnection, 
//     ASPDataset, ASPDate)之前, 务必确保库环境初始化, 否则使用这用上述类型, 均会
//     返回空值
//     2. 封装库环境初始化与注销应用程序中只做一次即可, 多次初始化不会产生副作用.
// ****************************************************************************** 
// 采用RAII手法实现"自动初始化" 示例: 
/*class ASPAutoInit
{
	public:
		ASPAutoInit() { asppro::oracle::Environment::ASPInitialize(); }
		~ASPAutoInit() { asppro::oracle::Environment::ASPCleanup(); }
};

static ASPAutoInit aspAutoInit;	
*/
void  ASPInitialize();		// 库环境初始化(分配相关全局共享资源)
void  ASPCleanup();			// 库环境注销(使用分配的全局共享资源)


// ****************************************************************************** 
// 封装库日期类
// ******************************************************************************
class ASPDate
{
	typedef enum { NullDate } NullDateType;		// 空日期类型

public:
	// ASPDate()默认构造为"空日期", 可以使用IsNull()查询状态, 
	// "空日期"下, 调用除IsNull()之外的其他成员函数均会抛出异常
	ASPDate(NullDateType dt = NullDate);	

	// 使用time_t构造
	explicit ASPDate(const time_t& tt);

	// 字符格式日期构造
	// 注: ASPDate中字符串日期格式化符遵循"Oracle数据库Data格式规则", 
	//    并且"大小写敏感"
	//    年:		yyyy
	//    月:		mm
	//    天:		dd
	//    24小时:   hh24
	//	  分钟:		mi
	//    秒:		ms
	//    示例: yyyy-mm-dd hh24:mi:ss  或 yyyy/mm/dd hh24:mi:ss
	explicit ASPDate(const std::string& str, const std::string& format = "yyyy-mm-dd hh24:mi:ss");
	ASPDate(const ASPDate& dt);
	~ASPDate(void);

	// 查询是否为"空日期"
	//   当查询的数据库Date字段为空时, 返回的ASPData为"空日期"
	bool	IsNull()	const;

	// 使用日期字符串初始化日期
	void FromString(const std::string& str, const std::string& format = "yyyy-mm-dd hh24:mi:ss");

	// 使用time_t初始化日期
	void FromTimet(const time_t& tt);

	std::string ToString( const std::string& format = "yyyy-mm-dd hh24:mi:ss") const;
	time_t GetTimet() const;

	ASPDate& operator=(const ASPDate& dt);

	ASPDate&	operator++();						// by second				
	ASPDate		operator++(int);					// by second
	ASPDate&	operator--();						// by second
	ASPDate		operator--(int);						// by second
	ASPDate		operator+(int seconds);				// by seconds
	ASPDate		operator-(int seonds);				// by seconds
	ASPDate&	operator+= (int seconds);			// by seconds
	ASPDate&	operator-= (int seconds);			// by seconds

	bool operator == (const ASPDate& dt)	const;
	bool operator != (const ASPDate& dt)	const;
	bool operator >  (const ASPDate& dt)	const;
	bool operator <  (const ASPDate& dt)	const;
	bool operator >= (const ASPDate& dt)	const;
	bool operator <= (const ASPDate& dt)	const;

	// static function
	// ++++++++++++++++++++++
	static ASPDate SysDate();
	static time_t Str2Time(const std::string& timeStr, const std::string& format = "yyyy-mm-dd hh24:mi:ss");	
	static std::string Time2Str(const time_t& time, const std::string& format = "yyyy-mm-dd hh24:mi:ss");
	// ----------------------
private:
	static const std::string& ASPDate::DefFmt();

	time_t m_tt;
};


// ****************************************************************************** 
// 封装库Dataset类
// 返回数据库批量查询结果
// ******************************************************************************
class ASPConnection;
class  ASPDataset
{
	friend class ASPConnection;

public:
	ASPDataset();
	ASPDataset(const ASPDataset& dt);
	ASPDataset& operator=(const ASPDataset& dt);
	~ASPDataset(void);

	// 指向查询集合的下一行
	// 
	// 数据查询获取到ASPDataset后, 第一(即: index = 0)行为标题行,
	// 故要取数据之前, 必须要先调用ASPDataset.Next(), 再取数据.
	// 取完一行数据, 再依次调用Next()指向下一行, 依次进行
	// 当指向数据行尾, Next()返回false
	bool Next();

	// 根据index获取当前行字段, index 从 1 开始
	//
	// 注: 数据库行第一(即: index = 0)列, 为Oracle 的RowID
	// 故数据字段查询index 从 1 开始
	template <typename TDataType>
	TDataType Get(unsigned int index);

	// 根据列名获取当前行字段
	template <typename TDataType>
	TDataType Get(const std::string& fieldName);

private:
	ASPDataset(const ASPConnection& con, const std::string& sql);
	void Release();

	ocilib::Connection* m_pCon;
	ocilib::Statement* m_pSt;
	ocilib::Resultset* m_pRs;
};


// ****************************************************************************** 
// 封装库Connection类
// ******************************************************************************
class  ASPConnection
{
	friend class ASPPool;
	friend class ASPDataset;

public:
	ASPConnection(void);

	// 打开数据库
	// dbServiceName: 数据库服务名
	// user:		  用户名
	// pwd:			  密码
	ASPConnection(const std::string& dbServiceName, const std::string& user, 
		const std::string& pwd);
	ASPConnection(const ASPConnection& con);
	ASPConnection& operator=(const ASPConnection& con);

	// 如果当前数据库为打开状态, 在析构函数中会自动关闭
	~ASPConnection(void);

	// 打开数据库
	// dbServiceName: 数据库服务名
	// user:		  用户名
	// pwd:			  密码
	void Open(const std::string& dbServiceName, const std::string& user, 
	const std::string& pwd);

	// 关闭数据库
	void Close();

	// 提交数据库操作
	void Commit();

	// 回滚数据库操作
	void Rollback();

	// 中断当前查询
	void Break();

	// 设置数据库自动提交模式 (ASPConnection 默认 GetAutoCommit() == false)
	void SetAutoCommit(bool enabled);

	// 查询当前数据库自动提交模式
	bool GetAutoCommit() const;

	// 查询数据库是否处于连接状态
	bool IsConnected() const;

	// 数据库单个值查询
	template<typename TDataType>
	TDataType SelectOne(const std::string& sql) const;

	// 数据库批量数据查询
	ASPDataset Select(const std::string& sql);

	// 数据库更改操作
	void Execute(const std::string& sql);

	// 数据库批量更改操作
	void Execute(const std::vector<std::string>& sqlList);

private:
	ocilib::Connection* m_pCon;

	ASPConnection(const ocilib::Connection& con);
	void Release();
};


// ****************************************************************************** 
// 封装库连接池Pool类
// ******************************************************************************
class  ASPPool
{
public:
	ASPPool(void);

	// 打开连接池
	// dbServiceName:		数据库服务名
	// user:				用户名
	// pwd:					密码
	// minSize:				连接池最小连接数  
	// maxSize:				连接池最大连接数  assert(maxSize > minSize)
	// increment:			连接池增长数
	ASPPool(const std::string& dbServiceName, const std::string& user,
		const std::string& pwd, unsigned int minSize, unsigned int maxSize,
		unsigned int increment = 1);

	// 析构函数中自动关闭打开的连接池
	~ASPPool(void);

	// 打开连接池
	// dbServiceName:		数据库服务名
	// user:				用户名
	// pwd:					密码
	// minSize:				连接池最小连接数  
	// maxSize:				连接池最大连接数  assert(maxSize > minSize)
	// increment:			连接池增长数
	void Open(const std::string& dbServiceName, const std::string& user,
		const std::string& pwd, unsigned int minSize, unsigned int maxSize,
		unsigned int increment = 1);

	// 关闭连接池
	void Close();

	// 从连接池获取一个可以连接
	ASPConnection GetConnection();

private:
	ASPPool(const ASPPool&);
	ASPPool& operator=(const ASPPool&);
	void Release();

	ocilib::Pool* m_pPool;
};

} // oracle 
} // asppro

#include "detail/orautil_iml.hpp"

#endif // ASPPRO_ORACLE_ORAUTIL_H_
