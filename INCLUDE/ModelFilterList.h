#ifndef __MODEL_FILTER_H
#define __MODEL_FILTER_H

#include "globaldata.h"
#include "IModelWindow.h"

namespace wh{

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
enum class FilterKind
{
	EqOneValue  = 0 // 
	,EqMultiValue = 10 // 
	,EqIntervalValue = 20 // 
	
};
//-----------------------------------------------------------------------------
class ModelFilter
{
	wxString	mTitle;
	wxString	mSysTitle;
	FilterKind	mKind;
	std::vector<wxString> mValue;

public:
	ModelFilter()
	{
	}
	ModelFilter(const wxString& title, const wxString& systitle, FilterKind kind)
		:mTitle(title), mSysTitle(systitle), mKind(kind)
	{
	}

	const wxString& GetSysTitle()const	{ return mSysTitle; }
	const wxString& GetTitle()const		{ return mTitle; }
	FilterKind		GetKind()const		{ return mKind; }
	size_t			GetValueQty()const	{ return mValue.size(); }
	const wxString& GetValue(size_t pos = 0)const
	{ 
		return mValue.size() ? mValue[0] : wxEmptyString2;
	}
	wxString AsString()const
	{
		wxString ret;
		switch (mKind)
		{
		case FilterKind::EqOneValue:
			ret = wxString::Format(" %s=%s", GetSysTitle(), mValue[0]);
			break;
		case FilterKind::EqMultiValue:
		{
			for (const auto& item : mValue)
				ret += wxString::Format(" OR %s=%s", GetSysTitle(), item);
			ret.Replace("OR", "", false);
		}break;
		case FilterKind::EqIntervalValue:
		{
			ret = wxString::Format(" %s BETWEEN %s AND %s"
				, GetSysTitle(), mValue[0], mValue[1]);
		}break;
		default:break;
		}

		return ret;
	}//wxString AsString()const

};
//-----------------------------------------------------------------------------


class ModelFilterList : public IModelWindow
{
	struct extr_ptr
	{
		typedef const void* result_type;
		inline result_type operator()(const std::shared_ptr<ModelFilter>& r)const
		{
			return r.get();
		}
	};

	using FilterList =
	boost::multi_index_container
	<
		std::shared_ptr<ModelFilter>,
		indexed_by
		<
			random_access<> //order
			,ordered_unique< extr_ptr >
		>
	>;


	FilterList mList;
public:
	virtual size_t size()const { return mList.size(); }
	virtual std::shared_ptr<const ModelFilter> at(size_t pos)const
	{
		//const std::shared_ptr<const ModelFilter> tmp = mList.at(pos);
		return mList.at(pos);
	}
	
	virtual void Clear()
	{
		mList.clear();
	}

	typedef std::pair<const std::shared_ptr<const ModelFilter>&
		, const std::shared_ptr<const ModelFilter>& > NotyfyItem;
	// pair(old,new) - update
	// pair(null,new) - insert
	// pair(old,null) - delete

	virtual void Update(const std::vector<NotyfyItem>&)
	{

	}
	

	
	sig::signal<void(const std::vector<NotyfyItem>&)>	sigUpdate;




	
};









//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H