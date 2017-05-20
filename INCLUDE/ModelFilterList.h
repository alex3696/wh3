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
	bool		mVisible;
	wxString	mTitle;
	wxString	mSysTitle;
	FilterOp	mKind;
	FieldType	mFieldType;
	std::vector<wxString> mValue;

	void BuildEqString(wxString& ret)const;
	void BuildNotEqString(wxString& ret)const;
	void BuildLessString(wxString& ret)const;
	void BuildMoreString(wxString& ret)const;
	void BuildLessEqString(wxString& ret)const;
	void BuildMoreEqString(wxString& ret)const;
	void BuildLikeString(wxString& ret)const;
	void BuildNotLikeString(wxString& ret)const;
	void BuildBetweenString(wxString& ret)const;
	void BuildNotBetweenString(wxString& ret)const;

public:
	ModelFilter();
	ModelFilter(const wxString& title, const wxString& systitle
		, FilterOp kind, FieldType field_type);
	ModelFilter(const wxString& title, const wxString& systitle
		, FilterOp kind, FieldType field_type, bool visible);


	const wxString& GetSysTitle()const	{ return mSysTitle; }
	FieldType		GetFieldType()const	{ return mFieldType; }
	void SetFieldType(const FieldType ft)	{ mFieldType=ft; }
	const wxString& GetTitle()const		{ return mTitle; }
	FilterOp		GetOperation()const		{ return mKind; }
	void SetOperation(const FilterOp fo)	{ mKind = fo; }

	void SetValue(const std::vector<wxString>& val)	{ mValue = val; }
	const std::vector<wxString>& GetValueVec()const	{ return mValue; }
	const size_t GetValueVecSize()const	{ return mValue.size(); }
	bool IsVisible()const		{ return mVisible; }
	void SetVisible(bool show)	{ mVisible = show; }

	wxString AsString()const;

	inline bool operator==(const ModelFilter& la)const
	{
		return (la.mTitle == mTitle
			&& la.mSysTitle == mSysTitle
			&& la.mKind == mKind
			&& la.mFieldType == mFieldType
			&& la.mValue == mValue
			&& la.mVisible == mVisible
			);
	}
	inline bool operator!=(const ModelFilter& la)const
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

	const std::shared_ptr<const ModelFilter> FindBySysName(const wxString& sys_name)const
	{
		const auto& idxSysName = mList.get<2>();
		const auto it = idxSysName.find(sys_name);
		if (idxSysName.cend() != it)
			return *it;
		return nullptr;
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

	void UpdateFilter(const wxString& sys_title,const wxString& val);


	void Apply();

	template<typename ...Types>
	std::shared_ptr<const ModelFilter> Insert(Types... args)
	{
		auto new_item = std::make_shared<const ModelFilter>(std::forward<Types>(args)...);
		std::shared_ptr<const ModelFilter> old_item(nullptr);
		std::vector<NotyfyItem> ins;
		ins.emplace_back(NotyfyItem(old_item, new_item));
		Update(ins);
		return new_item;
	}

	wxString GetSqlString()const;
	
	sig::signal<void(const std::vector<NotyfyItem>&)>	sigUpdate;
	sig::signal<void(const wxString&)>	sigApply;




	
};









//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H