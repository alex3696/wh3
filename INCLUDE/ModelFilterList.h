#ifndef __MODEL_FILTER_H
#define __MODEL_FILTER_H

#include "globaldata.h"
#include "dbFieldType.h"
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
	FieldType	mFieldType;
	std::vector<wxString> mValue;

public:
	ModelFilter()
		:mKind(FilterKind::EqMultiValue), mFieldType(ftText)
	{
	}
	ModelFilter(const wxString& title, const wxString& systitle
		, FilterKind kind, FieldType field_type)
		:mTitle(title), mSysTitle(systitle), mKind(kind), mFieldType(field_type)
	{
	}

	const wxString& GetSysTitle()const	{ return mSysTitle; }
	FieldType		GetFieldType()const	{ return mFieldType; }
	const wxString& GetTitle()const		{ return mTitle; }
	FilterKind		GetKind()const		{ return mKind; }

	void SetValue(const std::vector<wxString>& val)	{ mValue = val; }
	const std::vector<wxString>& GetValueVec()const	{ return mValue; }

	size_t			GetValueQty()const	{ return mValue.size(); }
	const wxString& GetValue(size_t pos)const
	{ 
		return mValue.size()>pos ? mValue[pos] : wxEmptyString2;
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
		inline result_type operator()(const std::shared_ptr<const ModelFilter>& r)const
		{
			return r.get();
		}
	};

	using FilterList =
	boost::multi_index_container
	<
		std::shared_ptr<const ModelFilter>,
		indexed_by
		<
			random_access<> //order
			,ordered_unique< extr_ptr >
		>
	>;


	FilterList mList;
public:
	ModelFilterList();

	virtual size_t size()const { return mList.size(); }
	virtual const std::shared_ptr<const ModelFilter>& at(size_t pos)const
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

	virtual void Update(const std::vector<NotyfyItem>& data);
	
	template<typename ...Types>
	void Insert(Types... args)
	{
		auto new_item = std::make_shared<const ModelFilter>(std::forward<Types>(args)...);
		std::shared_ptr<const ModelFilter> old_item(nullptr);
		std::vector<NotyfyItem> ins;
		ins.emplace_back(NotyfyItem(old_item, new_item));
		Update(ins);
	}
	
	sig::signal<void(const std::vector<NotyfyItem>&)>	sigUpdate;




	
};









//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H