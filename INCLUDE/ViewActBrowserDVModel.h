#ifndef __VIEWACTBROWSERDVMODEL_H
#define __VIEWACTBROWSERDVMODEL_H

#include "ModelActTable.h"

namespace wh {
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class wxDVTableActBrowser
	: public wxDataViewModel
{
	std::shared_ptr<const ModelActTable> mTable;

	void GetErrorValue(wxVariant &variant, unsigned int col) const;
	void GetValueInternal(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const;
	void GetActValue(wxVariant &variant, unsigned int col
		, const IAct64& cls) const;

public:
	wxDVTableActBrowser();
	~wxDVTableActBrowser();

	virtual unsigned int	GetColumnCount() const override;
	virtual wxString		GetColumnType(unsigned int col) const override;
	virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const override;

	virtual bool IsContainer(const wxDataViewItem &item)const override;

	virtual void GetValue(wxVariant &variant,
		const wxDataViewItem &dvitem, unsigned int col) const override;
	virtual bool GetAttr(const wxDataViewItem &item, unsigned int col,
		wxDataViewItemAttr &attr) const override;
	virtual bool SetValue(const wxVariant &variant, 
		const wxDataViewItem &item, unsigned int col)override;

	virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2
		, unsigned int column, bool ascending) const override;

	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override;

	virtual unsigned int GetChildren(const wxDataViewItem &parent
		, wxDataViewItemArray &arr) const override;
	virtual bool  IsListModel() const override;

	//
	void SetData(std::shared_ptr<const ModelActTable>& table);
	


};



}//namespace wh{
#endif // __****_H