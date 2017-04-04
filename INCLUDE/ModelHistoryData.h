#ifndef __MODEL_DATAHISTORY_H
#define __MODEL_DATAHISTORY_H

#include "globaldata.h"
#include "db_rec.h"

namespace wh{
//-----------------------------------------------------------------------------
class ActRec
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mColour;
};

using ActTable =
boost::multi_index_container
<
	std::shared_ptr<ActRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ActRec, wxString, mId)>
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(ActRec, wxString, mTitle)>
	>
>;
//-----------------------------------------------------------------------------

class PropRec
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mKind;
	//wxString	mVarArray;
	//wxString	mVarStrict;

};
using PropTable =
boost::multi_index_container
<
	std::shared_ptr<PropRec>,
	indexed_by
	<
		random_access<> //SQL order
		, ordered_unique< BOOST_MULTI_INDEX_MEMBER(PropRec, wxString, mId)>
		
	>
>;
//-----------------------------------------------------------------------------

struct PropValRec
{
	std::shared_ptr<PropRec>	mProp;
	wxString					mVal;
};

struct extr_pid_PropValRec
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<PropValRec>& r)const
	{
		return r->mProp->mId;
	}
};

struct extr_ptitle_PropValRec
{
	typedef const wxString& result_type;
	inline result_type operator()(const std::shared_ptr<PropValRec>& r)const
	{
		return r->mProp->mTitle;
	}
};


using PropValTable =
boost::multi_index_container
<
	std::shared_ptr<PropValRec>,
	indexed_by
	<
		  ordered_unique< extr_pid_PropValRec >
		, random_access<> //SQL order
		//, ordered_unique< extr_ptitle_PropValRec >
		
	>
>;


//-----------------------------------------------------------------------------

class ModelHistoryTableData
{
public:
	virtual ~ModelHistoryTableData(){}
	virtual size_t size()const = 0;
	virtual const wxString& GetLogId(const size_t row)const = 0;
	virtual const wxString& GetUser(const size_t row)const = 0;
	virtual const wxString& GetDate(const size_t row)const = 0;

	virtual const wxString& GetCId(const size_t row)const = 0;
	virtual const wxString& GetCTiltle(const size_t row)const = 0;
	virtual const wxString& GetCKind(const size_t row)const = 0;
	virtual const wxString& GetCMeasure(const size_t row)const = 0;

	virtual const wxString& GetOId(const size_t row)const = 0;
	virtual const wxString& GetOTiltle(const size_t row)const = 0;
	virtual const wxString& GetQty(const size_t row)const = 0;

	virtual const ActRec& GetActRec(const size_t row)const = 0;
	virtual const PropValTable& GetProperties(const size_t row)const = 0;
	virtual const PropValTable& GetActProperties(const size_t row)const = 0;

	virtual const wxString& GetSrcPath(const size_t row)const { return wxEmptyString2; };
	virtual const wxString& GetDstPath(const size_t row)const { return wxEmptyString2; };
};
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __IMVP_H