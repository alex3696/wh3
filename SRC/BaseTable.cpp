#include "_pch.h"
#include "BaseTable.h"


using namespace wh;
using namespace wh::view;


//---------------------------------------------------------------------------
void wh::view::GetStringValue(const wxVariant& var, wxString& retStr )
{
	wxString type=var.GetType();
	if(L"wxDataViewIconText"==type)
		retStr=var.GetAny().As<wxDataViewIconText>().GetText();
	else if(L"string"==type)
		retStr=var.GetString();
}
//---------------------------------------------------------------------------
int wh::view::CompareStringNumFirst (const wxVariant& var1, const wxVariant& var2, bool ascending)
{
	wxString str1,str2;
	wh::view::GetStringValue(var1, str1);
	wh::view::GetStringValue(var2, str2);
	
	double	value1, value2;
	//bool	isNumberVal1 = str1.ToDouble( &value1 );
	//bool	isNumberVal2 = str2.ToDouble( &value2 );
	char isNum2 = 0;
	if (str1.ToDouble(&value1))
		isNum2++;
	if (str2.ToDouble(&value2))
		isNum2+=10;

	//if(	isNumberVal1 && isNumberVal2 )
	//	return ascending ? 	value1>value2 : value1<value2 ;
	//if ( isNumberVal1 && !isNumberVal2 )
	//	return ascending ? -1 : 1 ;
	//if (!isNumberVal1 && isNumberVal2 )
	//	return ascending ? 1 : -1 ;
	switch (isNum2)
	{
		default:	break;
		case 01:	return ascending ? -1 : 1;
		case 10:	return ascending ? 1 : -1;
		case 11:	if (value1 == value2)
						return 0;
					return ascending ? value1>value2 : value1<value2;
	}
	return ascending ? str1.CmpNoCase(str2) : str2.CmpNoCase(str1);
}






//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

BaseDataView::BaseDataView (	wxWindow*		parent,
								wxWindowID		id,
								const wxPoint&	pos, 
								const wxSize &	size,
								long			style)
	: wxDataViewCtrl(parent,id,pos,size,style) 
{
	Bind(wxEVT_SIZE,&BaseDataView::OnResize,this );
}
//---------------------------------------------------------------------------
BaseDataView::~BaseDataView() 
{
	Unbind(wxEVT_SIZE,&BaseDataView::OnResize,this );
}
//---------------------------------------------------------------------------
void BaseDataView::EnableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.insert(columnNo);
}
//---------------------------------------------------------------------------
void BaseDataView::DisableAutosizeColumn(unsigned int columnNo)
{
	mAutosizeColumn.erase(columnNo);
}
//---------------------------------------------------------------------------
void BaseDataView::SetMultiselect ( bool enabled )
{
	long currentFlags = this->GetWindowStyleFlag();
	currentFlags = enabled ? currentFlags | wxDV_MULTIPLE : currentFlags & ~wxDV_MULTIPLE;
	this->SetWindowStyleFlag( currentFlags );
}
//---------------------------------------------------------------------------
void BaseDataView::OnResize(wxSizeEvent& evt) 
{
	if ( !mAutosizeColumn.empty() ) 
	{
		wxWindowUpdateLocker	wndDisabler(this);
		int sum =0,asum =0;
		for(unsigned int i=0;i<GetColumnCount();i++)
			if(mAutosizeColumn.end()==mAutosizeColumn.find(i) )
				sum+= GetColumn(i)->IsHidden() ? 0 : GetColumn(i)->GetWidth();
			else
				asum=wxDVC_DEFAULT_WIDTH;
		
		int ctrl_width=this->GetClientSize().GetWidth();
		int auto_width = (ctrl_width > (sum+asum)) ? ((ctrl_width-sum)/mAutosizeColumn.size()) :wxDVC_DEFAULT_WIDTH;
			
		for(auto it=mAutosizeColumn.begin();it!=mAutosizeColumn.end();++it) {
			if( *it < GetColumnCount() )
				GetColumn(*it)->SetWidth(auto_width);
		}
	}//if ( mAutosizeColumn.size() )
	evt.Skip(); 	
}//virtual void OnResize






//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//BaseTable
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


BaseTable::BaseTable (	wxWindow*		parent,
						wxWindowID		id,
						const wxPoint&	pos, 
						const wxSize &	size,
						long			style)
	//: wxDataViewCtrl(parent,id,pos,size,style|wxDV_ROW_LINES|wxDV_VERT_RULES) 
	: BaseDataView(parent,id,pos,size,style|wxDV_ROW_LINES|wxDV_VERT_RULES) 
{
	mDataViewModel  = new InternalBaseTableModel(this);
	wxDataViewCtrl::AssociateModel(mDataViewModel);
	mDataViewModel->DecRef();
	//Bind(wxEVT_SIZE,&BaseTable::OnResize,this );
}
//---------------------------------------------------------------------------

wxDataViewItem	BaseTable::GetItem (unsigned int row) const 
{ 
	return mDataViewModel->GetItem(row); 
}
//---------------------------------------------------------------------------

unsigned int BaseTable::GetRow (const wxDataViewItem &item) const 
{
	if(!item.IsOk())
		BOOST_THROW_EXCEPTION( error()<<wxstr("wrong dataview item ") );
	return mDataViewModel->GetRow(item); 
}
//---------------------------------------------------------------------------

void	BaseTable::SelectRow(unsigned int row) 
{ 
	Select(GetItem(row)); 
}
//---------------------------------------------------------------------------
unsigned int BaseTable::GetModelColumnCount() const 
{
	return GetColumnCount();
}
//---------------------------------------------------------------------------
unsigned int BaseTable::GetModelRowCount() const
{
	return mDataViewModel->GetCount();
}
//---------------------------------------------------------------------------
bool BaseTable::GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const 
{
	/*
	wxColour light(255,255,255);
	wxColour dark(240,240,240);
	attr.SetBackgroundColour( row%2 ? light : dark );
	return true;
	*/
	return false;
}
//---------------------------------------------------------------------------
bool BaseTable::SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col )
{
	return false;
}
//---------------------------------------------------------------------------
wxString BaseTable::GetColumnType( unsigned int col ) const
{
	return L"string";
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// InternalBaseTableModel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
InternalBaseTableModel::InternalBaseTableModel(BaseTable* view)
	:mView(view)
{}
//---------------------------------------------------------------------------

unsigned int InternalBaseTableModel::GetColumnCount() const 
{
	return mView->GetModelColumnCount();	
}
//---------------------------------------------------------------------------

wxString	InternalBaseTableModel::GetColumnType( unsigned int col ) const
{ 
	return mView->GetColumnType(col);		
}
//---------------------------------------------------------------------------

bool	InternalBaseTableModel::GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr )const
{
	return mView->GetAttrByRow(row,col,attr);
}
//---------------------------------------------------------------------------

bool InternalBaseTableModel::SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col )
{ 	
	return mView->SetValueByRow(variant,row,col); 
}
//---------------------------------------------------------------------------

void InternalBaseTableModel::GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const
{
	return mView->GetValueByRow(variant,row,col); 
}
//---------------------------------------------------------------------------

int InternalBaseTableModel::Compare (const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column, bool ascending) const
{
	wxVariant var1,var2;
	mView->GetValueByRow(var1,GetRow(item1),column);
	mView->GetValueByRow(var2,GetRow(item2),column);
	return wh::view::CompareStringNumFirst(var1,var2,ascending);
}



