#include "_pch.h"
#include "PathPattern_model.h"

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
void Array::SetArr2Id2Title(const wxString& arrId, const wxString& arrTitle)
{
	Clear();
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
wxString Array::GetArr2Id(bool includeLast)const
{
	// поскольку в базе требуется только путь, без учёта объекта и его класса
	// последний элемент массива не берём в учёт и не отсылаем в SQL

	auto qty = includeLast && 1<GetChildQty() ?
		this->GetChildQty() : this->GetChildQty() - 1;

	wxString str;
	for (size_t i = 0; i < qty; ++i)
	{
		const auto& nd = at(i)->GetData();

		const wxString cls = nd.mCls.mId.IsNull() ? "NULL" : nd.mCls.mId.toStr();
		const wxString obj = nd.mObj.mId.IsNull() ? "NULL" : nd.mObj.mId.toStr();
		str += wxString::Format("{%s,%s},", cls, obj);
	}

	if (!str.IsEmpty())
	{
		str.RemoveLast();
		str = wxString::Format("{%s}", str);;
	}
	else
		str = "{%}";

	return str;
}
//-----------------------------------------------------------------------------
wxString Array::GetArr2Title(bool includeLast)const
{
	// поскольку в базе требуется только путь, без учёта объекта и его класса
	// последний элемент массива не берём в учёт и не отсылаем в SQL

	auto qty = includeLast && 1<GetChildQty() ?
		this->GetChildQty() : this->GetChildQty() - 1;

	wxString str;
	for (size_t i = 0; i < qty; ++i)
	{
		const auto& nd = at(i)->GetData();
		const wxString cls = nd.mCls.mLabel.IsNull() ? "NULL" : nd.mCls.mLabel.toStr();
		const wxString obj = nd.mObj.mLabel.IsNull() ? "NULL" : nd.mObj.mLabel.toStr();
		str += wxString::Format("{%s,%s},", cls, obj);
	}

	if (!str.IsEmpty())
	{
		str.RemoveLast();
		str = wxString::Format("{%s}", str);;
	}
	else
		str = "{%}";

	return str;
}

