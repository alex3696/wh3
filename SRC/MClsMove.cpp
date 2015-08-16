#include "_pch.h"
#include "MClsMove.h"
#include "MObjCatalog.h"

using namespace wh;

const std::vector<Field> gClsMoveFieldVec = {
		{ "Доступ", FieldType::ftName, true },
		{ "Группа", FieldType::ftName, true },
		{ "Объект", FieldType::ftName, true },
		{ "SrcPath", FieldType::ftText, true },
		{ "DstPath", FieldType::ftText, true },
		{ "ID", FieldType::ftInt, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsMove::MClsMove(const char option)
:TModelData<rec::ClsSlotAccess>(option)
{
}

//-------------------------------------------------------------------------
bool MClsMove::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("0" == cls.mType)
			return false;
		//const rec::ClsSlotAccess& oldClsSlot = this->GetStored();
		const rec::ClsSlotAccess& newClsSlot = this->GetData();

		query = wxString::Format(
			"SELECT id, access_group, access_disabled, script_restrict, "
			" dst_cls_id, dst_obj_id, dst_path, "
			" mov_cls_id, mov_obj_id, src_path "
			" FROM t_access_slot "
			" WHERE id = %s ",
			newClsSlot.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsMove::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("0" == cls.mType)
			return false;
		//const rec::ClsSlotAccess& oldClsSlot = this->GetStored();
		const rec::ClsSlotAccess& newClsSlot = this->GetData();

		query = wxString::Format(
			"INSERT INTO t_access_slot "
			"(access_group, access_disabled, script_restrict, "
			" dst_cls_id, dst_obj_id, dst_path, "
			" mov_cls_id, mov_obj_id, src_path "
			") VALUES( '%s', %s, '%s', "
			" '%s', '%s', '%s', "
			" '%s', '%s', '%s' ) "
			" RETURNING id "
			, newClsSlot.mAcessGroup
			, newClsSlot.mAccessDisabled
			, newClsSlot.mScriptRestrict
			, cls.mID
			, newClsSlot.mDstObj
			, newClsSlot.mDstPath
			, newClsSlot.mMovCls
			, newClsSlot.mMovObj
			, newClsSlot.mSrcPath);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsMove::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("0" == cls.mType)
			return false;
		const rec::ClsSlotAccess& oldClsSlot = this->GetStored();
		const rec::ClsSlotAccess& newClsSlot = this->GetData();

		query = wxString::Format(
			"UPDATE	t_access_slot "
			" SET access_group='%s', access_disabled=%s, script_restrict='%s' "
			" dst_cls_id=%s, dst_obj_id=%s, dst_path=%s "
			" mov_cls_id=%s, mov_obj_id=%s, src_path=%s "
			" WHERE id=%s "
			, newClsSlot.mAcessGroup
			, newClsSlot.mAccessDisabled
			, newClsSlot.mScriptRestrict
			, newClsSlot.mDstCls
			, newClsSlot.mDstObj
			, newClsSlot.mDstPath
			, newClsSlot.mMovCls
			, newClsSlot.mMovObj
			, newClsSlot.mSrcPath
			, oldClsSlot.mID);

		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsMove::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsMoveArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		//const rec::Cls& cls = parentCls->GetStored();

		const rec::ClsSlotAccess& oldClsSlot = this->GetStored();

		query = wxString::Format(
			"DELETE FROM t_access_slot WHERE id = %s ",
			oldClsSlot.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsMove::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mID);
	table->GetAsString(1, row, data.mAcessGroup);
	table->GetAsString(2, row, data.mAccessDisabled);
	table->GetAsString(3, row, data.mScriptRestrict);
	
	table->GetAsString(4, row, data.mDstCls);
	table->GetAsString(5, row, data.mDstObj);
	table->GetAsString(6, row, data.mDstPath);
	
	table->GetAsString(7, row, data.mMovCls);
	table->GetAsString(8, row, data.mMovObj);
	table->GetAsString(9, row, data.mSrcPath);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsMove::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:
		variant = variant << wxDataViewIconText(
			("1" == data.mAccessDisabled) ? "Запретить" : "Разрешить", mgr->m_ico_accept24);
		break;
	case 2:	variant = data.mAcessGroup;						break;
	case 3: variant = wxString::Format("[%s]%s", data.mMovCls, data.mMovObj);		break;
	case 4: variant = data.mSrcPath;							break;
	case 5: variant = data.mDstPath + "/[THIS]" + data.mDstObj;	break;
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
	if (parentCls)
	{
		const auto& cls = parentCls->GetStored();

		query = wxString::Format(
			"SELECT id, access_group, access_disabled, script_restrict, "
			" dst_cls_id, dst_obj_id, dst_path, "
			" mov_cls_id, mov_obj_id, src_path "
			" FROM t_access_slot "
			" WHERE dst_cls_id = '%s' "
			, cls.mID);
			
		return true;
	}
	return false;
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
