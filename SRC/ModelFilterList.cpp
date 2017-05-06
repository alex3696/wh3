#include "_pch.h"
#include "ModelFilterList.h"

using namespace wh;

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
	sigApply();
}
//---------------------------------------------------------------------------
wxString ModelFilterList::GetSqlString()const
{
	return wxEmptyString;
}