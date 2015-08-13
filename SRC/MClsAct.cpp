#include "_pch.h"
#include "MClsAct.h"
#include "MClsTree.h"

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
}

//-------------------------------------------------------------------------
bool MClsAct::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		const rec::ClsActAccess& oldClsAct = this->GetStored();
		//const rec::ClsActAccess& newClsAct = this->GetData();

		query = wxString::Format(
			"SELECT t_access_act.id, access_group, access_disabled, script_restrict "
			", act_id, t_act.label "
			", cls_id, t_cls.label, obj_label "
			", src_path "
			" FROM t_access_act "
			" LEFT JOIN t_act ON t_act.id = t_access_act.act_id "
			" LEFT JOIN t_cls ON t_cls.id = t_access_act.cls_id "
			" WHERE t_access_act.id = %s "
			, oldClsAct.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsAct::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		//const rec::ClsActAccess& oldClsAct = this->GetStored();
		const rec::ClsActAccess& newClsAct = this->GetData();
		
		const wxString obj_id = newClsAct.mObjLabel.IsEmpty() ? "NULL" : newClsAct.mObjLabel;

		query = wxString::Format("INSERT INTO t_access_act("
			" access_group, access_disabled, script_restrict "
			", act_id "
			", cls_id, obj_id"
			", src_path  "
			") VALUES ('%s', %s, '%s', %s, %s, %s, '%s') "
			" RETURNING id, access_group, access_disabled, script_restrict "
			", act_id, NULL "
			", cls_id, NULL, obj_id "
			", src_path "
			, newClsAct.mAcessGroup, newClsAct.mAccessDisabled, newClsAct.mScriptRestrict
			, newClsAct.mActID
			, cls.mID, obj_id
			, newClsAct.mPath
			);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsAct::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		const rec::ClsActAccess& oldClsAct = this->GetStored();
		const rec::ClsActAccess& newClsAct = this->GetData();

		query = wxString::Format(
			"UPDATE	t_access_act "
			" SET access_group='%s', access_disabled=%s, script_restrict='%s'  "
			", act_id=%s "
			", cls_id=%s, obj_label='%s' "
			", src_path='%s' "
			" WHERE id=%s "
			, newClsAct.mAcessGroup, newClsAct.mAccessDisabled, newClsAct.mScriptRestrict
			, newClsAct.mActID
			, cls.mID, newClsAct.mObjLabel
			, newClsAct.mPath
			, oldClsAct.mID);

		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsAct::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsActArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		const rec::ClsActAccess& oldClsAct = this->GetStored();
		//const rec::ClsActAccess& newClsAct = this->GetData();

		query = wxString::Format(
			"DELETE FROM t_access_act WHERE id = %s ",
			//"DELETE FROM t_ref_class_act WHERE cls_id=% AND act_id=%"
			oldClsAct.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsAct::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mAcessGroup);
	table->GetAsString(2, row, data.mAccessDisabled);
	table->GetAsString(3, row, data.mScriptRestrict);
	
	table->GetAsString(4, row, data.mActID);
	table->GetAsString(5, row, data.mActLabel);

	table->GetAsString(6, row, data.mClsID);
	table->GetAsString(7, row, data.mClsLabel);
	table->GetAsString(8, row, data.mObjID);
	table->GetAsString(9, row, data.mObjLabel);
	
	table->GetAsString(10, row, data.mPath);
	
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
	case 1:	variant = variant << wxDataViewIconText(data.mActLabel, mgr->m_ico_act24);
		break;
	case 2:	variant = ("1" == data.mAccessDisabled) ? "Запретить" : "Разрешить";	break;
	case 3:	variant = data.mAcessGroup;	break;
	case 4: variant = data.mObjLabel;	break;
	case 5: variant = data.mPath;		break;
	case 6: variant = data.mID;			break;
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
:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsActArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<MClsNode*>(this->mParent);
	if (parentCls)
	{
		const auto& cls = parentCls->GetStored();

		query = wxString::Format(
			"SELECT t_access_act.id, access_group, access_disabled, script_restrict "
			", act_id, t_act.label "
			", t_access_act.cls_id, t_cls.label "
			", obj_id, t_objnum.obj_label"
			", src_path "
			" FROM t_access_act "
			" LEFT JOIN t_act ON t_act.id = t_access_act.act_id "
			" LEFT JOIN t_cls ON t_cls.id = t_access_act.cls_id "
			" LEFT JOIN t_objnum ON t_objnum.id = t_access_act.obj_id "
			" WHERE t_access_act.cls_id = %s "
			, cls.mID);

		return true;
	}
	return false;

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