#include "_pch.h"
#include "whModel_MovHistory.h"

//---------------------------------------------------------------------------
whModel_MovHistory::whModel_MovHistory()
	:wxDataViewVirtualListModel(), m_QData(NULL), m_Table(new whTable), m_ViewCompact(true)
{
	
}
//---------------------------------------------------------------------------
whModel_MovHistory::~whModel_MovHistory()
{
	delete m_Table;
}
//---------------------------------------------------------------------------
// implementation of base class virtuals to define model
//virtual 
unsigned int whModel_MovHistory::GetColumnCount() const
{
	return m_ViewCompact ? 6 : 10 ;
/*	
	if(m_QData)
		return m_QData->m_field_table.size();
	return 6;
*/
}
//---------------------------------------------------------------------------
//virtual ?????? не вызывается никогда ?????? 
wxString whModel_MovHistory::GetColumnType( unsigned int col ) const
{
    //if(col==1)
	//	 return wxT("wxDataViewIconText");
	return wxT("string");
}
//---------------------------------------------------------------------------
unsigned int whModel_MovHistory::GetRowCount()const
{
	return m_Table->GetRowCount();
}
//---------------------------------------------------------------------------
//virtual 
void whModel_MovHistory::GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const
{
	if(m_ViewCompact)
	{
		wxString str;
		wxDateTime dt;
		wh::ObjKeyPath path;
		switch(col)
		{
			default:	break;
			case 0:		dt=m_Table->GetAsDate("movlogtime",row);
						str=dt.Format("%d %B %Y\n%X");
						//str+=wxString::Format(" [%s]",m_Table->GetAsString("movlogid",row) );
						variant = str;
						break;
			case 1:		str=m_Table->GetAsString("rolname",row);
						variant =str;
						break;

			case 2:		str=wxString::Format("%s\n%s",
												m_Table->GetAsString("classname",row),
												//m_Table->GetAsString("classtype",row),
												m_Table->GetAsString("objname",row) );
						variant = str;	
						break;
			case 3:		str= m_Table->GetAsString("qty",row);
						variant = str;	
						break;
			case 4:		str= m_Table->GetAsString("movlogcomment",row);
						variant = str;	
						break;
			case 5:		path.ParseArray( m_Table->GetAsString("src_path",row) );
						str=path.GetPathString();
						path.clear();
						path.ParseArray( m_Table->GetAsString("dst_path",row) );
						variant = str+"\n"+path.GetPathString();
						break;
		}; //switch(col)

	

	}//if(m_ViewCompact)
	else
	{
		wxString str;
		wxDateTime dt;
		wh::ObjKeyPath path;
		switch(col)
		{
			default:variant = m_Table->GetAsString(col,row);;	
					break;

			case 1:	dt=m_Table->GetAsDate("movlogtime",row);
					str=dt.Format("%d %B %Y\n%X");
					variant = str;
					break;

			case 8:	path.ParseArray( m_Table->GetAsString("src_path",row) ) ;
					variant = path.GetPathString();
					break;
			case 9:	path.ParseArray( m_Table->GetAsString("dst_path",row) ) ;
					variant = path.GetPathString();
					break;

			
			
		}//switch
		
	
	}
	

}
//---------------------------------------------------------------------------
//virtual 
bool whModel_MovHistory::GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const
{
	if ( row % 2 )
		attr.SetBackgroundColour(*wxWHITE );
	else
		attr.SetBackgroundColour(wxColour(240,245,250));
	return true;
}
//---------------------------------------------------------------------------
//virtual 
bool whModel_MovHistory::SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col )
{
	return false;
}
//---------------------------------------------------------------------------
wxString whModel_MovHistory::GetColumnName(unsigned int nCol)
{
	if(m_ViewCompact)
	{
		switch(nCol)
		{
			case 0: return "Дата/Время";//[ID]
			case 1: return "Пользователь";
			case 2: return "Тип/Имя ";//[подтип]
			case 3: return "количество";
			case 4: return "комментарий";
			case 5: return "источник/приёмник";
			default:break;
		}
	}
	
	if(m_QData && nCol<m_QData->m_field_table.size() )
		return m_QData->m_field_table[nCol].m_UserLabel;
	return m_Table->GetColumnName(nCol); 
}
//---------------------------------------------------------------------------
int	whModel_MovHistory::Refresh()
{
	wxString table="wh_movlog";
	
	wxString fields=m_QData->GetSQL_Fields();
	
	wxString filter=m_QData->GetSQL_Filters();

	wxString sorter=m_QData->GetSQL_Order();

	wxString query=	wxString::Format("SELECT %s FROM %s %s %s",
										fields,table,filter,sorter);

	//wxString query= m_tablemgr.GetSQLQuery();
	
	int err = m_Table->Exec(query);

	//int err = m_Table->Exec(query+m_tablemgr.GetSQLOrder() );
	Reset( GetRowCount() );
	return err;
}
