#include "_pch.h"
#include "ModelDetail.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelObjDetail::ModelObjDetail()
{

}
//---------------------------------------------------------------------------
void ModelObjDetail::Load()
{
	wxString query = wxString::Format(
		"SELECT cls.id, cls.title, cls.kind, cls.measure "
		"     , o.id, o.title  "
		" FROM obj o "
		" RIGHT JOIN acls cls      ON cls.id = o.cls_id "
		" WHERE o.id = %s "
		, mObjInfo.mObj.mId.toStr());

	whDataMgr::GetDB().BeginTransaction();

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
	{
		mObjInfo.mCls.mId = table->GetAsString(0, 0);
		mObjInfo.mCls.mLabel = table->GetAsString(1, 0);
		mObjInfo.mCls.mType = table->GetAsString(2, 0);
		mObjInfo.mCls.mMeasure = table->GetAsString(3, 0);

		//mObjInfo.mObj.mId = table->GetAsString(4, 0);
		mObjInfo.mObj.mLabel = table->GetAsString(5, 0);
	}

	query = wxString::Format(
		"SELECT o.qty, o.pid, get_path_obj(o.pid, 1) "
		" FROM obj o "
		" WHERE o.id = %s "
		, mObjInfo.mObj.mId.toStr());

	if (mObjInfo.mCls.IsQuantity())
		query += wxString::Format("AND o.pid = %s", mObjInfo.mObj.mParent.mId.toStr());

	table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
	{
		mObjInfo.mObj.mQty = table->GetAsString(0, 0);
		mObjInfo.mObj.mParent.mId = table->GetAsString(1, 0);
		//mObjInfo.mObj.mPath = table->GetAsString(2, 0);
	}

	whDataMgr::GetDB().Commit();
	sigObjDetailUpdated(mObjInfo);
}
//---------------------------------------------------------------------------
const rec::ObjInfo& ModelObjDetail::Get()const
{
	return mObjInfo;
}
//---------------------------------------------------------------------------
void ModelObjDetail::Set(const wxString& oid, const wxString& parent_oid)
{
	mObjInfo.mObj.mId = oid;
	mObjInfo.mObj.mParent.mId = parent_oid;
}




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelPageDetail::ModelPageDetail(const std::shared_ptr<rec::ObjInfo>& oi
	, const std::shared_ptr<rec::PageHistory>& data)
{
	// Сохраняем идентификатор и местоположение
	//mObjInfo = *oi;
	mModelObjDetail = std::make_shared<ModelObjDetail>();
	mModelObjDetail->Set(oi->mObj.mId, oi->mObj.mParent.mId);
	connModel_ObjDetailUpdated = mModelObjDetail->sigObjDetailUpdated
		.connect([this](const rec::ObjInfo& rt)
		{
			UpdateTitle();
		});
	
	// настройки окна истории
	auto cfg = std::make_shared<rec::PageHistory>(*data);
	cfg->mVisibleColumnClsObj = false; 
	mModelPageHistory = std::make_shared<ModelPageHistory>(cfg);
	
	// настройки фильтров
	auto filter_list = mModelPageHistory->GetModelHistory().GetFilterList();
	std::vector<ModelFilterList::NotyfyItem> nv(2);

	std::shared_ptr<const ModelFilter> null_filter;

	bool visible = false;
	std::vector<wxString> val(1);
	auto fobj = std::make_shared<ModelFilter>("Объект(Oid)", "mobj_id", FilterOp::foEq, ftLong, visible);
	val[0] = oi->mObj.mId;// "3811";
	fobj->SetValue(val);
	auto oid_filter = std::shared_ptr<const ModelFilter>(fobj);
	
	nv.emplace_back(ModelFilterList::NotyfyItem(null_filter, oid_filter));
	nv.emplace_back(ModelFilterList::NotyfyItem(filter_list->FindBySysName("mcls_title"), null_filter));
	nv.emplace_back(ModelFilterList::NotyfyItem(filter_list->FindBySysName("mobj_title"), null_filter));
	
	filter_list->Update(nv);
	

}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::UpdateTitle() //override;
{
	const rec::ObjInfo& oi = mModelObjDetail->Get();

	const wxString lbl = wxString::Format("[%s]%s (%s %s)"
		, oi.mCls.mLabel.toStr()
		, oi.mObj.mLabel.toStr()
		, oi.mObj.mQty.toStr()
		, oi.mCls.mMeasure.toStr()
		);
	const wxIcon*  ico(&wxNullIcon);
	if (!oi.mCls.mType.IsNull())
		switch (oi.mCls.GetClsType())
	{
		default:
			ico = &ResMgr::GetInstance()->m_ico_type_abstract24;
			break;
		case wh::ctQtyByFloat:	case wh::ctQtyByOne:
			ico = &ResMgr::GetInstance()->m_ico_obj_qty24;
			break;
		case wh::ctSingle:
			ico = &ResMgr::GetInstance()->m_ico_obj_num24;
			break;
	}//switch
	sigUpdateTitle(lbl, *ico);
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Init()//override
{
	mModelObjDetail->Load();
	
	const rec::ObjInfo& oi = mModelObjDetail->Get();
	auto filter_list = mModelPageHistory->GetModelHistory().GetFilterList();
	filter_list->UpdateFilter("mobj_id", oi.mObj.mId.toStr());
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
		mModelPageHistory->Load(it->second);
	}
	const wxString oid = page_val.get<std::string>("CtrlPageDetail.Oid");
	const wxString parent_oid = page_val.get<std::string>("CtrlPageDetail.parentOid");
	mModelObjDetail->Set(oid, parent_oid);
	
	/////Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelPageDetail::Save(boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	mModelPageHistory->Save(content);

	const rec::ObjInfo& obj = mModelObjDetail->Get();
	content.put("Oid", obj.mObj.mId.toStr());
	content.put("parentOid", obj.mObj.mParent.mId.toStr());

	page_val.push_back(std::make_pair("CtrlPageDetail", content));

}
