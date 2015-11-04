#include "_pch.h"
#include "MClsAct.h"
#include "MObjCatalog.h"

using namespace wh;

const std::vector<Field> gClsActFieldVec = {
		{ "Действие", FieldType::ftName, true },
		{ "Доступ", FieldType::ftName, true },
		{ "Группа", FieldType::ftName, true },
		{ "Объект", FieldType::ftName, true },
		{ "Путь", FieldType::ftText, true },
		{ "ID", FieldType::ftInt, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsAct::MClsAct(const char option)
:TModelData<rec::ClsActAccess>(option)
{
	mSrcPathArr = std::make_shared<temppath::model::Array>();
	this->AddChild(mSrcPathArr);
}

//-------------------------------------------------------------------------
bool MClsAct::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;
	
	const rec::Cls& cls = parentCls->GetStored();
	if ("1" != cls.mType)
		return false;
	const rec::ClsActAccess& oldPerm = this->GetStored();

	query = wxString::Format(
		"SELECT perm_act.id, access_group, access_disabled, script_restrict "
		"     , cls.id, cls.title, obj.id, obj.title "
		"     , act.id, act.title "
		"     , arr_2title, arr_2id "
		"  FROM perm_act "
		"	 LEFT JOIN LATERAL tmppath_to_2id_info(src_path) x ON true "
		"    LEFT JOIN cls   ON cls.id = perm_act.cls_id "
		"    LEFT JOIN obj   ON obj.id = perm_act.obj_id "
		"    LEFT JOIN act ON act.id = perm_act.act_id "
		"  WHERE perm_act.id = %s "
		, oldPerm.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MClsAct::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;
	const rec::Cls& cls = parentCls->GetStored();
	if ("1" != cls.mType)
		return false;
	//const rec::ClsActAccess& oldClsAct = this->GetStored();
	const rec::ClsActAccess& newPerm = this->GetData();

	

	query = wxString::Format(
		" INSERT INTO perm_act( "
		"  access_group, access_disabled, script_restrict "
		" ,cls_id, obj_id, src_path "
		" ,act_id "
		")VALUES("
		"   %s, %s, %s "
		"  ,%s, %s, %s "
		"  ,%s ) "
		" RETURNING id, access_group, access_disabled, script_restrict "
		"          ,cls_id, obj_id, src_path "
		"          ,act_id "
		, newPerm.mAcessGroup.SqlVal()
		, newPerm.mAccessDisabled.SqlVal()
		, newPerm.mScriptRestrict.SqlVal()

		, cls.mId.SqlVal() //newPerm.mSrcCls.mId = cls.mID;
		, newPerm.mObj.mId.SqlVal()
		, mSrcPathArr->GetTmpPathArr2IdSql()

		, newPerm.mAct.mId.SqlVal()
		);
	return true;
	
}
//-------------------------------------------------------------------------
bool MClsAct::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;

	const rec::Cls& cls = parentCls->GetStored();
	if ("1" != cls.mType)
		return false;
	const rec::ClsActAccess& oldPerm = this->GetStored();
	const rec::ClsActAccess& newPerm = this->GetData();

	query = wxString::Format(
			"UPDATE perm_act SET "
			"  access_group=%s, access_disabled=%s, script_restrict=%s  "
			" ,cls_id = %s, obj_id = %s, src_path = %s "
			" ,act_id = %s "
			" WHERE id=%s "
			, newPerm.mAcessGroup.SqlVal()
			, newPerm.mAccessDisabled.SqlVal()
			, newPerm.mScriptRestrict.SqlVal()

			, cls.mId.SqlVal() //newPerm.mSrcCls.mId = cls.mID;
			, newPerm.mObj.mId.SqlVal()
			, mSrcPathArr->GetTmpPathArr2IdSql()
			
			, newPerm.mAct.mId.SqlVal()

			, oldPerm.mId.SqlVal()
			);

	return true;
}
//-------------------------------------------------------------------------
bool MClsAct::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (!parentCls)
		return false;
	
	const rec::Cls& cls = parentCls->GetStored();
	if ("1" != cls.mType)
		return false;
	const rec::ClsActAccess& oldPerm = this->GetStored();

	query = wxString::Format(
		"DELETE FROM perm_act WHERE id = %s ",
		oldPerm.mId.SqlVal() );
	return true;
	
}
//-------------------------------------------------------------------------
bool MClsAct::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	unsigned int i = 0;
	T_Data data;
	data.mId             = table->GetAsString(i++, row);
	data.mAcessGroup     = table->GetAsString(i++, row );
	data.mAccessDisabled = table->GetAsString(i++, row);
	data.mScriptRestrict = table->GetAsString(i++, row);

	data.mCls.mId     = table->GetAsString(i++, row);
	data.mCls.mLabel  = table->GetAsString(i++, row);
	data.mObj.mId     = table->GetAsString(i++, row);
	data.mObj.mLabel  = table->GetAsString(i++, row);
	
	data.mAct.mId        = table->GetAsString(i++, row);
	data.mAct.mLabel     = table->GetAsString(i++, row);
	
	auto arr2title = table->GetAsString(i++, row);
	auto arr2id    = table->GetAsString(i++, row);

	mSrcPathArr->SetTmpPath(arr2id, arr2title);

	

	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsAct::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:	variant = variant << wxDataViewIconText(data.mAct.mLabel, mgr->m_ico_act24);
		break;
	case 2:	variant = ("1" == data.mAccessDisabled) ? "Запретить" : "Разрешить";	break;
	case 3:	variant = data.mAcessGroup;	break;
	case 4: variant = data.mObj.mLabel;	break;
	case 5: variant = mSrcPathArr->GetTmpPath();	break;
	case 6: variant = data.mId.toStr();	break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsAct::GetFieldVector()const
{
	return gClsActFieldVec;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsActArray::MClsActArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MClsActArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(this->mParent);
	if (!parentCls)
		return false;

	const auto& cls = parentCls->GetStored();

	query = wxString::Format(
		"SELECT perm_act.id, access_group, access_disabled, script_restrict "
		"     , cls.id, cls.title, obj.id, obj.title "
		"     , act.id, act.title "
		"     , arr_2title, arr_2id "
		"  FROM perm_act "
		"	 LEFT JOIN LATERAL tmppath_to_2id_info(src_path) x ON true "
		"    LEFT JOIN cls   ON cls.id = perm_act.cls_id "
		"    LEFT JOIN obj   ON obj.id = perm_act.obj_id "
		"    LEFT JOIN act ON act.id = perm_act.act_id "
        "  WHERE perm_act.cls_id = %s "
		, cls.mId.SqlVal() );

	return true;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsActArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsActArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsAct>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsActArray::GetFieldVector()const
{
	return gClsActFieldVec;
}