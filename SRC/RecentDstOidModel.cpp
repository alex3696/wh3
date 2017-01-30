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
void RecentDstOidModel::GetFromConfig()
{
	using ptree = boost::property_tree::ptree;
	const ptree& app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();
	Clear();
	ptree::const_assoc_iterator it = app_cfg.find("RecentDstOid");

	if (it != app_cfg.not_found())
	{
		mFilterEnable = it->second.get<int>("FilterEnable", 1);
		mRecentEnable = it->second.get<int>("RecentEnable", 1);
		mMaxItems = it->second.get<int>("MaxQty", 10);
		for (const ptree::value_type &v : it->second.get_child("Oid"))
		{
			wxString str = v.second.get_value<std::string>();
			InsertWithoutSig(str);
		}
	}

	DoSigUpdate();
}
//-----------------------------------------------------------------------------
void RecentDstOidModel::SetToConfig()const
{
	using ptree = boost::property_tree::ptree;
	ptree app_cfg = whDataMgr::GetInstance()->mDbCfg->mGuiCfg->GetData();

	ptree recent;
	recent.put("MaxQty", mMaxItems);
	recent.put("FilterEnable", (int)mFilterEnable);
	recent.put("RecentEnable", (int)mRecentEnable);

	ptree recent_dst_oid;
	ptree recent_oid;

	for (const auto& item : mMruList)
	{
		std::string str = item.c_str();
		recent_dst_oid.push_back(ptree::value_type("", ptree(str)));
	}
	
	//recent_oid.put_value("11"); recent_dst_oid.push_back(std::make_pair("", recent_oid));
	//recent_dst_oid.push_back(ptree::value_type("", ptree("22")));
	//recent_dst_oid.push_back(ptree::value_type("", ptree("33")));
	
	
	recent.add_child("Oid", recent_dst_oid);
	app_cfg.add_child("RecentDstOid", recent);

	whDataMgr::GetInstance()->mDbCfg->mGuiCfg->SetData(app_cfg);
}
