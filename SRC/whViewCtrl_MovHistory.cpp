#include "_pch.h"
#include "whViewCtrl_MovHistory.h"

//---------------------------------------------------------------------------
whViewCtrl_MovHistory::whViewCtrl_MovHistory (wxWindow *parent, wxWindowID id, const wxPoint &pos, 
	const wxSize &size, long style, const wxValidator &validator)
	:wxDataViewCtrl(parent,id,pos,size,style,validator),m_QData(NULL)
{
	m_Model = new whModel_MovHistory();
	this->AssociateModel( m_Model);
	m_Model->DecRef();
	SetRowHeight(32);

	this->SetSortingColumnIndex(0);

}
//---------------------------------------------------------------------------
// virtual_destr 
whViewCtrl_MovHistory::~whViewCtrl_MovHistory()
{

}
//---------------------------------------------------------------------------
int whViewCtrl_MovHistory::ConnectDB(whDB *db)
{
	m_Model->SetDB(db);
	return 0;
}
//---------------------------------------------------------------------------
void whViewCtrl_MovHistory::UpdateData()
{
	m_Model->Refresh();
}
//---------------------------------------------------------------------------
void whViewCtrl_MovHistory::UpdateView()
{
	unsigned int new_col_count = m_Model->GetColumnCount();
	this->ClearColumns();
	for(unsigned int i=0;i<new_col_count;i++)
	{
		switch(i)
		{
			
			case 2: AppendTextColumn( m_Model->GetColumnName(i) ,i,wxDATAVIEW_CELL_INERT,120,wxALIGN_LEFT,
						wxDATAVIEW_COL_RESIZABLE|/*wxDATAVIEW_COL_SORTABLE|*/wxDATAVIEW_COL_REORDERABLE);
					break;
			//case 2:	AppendIconTextColumn( m_Model->GetColumnName(i) ,i,wxDATAVIEW_CELL_INERT,100,wxALIGN_CENTER,
			//			wxDATAVIEW_COL_RESIZABLE|/*wxDATAVIEW_COL_SORTABLE|*/wxDATAVIEW_COL_REORDERABLE);
			//		break;
			
			default:AppendTextColumn( m_Model->GetColumnName(i) ,i,wxDATAVIEW_CELL_INERT,110,wxALIGN_LEFT,
						wxDATAVIEW_COL_RESIZABLE|/*wxDATAVIEW_COL_SORTABLE|*/wxDATAVIEW_COL_REORDERABLE);
					break;
		}
	}


}
//---------------------------------------------------------------------------
