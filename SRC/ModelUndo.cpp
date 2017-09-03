#include "_pch.h"
#include "ModelUndo.h"
#include "ModelHistory.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class ModelHistoryRecordImpl 
	: public ModelHistoryRecord
	, public LogRec
{
public:
	//LogRec mRecord;

	virtual const wxString& GetLogId()const override	{ return mId;}
	virtual const wxString& GetUser()const override		{ return mUser; }
	virtual const wxString& GetDate()const override		{ return mTimestamp; }

	virtual const IObj& GetObj()const override			{return *mObj; }
	virtual const IAct& GetAct()const override			{ return mDetail->GetActRec(); }
	virtual const PropValTable& GetActProperties()const override{ return mDetail->GetActProperties(); }
	virtual const PropValTable& GetProperties()const override	{ return mDetail->GetProperties(); }
	virtual const IObjPath&	GetPath()const override		{ return mDetail->GetPath(); }
	virtual const IObjPath& GetDstPath()const override	{ return mDetail->GetDstPath(); }
	virtual const wxString& GetQty()const override		{ return mDetail->GetQty(); }
};//ModelHistoryRecordImpl 

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelUndo::ModelUndo()
	:mRecord(std::make_shared<ModelHistoryRecordImpl>())
{
	//mRecord = std::make_shared<ModelHistoryRecordImpl>();
}
//---------------------------------------------------------------------------
ModelUndo::ModelUndo(const wxString& oid, const wxString& parent_oid)
	: mRecord(std::make_shared<ModelHistoryRecordImpl>())
{
	//mRecord = std::make_shared<ModelHistoryRecordImpl>();
	Set(oid, parent_oid);
}
//---------------------------------------------------------------------------
void ModelUndo::Load()
{
	PropTable		prop_table;
	auto log_rec = std::make_shared<ModelHistoryRecordImpl>();
	mRecord = log_rec;
	wxString query;

	whDataMgr::GetDB().BeginTransaction();

	query = wxString::Format(
		"SELECT log_id, log_dt, log_user"
		" ,mcls_id, mcls_title, mobj_id, mobj_title, qty"
		" ,act_id, act_title, act_color, prop"
		" ,src_cid, src_oid, src_ipath, src_path"
		" ,dst_cid, dst_oid, dst_ipath, dst_path"
		" ,prop_lid, mcls_kind, mcls_measure "
		" FROM log "
		" WHERE mobj_id=%s "
		" ORDER BY log_dt DESC LIMIT 1"
		, mOid);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
	{
		size_t i = 0;
		table->GetAsString(0, i, log_rec->mId);
		table->GetAsString(1, i, log_rec->mTimestamp);
		table->GetAsString(2, i, log_rec->mUser);
		//log_rec = *mLog.emplace_back(log_rec).first;

		auto cls_rec = std::make_shared<ClsRec>();
		table->GetAsString(3, i, cls_rec->mId);
		table->GetAsString(4, i, cls_rec->mTitle);
		table->GetAsString(21, i, cls_rec->mKind);
		table->GetAsString(22, i, cls_rec->mMeasure);
		//cls_rec = *mCls.emplace_back(cls_rec).first;

		auto obj_rec = std::make_shared<ObjRec>();
		table->GetAsString(5, i, obj_rec->mId);
		table->GetAsString(6, i, obj_rec->mTitle);


		obj_rec->mCls = cls_rec;
		//obj_rec = *mObj.emplace_back(obj_rec).first;

		log_rec->mObj = obj_rec;

		// load properties
		boost::property_tree::wptree prop_arr;
		wxString json_prop;
		table->GetAsString(11, i, json_prop);
		if (!json_prop.IsEmpty())
		{
			std::wstringstream ss; ss << json_prop;
			boost::property_tree::read_json(ss, prop_arr);
		}

		auto propval_table = std::make_shared<PropValTable >();
		for (const auto& pv : prop_arr)
		{
			auto prop_rec = std::make_shared<PropRec>();
			prop_rec->mId = wxString(pv.first.c_str());
			auto iprop_rec = *prop_table.emplace_back(prop_rec).first;

			auto propval_rec = std::make_shared<PropValRec>();
			propval_rec->mProp = iprop_rec;
			propval_rec->mVal = pv.second.get_value<std::wstring>();

			propval_table->emplace(propval_rec);
		}//for
		// load properties

		wxString aid;
		table->GetAsString(8, i, aid);

		if (aid.empty())
		{
			auto log_mov_rec = std::make_shared<LogMovRec>();

			table->GetAsString(7, i, log_mov_rec->mQty);
			table->GetAsString(19, i, log_mov_rec->mDstPath.mPath);
			table->GetAsString(20, i, log_mov_rec->mPropLId);
			//table->GetAsString(11, i, *log_mov_rec->mProperties);
			log_mov_rec->mProperties = propval_table;
			log_rec->mDetail = log_mov_rec;
		}
		else
		{
			auto act_rec = std::make_shared<ActRec>();
			act_rec->mId = aid;

			table->GetAsString(9, i, act_rec->mTitle);
			table->GetAsString(10, i, act_rec->mColour);
			//act_rec = *mAct.emplace_back(act_rec).first;

			auto log_act_rec = std::make_shared<LogActRec>();
			log_act_rec->mActRec = act_rec;

			log_act_rec->mProperties = propval_table;

			log_rec->mDetail = log_act_rec;
		}// else if (aid.empty())


		table->GetAsString(15, i, log_rec->mDetail->mPath.mPath);
	}
	


	LoadPropertyDetails(prop_table);
	LoadActAndPrepareProp();

	whDataMgr::GetDB().Commit();
	
	sigHistoryRecordLoaded(*mRecord);
}
//---------------------------------------------------------------------------
void ModelUndo::LoadPropertyDetails(PropTable& prop_table)
{
	wxString where_prop_id;
	for (const auto& prop : prop_table)
		where_prop_id += wxString::Format("OR id=%s ", prop->GetId());

	where_prop_id.Replace("OR", "WHERE", false);

	wxString query = wxString::Format(
		"SELECT id, title, kind, var "//, var_strict
		" FROM prop "
		" %s "
		, where_prop_id);
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
		return;

	unsigned int rowQty = table->GetRowCount();
	if (!rowQty)
		return;

	for (unsigned int i = 0; i < rowQty; ++i)
	{
		wxString id;
		table->GetAsString(0, i, id);
		auto it = prop_table.get<1>().find(id);
		if (it != prop_table.get<1>().end())
		{
			// multiitndex Modifier
			prop_table.get<1>().modify(it,
				[table, i](std::shared_ptr<IProp>& e)
			{
				auto prop_rec = std::dynamic_pointer_cast<PropRec>(e);
				table->GetAsString(1, i, prop_rec->mTitle);
				table->GetAsString(2, i, prop_rec->mKind);
			});
			//auto prop_rec = std::dynamic_pointer_cast<PropRec>(*it);
			//table->GetAsString(1, i, prop_rec->mTitle);
			//table->GetAsString(2, i, prop_rec->mKind);
		}//if (it != prop_table.get<1>().end())
	}//for (unsigned int i = 0; i < rowQty; ++i)
}
//---------------------------------------------------------------------------
void ModelUndo::LoadActAndPrepareProp()
{
	if (mRecord->GetAct().GetId().IsEmpty())
		return;
	
	wxString query = wxString::Format(
		"SELECT id, act_id, prop_id "
		" FROM ref_act_prop "
		" WHERE act_id=%s "
		, mRecord->GetAct().GetId());

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
		return;
	unsigned int rowQty = table->GetRowCount();

	auto log_rec = std::dynamic_pointer_cast<LogRec>(mRecord);

	log_rec->mDetail->mActProperties = std::make_shared<PropValTable>();

	for (unsigned int i = 0; i < rowQty; ++i)
	{
		
		wxString  pid;
		//table->GetAsString(1, i, aid);
		table->GetAsString(2, i, pid);
		const auto& pid_idx_allprop = mRecord->GetProperties().get<0>();
		auto it = pid_idx_allprop.find(pid);
		if (pid_idx_allprop.end() != it)
		{
			log_rec->mDetail->mActProperties->emplace(*it);
		}

	}//for (unsigned int i = 0; i < rowQty; ++i)

}
//---------------------------------------------------------------------------
void ModelUndo::Set(const wxString& oid, const wxString& parent_oid)
{
	mOid = oid;
	mParentOid = oid;
}
//---------------------------------------------------------------------------
const ModelHistoryRecord& ModelUndo::GetHistoryRecord()const
{
	return *mRecord;
}
//---------------------------------------------------------------------------
void ModelUndo::ExecuteUndo()
{
	wxString query = wxString::Format(
		"DELETE FROM log WHERE log_id = %s "
		, mRecord->GetLogId());
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();
	sigHistoryRecordRemoved(*mRecord);
}




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelUndoWindow::ModelUndoWindow(const wxString& oid, const wxString& parent_oid)
	:mModelUndo ( std::make_shared<ModelUndo>())
{
	//mModelUndo = std::make_shared<ModelUndo>();
	connModelUndo_RecordLoaded = mModelUndo->sigHistoryRecordLoaded
		.connect([this](const ModelHistoryRecord& rt)
	{
		UpdateTitle();
	});

	mModelUndo->Set(oid, parent_oid);

}
//---------------------------------------------------------------------------
ModelUndoWindow::ModelUndoWindow(const rec::ObjInfo& oi)
	:ModelUndoWindow(oi.mObj.mId, oi.mObj.mParent.mId)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelUndoWindow::UpdateTitle() //override;
{
	const auto& log_record = mModelUndo->GetHistoryRecord();

	const wxString lbl = wxString::Format("Откат действия для [%s]%s"
		, log_record.GetObj().GetCls().GetTitle()
		, log_record.GetObj().GetTitle()
		);

	const wxIcon&  ico = wxArtProvider::GetIcon(wxART_UNDO, wxART_MENU);
	sigUpdateTitle(lbl, ico);
}
//---------------------------------------------------------------------------
//virtual 
void ModelUndoWindow::Init()//override
{
	mModelUndo->Load();
}
//---------------------------------------------------------------------------
//virtual 
void ModelUndoWindow::Load(const boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	auto it = page_val.find("UndoWindow");//CtrlPageHistory
	//const wxString oid = page_val.get<std::string>("CtrlPageDetail.Oid");
	//const wxString parent_oid = page_val.get<std::string>("CtrlPageDetail.parentOid");
	//mModelUndo->Set(oid, parent_oid);

	/////Update();
}
//---------------------------------------------------------------------------
//virtual 
void ModelUndoWindow::Save(boost::property_tree::ptree& page_val)//override;
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	//const rec::ObjInfo& obj = mModelObjDetail->Get();
	//content.put("Oid", obj.mObj.mId.toStr());
	//content.put("parentOid", obj.mObj.mParent.mId.toStr());
	//page_val.push_back(std::make_pair("UndoWindow", content));

}
