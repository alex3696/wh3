#ifndef __MLOG_H
#define __MLOG_H

#include "TModel.h"
#include "MLogArray.h"
#include "db_filter.h"

namespace wh{
//-------------------------------------------------------------------------
class LogCfg
{
public:
	unsigned int	mRowsPerPage = 100;
	bool			mEnableFavProp = true; // else show all object properties
	bool			mCompactMode = true;
	bool			mShowDebugColumns = __DEBUG; /* #ifdef __DEBUG true #elif false #endif;	*/
	
};//class LogCfg

class DbFieldInfo
	:public boost::noncopyable
{
public:
	class Filter
		:public boost::noncopyable
	{
	public:
		Filter(const DbFieldInfo& f):mField(f){}
		const DbFieldInfo& mField;
		wxString	mVal;
		FilterOp	mOp;
		FilterConn	mConn;
		bool		mEnable;
		
		wxString GetSql()const
		{
			wxString str;
			if (mEnable)
				str << ToSqlString(mConn) << " " << mField.mTitle <<
					ToSqlString(mOp) << "'" << mVal << "' ";
			return str;
		};
	};

	enum Action
	{
		dbaSELECT = 0x01,
		dbaINSERT = 0x02,
		dbaUPDATE = 0x04,
		dbaDELETE = 0x08
	};
	std::unique_ptr<DbFieldInfo::Filter>	mFilter;
	
	wxString	mTitle;
	wxString	mDbTitle;
	FieldType	mType	 = ftText;
	bool		mGuiShow = true;
	bool		mGuiEdit = false;
	char		mDbAction = dbaSELECT | dbaUPDATE;
	int			mSort = 0;

	DbFieldInfo();
	~DbFieldInfo();

	bool HasFilter()const;
	const Filter& GetFilter()const;
	const Filter& SetFilter(const wxString& val, FilterOp fo = foEq
		, FilterConn fc = fcAND, bool enable = true);

	void EnabeFilter(bool enable = true);
	bool IsEnabedFilter()const;


private:



};



//-------------------------------------------------------------------------
class MLog
{
	typedef std::vector<wxString>		Record;
	typedef std::vector<Record>			Table;
	typedef std::vector<std::shared_ptr<DbFieldInfo>>	FieldVec;

	

	// 
	//std::shared_ptr<TModelData<LogCfg>>	mCfg;
	//std::shared_ptr<MLogArray>			mLogArray;
	//std::shared_ptr<MFilterArray>	mFilterArray;
	//std::shared_ptr<MFavPropArray>	mFavPropArray;
	//SigArray mSig;

	Table		mTable;
	FieldVec	mField;
public:
	MLog()
	{
		DbFieldInfo* fTitle = new DbFieldInfo();
		fTitle->mTitle = "имя";
		fTitle->mDbTitle = "cls.title";
		mField.emplace_back(fTitle);

		DbFieldInfo* fId = new DbFieldInfo();
		fId->mTitle = "#";
		fId->mDbTitle = "cls.id";
		mField.emplace_back(fId);
		
	};

	void Load()
	{
		std::multimap<int, const wxString&> sort_order;

		wxString fields;
		wxString filters;
		wxString sorters;
		for (unsigned int i = 0; i<mField.size();++i)
		{
			const auto& fd = *mField[i];
			if (fd.mDbAction & DbFieldInfo::dbaSELECT)
			{
				fields << fd.mDbTitle << ",";
				if (fd.mSort)
					sort_order.emplace(std::make_pair(fd.mSort, fd.mDbTitle));
			}
				
			if (fd.HasFilter())
				filters << fd.GetFilter().GetSql();
		}
			
		fields.RemoveLast();
		
		if (!filters.IsEmpty())
		{
			filters.Remove(0, filters.Find(' '));
			filters = "WHERE " + filters;
		}
		

		for (const auto& sort : sort_order)
			sorters << sort.second
				<< (sort.first > 0 ? " DESC " : " ASC ") << ",";

		if (!sorters.IsEmpty())
		{
			sorters.RemoveLast();
			sorters = "ORDER BY " + sorters;
		}
			
		const wxString query = "SELECT " + fields + " FROM cls " + filters + sorters;

		auto p1 = GetTickCount();
		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		if (table)
		{
			const auto col_qty = table->GetColumnCount();
			const auto row_qty = table->GetRowCount();
			
			
			for (unsigned int row = 0; row < row_qty; ++row)
			{
				Record rec(col_qty);
				for (size_t col = 0; col < col_qty; col++)
					table->GetAsString(col, row, rec[col]);
				mTable.emplace_back (rec);
			}
		}
		auto p2 = GetTickCount();
	};

	sig::connection Connect(SigArray::Op op, const SigArray::Slot slot);
	void            Disconnect(SigArray::Op op, const SigArray::Slot slot);
















}; //class MLog


//-------------------------------------------------------------------------	
} //namespace wh

#endif // __****_H