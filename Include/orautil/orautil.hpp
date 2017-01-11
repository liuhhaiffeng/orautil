#ifndef ASPPRO_ORACLE_ORAUTIL_H_
#define ASPPRO_ORACLE_ORAUTIL_H_

#pragma once

#include "detail/ocilib/ocilib.hpp"
#include <memory>

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
class OciEnvInit
{
	public:
		OciEnvInit(ocilib::Environment::EnvironmentFlags mode 
			= ocilib::Environment::Default | ocilib::Environment::Threaded, 
			bool enableWarnings = true,
			const ocilib::ostring& libpath = "") 
		{
			// 库环境初始化(分配相关全局共享资源)
			ocilib::Environment::Initialize(mode, libpath);
			ocilib::Environment::EnableWarnings(enableWarnings);
		}

		~OciEnvInit() 
		{
			// 库环境注销(使用分配的全局共享资源)
			ocilib::Environment::Cleanup();
		}
};

// ****************************************************************************** 
// 查询数据库是否处于连接状态
// ****************************************************************************** 
bool IsConnected(const ocilib::Connection& con);

// ****************************************************************************** 
// 单值查询
// ****************************************************************************** 
template<typename TDataType>
TDataType SelectOne(const ocilib::Connection& con, const ocilib::ostring& sql);

// ****************************************************************************** 
// ocilib::Date 按秒加/减  (正数加, 负数减)
// ****************************************************************************** 
void AddHours(ocilib::Date& dt, int hours);
void AddMintues(ocilib::Date& dt, int mintues);
void AddSeconds(ocilib::Date& dt, int seconds);

// ****************************************************************************** 
// 日期工具函数
// ****************************************************************************** 
ocilib::Date Time2Date(const time_t& tt);
time_t Date2Time(const ocilib::Date& dt);

time_t Str2Time(const ocilib::ostring& timeStr, 
	const ocilib::ostring& format = OTEXT("yyyy-mm-dd hh24:mi:ss"));	
ocilib::ostring Time2Str(const time_t& time, 
	const ocilib::ostring& format = OTEXT("yyyy-mm-dd hh24:mi:ss"));

} // oracle 
} // asppro

#include "detail/orautil_iml.hpp"

#endif // ASPPRO_ORACLE_ORAUTIL_H_
