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

	for (const auto& val : mValue)
	{
		switch (mKind)
		{
		case foBetween:
		{
			wxString begin, end;
			int pos = val.Find('\t');
			if (wxNOT_FOUND != pos)
			{
				begin = val.SubString(0, pos - 1);
				end = val.SubString(pos + 1, val.size());
			}
			ret += wxString::Format("OR (%s>='%s' AND %s<='%s') "
				, GetSysTitle()
				, begin
				, GetSysTitle()
				, end
				);
		}
		break;

		case foNotBetween:
		{
			wxString begin, end;
			int pos = val.Find('\t');
			if (wxNOT_FOUND != pos)
			{
				begin = val.SubString(0, pos - 1);
				end = val.SubString(pos + 1, val.size());
			}
			ret += wxString::Format("OR (%s<='%s' AND %s>='%s') "
				, GetSysTitle()
				, begin
				, GetSysTitle()
				, end
				);
		}
		break;

		case foLike:
		case foNotLike:
			ret += wxString::Format("OR %s::TEXT %s'%%%s%%'"
				, GetSysTitle()
				, ToSqlString(mKind)
				, val);
			break;

		default: 
			ret += wxString::Format("OR %s%s'%s'"
				, GetSysTitle()
				, ToSqlString(mKind)
				, val);
			break;
		}
		

	}
	ret.Replace("OR", " (", false);
	ret += ")";

	return ret;
}//wxString AsString()const




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