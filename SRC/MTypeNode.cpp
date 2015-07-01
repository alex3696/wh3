#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;

const std::vector<Field> gTypeItemFieldVec =
{
	{ "Тип", FieldType::ftName, true },
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MTypeItem::MTypeItem(const char option)
	:TModelData<rec::Cls>(option)
	, mObjArray(new MObjArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mObjArray));
}




//-------------------------------------------------------------------------
bool MTypeItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mLabel);
	table->GetAsString(1, row, mQty);
	table->GetAsString(2, row, data.mID);
	table->GetAsString(3, row, data.mType);
	table->GetAsString(4, row, data.mMeasure);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MTypeItem::LoadChilds()
{
	//auto type_array = dynamic_cast<MTypeArray*>(this->GetParent());
	//auto catalog = dynamic_cast<MObjCatalog*>(type_array->GetParent());
	if ( "0" != GetData().mType)
		mObjArray->Load();
	
}
//-------------------------------------------------------------------------
wxString MTypeItem::GetQty()const
{
	wxString result;
	/*
	хреновый вариант - точность хромает, проще спросить у БД
	if ("1" != GetData().mClsType)
		result = wxString::Format("%d", mObjArray->GetChildQty());
	if ("2" != GetData().mClsType || "3" != GetData().mClsType)
	{
		double sum = 0.0;
		for (unsigned int i = 0; i < mObjArray->GetChildQty(); ++i)
		{
			auto child = mObjArray->GetChild(i);
			auto obj = std::dynamic_pointer_cast<MObjItem>(child);
			double tmp;
			if (obj->GetData().mQty.ToDouble(&tmp) )
				sum += tmp ;
		}
		result = wxString::Format("%f", sum);
	}
	*/	

	result = wxString::Format("%s", mQty);
	
	return result;
}
//-------------------------------------------------------------------------



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MTypeArray::GetSelectChildsQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (!catalog)
		return false;

	const auto& root = catalog->GetData();

	if (catalog->mCfg->GetData().mObjCatalog)
	{
		query = wxString::Format(
			"SELECT cls_label, COALESCE(SUM(qty),0), cls_id,  type, measurename "
			" FROM w_obj "
			" WHERE obj_pid = %s "
			" GROUP BY cls_label, cls_id, type, measurename "
			, root.mObj.mID );
		return true;
	}
	else
	{
		query = wxString::Format(
			"SELECT label, COALESCE(SUM(w_obj.qty),0), id, t_cls.type, t_cls.measurename "
			" FROM t_cls "
			" LEFT JOIN w_obj ON w_obj.cls_id = t_cls.id "
			" WHERE pid = %s "
			" GROUP BY label "
			, root.mCls.mID );
		return true;

	}


	return false;
}


