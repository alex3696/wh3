#include "_pch.h"
#include "dlg_act_model_Obj.h"

using namespace wh;
using namespace wh::dlg_act::model;

//-----------------------------------------------------------------------------
Obj::Obj(const char option)
	:TModelData<DataType>()
	, mActArray(new ActArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mActArray));
}

//-----------------------------------------------------------------------------
void Obj::Unlock()
{
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& subj = GetData();

	wxString query = wxString::Format(
		"SELECT lock_reset(%s,%s,%s)"
		, subj.mCls.mId.SqlVal()
		, subj.mObj.mId.SqlVal()
		, subj.mObj.mParent.mId.SqlVal() );

	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();
}

