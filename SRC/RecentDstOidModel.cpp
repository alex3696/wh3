#include "_pch.h"
#include "RecentDstOidModel.h"

using namespace wh;

//-----------------------------------------------------------------------------
RecentDstOidModel::RecentDstOidModel(std::size_t max_items)
	:mMaxItems(max_items)
{

}
//-----------------------------------------------------------------------------
void RecentDstOidModel::Clear(std::size_t max_items)
{
	mMruList.clear();
	mMaxItems = max_items;
	DoSigUpdate();
}
//-----------------------------------------------------------------------------
bool RecentDstOidModel::Check(const wxString& item)const
{
	const auto& uniqueIdx = mMruList.get<1>();
	return (uniqueIdx.end() != uniqueIdx.find(item));
}
//-----------------------------------------------------------------------------
void RecentDstOidModel::InsertWithoutSig(const wxString& item)
{
	std::pair<rec::MruList::iterator, bool>
		p = mMruList.push_front(item);

	if (!p.second){                     /* duplicate item */
		mMruList.relocate(mMruList.begin(), p.first); /* put in front */
	}
	else if (mMruList.size()>mMaxItems){  /* keep the length <= max_num_items */
		mMruList.pop_back();
	}
}
//-----------------------------------------------------------------------------
void RecentDstOidModel::Insert(const wxString& item)
{
	InsertWithoutSig(item);
	DoSigUpdate();
}
//-----------------------------------------------------------------------------
void RecentDstOidModel::Load(const boost::property_tree::wptree& app_cfg)
{
	TEST_FUNC_TIME;
	using ptree = boost::property_tree::wptree;
	Clear();
	ptree::const_assoc_iterator it = app_cfg.find(L"RecentDstOid");

	if (it != app_cfg.not_found())
	{
		mFilterEnable = it->second.get<int>(L"FilterEnable", 1);
		mRecentEnable = it->second.get<int>(L"RecentEnable", 1);
		mMaxItems = it->second.get<int>(L"MaxQty", 10);
		for (const ptree::value_type &v : it->second.get_child(L"Oid"))
		{
			wxString str = v.second.get_value<std::wstring>();
			InsertWithoutSig(str);
		}
	}

	DoSigUpdate();
}
//-----------------------------------------------------------------------------
void RecentDstOidModel::Save(boost::property_tree::wptree& app_cfg)const
{
	TEST_FUNC_TIME;
	using ptree = boost::property_tree::wptree;

	ptree recent;
	recent.put(L"MaxQty", mMaxItems);
	recent.put(L"FilterEnable", (int)mFilterEnable);
	recent.put(L"RecentEnable", (int)mRecentEnable);

	ptree recent_dst_oid;
	ptree recent_oid;

	for (const auto& item : mMruList)
	{
		std::wstring str = item.wc_str();
		recent_dst_oid.push_back(ptree::value_type(L"", ptree(str)));
	}
	
	//recent_oid.put_value("11"); recent_dst_oid.push_back(std::make_pair("", recent_oid));
	//recent_dst_oid.push_back(ptree::value_type("", ptree("22")));
	//recent_dst_oid.push_back(ptree::value_type("", ptree("33")));
	
	
	recent.add_child(L"Oid", recent_dst_oid);
	app_cfg.add_child(L"RecentDstOid", recent);

}
