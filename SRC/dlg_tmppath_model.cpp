#include "_pch.h"
#include "dlg_tmppath_model.h"

using namespace wh;
using namespace wh::temppath::model;


//-----------------------------------------------------------------------------
// Item
//-----------------------------------------------------------------------------
Item::Item(const char option)
	:TModelData<DataType>(option)
{


}

//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------
Array::Array(const char option)
	: TModelArray<ItemType>(option)
{
}
//-----------------------------------------------------------------------------
void Array::SetTmpPath(const wxString& arrId, const wxString& arrTitle)
{
	wh::ObjKeyPath pathId;
	wh::ObjKeyPath pathTitle;

	if (!pathId.ParseArray(arrId) || !pathTitle.ParseArray(arrTitle)
		|| pathId.size() != pathTitle.size())
		return;

	for (size_t i = 0; i < pathId.size(); ++i)
	{
		std::shared_ptr<Item> item = this->MakeItem();
		rec::PathNode item_data;

		item_data.mCls.mId = pathId[i].m_Type == "NULL" ? wxEmptyString : pathId[i].m_Type;
		item_data.mCls.mLabel = pathTitle[i].m_Type == "NULL" ? wxEmptyString : pathTitle[i].m_Type;

		item_data.mObj.mId = pathId[i].m_Name == "NULL" ? wxEmptyString : pathId[i].m_Name;
		item_data.mObj.mLabel = pathTitle[i].m_Name == "NULL" ? wxEmptyString : pathTitle[i].m_Name;

		item->SetData(item_data);
		AddChild(item);
	}


	

}
//-----------------------------------------------------------------------------
wxString Array::GetTmpPathArr2IdSql()const
{
	wxString str="'{";
	for (size_t i = 0; i < this->GetChildQty(); ++i)
	{
		const auto& nd = at(i)->GetData();
		str += wxString::Format("{%d,%d}", (long)nd.mCls.mId, (long)nd.mObj.mId);
	}

	if (str.size() > 2)
		str[str.size() - 1] = '}';
	else
		str += "}";

	str += "'";

	return str;
}
//-----------------------------------------------------------------------------
wxString Array::GetTmpPath()const
{
	wxString str;
	
	if (this->GetChildQty())
	{
		str.clear();
		for (size_t i = 0; i < this->GetChildQty(); ++i)
		{
			const auto& nd = at(i)->GetData();
			if (nd.mCls.mLabel.IsNull() && nd.mObj.mLabel.IsNull())
				str += "/%";
			else
				str += wxString::Format("/[%s]%s"
					, nd.mCls.mLabel.toStr()
					, nd.mObj.mLabel.toStr() );
		}
			
	}
	else
		str = "/";
	
	return str;

}


