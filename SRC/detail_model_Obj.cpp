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

	data.mCls.mID = table->GetAsString(5, row);
	data.mCls.mLabel = table->GetAsString(6, row);
	data.mCls.mType = table->GetAsString(7, row);
	data.mCls.mMeasure = table->GetAsString(8, row);
	data.mCls.mDefaultObjPid.mId = table->GetAsString(9, row);

	data.mCls.mParent.mId = table->GetAsLong (10, row );
	data.mCls.mParent.mLabel = table->GetAsString(11, row);

	SetData(data,true);
	return true;
};
//-----------------------------------------------------------------------------
bool Obj::GetSelectQuery(wxString& query)const
{
	const auto& data = GetData();
	if (data.mCls.mID.IsNull() || data.mObj.mID.IsEmpty() || data.mObj.mPID.IsEmpty())
		return false;
	
	query = wxString::Format(
		"SELECT  o.id, o.pid, o.title, o.qty, o.move_logid "
		"      , co.id, co.title, co.kind, co.measure, NULL as defaultPid "
		"      , cparent.id, cparent.title "
		" FROM obj_tree o "
		" LEFT JOIN cls_real co ON co.id = o.cls_id "
		" LEFT JOIN cls cparent ON co.pid = cparent.id "
		" WHERE o.cls_id=%s  AND o.id=%s AND o.pid=%s "
		, data.mCls.mID.SqlVal()
		, data.mObj.mID
		, data.mObj.mPID
		);

	return true;

	/*
	SELECT obj_id, obj_pid, obj.label, qty, obj.last_log_id -- obj info
	, obj.cls_id,  cls_label, obj.type, obj.measurename, obj.cls_default_pid
	,parent.id AS cls_parent_id ,parent.label AS cls_parent_label
	, pobj.id, pobj.label
	, pobjcls.id, pobjcls.label
	FROM w_obj obj
	LEFT JOIN t_objnum pobj ON pobj.id = obj.obj_pid
	LEFT JOIN t_cls parent  ON cls_pid = parent.id
	LEFT JOIN t_cls pobjcls ON pobj.cls_id = pobjcls.id
	WHERE obj.obj_id=103 AND obj.cls_id=105 AND obj.obj_pid=100
	*/


}
//-----------------------------------------------------------------------------
void Obj::LoadChilds()
{
	mClsProp->Load();
	mObjProp->Load();
	mObjPropValLoader->Load();
}