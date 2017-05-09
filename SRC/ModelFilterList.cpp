#include "_pch.h"
#include "ModelFilterList.h"

using namespace wh;

//---------------------------------------------------------------------------
ModelFilter::ModelFilter()
	:mKind(FilterOp::foEq), mFieldType(ftText), mVisible(true)
{
}
//---------------------------------------------------------------------------
ModelFilter::ModelFilter(const wxString& title, const wxString& systitle
	, FilterOp kind, FieldType field_type)
	: mTitle(title), mSysTitle(systitle), mKind(kind), mFieldType(field_type), mVisible(true)
{
}
//---------------------------------------------------------------------------
ModelFilter::ModelFilter(const wxString& title, const wxString& systitle
	, FilterOp kind, FieldType field_type, bool show = true)
	: mTitle(title), mSysTitle(systitle), mKind(kind), mFieldType(field_type), mVisible(show)
{
}
//---------------------------------------------------------------------------
wxString ModelFilter::AsString()const
{
	if (!mValue.size())
		return wxEmptyString;
	wxString ret;
	switch (mKind)
	{
	case foEq:		BuildEqString(ret); break;
	case foNotEq:	BuildNotEqString(ret); break;
	case foLess:	BuildLessString(ret); break;
	case foMore:	BuildMoreString(ret); break;
	case foLessEq:	BuildLessEqString(ret); break;
	case foMoreEq:	BuildMoreEqString(ret); break;
	case foLike:	BuildLikeString(ret); break;
	case foNotLike:	BuildNotLikeString(ret); break;
	case foBetween:	BuildBetweenString(ret); break;
	case foNotBetween:BuildNotBetweenString(ret); break;
	break;
	}//switch (mKind)
	return ret;
}//wxString AsString()const
//---------------------------------------------------------------------------

void ModelFilter::BuildEqString(wxString& ret)const
{
	for(const auto& val : mValue)
	{
		if (val.IsEmpty())
			ret += wxString::Format("OR %s IS NULL ", GetSysTitle());
		ret += wxString::Format("OR %s='%s'", GetSysTitle(), val);
	}
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildNotEqString(wxString& ret)const
{
	for(const auto& val : mValue)
	{
		if (val.IsEmpty())
			ret += wxString::Format("AND (%s IS NOT NULL OR %s<>'')"
			, GetSysTitle()
			, GetSysTitle());
		else
			ret += wxString::Format("AND %s<>'%s'", GetSysTitle(), val);
	}
	ret.Replace("AND", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildLessString(wxString& ret)const
{
	for(const auto& val : mValue)
		ret += wxString::Format("OR %s<'%s'", GetSysTitle(), val);
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildMoreString(wxString& ret)const
{
	for(const auto& val : mValue)
		ret += wxString::Format("OR %s>'%s'", GetSysTitle(), val);
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildLessEqString(wxString& ret)const
{
	for(const auto& val : mValue)
		ret += wxString::Format("OR %s<='%s'", GetSysTitle(), val);
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildMoreEqString(wxString& ret)const
{
	for(const auto& val : mValue)
		ret += wxString::Format("OR %s>='%s'", GetSysTitle(), val);
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildLikeString(wxString& ret)const
{
	for(const auto& val : mValue)
	{
		if (val.IsEmpty())
			ret += wxString::Format("OR %s IS NULL OR %s='' ", GetSysTitle(), GetSysTitle());
		else
			ret += wxString::Format("OR %s::TEXT ~~*'%%%s%%'", GetSysTitle(), val);
	}
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildNotLikeString(wxString& ret)const
{
	for(const auto& val : mValue)
	{ 
		if (val.IsEmpty())
			ret += wxString::Format("AND (%s IS NOT NULL OT %s<>'')"
			, GetSysTitle()
			, GetSysTitle());
		else
			ret += wxString::Format("AND %s::TEXT !~~*'%%%s%%'", GetSysTitle(), val);
	}
	ret.Replace("AND", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildBetweenString(wxString& ret)const
{
	for(const auto& val : mValue)
	{
		wxString begin, end;
		int pos = val.Find('\t');
		if (wxNOT_FOUND != pos)
		{
			begin = val.SubString(0, pos - 1);
			end = val.SubString(pos + 1, val.size());
			ret += wxString::Format("OR (%s>='%s' AND %s<='%s') "
				, GetSysTitle(), begin
				, GetSysTitle(), end );
		}//if (wxNOT_FOUND != pos)
	}//for
	ret.Replace("OR", " (", false);
	ret += ")";
}
//---------------------------------------------------------------------------
void ModelFilter::BuildNotBetweenString(wxString& ret)const
{
	for(const auto& val : mValue)
	{
		wxString begin, end;
		int pos = val.Find('\t');
		if (wxNOT_FOUND != pos)
		{
			begin = val.SubString(0, pos - 1);
			end = val.SubString(pos + 1, val.size());
			ret += wxString::Format("AND (%s<='%s' AND %s>='%s') "
				, GetSysTitle(), begin
				, GetSysTitle(), end );
		}//if (wxNOT_FOUND != pos)
	}//for
	ret.Replace("AND", " (", false);
	ret += ")";
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelFilterList::ModelFilterList()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelFilterList::Update(const std::vector<NotyfyItem>& data)
{
	std::vector<NotyfyItem> ni;

	for (const auto& item : data)
	{
		if (item.first && item.second)
		{
			auto& idxPtr = mList.get<1>();
			const auto& it = idxPtr.find(item.first.get());
			if (idxPtr.cend() != it)
			{
				auto new_item = std::make_shared<const ModelFilter>(*item.second);
				auto old_item = *it;
				
				if (idxPtr.replace(it, new_item))
				{
					NotyfyItem notify_item(old_item, new_item);
					ni.emplace_back(notify_item);
				}
					
			}

		}
		else if (item.first && !item.second)
		{
			auto& idxPtr = mList.get<1>();
			const auto& it = idxPtr.find(item.first.get());
			if (idxPtr.cend() != it)
			{
				std::shared_ptr<const ModelFilter> new_item;
				auto old_item = *it;
				ni.emplace_back(old_item, new_item);
				idxPtr.erase(it);
			}
		}
		else if (!item.first && item.second)
		{
			auto new_item = std::make_shared<const ModelFilter>(*item.second);
			std::shared_ptr<const ModelFilter> old_item;
			mList.emplace_back(new_item);
			ni.emplace_back(NotyfyItem(old_item, new_item));
		}
	}
	sigUpdate(ni);

}
//---------------------------------------------------------------------------
void ModelFilterList::UpdateFilter(const wxString& title, const wxString& sys_title
	, FilterOp op, FieldType type
	, const std::vector<wxString>& val)
{
	auto& sysTitleIdx = mList.get<2>();
	auto it = sysTitleIdx.find(sys_title);
	if (sysTitleIdx.end() != it)
	{
		auto new_filter = std::make_shared<ModelFilter>( *(*it) );
		new_filter->SetOperation(op);
		new_filter->SetFieldType(type);
		new_filter->SetValue(val);

		if (*new_filter != *(*it))
		{
			std::shared_ptr<const ModelFilter> const_new_filter = new_filter;
			std::vector<NotyfyItem> data;
			data.emplace_back(std::make_pair(*it, const_new_filter));
			Update(data);
		}
	}//if (sysTitleIdx.end() != it)
}
//---------------------------------------------------------------------------
void ModelFilterList::Apply()
{
	sigApply(GetSqlString());
}
//---------------------------------------------------------------------------
wxString ModelFilterList::GetSqlString()const
{
	wxString where;
	for (const auto& filter : mList)
	{
		if (filter->GetValueVecSize())
			where += " AND " + filter->AsString();
	}
	where.Replace("AND", "", false);
	
	return where;
}