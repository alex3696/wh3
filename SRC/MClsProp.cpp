#include "_pch.h"
#include "MClsProp.h"
#include "MObjCatalog.h"

using namespace wh;


const std::vector<Field> gClsPropFieldVec = {
		{ "Имя", FieldType::ftName, true },
		{ "Значение", FieldType::ftText, true },
		{ "Тип", FieldType::ftName, true },
		{ "PropID", FieldType::ftLong, true },
		{ "ID", FieldType::ftLong, true }
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsProp::MClsProp(const char option)
:TModelData<rec::ClsProp>(option)
{
}

//-------------------------------------------------------------------------
bool MClsProp::GetSelectQuery(wxString& query)const
{
	auto clsProp = GetData();
	query = wxString::Format(
		"SELECT prop.id AS prop_id "
		" , prop.kind AS prop_type "
		" , prop.title AS prop_label "
		" , val "
		" , prop_cls.id AS id "
		" FROM prop_cls "
		" LEFT JOIN prop ON prop.id = prop_cls.prop_id "
		" WHERE prop_cls.id = %s "
		, clsProp.mId.SqlVal());
	return true;
}
//-------------------------------------------------------------------------
bool MClsProp::GetInsertQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(GetParent());
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		const rec::Cls& cls = parentCls->GetData();
		
		query = wxString::Format(
			"WITH ins AS("
			"  INSERT INTO prop_cls(val, cls_id, cls_kind, prop_id)"
			"    VALUES(%s, %s, %d, %s)  RETURNING id, val, prop_id)"
			"SELECT prop.id, prop.title, prop.kind, ins.val, ins.id FROM ins "
			"  LEFT JOIN prop ON ins.prop_id = prop.id "
			, prop.mVal.SqlVal()
			, cls.mId.SqlVal()
			, (int)cls.GetClsType()
			, prop.mProp.mId.SqlVal() );
		return true;
	}
	return false;




}
//-------------------------------------------------------------------------
bool MClsProp::GetUpdateQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(this->mParent);
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		const rec::Cls& cls = parentCls->GetData();

		query = wxString::Format(
			"UPDATE	prop_cls "
			" SET val=%s, cls_id=%s, prop_id=%s "
			" WHERE id=%s "
			, prop.mVal.SqlVal()
			, cls.mId.SqlVal()
			, prop.mProp.mId.SqlVal()
			, prop.mId.SqlVal() );
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsProp::GetDeleteQuery(wxString& query)const
{
	auto parentArray = dynamic_cast<MClsPropArray*>(GetParent());
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(parentArray->GetParent());

	if (parentCls)
	{
		const rec::ClsProp& prop = this->GetData();
		//const rec::Cls& cls = parentCls->GetData();

		query = wxString::Format(
			"DELETE FROM prop_cls WHERE id = %s ",
			prop.mId.SqlVal() );
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
bool MClsProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mProp.mId = table->GetAsLong(0, row);
	data.mProp.mLabel = table->GetAsString(1, row);
	data.mProp.mType = ToFieldType(table->GetAsString(2, row));
	data.mVal = table->GetAsString(3, row);
	data.mId = table->GetAsLong(4, row);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
bool MClsProp::GetFieldValue(unsigned int col, wxVariant &variant)
{
	const auto& data = this->GetData();
	auto mgr = ResMgr::GetInstance();
	switch (col)
	{
	default:	break;
	case 1:
		variant = variant << wxDataViewIconText(data.mProp.mLabel, mgr->m_ico_classprop24);
		break;
	case 2:	variant = data.mVal;					break;
	case 3: variant = ToText(data.mProp.mType);		break;
	case 4: variant = data.mProp.mId.toStr();		break;
	case 5: variant = data.mId.toStr();				break;
	}//switch(col) 
	return true;
}
//-------------------------------------------------------------------------
const std::vector<Field>& MClsProp::GetFieldVector()const
{
	return gClsPropFieldVec;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MClsPropArray::MClsPropArray(const char option)
	:TModelArray<T_Item>(option)
{
}
//-------------------------------------------------------------------------
bool MClsPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<object_catalog::MTypeItem*>(GetParent());
	if (parentCls)
	{
		auto cls = parentCls->GetData();

		query = wxString::Format(
			"SELECT prop.id AS prop_id "
			" , prop.title AS prop_label "
			" , prop.kind AS prop_type "
			" , val "
			" , prop_cls.id AS id "
			" FROM prop_cls "
			" LEFT JOIN prop ON prop.id = prop_cls.prop_id "
			" WHERE prop_cls.cls_id = %s "
			, cls.mId.SqlVal());
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<IModel> MClsPropArray::CreateChild()
{
	auto child = new T_Item;
	child->SetData(T_Item::T_Data());
	return std::shared_ptr<IModel>(child);
}

//-------------------------------------------------------------------------
bool MClsPropArray::LoadChildDataFromDb(std::shared_ptr<IModel>& model,
	std::shared_ptr<whTable>& table, const size_t pos)
{
	auto child = std::dynamic_pointer_cast<MClsProp>(model);
	if (child)
		return child->LoadThisDataFromDb(table, pos);
	return false;
};

//-------------------------------------------------------------------------
const std::vector<Field>& MClsPropArray::GetFieldVector()const
{
	return gClsPropFieldVec;
}

