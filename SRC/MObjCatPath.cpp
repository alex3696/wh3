#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;



//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MPathItem::MPathItem(const char option)
	:TModelData<T_Data>(option)
{
}

//-------------------------------------------------------------------------
bool MPathItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	if (table->GetColumnCount()>2)
	{
		table->GetAsString(0, row, data.mObj.mID);
		table->GetAsString(1, row, data.mObj.mLabel);
		table->GetAsString(2, row, data.mCls.mID);
		table->GetAsString(3, row, data.mCls.mLabel);
		SetData(data);
		return true;
	}
	else
	{
		table->GetAsString(0, row, data.mCls.mID);
		table->GetAsString(1, row, data.mCls.mLabel);
		SetData(data);
		return true;
	}
	return false;
};





//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MPath::GetSelectChildsQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());

	if (catalog)
	{
		const auto& data = catalog->GetData();
		if(catalog->mCfg->GetData().mObjCatalog)
		{ 
			query = wxString::Format(
				" SELECT _id, _obj_label, _cls_id, _cls_label "
				" FROM fget_objnum_pathinfo_table(%s)"
				, data.mObj.mID
				);
			return true;
		}
		else
		{
			query = wxString::Format(
				" SELECT _id, _label "
				" FROM fget_cls_pathinfo_table(%s)"
				, data.mCls.mID
				);
			return true;
		}
	}
	return false;
}
//-------------------------------------------------------------------------

wxString MPath::GetPathStr()const
{
	auto catalog = dynamic_cast<MObjCatalog*>(GetParent());
	if (!catalog)
		return "**error**";

	wxString str_path = "/";
	unsigned int qty = GetChildQty();
	bool objCatalog = catalog->mCfg->GetData().mObjCatalog;

	for (unsigned int i = qty; i > 0; i--)
	{
		auto node = std::dynamic_pointer_cast<object_catalog::MPathItem>(GetChild(i - 1));

		if (objCatalog)
		{
			str_path += wxString::Format("[%s]%s/"
				, node->GetData().mCls.mLabel
				, node->GetData().mObj.mLabel);
		}
		else
		{
			str_path += wxString::Format("%s/"
				, node->GetData().mCls.mLabel);
		}
	}
	return str_path;
}
