#include "_pch.h"
#include "MClsObjQty.h"
#include "MClsTree.h"

using namespace wh;

const std::vector<Field> gClsObjQtyFieldVec = {
		{ "Имя", FieldType::ftName, true },
		{ "Количество", FieldType::ftInt, true },
		{ "PID", FieldType::ftInt, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsObjQty::MClsObjQty(const char option)
:TModelData<rec::ClsObjQty>(option)
{
}

//-------------------------------------------------------------------------
bool MClsObjQty::GetSelectQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjQtyArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("2" != cls.mType)
			return false;
		const rec::ClsObjQty& oldObjQty = this->GetStored();
		//const rec::ClsObjQty& newObjQty = this->GetData();

		query = wxString::Format(
			"SELECT obj_label, qty, pid "
			" FROM t_objqty "
			" WHERE pid = %s AND obj_label='%' "
			, oldObjQty.mPID, cls.mLabel);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjQty::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjQtyArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("2" != cls.mType && "3" != cls.mType)
			return false;
		//const rec::ClsObjQty& oldObjQty = this->GetStored();
		const rec::ClsObjQty& newObjQty = this->GetData();

		wxString pid = (newObjQty.mPID.IsEmpty()) ? "0" : newObjQty.mPID;

		query = wxString::Format(
			" SELECT fn_insert_objqty(%s, '%s', %s, %s) "
			, cls.mID
			, newObjQty.mLabel
			, pid
			, newObjQty.mQty);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjQty::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjQtyArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::Cls& cls = parentCls->GetStored();
		if ("2" != cls.mType)
			return false;
		const rec::ClsObjQty& oldObjQty = this->GetStored();
		const rec::ClsObjQty& newObjQty = this->GetData();

		query = wxString::Format(
			" SELECT fn_update_objqty(%s, %s, '%s', %s, %s) "
			, oldObjQty.mID, oldObjQty.mPID
			, newObjQty.mLabel, newObjQty.mPID, newObjQty.mQty
			);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjQty::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsObjQtyArray*>(this->mParent);
	auto parentCls = dynamic_cast<MClsNode*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsObjQty& oldObjQty = this->GetStored();
		//const rec::ClsObjQty& newObjQty = this->GetData();
		//const rec::Cls& cls = parentCls->GetStored();

		query = wxString::Format(
			"SELECT fn_delete_objqty(%s, %s)"
			, oldObjQty.mID, oldObjQty.mPID
			);

		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsObjQty::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mLabel);
	table->GetAsString(1, row, data.mQty);
	table->GetAsString(2, row, data.mPID);
	table->GetAsString(3, row, data.mID);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsObjQty::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default: break;
	case 1: variant = variant << wxDataViewIconText(data.mLabel, mgr->m_icoObj24); break;
	case 2: variant = data.mQty; break;
	case 3: variant = data.mPID; break;
	}//switch(col) 
	mgr->FreeInst();
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsObjQty::GetFieldVector()const
{
	return gClsObjQtyFieldVec;
}





//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsObjQtyArray::MClsObjQtyArray(const char option)
:IModel(option)
{
}
//-------------------------------------------------------------------------
bool MClsObjQtyArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<MClsNode*>(this->mParent);
	if (parentCls)
	{
		const auto& cls = parentCls->GetStored();

		query = wxString::Format(
			"SELECT obj_label, qty, pid, objqty_id "
			" FROM t_objqty "
			" LEFT JOIN t_objqtykey ON t_objqtykey.id = t_objqty.objqty_id "
			" WHERE cls_id = %s "
			, cls.mID);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsObjQtyArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsObjQtyArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsObjQty>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};
//-------------------------------------------------------------------------
const std::vector<Field>& MClsObjQtyArray::GetFieldVector()const
{
	return gClsObjQtyFieldVec;
}

