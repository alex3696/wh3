#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;
using namespace wh::object_catalog::model;



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
		data.mObj.mId = table->GetAsString(0, row);
		data.mObj.mLabel = table->GetAsString(1, row);
		data.mCls.mId = table->GetAsString(2, row );
		data.mCls.mLabel = table->GetAsString(3, row);
		SetData(data);
		return true;
	}
	else
	{
		data.mCls.mId = table->GetAsString(0, row);
		data.mCls.mLabel = table->GetAsString(1, row);
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
	if (!catalog)
		return false;

	const auto& root = catalog->GetRoot();
	if (catalog->IsObjTree())
	{ 
		query = wxString::Format(
			"SELECT oid, otitle, cid, ctitle "
			" FROM get_path_obj_info(%s)"
			, root.mObj.mId.SqlVal()
			);
		return true;
	}

	if (catalog->IsClsTree())
	{
		query = wxString::Format(
			"SELECT id, title "
			" FROM get_path_cls_info(%s)"
			, root.mCls.mId.SqlVal()
			);
		return true;
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

	if (qty > 0 && catalog->GetData().mHideSystemRoot)
		qty -= 1;

	for (unsigned int i = qty; i > 0; i--)
	{
		auto node = std::dynamic_pointer_cast<MPathItem>(GetChild(i - 1));

		if (catalog->IsObjTree())
		{
			str_path += wxString::Format("[%s]%s/"
				, node->GetData().mCls.mLabel.toStr()
				, node->GetData().mObj.mLabel.toStr() );
		}
		else
		{
			str_path += wxString::Format("%s/"
				, node->GetData().mCls.mLabel.toStr());
		}
	}
	return str_path;
}

//-------------------------------------------------------------------------
wxString MPath::GetLastItemStr()const
{
	auto catalog = dynamic_cast<MObjCatalog*>(GetParent());
	if (!catalog)
		return "**error**";
	wxString str_path = "/";
	unsigned int qty = GetChildQty();
	unsigned int skip_root = catalog->GetData().mHideSystemRoot ? 1 : 0;
	if (qty>skip_root)
	{
		auto node = std::dynamic_pointer_cast<MPathItem>(GetChild(0));
		if (catalog->IsObjTree())
		{
			str_path = wxString::Format("../[%s]%s"
					, node->GetData().mCls.mLabel.toStr()
					, node->GetData().mObj.mLabel.toStr() );
		}
		else
		{
			str_path = wxString::Format("../%s"
					, node->GetData().mCls.mLabel.toStr());
		}
	}
	return str_path;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
ClsPathItem::ClsPathItem(const char option)
	:TModelData<T_Data>(option)
{
}
//-------------------------------------------------------------------------
bool ClsPathItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mId = table->GetAsString(0, row);
	data.mLabel= table->GetAsString(1, row );
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool ClsPath::GetSelectChildsQuery(wxString& query)const
{
	auto cls = dynamic_cast<object_catalog::MTypeItem*>(this->GetParent());

	if (cls)
	{
		const auto& cls_data = cls->GetData();
		query = wxString::Format(
				"SELECT id, title "
				" FROM get_path_cls_info(%s)"
				, cls_data.mId.SqlVal()
				);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------
wxString ClsPath::AsString()const
{

	wxString str_path = "/";
	unsigned int qty = GetChildQty();

	for (unsigned int i = qty; i > 0; i--)
	{
		auto node = std::dynamic_pointer_cast<ClsPathItem>(GetChild(i - 1));
		str_path += wxString::Format("%s/", node->GetData().mLabel.toStr() );
	}
	return str_path;
}




//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
ObjPathItem::ObjPathItem(const char option)
	:TModelData<T_Data>(option)
{
}

//-------------------------------------------------------------------------
bool ObjPathItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mObj.mId = table->GetAsString(0, row);
	data.mObj.mLabel = table->GetAsString(1, row);
	data.mCls.mId = table->GetAsString(2, row);
	data.mCls.mLabel = table->GetAsString(3, row);
	SetData(data);
	return true;
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool ObjPath::GetSelectChildsQuery(wxString& query)const
{
	auto obj = dynamic_cast<object_catalog::MObjItem*>(this->GetParent());

	if (obj)
	{
		const auto& obj_data = obj->GetData();
		query = wxString::Format(
				"SELECT oid, otitle, cid, ctitle "
				" FROM get_path_obj_info(%s)"
				, obj_data.mParent.mId.IsNull() ? "0" : obj_data.mParent.mId.toStr()
				);
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------

wxString ObjPath::AsString()const
{
	wxString str_path = "/";
	unsigned int qty = GetChildQty();

	for (unsigned int i = qty; i > 0; i--)
	{
		auto node = std::dynamic_pointer_cast<ObjPathItem>(GetChild(i - 1));
		str_path += wxString::Format("[%s]%s/"
			, node->GetData().mCls.mLabel.toStr()
			, node->GetData().mObj.mLabel.toStr() );

	}
	return str_path;
}
