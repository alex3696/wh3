#include "_pch.h"
#include "dlg_move_model_MovableObj.h"

using namespace wh;
using namespace wh::dlg_move::model;


//-----------------------------------------------------------------------------
MovableObj::MovableObj()
	:TModelData<DataType>(ModelOption::CommitLoad)
	, mDstTypes(new DstTypeArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mDstTypes));

}

//-----------------------------------------------------------------------------
std::shared_ptr<DstTypeArray>	MovableObj::GetDstTypes()const
{
	return mDstTypes;
}

//-----------------------------------------------------------------------------
void MovableObj::LoadChilds()
{
	mDstTypes->Load();
}
//-----------------------------------------------------------------------------
void MovableObj::Unlock()
{
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = GetData();

	wxString query = wxString::Format(
		"SELECT lock_reset(%s,%s,%s)"
		, movable.mCls.mID
		, movable.mObj.mID
		, movable.mObj.mPID);
	
	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void MovableObj::Move(std::shared_ptr<wh::dlg_move::model::DstObj> dst, 
	const wxString& qty)
{
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = GetData();
	const rec::ObjTitle& destination = dst->GetData();

	wxString query = wxString::Format(
		"SELECT move_object(%s,%s,%s,%s,%s)"
		, movable.mCls.mID
		, movable.mObj.mID
		, movable.mObj.mPID
		, destination.mID
		, qty
		);
	whDataMgr::GetDB().Exec(query);


	query = wxString::Format(
		"SELECT lock_reset(%s,%s,%s)"
		, movable.mCls.mID
		, movable.mObj.mID
		, movable.mObj.mPID);
	whDataMgr::GetDB().Exec(query);


	whDataMgr::GetDB().Commit();
}