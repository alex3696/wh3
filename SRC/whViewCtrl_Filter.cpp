#include "_pch.h"
#include "whModel_MovHistory.h"
#include "whViewCtrl_Filter.h"

//#include <boost/tuple/tuple.hpp>

//---------------------------------------------------------------------------
/** Модель  для вьюшки(отображения сортировщика полей) 
  Имя,Порядок сортировки
*/ 

class whModelGui_Filter: public wxDataViewVirtualListModel, public ctrlWithResMgr
{
private:
	unsigned int		m_GroupID;
	whLogQueryData*		m_QData;
public:
	whModelGui_Filter()
		:wxDataViewVirtualListModel(), m_GroupID(-1), m_QData(NULL)
	{}
	//---------------------------------------------------------------------------
	~whModelGui_Filter()	
	{
		m_QData=NULL;
		m_GroupID=-1;
	};	
	//---------------------------------------------------------------------------
	
	void SetLogQueryData(whLogQueryData* qdata,unsigned int groupid)
	{
		m_GroupID=groupid;
		m_QData=qdata;
		Update();
	}
	//---------------------------------------------------------------------------
	void Update()	
	{	
		if(m_QData)
		{
			const whGroupFilter& gf=m_QData->m_Filter[m_GroupID];
			unsigned int sz = gf.size();
			Reset(sz);
		}
		else
			Reset(0);
	}	
	//---------------------------------------------------------------------------
    // override sorting to always sort branches ascendingly
    int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
                 unsigned int column, bool ascending ) const
	{
		return wxDataViewModel::Compare( item1, item2, column, ascending );
	}
	
	//---------------------------------------------------------------------------
    // implementation of base class virtuals to define model
    virtual unsigned int GetColumnCount() const
	{	
		return 2;    
	}

    //---------------------------------------------------------------------------
	virtual wxString GetColumnType( unsigned int col ) const
    {
		//if(col==1)
		//	return "wxDataViewIconText";
		return "string";
	}

	//---------------------------------------------------------------------------
    virtual void GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const
	{
		if(m_QData && row<m_QData->m_Filter[m_GroupID].size() )
		{
			const whFieldFilter& filter=m_QData->m_Filter[m_GroupID][row];
			switch (col)
			{
				default:break;	
				case 0: variant = filter.m_SysLabel;
						break;
				case 1:	variant = CompOpToStr( filter.m_Operator);
						break;
				case 2:	variant = filter.m_RVal;
						break;
				case 3: variant = LogicOpToStr( filter.m_Connector);
						break;
			}//switch (col)
		
		}//if
	}
	//---------------------------------------------------------------------------
    virtual bool SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col )
	{
		if(m_QData && row<m_QData->m_Filter[m_GroupID].size() )
		{
			whFieldFilter& filter=m_QData->m_Filter[m_GroupID][row];
			switch (col)
			{
				default:break;
				case 0:	filter.m_SysLabel=variant.GetString();
						break;
				case 1:	filter.m_Operator=StrToCompOp(variant.GetString());
						break;
				case 2:	filter.m_RVal=variant.GetString();
						break;
				case 3:	filter.m_Connector	= StrToLogicOp(variant.GetString()) ;
						break;
			}//switch (col)
		}//if
		
		return false;
	}
	//---------------------------------------------------------------------------
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const								
	{
		return wxDataViewItem(0);
	}
	//---------------------------------------------------------------------------
    virtual bool IsContainer( const wxDataViewItem &item ) const										{return false;}
	//---------------------------------------------------------------------------
	virtual unsigned int GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const	{return 0;}
	//---------------------------------------------------------------------------
	virtual bool GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const
	{
		return false;
	}
	//---------------------------------------------------------------------------

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void whViewCtrl_Filter::OnActivated( wxDataViewEvent &event )
{
    wxDataViewItem		item = event.GetItem() ;
	if(item.GetID())
	{
		wxDataViewColumn*	column =  event.GetDataViewColumn();
		if(column )
		{
			wxRect labelRect = GetItemRect(item, column );
			wxDataViewRenderer* render = column->GetRenderer();
			if(render)
				render->StartEditing( item , labelRect );
		}
	}
}

whViewCtrl_Filter::whViewCtrl_Filter (wxWindow *parent, wxWindowID id, const wxPoint &pos, 
	const wxSize &size, long style, const wxValidator &validator)
		:wxDataViewCtrl(parent,id,pos,size,style,validator),m_LogQueryData(NULL),m_FieldDlg(NULL)
{
	whModelGui_Filter* model = new whModelGui_Filter();
	this->AssociateModel(model);
	model->DecRef();
	//SetRowHeight(24);
	//this->SetSortingColumnIndex(0);

	// Имя поля для фильтрации
	wxDataViewComboRenderer* filed_render = new wxDataViewComboRenderer( wxDATAVIEW_CELL_EDITABLE);
	m_FieldDlg=new whListBoxSelectTextDialog(this);
	filed_render->m_Dlg=m_FieldDlg; 
	
	
	wxDataViewColumn *field_column = new wxDataViewColumn( "Поле", filed_render,0,120 ,wxALIGN_LEFT);
	AppendColumn( field_column);


	wxArrayString choices;
	choices.Add( CompOpToStr(whCO_EQ));
	choices.Add( CompOpToStr(whCO_NOTEQ));
	choices.Add( CompOpToStr(whCO_LESS));
	choices.Add( CompOpToStr(whCO_MORE));
	choices.Add( CompOpToStr(whCO_LESSEQ));
	choices.Add( CompOpToStr(whCO_MOREEQ));
	choices.Add( CompOpToStr(whCO_LIKE));
	choices.Add( CompOpToStr(whCO_NOTLIKE));
	choices.Add( CompOpToStr(whCO_UNKNOWN));

	wxDataViewChoiceRenderer* crender = new wxDataViewChoiceRenderer( choices, wxDATAVIEW_CELL_EDITABLE);
    wxDataViewColumn *column3 = new wxDataViewColumn( "Условие", crender, 1,70 ,wxALIGN_LEFT);
    AppendColumn( column3 );

	//wxDataViewTextRenderer* val_render = new wxDataViewTextRenderer("string", wxDATAVIEW_CELL_EDITABLE);
	wxDataViewComboRenderer* val_render = new wxDataViewComboRenderer( wxDATAVIEW_CELL_EDITABLE);
	val_render->m_Dlg= new whSelectTextDialog(this);
	
	wxDataViewColumn *val_column = new wxDataViewColumn( "Значение", val_render, 2,100 ,wxALIGN_LEFT);
	AppendColumn( val_column);

	wxArrayString connectors;
	connectors.Add( LogicOpToStr(whLO_AND));
	connectors.Add( LogicOpToStr(whLO_OR));
	wxDataViewChoiceRenderer* connector_crender = new wxDataViewChoiceRenderer( connectors, wxDATAVIEW_CELL_EDITABLE);
    wxDataViewColumn *connector_column = new wxDataViewColumn( "Connector", connector_crender, 3,70 ,wxALIGN_LEFT);
    AppendColumn( connector_column );



	Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( whViewCtrl_Filter::OnActivated ), NULL ,this );
	
}


//---------------------------------------------------------------------------
whViewCtrl_Filter::~whViewCtrl_Filter()
{
	Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( whViewCtrl_Filter::OnActivated ), NULL ,this );

	//this->SetFocus();
}
//---------------------------------------------------------------------------
void whViewCtrl_Filter::SetLogQueryData(whLogQueryData* qdata,unsigned int groupid)
{
	m_GroupID=groupid;
	m_LogQueryData=qdata;
	whModelGui_Filter* model = static_cast<whModelGui_Filter*> ( this->GetModel() );
	model->SetLogQueryData(qdata,groupid);	

	m_FieldDlg->ClearChoice();
	for(field_table::idx_random::iterator 
		it= m_LogQueryData->m_field_table.begin_idx_random();
		it!=m_LogQueryData->m_field_table.end_idx_random();
		++it)
	{
		m_FieldDlg->AddChoice( it->m_UserLabel);
	}

	

};
//---------------------------------------------------------------------------
void whViewCtrl_Filter::UpdateView()
{
	whModelGui_Filter* model = static_cast<whModelGui_Filter*> (this->GetModel());
	model->Update();	
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// whPanel_Filter
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

whPanel_Filter::whPanel_Filter( wxWindow* parent, wxWindowID id,const wxPoint& pos,const wxSize& size,long style)
	:wxPanel( parent, id, pos, size, style ),m_GroupID(-1),m_LogQueryData(NULL)
{
	wxBoxSizer* szrMain;
	szrMain = new wxBoxSizer( wxHORIZONTAL );
	
	m_FilterList = new whViewCtrl_Filter(this,wxID_ANY,wxDefaultPosition,wxSize(400,400),wxSTATIC_BORDER);
	szrMain->Add( m_FilterList, 1, wxEXPAND);

	m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_VERTICAL ); 
	m_ToolBar->AddTool( whID_TBTN_ADDFILTER, wxT("+"), GetResMgr()->m_ico_plus16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddTool( whID_TBTN_DELFILTER, wxT("-"), GetResMgr()->m_ico_delete16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	m_ToolBar->AddSeparator();
	m_ToolBar->Realize();

	szrMain->Add( m_ToolBar, 0, wxEXPAND);


	this->SetSizer( szrMain );
	this->Layout();

	this->Connect( whID_TBTN_ADDFILTER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( whPanel_Filter::OnAddFilter ) );
	this->Connect( whID_TBTN_DELFILTER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( whPanel_Filter::OnDelFilter ) );

}
//---------------------------------------------------------------------------

whPanel_Filter::~whPanel_Filter()
{
	// Disconnect Events
	this->Disconnect( whID_TBTN_ADDFILTER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( whPanel_Filter::OnAddFilter ) );
	this->Disconnect( whID_TBTN_DELFILTER, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( whPanel_Filter::OnDelFilter ) );
	// Disconnect data and delete filter
	if( m_LogQueryData && m_GroupID<m_LogQueryData->m_Filter.size())
	{
		m_LogQueryData->m_Filter.erase(m_GroupID);
	}
	
	
	m_LogQueryData=NULL;
	m_GroupID=-1;

}
//---------------------------------------------------------------------------
void whPanel_Filter::SetLogQueryData(whLogQueryData* qdata,unsigned int groupid)
{
	if(qdata)
	{
		m_LogQueryData=qdata;
		m_GroupID=groupid;
		this->SetName(wxString::Format("Filter group %d",m_GroupID)   );
		m_FilterList->SetLogQueryData(qdata,m_GroupID);
	}
}
//---------------------------------------------------------------------------
//virtual 
void whPanel_Filter::OnAddFilter( wxCommandEvent& event )
{
	m_LogQueryData->m_Filter[m_GroupID].push_back( whFieldFilter());
	m_FilterList->SetLogQueryData(m_LogQueryData,m_GroupID);
}
//---------------------------------------------------------------------------
//virtual 
void whPanel_Filter::OnDelFilter( wxCommandEvent& event )
{
	whGroupFilter&	gf = m_LogQueryData->m_Filter[m_GroupID];
	
	wxDataViewItem	item=m_FilterList->GetSelection();
	if(item.IsOk()) // !=0
	{
		unsigned int row= (unsigned int)item.GetID();
		row--; // приводим к 0based
		if(row<gf.size() )
		{
			gf.erase(gf.begin()+row);
			if(row)
				m_FilterList->Select(wxDataViewItem((void*)row) );// выбираем фильтр выше удалённого
			m_FilterList->UpdateView();
		} // if(row<gf.size() )
	} // if(item.IsOk())
}

