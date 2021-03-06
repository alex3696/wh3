#include "_pch.h"
#include "MClsMove.h"
#include "MObjCatalog.h"
#include "PathPattern_model.h"

using namespace wh;

const std::vector<Field> gClsMoveFieldVec = {
		{ "������", FieldType::ftName, true },
		{ "��������", FieldType::ftText, true },
		{ "������", FieldType::ftName, true },
		{ "�������", FieldType::ftText, true },
		{ "ID", FieldType::ftLong, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsMove::MClsMove(const char option)
:TModelData<rec::ClsSlotAccess>(option)
{
	namespace ph = std::placeholders;

	auto onChange = std::bind(&MClsMove::OnChange, this, ph::_1, ph::_2);

	connChange = DoConnect(moAfterUpdate, onChange);

}
//-------------------------------------------------------------------------
void MClsMove::OnChange(const IModel* model, const DataType* dt)
{
	if (!model)
		return;
	auto state = model->GetState();

	if (!dt) // MarkDeleted 
		return;

	wh::temppath::model::Array pp;
	
	mSrcPathGui.clear();
	pp.SetArr2Id2Title(dt->mSrcArrId, dt->mSrcArrTitle);

	rec::PathNode src;
	src.mCls = dt->mSrcCls;
	src.mObj = dt->mSrcObj;
	pp.Insert(pp.CreateItem(src));
	pp.GetPath(mSrcPathGui,false);

	mDstPathGui.clear();
	pp.SetArr2Id2Title(dt->mDstArrId, dt->mDstArrTitle);

	rec::PathNode dst;
	dst.mCls = dt->mDstCls;
	dst.mObj = dt->mDstObj;
	pp.Insert(pp.CreateItem(dst));

	pp.GetPath(mDstPathGui,false);


}
//-------------------------------------------------------------------------
bool MClsMove::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;
	
	const rec::Cls& cls = parentCls->GetStored();
	if ("0" == cls.mType)
		return false;
	const rec::ClsSlotAccess& oldPerm = this->GetData();

	query = wxString::Format(
		"SELECT perm_move.id, access_group, access_disabled, script_restrict "
		"     , mov_cls.id, mov_cls.title, mov_obj.id, mov_obj.title "
		"     , src_cls.id, src_cls.title, src_obj.id, src_obj.title "
		"     , dst_cls.id, dst_cls.title, dst_obj.id, dst_obj.title "
		"     , src.arr_2title, src.arr_2id "
		"     , dst.arr_2title, dst.arr_2id "
		"  FROM perm_move "
		"    LEFT JOIN LATERAL tmppath_to_2id_info(src_path) src ON true "
		"	 LEFT JOIN LATERAL tmppath_to_2id_info(dst_path) dst ON true "
		"    LEFT JOIN acls     mov_cls ON mov_cls.id = perm_move.cls_id "
		"    LEFT JOIN obj      mov_obj ON mov_obj.id = perm_move.obj_id "
		"    LEFT JOIN acls     src_cls ON src_cls.id = perm_move.src_cls_id "
		"    LEFT JOIN obj      src_obj ON src_obj.id = perm_move.src_obj_id "
		"    LEFT JOIN acls     dst_cls ON dst_cls.id = perm_move.dst_cls_id "
		"    LEFT JOIN obj      dst_obj ON dst_obj.id = perm_move.dst_obj_id "
		"  WHERE perm_act.id = %s "
		, oldPerm.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MClsMove::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	if (!(cls.IsNumberic() || cls.IsQuantity()))
		return false;

	const rec::ClsSlotAccess& newPerm = this->GetData();

	query = wxString::Format(
		" INSERT INTO perm_move( "
		"  access_group, access_disabled, script_restrict "
		" ,cls_id, obj_id "
		" ,src_cls_id, src_obj_id, src_path "
		" ,dst_cls_id, dst_obj_id, dst_path "
		")VALUES("
		"   %s, %s, %s "
		"  ,%s, %s "
		"  ,%s, %s, %s "
		"  ,%s, %s, %s ) "
		" RETURNING id, access_group, access_disabled, script_restrict "
		"          ,cls_id, (SELECT title FROM acls WHERE id=cls_id) "
		"          ,obj_id, (SELECT title FROM obj WHERE id=obj_id) "
		"          ,src_cls_id, (SELECT title FROM acls WHERE id=src_cls_id) "
		"          ,src_obj_id, (SELECT title FROM obj WHERE id=src_obj_id) "
		"          ,dst_cls_id, (SELECT title FROM acls WHERE id=dst_cls_id) "
		"          ,dst_obj_id, (SELECT title FROM obj WHERE id=dst_obj_id) "
		"    	   ,(SELECT arr_2title FROM  tmppath_to_2id_info(src_path)) "
		"    	   ,(SELECT arr_2id FROM  tmppath_to_2id_info(src_path)) "
		"    	   ,(SELECT arr_2title FROM  tmppath_to_2id_info(dst_path)) "
		"    	   ,(SELECT arr_2id FROM  tmppath_to_2id_info(dst_path)) "

		, newPerm.mAcessGroup.SqlVal()
		, newPerm.mAccessDisabled.SqlVal()
		, newPerm.mScriptRestrict.SqlVal()

		, newPerm.mCls.mId.SqlVal()
		, newPerm.mObj.mId.SqlVal()

		, newPerm.mSrcCls.mId.SqlVal()
		, newPerm.mSrcObj.mId.SqlVal()
		, newPerm.mSrcArrId.SqlVal()

		, newPerm.mDstCls.mId.SqlVal()
		, newPerm.mDstObj.mId.SqlVal()
		, newPerm.mDstArrId.SqlVal()

		);
	return true;

}
//-------------------------------------------------------------------------
bool MClsMove::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());
	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	if (!(cls.IsNumberic() || cls.IsQuantity() ))
		return false;

	const rec::ClsSlotAccess& oldPerm = this->GetStored();
	const rec::ClsSlotAccess& newPerm = this->GetData();

	query = wxString::Format(
		"UPDATE perm_move SET "
		"  access_group=%s, access_disabled=%s, script_restrict=%s  "
		" ,cls_id='%s', obj_id=%s "
		" ,src_cls_id = %s , src_obj_id = %s , src_path = %s "
		" ,dst_cls_id = %s , dst_obj_id = %s , dst_path = %s "
		" WHERE id=%s "
		, newPerm.mAcessGroup.SqlVal()
		, newPerm.mAccessDisabled.SqlVal()
		, newPerm.mScriptRestrict.SqlVal()

		, newPerm.mCls.mId.SqlVal()
		, newPerm.mObj.mId.SqlVal()

		, newPerm.mSrcCls.mId.SqlVal()
		, newPerm.mSrcObj.mId.SqlVal()
		, newPerm.mSrcArrId.SqlVal()

		, newPerm.mDstCls.mId.SqlVal()
		, newPerm.mDstObj.mId.SqlVal()
		, newPerm.mDstArrId.SqlVal()

		, oldPerm.mId.SqlVal()
		);

	return true;
	
}
//-------------------------------------------------------------------------
bool MClsMove::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;
	
	const rec::ClsSlotAccess& oldPerm = this->GetStored();

	query = wxString::Format(
			"DELETE FROM perm_move WHERE id = %s ",
			oldPerm.mId.SqlVal());

	return true;
}
//-------------------------------------------------------------------------
bool MClsMove::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	unsigned int i = 0;
	T_Data data;
	data.mId = table->GetAsString(i++, row);
	data.mAcessGroup = table->GetAsString(i++, row);
	data.mAccessDisabled = table->GetAsString(i++, row);
	data.mScriptRestrict = table->GetAsString(i++, row);

	data.mCls.mId = table->GetAsString(i++, row);
	data.mCls.mLabel = table->GetAsString(i++, row);
	data.mObj.mId = table->GetAsString(i++, row);
	data.mObj.mLabel = table->GetAsString(i++, row);

	data.mSrcCls.mId = table->GetAsString(i++, row);
	data.mSrcCls.mLabel = table->GetAsString(i++, row);
	data.mSrcObj.mId = table->GetAsString(i++, row);
	data.mSrcObj.mLabel = table->GetAsString(i++, row);
	

	data.mDstCls.mId = table->GetAsString(i++, row);
	data.mDstCls.mLabel = table->GetAsString(i++, row);
	data.mDstObj.mId = table->GetAsString(i++, row);
	data.mDstObj.mLabel = table->GetAsString(i++, row);
	
	data.mSrcArrTitle = table->GetAsString(i++, row);
	data.mSrcArrId = table->GetAsString(i++, row);
	
	data.mDstArrTitle = table->GetAsString(i++, row);
	data.mDstArrId = table->GetAsString(i++, row);

	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsMove::GetFieldValue(unsigned int col, wxVariant &variant)
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;

	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:
		variant = variant << wxDataViewIconText(data.mAcessGroup,
			("1" == data.mAccessDisabled) ? mgr->m_ico_reject24 : mgr->m_ico_accept24);
		break;
	case 2:	variant = mSrcPathGui;	break;
	case 3:	variant = wxString::Format("[%s]%s"
		, data.mCls.mLabel.toStr()
		, data.mObj.mLabel.IsNull() ? "*" :	data.mObj.mLabel.toStr()); 
		break;
	case 4: variant = mDstPathGui;	break;
	case 5: variant = data.mId.operator wxString();			break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsMove::GetFieldVector()const
{
	return gClsMoveFieldVec;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsMoveArray::MClsMoveArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MClsMoveArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(GetParent());
	if (!parentCls)
		return false;
	const auto& cls = parentCls->GetStored();

	query = wxString::Format(
		"SELECT perm_move.id, access_group, access_disabled, script_restrict "
		"     , mov_cls.id, mov_cls.title, mov_obj.id, mov_obj.title "
		"     , src_cls.id, src_cls.title, src_obj.id, src_obj.title "
		"     , dst_cls.id, dst_cls.title, dst_obj.id, dst_obj.title "
		"     , src.arr_2title, src.arr_2id "
		"     , dst.arr_2title, dst.arr_2id "
		"  FROM perm_move "
		"    LEFT JOIN LATERAL tmppath_to_2id_info(src_path) src ON true "
		"	 LEFT JOIN LATERAL tmppath_to_2id_info(dst_path) dst ON true "
		"    LEFT JOIN acls      mov_cls ON mov_cls.id = perm_move.cls_id "
		"    LEFT JOIN obj      mov_obj ON mov_obj.id = perm_move.obj_id "
		"    LEFT JOIN acls      src_cls ON src_cls.id = perm_move.src_cls_id "
		"    LEFT JOIN obj      src_obj ON src_obj.id = perm_move.src_obj_id "
		"    LEFT JOIN acls      dst_cls ON dst_cls.id = perm_move.dst_cls_id "
		"    LEFT JOIN obj      dst_obj ON dst_obj.id = perm_move.dst_obj_id "
		"  WHERE dst_cls_id = %s "
		, cls.mId.SqlVal() );
			
	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsMoveArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsMoveArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsMove>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsMoveArray::GetFieldVector()const
{
	return gClsMoveFieldVec;
}
