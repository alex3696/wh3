#include "_pch.h"
#include "MClsObjNum.h"
#include "MClsTree.h"

using namespace wh;

const std::vector<Field> gClsObjNumFieldVec = {
		{ "Èìÿ", FieldType::ftName, true },
		{ "PID", FieldType::ftInt, true },
		{ "ID", FieldType::ftInt, true }
};



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsObjNum::MClsObjNum(const char option)
:TModelData<rec::ClsObjNum>(option)
{
}

//-------------------------------------------------------------------------
bool MClsObjNum::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjNumArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		const rec::ClsObjNum& oldObjNum = this->GetStored();
		//const rec::ClsObjNum& newObjNum = this->GetData();

		query = wxString::Format(
			"SELECT obj_label, pid, id "
			" FROM t_objnum "
			" WHERE id = %s "
			, oldObjNum.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjNum::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjNumArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		//const rec::ClsObjNum& oldObjNum = this->GetStored();
		const rec::ClsObjNum& newObjNum = this->GetData();

		query = wxString::Format(
			"INSERT INTO t_objnum (cls_id, obj_label, pid ) "
			" VALUES (%s, '%s', %s ) RETURNING id ",
			cls.mID, newObjNum.mLabel, newObjNum.mPID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjNum::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjNumArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("1" != cls.mType)
			return false;
		const rec::ClsObjNum& oldObjNum = this->GetStored();
		const rec::ClsObjNum& newObjNum = this->GetData();

		query = wxString::Format(
			"UPDATE t_objnum SET "
			" obj_label='%s', pid=%s "
			" WHERE id=%s ",
			newObjNum.mLabel, newObjNum.mPID,
			oldObjNum.mID);

		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjNum::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjNumArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsObjNum& oldObjNum = this->GetStored();
		//const rec::ClsObjNum& newObjNum = this->GetData();

		//const rec::Cls& cls = parentCls->GetStored();

		query = wxString::Format(
			"DELETE FROM t_objnum WHERE id=%s "
			, oldObjNum.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjNum::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mLabel);
	table->GetAsString(1, row, data.mPID);
	table->GetAsString(2, row, data.mID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsObjNum::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_icoObj24); break;
	case 2: variant = data.mPID; break;
	case 3: variant = data.mID; break;
	}//switch(col) 
	mgr->FreeInst();
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsObjNum::GetFieldVector()const
{
	return gClsObjNumFieldVec;
}





//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsObjNumArray::MClsObjNumArray(const char option)
:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsObjNumArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<MClsNode*>(this->mParent);
	if (parentCls)
	{
		const auto& cls = parentCls->GetStored();

		query = wxString::Format(
			"SELECT obj_label, pid, id "
			" FROM t_objnum "
			" WHERE cls_id = %s "
			, cls.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsObjNumArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsObjNumArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsObjNum>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsObjNumArray::GetFieldVector()const
{
	return gClsObjNumFieldVec;
}
