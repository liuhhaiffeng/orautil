/*!
 * \date 2016/12/13
 * \author 刘海峰
 * Contact: liuhaifeng@asppro.com.cn
 *
 * \brief 
*/

#pragma once

#include "ocilib/ocilib.hpp"

namespace asppro {
namespace oracle {

	typedef ocilib::Date			DTDate;		// 对应数据库 Date Type

	class Dataset
	{
		friend class OracleUtil;
	public:
		~Dataset();

		bool Next();
		template <typename DbType>
		DbType Get(int index);
		template <>
		DTDate Get<DTDate>(int index);

	private:
		Dataset(ocilib::Connection conn, const ocilib::ostring& sql);		
		void Init();

		ocilib::Connection		m_Conn;
		ocilib::ostring			m_Sql;
		ocilib::Statement		m_St;
		ocilib::Resultset*		m_pRs;
	};	

} // namesapce oracle
} // namespace asppro

#include "dataset_iml.hpp"