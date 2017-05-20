#include "_pch.h"
#include "ModelDetail.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelPageDetail::ModelPageDetail(const std::shared_ptr<rec::ObjInfo>& oi
	, const std::shared_ptr<rec::PageHistory>& data)
{
	data->mVisibleColumnClsObj = false;

	mObjInfo = *oi;

	mModelHistory = std::make_shared<ModelPageHistory>(data);

	std::vector<wxString> val(1);
	auto fobj = std::make_shared<ModelFilter>("Îáúåêò(Oid)", "mobj_id", FilterOp::foEq, ftLong, true);
	val[0] = oi->mObj.mId;// "3811";
	fobj->SetValue(val);

	auto filter_list = mModelHistory->GetModelHistory().GetFilterList();

	std::vector<ModelFilterList::NotyfyItem> nv(2);
	auto new_item2 = std::shared_ptr<const ModelFilter>(fobj);
	//auto new_item1 = std::shared_ptr<const ModelFilter>(ftype);
	std::shared_ptr<const ModelFilter> old_item;
	//nv.emplace_back(ModelFilterList::NotyfyItem(old_item, new_item1));
	nv.emplace_back(ModelFilterList::NotyfyItem(old_item, new_item2));
	filter_list->Update(nv);
	

}
//---------------------------------------------------------------------------
void ModelPageDetail::Update()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::UpdateTitle() //override;
{
	sigUpdateTitle(mTitle, mIco);
	Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Show()//override;
{
	//Update();
	sigShow();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Init()//override
{
	auto filter_list = mModelHistory->GetModelHistory().GetFilterList();
	filter_list->Apply();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Load(const boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	auto it = page_val.find("CtrlPageDetail");//CtrlPageHistory
	if (page_val.not_found() != it)
	{
		mModelHistory->Load(it->second);
	}
	mObjInfo.mObj.mId = page_val.get<std::string>("CtrlPageDetail.Oid");
	mObjInfo.mObj.mParent.mId = page_val.get<std::string>("CtrlPageDetail.parentOid");

	auto filter_list = mModelHistory->GetModelHistory().GetFilterList();
	const auto& filter_oid = filter_list->FindBySysName("mobj_id");
	filter_list->UpdateFilter("mobj_id", mObjInfo.mObj.mId.toStr());
	
	/////Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Save(boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	mModelHistory->Save(content);
	page_val.push_back(std::make_pair("CtrlPageDetail", content));

	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		const rec::ObjInfo& obj = mObjInfo;
		content.put("Oid", obj.mObj.mId.toStr());
		content.put("parentOid", obj.mObj.mParent.mId.toStr());
		page_val.push_back(std::make_pair("CtrlPageDetail", content));
	}

}
