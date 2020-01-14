#include "_pch.h"
#include "MoveObjModel.h"
#include "globaldata.h"
#include "RecentDstOidPresenter.h"

using namespace wh;



//-----------------------------------------------------------------------------
ModelMoveExecWindow::ModelMoveExecWindow()
	: mModelObjBrowser(std::make_shared<ModelBrowser>())
	, mDstObject(0,0,0, nullptr)
{
	//auto x = make_shared_model<bool>(true);
	
	//mEnableRecent.SetData(true);
	//mMoveble.SetData(rec::PathItem());
	//mDst.SetData(ObjTree());
	//mRecent.SetData(ObjTree());


	
}
//---------------------------------------------------------------------------
ModelMoveExecWindow::~ModelMoveExecWindow()
{

}
//---------------------------------------------------------------------------
void ModelMoveExecWindow::UnlockObjectsWithoutTransaction()
{
	wxString query;
	for (const auto& obj : mObjects)
	{
		query = wxString::Format(
			"SELECT lock_reset(%s,%s)"
			, obj.GetId_AsString()
			, obj.GetParentId_AsString());
		whDataMgr::GetDB().Exec(query);
	}

}
//---------------------------------------------------------------------------
void ModelMoveExecWindow::UnlockObjects()
{
	whDataMgr::GetDB().BeginTransaction();
	//UnlockObjectsWithoutTransaction();
	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
int ModelMoveExecWindow::BuildExecQuery(wxString& query)
{
	return 1;
}

//-----------------------------------------------------------------------------
const ObjStore::iterator ModelMoveExecWindow::FindObj(const wxString& str)const
{
	std::function<bool(const Obj&)>
		isInTitle = [&str](const Obj& obj)
	{
		return obj.mId.Find(str) != wxNOT_FOUND;
	};

	return std::find_if(mDst.mObj.cbegin(), mDst.mObj.cend(), isInTitle);
}

//-----------------------------------------------------------------------------
void ModelMoveExecWindow::DoExecute()
{
	/*
	const wxString& oid, const wxString& qty
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = mMoveble.GetData();

	wxString query = wxString::Format(
		"SELECT do_move(%s,%s,%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal()
		, oid
		, qty
		);
	whDataMgr::GetDB().Exec(query);

	query = wxString::Format(
		"SELECT lock_reset(%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal());
	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();

	whDataMgr::GetInstance()->mRecentDstOidPresenter->Insert(oid);
	*/

}
//-----------------------------------------------------------------------------
bool ModelMoveExecWindow::GetRecentEnable()const
{
	return whDataMgr::GetInstance()->mRecentDstOidPresenter->GetRecentEnable()==1 ;
}
//-----------------------------------------------------------------------------
void ModelMoveExecWindow::SetRecentEnable(bool enable)
{
	char i = enable ? 1 : 0;
	whDataMgr::GetInstance()->mRecentDstOidPresenter->SetRecentEnable(i);
}
//-----------------------------------------------------------------------------
void ModelMoveExecWindow::LockObjects(const std::set<ObjectKey>& obj)
{
	TEST_FUNC_TIME;
	mObjects = obj;
	wxBusyCursor busyCursor;
	mDst.Clear();
	mRecent.Clear();

	const auto& global_recent = whDataMgr::GetInstance()->mRecentDstOidPresenter;

	wxString query = wxString::Format(
		" SELECT _dst_cls_id, acls.title "
		", _dst_obj_id, _dst_obj_label , get_path_objnum(_dst_obj_pid,1) AS DST_PATH "
		" FROM lock_for_move(%s,%s) "
		" LEFT JOIN acls ON acls.id = _dst_cls_id "
		" ORDER BY "
		"   acls.title ASC "
		"   ,(substring(_dst_obj_label, '^[0-9]{1,9}')::INT, _dst_obj_label ) ASC "
		, obj.begin()->GetId_AsString()
		, obj.begin()->GetParentId_AsString()
	);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		wxString cid;
		wxString ctitle;
		wxString oid;
		wxString otitle;
		wxString opath;

		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			mDst.mObj.reserve(rowQty);
			for (unsigned int i = 0; i < rowQty; ++i)
			{
				table->GetAsString(0, i, cid);
				table->GetAsString(1, i, ctitle);
				table->GetAsString(2, i, oid);
				table->GetAsString(3, i, otitle);
				table->GetAsString(4, i, opath);

				auto type_IterBool = mDst.mType.emplace_back(cid, ctitle);
				const Type* type = &(*type_IterBool.first);
				auto obj_IterBool = mDst.mObj.emplace_back(type, oid, otitle, opath);

				if (global_recent->Check(oid))
				{
					auto type_recent_itbool = mRecent.mType.emplace_back(cid, ctitle);
					const Type* type = &(*type_recent_itbool.first);
					mRecent.mObj.emplace_back(type, oid, otitle, opath);
				}
			}//for
		}// if (rowQty)

	}//if (table)

	whDataMgr::GetDB().Commit();
}
//---------------------------------------------------------------------------
//virtual 
void ModelMoveExecWindow::UpdateTitle()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelMoveExecWindow::Show()
{
	mCurrentPage = 0;
	sigSelectPage(mCurrentPage);
	sigShow();
}
//---------------------------------------------------------------------------
//virtual 
void ModelMoveExecWindow::Init()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelMoveExecWindow::Load(const boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelMoveExecWindow::Save(boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------