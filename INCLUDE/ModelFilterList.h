#ifndef __MODEL_FILTER_H
#define __MODEL_FILTER_H

#include "dbFieldType.h"
#include "filter_data.h"
#include "globaldata.h"

#include "IModelWindow.h"

namespace wh{

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class ModelFilter
{
	wxString	mTitle;
	wxString	mSysTitle;
	FilterOp	mKind;
	FieldType	mFieldType;
	std::vector<wxString> mValue;

public:
	ModelFilter()
		:mKind(FilterOp::foEq), mFieldType(ftText)
	{
	}
	ModelFilter(const wxString& title, const wxString& systitle
		, FilterOp kind, FieldType field_type)
		:mTitle(title), mSysTitle(systitle), mKind(kind), mFieldType(field_type)
	{
	}

	const wxString& GetSysTitle()const	{ return mSysTitle; }
	FieldType		GetFieldType()const	{ return mFieldType; }
	void SetFieldType(const FieldType ft)	{ mFieldType=ft; }
	const wxString& GetTitle()const		{ return mTitle; }
	FilterOp		GetOperation()const		{ return mKind; }
	void SetOperation(const FilterOp fo)	{ mKind = fo; }

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
		/*
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
		*/
		return ret;
	}//wxString AsString()const

	bool operator==(const ModelFilter& la)const
	{
		return (la.mTitle == mTitle
			&& la.mSysTitle == mSysTitle
			&& la.mKind == mKind
			&& la.mFieldType == mFieldType
			&& la.mValue == mValue
			);
	}

	bool operator!=(const ModelFilter& la)const
	{
		return !operator==(la);
	}
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

	struct extr_sys_name
	{
		typedef const wxString& result_type;
		inline result_type operator()(const std::shared_ptr<const ModelFilter>& r)const
		{
			return r->GetSysTitle();
		}
	};

	using FilterList =
	boost::multi_index_container
	<
		std::shared_ptr<const ModelFilter>,
		indexed_by
		<
			random_access<> //order
			, ordered_unique< extr_ptr >
			, ordered_unique< extr_sys_name >
		>
	>;


	FilterList mList;
public:
	ModelFilterList();

	size_t size()const { return mList.size(); }
	const std::shared_ptr<const ModelFilter>& at(size_t pos)const
	{
		//const std::shared_ptr<const ModelFilter> tmp = mList.at(pos);
		return mList.at(pos);
	}
	
	void Clear()
	{
		mList.clear();
	}

	typedef std::pair<std::shared_ptr<const ModelFilter>
		, std::shared_ptr<const ModelFilter> > NotyfyItem;
	// pair(old,new) - update
	// pair(null,new) - insert
	// pair(old,null) - delete

	void Update(const std::vector<NotyfyItem>& data);

	void UpdateFilter(const wxString& title, const wxString& sys_title
		, FilterOp op, FieldType type
		, const std::vector<wxString>& val);
	void Apply();

	template<typename ...Types>
	void Insert(Types... args)
	{
		auto new_item = std::make_shared<const ModelFilter>(std::forward<Types>(args)...);
		std::shared_ptr<const ModelFilter> old_item(nullptr);
		std::vector<NotyfyItem> ins;
		ins.emplace_back(NotyfyItem(old_item, new_item));
		Update(ins);
	}

	wxString GetSqlString()const;
	
	sig::signal<void(const std::vector<NotyfyItem>&)>	sigUpdate;
	sig::signal<void()>	sigApply;




	
};









//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H