#include "_pch.h"
#include "whViewCtrl_Sorter.h"


#include "whModel_MovHistory.h"




//---------------------------------------------------------------------------
/** Модель  для вьюшки(отображения сортировщика полей) 
  Имя,Порядок сортировки
*/ 

class whModelGui_Sorter: public wxDataViewVirtualListModel, public ctrlWithResMgr
{
private:
	whLogQueryData* m_QData;
public:
	whModelGui_Sorter()
		:wxDataViewVirtualListModel(),m_QData(NULL)
	{}
	//---------------------------------------------------------------------------
	~whModelGui_Sorter()	
	{};	
	//---------------------------------------------------------------------------
	void SetLogQueryData(whLogQueryData* qdata)
	{
		m_QData=qdata;
		Update();
	}
	//---------------------------------------------------------------------------
	whLogQueryData*	GetLogQueryData()
	{
		return m_QData;
	}
	//---------------------------------------------------------------------------
	void Update()	
	{	
		if(m_QData)
			Reset( m_QData->m_field_table.size() );
	}	
	//---------------------------------------------------------------------------
	
	int GetFieldSort(const wxDataViewItem &item,whSortDir& sort)
	{
		unsigned int row = wxPtrToUInt(item.GetID()) - 1;
		if(item.IsOk() && m_QData && row<m_QData->m_field_table.size() )
		{
			field_table::idx_random::iterator it=m_QData->m_field_table.get<0>().begin()+row;
			if(it!=m_QData->m_field_table.get<0>().end() )
			{
				sort=it->m_Sort;
				return 0;
			}
		}
		return 1;
	}

	int SetFieldSort(const wxDataViewItem &item,whSortDir sort)
	{
		unsigned int row = wxPtrToUInt(item.GetID()) - 1;
		if(item.IsOk() && m_QData && row<m_QData->m_field_table.size() )
		{
			field_table::idx_random::iterator it=m_QData->m_field_table.get<0>().begin()+row;
			if(it!=m_QData->m_field_table.get<0>().end() )
			{
				whDbField fd=*it;
				fd.m_Sort=sort;
				if ( m_QData->m_field_table.get<0>().replace(it,fd) )
				{
					ItemChanged(item);	
					return 0;
				}
			}
		}
		return 1;
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
		return 3;    
	}

    //---------------------------------------------------------------------------
	virtual wxString GetColumnType( unsigned int col ) const
    {
		if(col==1)
			return "wxDataViewIconText";
		return "string";
	}

	//---------------------------------------------------------------------------
    virtual void GetValueByRow( wxVariant &variant,unsigned int row, unsigned int col ) const
	{
		if(m_QData && row<m_QData->m_field_table.size() )
		{
			field_table::idx_random::iterator it=m_QData->m_field_table.get<0>().begin()+row;
			switch (col)
			{
				default:break;	
				case 0:	variant = it->m_SysLabel;
						break;
				case 1:	switch(it->m_Sort)
						{
							default:		variant.Clear();
											variant << wxDataViewIconText(it->m_UserLabel, wxNullIcon  );
											break;
							case whSORT_ASC:variant << wxDataViewIconText(it->m_UserLabel, GetResMgr()->m_ico_sort_asc16  );
											break;
							case whSORT_DESC:variant << wxDataViewIconText(it->m_UserLabel, GetResMgr()->m_ico_sort_desc16  );
											break;
						}
						break;
				case 2:	variant = SortDirToStr(it->m_Sort);
						break;
				
			}// switch
		
		}// if
	
	}

	//---------------------------------------------------------------------------
    virtual bool SetValueByRow( const wxVariant &variant,unsigned int row, unsigned int col )
	{
		if(col!=2)
			return false;

		field_table::idx_random::iterator it=m_QData->m_field_table.get<0>().begin()+row;
	
		if(it!=m_QData->m_field_table.get<0>().end() )
		{
				whDbField fd=*it;
				fd.m_Sort=StrToSortDir(variant.GetString());
				if ( m_QData->m_field_table.get<0>().replace(it,fd) )
					return true;
		}
		return false;
	}
	//---------------------------------------------------------------------------
    virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const
	{
		return wxDataViewItem(0);
	}
	//---------------------------------------------------------------------------
    virtual bool IsContainer( const wxDataViewItem &item ) const	
	{	
		return false;	
	}
	//---------------------------------------------------------------------------
	virtual unsigned int GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const
	{ 
		return 0;
	}
	//---------------------------------------------------------------------------
	virtual bool GetAttrByRow( unsigned int row, unsigned int col,wxDataViewItemAttr &attr ) const
	{
		if(m_QData && row<m_QData->m_field_table.size() )
		{
			field_table::idx_random::iterator it=m_QData->m_field_table.get<0>().begin()+row;
			switch(it->m_Sort)
			{
				case whSORT_ASC:		attr.SetColour(wxColour(0,153,0));	break;
				case whSORT_DESC:		attr.SetColour(*wxBLUE);			break;
				default:/*whSORT_NO*/	attr.SetColour(*wxLIGHT_GREY);		break;
			}//switch		
		
		}
		return true;
	}
	//---------------------------------------------------------------------------

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** поддержка перетаскивания полей, для DnD
*/
class whDataSortField: public wxDataObjectSimple
{
private:
	unsigned int	m_FieldNo;
	wxDataFormat	m_Format;
public:
	whDataSortField(unsigned int	FieldNo=0)
		:wxDataObjectSimple(),m_FieldNo(FieldNo)
	{
		m_Format.SetId("SortField");
		SetFormat (m_Format);
	}
	
	~whDataSortField(){}
	//---------------------------------------------------------------------------
	unsigned int GetSortFieldNo()const
	{
		return m_FieldNo;
	}
	//---------------------------------------------------------------------------
	virtual size_t  GetDataSize () const  
	{
		return sizeof(m_FieldNo);
	}
	//---------------------------------------------------------------------------
	virtual bool  GetDataHere (void *buf) const  //Copy the data to the buffer, return true on success. 
	{
		errno_t err=memcpy_s(buf,sizeof(m_FieldNo),&m_FieldNo,sizeof(m_FieldNo));
		if(err==0)
			return true;
		return false;
	}
	//---------------------------------------------------------------------------
	virtual bool  SetData (size_t len, const void *buf) //  Copy the data from the buffer, return true on success. 
	{
		errno_t err=memcpy_s(&m_FieldNo,sizeof(m_FieldNo),buf,sizeof(m_FieldNo));
		if(err==0)
			return true;
		return false;
	}
	//---------------------------------------------------------------------------
	const wxDataFormat &  GetFormat () const  //  Returns the (one and only one) format supported by this object. 
	{
		return m_Format;
	}
	//---------------------------------------------------------------------------
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void whViewCtrl_Sorter::OnBeginDrag( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );
	whModelGui_Sorter* m_Model = static_cast<whModelGui_Sorter*> (this->GetModel());
		
    // only allow drags for item, not containers
    if (m_Model->IsContainer( item ) )
	{
		event.Veto();
		return;
	}

	unsigned int node = (unsigned int) item.GetID();
	node--;

	whDataSortField *obj = new whDataSortField(node);
	event.SetDataObject( obj );

};
//---------------------------------------------------------------------------	
void whViewCtrl_Sorter::OnDropPossible( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );
	whModelGui_Sorter* m_Model = static_cast<whModelGui_Sorter*> (this->GetModel());
    // only allow drags for item, not containers
	if (m_Model->IsContainer( item ) )
		event.Veto();
    wxDataFormat& format=event.GetDataFormat();
	if (format!="SortField")
	   event.Veto();
};
//---------------------------------------------------------------------------
void whViewCtrl_Sorter::OnDrop( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );
	whModelGui_Sorter* m_Model = static_cast<whModelGui_Sorter*> (this->GetModel());
    // only allow drops for item, not containers
	if (m_Model->IsContainer( item ) )
	{
		event.Veto();
		return;
	}
	
	if (event.GetDataFormat() != "SortField")
	{
		event.Veto();
		return;
	}

	unsigned int new_row=(unsigned int)item.GetID();
	new_row--;
	

	whDataSortField obj;
	obj.SetData(  event.GetDataSize(), event.GetDataBuffer() );
	unsigned int old_row=obj.GetSortFieldNo();
  
	whLogQueryData* mgr=m_Model->GetLogQueryData();
	mgr->m_field_table.Relocate(new_row,old_row);

		
	m_Model->Update();
};
//---------------------------------------------------------------------------
void whViewCtrl_Sorter::OnActivated( wxDataViewEvent &event )
{
    wxDataViewItem		item = event.GetItem() ;
	if(item.GetID())
	{
		wxDataViewColumn*	column =  event.GetDataViewColumn();
		if(column && column->GetModelColumn()==2 )
		{
			wxRect labelRect = GetItemRect(item, column );
			wxDataViewRenderer* render = column->GetRenderer();
			if(render)
				render->StartEditing( item , labelRect );
		}

		if(column && column->GetModelColumn()==1 )
		{
			whModelGui_Sorter* model = static_cast<whModelGui_Sorter*> (this->GetModel());
			whSortDir sort;
			
			if(!model->GetFieldSort(item,sort))
			{
				switch(sort)
				{
					default:			model->SetFieldSort(item,whSORT_ASC);	break;
					case whSORT_ASC:	model->SetFieldSort(item,whSORT_DESC);	break;
					case whSORT_DESC:	model->SetFieldSort(item,whSORT_NO);	break;
				} // switch
			}// if
		}// if(column && column->GetModelColumn()==1 )

	}
}
//---------------------------------------------------------------------------
whViewCtrl_Sorter::whViewCtrl_Sorter (wxWindow *parent, wxWindowID id, const wxPoint &pos, 
	const wxSize &size, long style, const wxValidator &validator)
		:wxDataViewCtrl(parent,id,pos,size,style,validator)
{
	whModelGui_Sorter* m_Model = new whModelGui_Sorter();
	this->AssociateModel( m_Model);
	m_Model->DecRef();
	//SetRowHeight(24);
	//this->SetSortingColumnIndex(0);

	//#ifdef __DEBUG
	//	AppendTextColumn("Системное имя поля",0,wxDATAVIEW_CELL_INERT,100,wxALIGN_LEFT);
	//#else
		wxDataViewColumn* id_col=AppendTextColumn("Системное имя поля",0,wxDATAVIEW_CELL_INERT,100,wxALIGN_LEFT);
		id_col->SetHidden(true);
	//#endif



	AppendIconTextColumn("Имя",		 1,wxDATAVIEW_CELL_INERT,150,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);
	//AppendTextColumn(	"Сортировка",2,wxDATAVIEW_CELL_INERT,100,wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE|wxDATAVIEW_COL_SORTABLE);

	wxArrayString choices;
	choices.Add( SortDirToStr(whSORT_ASC));
	choices.Add( SortDirToStr(whSORT_DESC));
	choices.Add( SortDirToStr(whSORT_NO));
	
	wxDataViewChoiceRenderer2* crender = new wxDataViewChoiceRenderer2( choices, wxDATAVIEW_CELL_EDITABLE);
	wxSize sz=crender->GetSize();
    wxDataViewColumn *column3 = new wxDataViewColumn( "Сортировка", crender, 2,110 ,wxALIGN_LEFT,wxDATAVIEW_COL_SORTABLE);
    AppendColumn( column3 );



	Connect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( whViewCtrl_Sorter::OnBeginDrag ), NULL ,this );
	Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( whViewCtrl_Sorter::OnDropPossible ), NULL ,this );
	Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( whViewCtrl_Sorter::OnDrop ), NULL ,this );
	Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( whViewCtrl_Sorter::OnActivated ), NULL ,this );

	EnableDragSource("SortField");
    EnableDropTarget("SortField");
	//wxDF_UNICODETEXT

}
//---------------------------------------------------------------------------
whViewCtrl_Sorter::~whViewCtrl_Sorter()
{
	this->SetFocus();
	Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( whViewCtrl_Sorter::OnBeginDrag ), NULL ,this );
	Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( whViewCtrl_Sorter::OnDropPossible ), NULL ,this );
	Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( whViewCtrl_Sorter::OnDrop ), NULL ,this );
	Disconnect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( whViewCtrl_Sorter::OnActivated ), NULL ,this );

};
//---------------------------------------------------------------------------
void whViewCtrl_Sorter::SetLogQueryData(whLogQueryData* qdata)
{
	whModelGui_Sorter* m_Model = static_cast<whModelGui_Sorter*> (this->GetModel());
	m_Model->SetLogQueryData(qdata);	

};
//---------------------------------------------------------------------------
