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

	if (data.mObj.mId.IsNull() || data.mObj.mParent.mId.IsNull() )
		return false;
	
	//if (data.mCls.mType.IsNull() 
	//	|| (data.mCls.IsQuantity() && data.mObj.mParent.mId.IsNull()))
	//	return false;

	query = wxString::Format(
		"SELECT  o.id, o.pid, o.title, o.qty, o.move_logid "
		"      , cls.id, cls.title, cls.kind, cls.measure, cls.dobj as defaultPid "
		"      , cls.pid AS cls_parent_id, cls_parent.title AS cls_parent_title "
		" FROM obj o "
		" INNER JOIN acls cls      ON cls.id = o.cls_id "
		" INNER JOIN acls cls_parent ON cls.pid = cls_parent.id "
		" WHERE o.id=%s "
		" AND o.pid=%s" 
		, data.mObj.mId.SqlVal()
		, data.mObj.mParent.mId.SqlVal()
		);

	if (!data.mCls.mId.IsNull())
		query += wxString::Format("AND o.cls_id=%s ", data.mCls.mId.SqlVal());
	
	//if (data.mCls.IsQuantity())
	//	query += wxString::Format("AND o.pid=%s", data.mObj.mParent.mId.SqlVal());

	return true;
}
//-----------------------------------------------------------------------------
void Obj::LoadChilds()
{
	mClsProp->Load();
	mObjProp->Load();
	mObjPropValLoader->Load();
}