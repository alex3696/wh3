#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
Obj::Obj(const char option)
	:TModelData<rec::ObjInfo>(option)
	, mClsProp(new ClsPropArray)
	, mObjProp(new ObjPropArray)
	, mObjPropValLoader(new ObjPropValLoader)
{
	this->AddChild(mObjProp);
	this->AddChild(mClsProp);
	this->AddChild(mObjPropValLoader);
}
//-----------------------------------------------------------------------------
void Obj::SetObject(const wxString& cls_id, const wxString& obj_id, const wxString& obj_pid)
{
	T_Data data;

	data.mCls.mID = cls_id;
	data.mObj.mID = obj_id;
	data.mObj.mPID = obj_pid;

	mClsProp->Clear();
	mObjProp->Clear();

	SetData(data,true);

}
//-----------------------------------------------------------------------------
bool Obj::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	
	table->GetAsString(0, row, data.mObj.mID);
	table->GetAsString(1, row, data.mObj.mPID);
	table->GetAsString(2, row, data.mObj.mLabel);
	table->GetAsString(3, row, data.mObj.mQty);
	table->GetAsString(4, row, data.mObj.mLastLogId);

	table->GetAsString(5, row, data.mCls.mID);
	table->GetAsString(6, row, data.mCls.mLabel);
	table->GetAsString(7, row, data.mCls.mType);
	table->GetAsString(8, row, data.mCls.mMeasure);
	table->GetAsString(9, row, data.mCls.mDefaultPid);

	table->GetAsString(10, row, data.mCls.mParent.mId);
	table->GetAsString(11, row, data.mCls.mParent.mLabel);

	SetData(data,true);
	return true;
};
//-----------------------------------------------------------------------------
bool Obj::GetSelectQuery(wxString& query)const
{
	const auto& data = GetData();
	if (data.mCls.mID.IsEmpty() || data.mObj.mID.IsEmpty() || data.mObj.mPID.IsEmpty())
		return false;
	
	query = wxString::Format(
		" SELECT obj_id, obj_pid, obj_label, qty, last_log_id "
		"  , cls_id,  cls_label, w_obj.type, w_obj.measurename, w_obj.cls_default_pid "
		"    ,parent.id "
		"    ,parent.label "
		" FROM w_obj "
		" LEFT JOIN t_cls parent ON cls_pid = parent.id "
		" WHERE cls_id=%s  AND obj_id=%s AND obj_pid=%s "
		, data.mCls.mID
		, data.mObj.mID
		, data.mObj.mPID
		);

	return true;
}
//-----------------------------------------------------------------------------
void Obj::LoadChilds()
{
	mClsProp->Load();
	mObjProp->Load();
	mObjPropValLoader->Load();
}