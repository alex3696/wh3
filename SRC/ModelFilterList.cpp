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
					ni.emplace_back(NotyfyItem(old_item, new_item));
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
