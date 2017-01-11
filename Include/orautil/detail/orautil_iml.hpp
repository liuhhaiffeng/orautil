namespace asppro {
namespace oracle {

#include <time.h>		// localtime_s

inline bool IsConnected(const ocilib::Connection& con)
{
	return con.IsServerAlive();
}

template<typename TDataType>
inline TDataType SelectOne(const ocilib::Connection& con, 
	const ocilib::ostring& sql)
{
	ocilib::Statement st(con);
	st.Execute(sql);
	ocilib::Resultset rs = st.GetResultset();
	rs.Next();
	return rs.Get<TDataType>(1);
}

template<>
inline ocilib::Date SelectOne<ocilib::Date>(const ocilib::Connection& con, 
	const ocilib::ostring& sql)
{
	ocilib::Statement st(con);
	st.Execute(sql);
	ocilib::Resultset rs = st.GetResultset();
	rs.Next();
	return rs.Get<ocilib::Date>(1).Clone();
}

inline void AddHours(ocilib::Date& dt, int hours)
{
	AddSeconds(dt, hours * 3600);
}

inline void AddMintues(ocilib::Date& dt, int mintues)
{
	AddSeconds(dt, mintues * 60);
}

inline void AddSeconds(ocilib::Date& dt, int seconds)
{
	time_t tt = Date2Time(dt);
	tt += seconds;

	struct tm mytm = {0};
	errno_t err = localtime_s(&mytm, &tt);
	if (err)
	{
		throw std::exception("Invalid Argument to gmtime_s()");
	}

	dt.SetDateTime(mytm.tm_year + 1900, mytm.tm_mon + 1, mytm.tm_mday, 
		mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
}

inline ocilib::Date Time2Date(const time_t& tt)
{
	return ocilib::Date(Time2Str(tt, OTEXT("yyyy-mm-dd hh24:mi:ss")), 
		OTEXT("yyyy-mm-dd hh24:mi:ss"));
}

inline time_t Date2Time(const ocilib::Date& dt)
{
	return Str2Time(dt.ToString(OTEXT("yyyy-mm-dd hh24:mi:ss")), 
		OTEXT("yyyy-mm-dd hh24:mi:ss"));
}

inline ocilib::ostring Time2Str(const time_t& time, 
	const ocilib::ostring& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss") */)
{
	otext chTmp[100] = {0};
	struct tm mytm;
	errno_t err = localtime_s(&mytm, &time);
	if (err)
	{
		throw std::exception("Invalid Argument to gmtime_s()");
	}

	mytm.tm_year = mytm.tm_year + 1900;
	mytm.tm_mon = mytm.tm_mon + 1;

#ifdef OCI_CHARSET_ANSI
	_snprintf_s(chTmp, _countof(chTmp), OTEXT("%04d-%02d-%02d %02d:%02d:%02d"),
		mytm.tm_year, mytm.tm_mon, mytm.tm_mday, 
		mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
#else
	_snwprintf_s(chTmp, _countof(chTmp), OTEXT("%04d-%02d-%02d %02d:%02d:%02d"),
		mytm.tm_year, mytm.tm_mon, mytm.tm_mday, 
		mytm.tm_hour, mytm.tm_min, mytm.tm_sec);
#endif

	ocilib::Date date(ocilib::ostring(chTmp), OTEXT("yyyy-mm-dd hh24:mi:ss"));
	return date.ToString(format);
}

inline time_t Str2Time(const ocilib::ostring& timeStr, 
	const ocilib::ostring& format /*= OTEXT("yyyy-mm-dd hh24:mi:ss") */)
{
	ocilib::Date date(timeStr, format);

	struct tm mytm;
	time_t tt;
	mytm.tm_year = date.GetYear();
	mytm.tm_mon = date.GetMonth();
	mytm.tm_mday = date.GetDay();
	mytm.tm_hour = date.GetHours();
	mytm.tm_min = date.GetMinutes();
	mytm.tm_sec = date.GetSeconds();

	mytm.tm_year -= 1900;
	mytm.tm_mon--;
	mytm.tm_isdst = -1;

	tt = mktime(&mytm);

	return tt;
}

} // oracle
} // asppro
