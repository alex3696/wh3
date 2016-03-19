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
	this->Insert(mObjProp);
	this->Insert(mClsProp);
	this->Insert(mObjPropValLoader);
}
//-----------------------------------------------------------------------------
void Obj::SetObject(const rec::ObjInfo& oi)
{
	mClsProp->Clear();
	mObjProp->Clear();

	SetData(oi, true);

}
//-----------------------------------------------------------------------------
bool Obj::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	
	data.mObj.mId            = table->GetAsString(0, row);
	data.mObj.mParent.mId    = table->GetAsString(1, row);
	data.mObj.mLabel         = table->GetAsString(2, row);
	data.mObj.mQty           = table->GetAsString(3, row);
	data.mObj.mLastMoveLogId = table->GetAsString(4, row);

	data.mCls.mId = table->GetAsString(5, row);
	data.mCls.mLabel = table->GetAsString(6, row);
	data.mCls.mType = table->GetAsString(7, row);
	data.mCls.mMeasure = table->GetAsString(8, row);
	data.mCls.mDefaultObj.mId = table->GetAsString(9, row);

	data.mCls.mParent.mId = table->GetAsLong (10, row );
	data.mCls.mParent.mLabel = table->GetAsString(11, row);

	SetData(data,true);
	return true;
};
//-----------------------------------------------------------------------------
bool Obj::GetSelectQuery(wxString& query)const
{
	const auto& data = GetData();

	if (data.mCls.mId.IsNull() || data.mObj.mId.IsNull() )
		return false;
	
	if (data.mCls.mType.IsNull() 
		|| (data.mCls.IsQuantity() && data.mObj.mParent.mId.IsNull()))
		return false;
	
	query = wxString::Format(
		"SELECT  o.id, o.pid, o.title, o.qty, o.move_logid "
		"      , co.id, co.title, co.kind, co.measure, NULL as defaultPid "
		"      , cparent.id, cparent.title "
		" FROM obj o "
		" LEFT JOIN cls co      ON co.id = o.cls_id "
		" LEFT JOIN cls cparent ON co.pid = cparent.id "
		" WHERE o.cls_id=%s  AND o.id=%s " 
		, data.mCls.mId.SqlVal()
		, data.mObj.mId.SqlVal()
		);

	if (data.mCls.IsQuantity())
		query += wxString::Format("AND o.pid=%s ", data.mObj.mParent.mId.SqlVal());

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