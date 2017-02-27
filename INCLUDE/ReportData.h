#ifndef __REPORTDATA_H
#define __REPORTDATA_H

#include "_pch.h"
namespace wh{
namespace rec{
//---------------------------------------------------------------------------
class ReportItem
{
public:
	wxString mId;
	wxString mTitle;
	wxString mNote;
	wxString mScript;
};

struct extr_rep_id
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<const rec::ReportItem>& r)const
	{
		return r->mId;
	}
};

using ReportList =
boost::multi_index_container
<
	std::shared_ptr<const rec::ReportItem>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< extr_rep_id >
	>
>;



class ReportTable
{
public:
	std::vector<wxString> mColNames;

	typedef std::vector<wxString> Row;

	std::vector<Row> mRowList;
};



class ReportFilter
{
	wxString mName;
	wxString mType;
	wxString mDefault;
};

typedef std::vector<ReportFilter> ReportFilterTable;


//---------------------------------------------------------------------------
} //namespace rec{
} //namespace wh{
#endif // __IMVP_H