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

	bool reverse_order=true;

	if (   !pathId.ParseArray(arrId, reverse_order) 
		|| !pathTitle.ParseArray(arrTitle, reverse_order)
		|| pathId.size() != pathTitle.size())
		return;

	for (size_t i = 0; i < pathId.size(); ++i)
	{
		std::shared_ptr<Item> item = this->MakeItem();
		rec::PathNode item_data;

		if (pathId[i].m_Type != "%" && pathId[i].m_Type != "NULL")
			item_data.mCls.mId = pathId[i].m_Type;
		if (pathTitle[i].m_Type != "%" && pathTitle[i].m_Type != "NULL")
			item_data.mCls.mLabel = pathTitle[i].m_Type;
		if (pathId[i].m_Name != "%" && pathId[i].m_Name != "NULL")
			item_data.mObj.mId = pathId[i].m_Name;
		if (pathTitle[i].m_Name != "%" && pathTitle[i].m_Name != "NULL")
			item_data.mObj.mLabel = pathTitle[i].m_Name;

		item->SetData(item_data);
		Insert(item);
	}


	

}
//-----------------------------------------------------------------------------
wxString Array::GetArr2Id(bool includeLast, bool reverse )const
{
	// поскольку в базе требуется только путь, без учёта объекта и его класса
	// последний элемент массива не берём в учёт и не отсылаем в SQL

	auto qty = includeLast && 1<GetChildQty() ?
		this->GetChildQty() : this->GetChildQty() - 1;

	wxString str;

	auto process_item = [this, &str](size_t i)
	{
		const auto& nd = at(i)->GetData();

		const wxString cls = nd.mCls.mId.IsNull() ? "%" : nd.mCls.mId.toStr();
		const wxString obj = nd.mObj.mId.IsNull() ? "%" : nd.mObj.mId.toStr();

		if (nd.mCls.mId.IsNull() && nd.mObj.mId.IsNull())
			str += "%,";
		else
			str += wxString::Format("{%s,%s},", cls, obj);
	};
		
	if (qty)
		if (reverse)
			while (qty)
				process_item(--qty);
		else
			for (size_t i = 0; i < qty; ++i)
				process_item(i);
		
	if (!str.IsEmpty())
	{
		str.RemoveLast();
		str = wxString::Format("{%s}", str);;
	}
	else
		str = "{}";

	return str;
}
//-----------------------------------------------------------------------------
wxString Array::GetArr2Title(bool includeLast, bool reverse)const
{
	// поскольку в базе требуется только путь, без учёта объекта и его класса
	// последний элемент массива не берём в учёт и не отсылаем в SQL

	auto qty = includeLast && 1<GetChildQty() ?
		this->GetChildQty() : this->GetChildQty() - 1;

	wxString str;
	auto process_item = [this, &str](size_t i)
	{
		const auto& nd = at(i)->GetData();
		const wxString cls = nd.mCls.mLabel.IsNull() ? "%" : nd.mCls.mLabel.toStr();
		const wxString obj = nd.mObj.mLabel.IsNull() ? "%" : nd.mObj.mLabel.toStr();

		if (nd.mCls.mId.IsNull() && nd.mObj.mId.IsNull())
			str += "%,";
		else
			str += wxString::Format("{%s,%s},", cls, obj);
	};

	if (qty)
		if (reverse)
			while (qty)
				process_item(--qty);
		else
			for (size_t i = 0; i < qty; ++i)
				process_item(i);


	if (!str.IsEmpty())
	{
		str.RemoveLast();
		str = wxString::Format("{%s}", str);;
	}
	else
		str = "{}";

	return str;
}
//-----------------------------------------------------------------------------
std::shared_ptr<Item>	Array::GetLast()const
{
	std::shared_ptr<Item> item = this->CreateItem();
	if (!GetChildQty())
		return item;
	return this->at(GetChildQty() - 1);
}
//-----------------------------------------------------------------------------
bool Array::GetPath(wxString& path, bool reverse)const
{
	wxString str;
	auto process_item = [this, &str, &path](size_t i)
	{
		const auto& item = at(i)->GetData();

		if (item.mCls.mLabel.IsNull() && item.mObj.mLabel.IsNull())
			path += "/*";
		else
		{
			const wxString cls = item.mCls.mLabel.IsNull() ? "*" : item.mCls.mLabel.toStr();
			const wxString obj = item.mObj.mLabel.IsNull() ? "*" : item.mObj.mLabel.toStr();
			path += wxString::Format("/[%s]%s", cls, obj);
		}
	};
	
	auto qty = 	this->GetChildQty();

	if (qty)
	{
		if (reverse)
			while (qty)
				process_item(--qty);
		else
			for (size_t i = 0; i < qty; ++i)
				process_item(i);
		return true;
	}
	return false;
}