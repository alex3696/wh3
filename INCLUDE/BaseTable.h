#ifndef __BASETABLE_H
#define __BASETABLE_H

#include "globaldata.h"

namespace wh{
namespace view{
//using namespace mvc;

void GetStringValue(const wxVariant& var, wxString& retStr );
int CompareStringNumFirst (const wxVariant& var1, const wxVariant& var2, bool ascending);


//---------------------------------------------------------------------------
///  онтрол базовый вид с небольшими дополнени€ми по теме колонок
class BaseDataView
	: public wxDataViewCtrl, public ctrlWithResMgr
{
public:
	struct error:	virtual exception_base {};

	BaseDataView (	wxWindow*		parent, 
					wxWindowID		id=wxID_ANY,
					const wxPoint&	pos=wxDefaultPosition, 
					const wxSize &	size=wxDefaultSize,
					long			style=0);
	virtual	~BaseDataView() ;

	void	EnableAutosizeColumn(unsigned int columnNo);
	void	DisableAutosizeColumn(unsigned int columnNo);
	void	SetMultiselect ( bool enabled=true );

protected:
	std::set<unsigned int>	mAutosizeColumn;
	virtual void			OnResize(wxSizeEvent& evt); // define
};//BaseDataView
	
	
	
	
	
	
	
	
	
	
class InternalBaseTableModel;

//---------------------------------------------------------------------------
///  онтрол табличка
class BaseTable
	: public BaseDataView
{
public:
	BaseTable (	wxWindow*		parent, 
				wxWindowID		id=wxID_ANY,
				const wxPoint&	pos=wxDefaultPosition, 
				const wxSize &	size=wxDefaultSize,
				long			style=0);
	
	wxDataViewItem			GetItem (unsigned int row) const;
	unsigned int			GetRow (const wxDataViewItem &item) const;

	void					SelectRow(unsigned int row);
	
protected:
	friend class			InternalBaseTableModel;
	InternalBaseTableModel*	mDataViewModel;

	unsigned int			GetModelColumnCount() const ;
	unsigned int			GetModelRowCount() const;

	virtual bool			GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const;	// define
	virtual bool			SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col );		// define
	
	virtual wxString		GetColumnType( unsigned int col ) const ;											// define
	virtual void			GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col )=0;			// define

	
};//class BaseTable


//---------------------------------------------------------------------------
class InternalBaseTableModel: public wxDataViewIndexListModel 
{
public:	
	InternalBaseTableModel(BaseTable* view);
	virtual unsigned int	GetColumnCount() const override final;
	virtual wxString		GetColumnType( unsigned int col ) const override final;
	virtual bool			GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const override final;
	virtual bool			SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col ) override final;
	virtual void			GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const override final;
		
	//static void GetStringValue(const wxVariant& var, wxString& retStr );
	virtual int Compare (const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending) const override final;
private:	
	BaseTable*	mView;

};//class IModel




}//namespace view
}//namespace wh
#endif //__BASETABLE_H