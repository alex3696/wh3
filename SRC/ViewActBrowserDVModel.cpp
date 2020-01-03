#include "_pch.h"
#include "ViewActBrowserDVModel.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
wxDVTableActBrowser::wxDVTableActBrowser() 
{
}
//-----------------------------------------------------------------------------
wxDVTableActBrowser::~wxDVTableActBrowser()
{
}
//-----------------------------------------------------------------------------
void wxDVTableActBrowser::GetErrorValue(wxVariant &variant
	, unsigned int col) const
{
	variant = "*ERROR*";
	return;
}
//-----------------------------------------------------------------------------
void wxDVTableActBrowser::GetValueInternal(wxVariant &variant,
	const wxDataViewItem &dvitem, unsigned int col) const
{
	const auto node = static_cast<const IAct64*> (dvitem.GetID());
	if (node)
	{
		GetActValue(variant, col, *node);
		return;
	}
	GetErrorValue(variant, col);
}
//-----------------------------------------------------------------------------
void wxDVTableActBrowser::GetActValue(wxVariant &variant
	, unsigned int col , const IAct64& act) const
{
	switch (col)
	{
	case 0: variant = act.GetTitle(); break;
	case 1: variant = act.GetNote(); break;
	case 2: variant = act.GetColour(); break;
	case 3: variant = act.GetIdAsString(); break;
	default:break;
	}
}
//-----------------------------------------------------------------------------
//virtual 
unsigned int wxDVTableActBrowser::GetColumnCount() const
{
	return 2;
}
//-----------------------------------------------------------------------------
//virtual 
wxString wxDVTableActBrowser::GetColumnType(unsigned int col) const
{
	return "string";
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableActBrowser::HasContainerColumns(const wxDataViewItem& item)const
{
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableActBrowser::IsContainer(const wxDataViewItem &item)const
{
	if (!item.IsOk())
		return true;
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
void wxDVTableActBrowser::GetValue(wxVariant &variant
	, const wxDataViewItem &dvitem, unsigned int col) const
{
	GetValueInternal(variant, dvitem, col);
	if (variant.IsNull())
		variant = "";
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableActBrowser::GetAttr(const wxDataViewItem &item
	, unsigned int col,	wxDataViewItemAttr &attr) const
{
	if (!item.IsOk())
		return false;
	const auto node = static_cast<const IAct64*> (item.GetID());
	if (node)
	{
		/*
		switch (col)
		{
		case 0: attr.SetBold(true); break;
		default: break;
		}
		*/
		wxColour colour = node->GetColour();
		attr.SetBackgroundColour(colour);
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
bool wxDVTableActBrowser::SetValue(const wxVariant &variant
	, const wxDataViewItem &item, unsigned int col)
{
	return false;
}
//-----------------------------------------------------------------------------
//virtual 
int wxDVTableActBrowser::Compare(const wxDataViewItem &item1
	, const wxDataViewItem &item2, unsigned int column, bool ascending) const
{
	return wxDataViewModel::Compare(item1, item2, column, ascending);
}
//-----------------------------------------------------------------------------
//virtual 
wxDataViewItem wxDVTableActBrowser::GetParent(const wxDataViewItem &item) const
{
	return wxDataViewItem(nullptr);
}
//-----------------------------------------------------------------------------
//virtual 
unsigned int wxDVTableActBrowser::GetChildren(const wxDataViewItem &parent
	, wxDataViewItemArray &arr) const
{
	if (parent.IsOk())
		return 0;
	if(!mTable)
		return 0;

	typedef ModelActTable::Storage::nth_index<1>::type Idx1Type;
	typedef ModelActTable::Storage::nth_index<1>::type::const_iterator IdxIter;
	
	const ModelActTable::Storage& st = mTable->GetStorage();
	const Idx1Type& idxRnd = st.get<1>();

	for (const auto& act : idxRnd)
	{
		wxDataViewItem dvitem((void*)act.get());
		arr.push_back(dvitem);
	}
	return arr.size();

}
//-----------------------------------------------------------------------------
//virtual 
bool  wxDVTableActBrowser::IsListModel() const
{
	return true;
}
//-----------------------------------------------------------------------------
void wxDVTableActBrowser::SetData(std::shared_ptr<const ModelActTable>& table)
{
	auto old_table = mTable;
	mTable = table;
	Cleared();
}
