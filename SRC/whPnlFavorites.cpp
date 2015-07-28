#include "_pch.h"
#include "globaldata.h"
#include "favorites.h"
#include "MainFrame.h"

//----------------------------------------------------------------------------
using namespace wh::favorites;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** поддержка DnD, для избранных обектов и классов,
при смене позиций незамедлительнно коммитить изменения
*/
class FavoritesDndObject: public wxDataObjectSimple
{
private:
	wxDataViewItem	m_Item;
	wxDataFormat	m_Format;
public:
	FavoritesDndObject(const wxDataViewItem	ptr=wxDataViewItem(NULL) )
		:wxDataObjectSimple(),m_Item(ptr)
	{
		m_Format.SetId("FavoritesDndObject");
		SetFormat (m_Format);
	}
	
	~FavoritesDndObject(){}
	//---------------------------------------------------------------------------
	wxDataViewItem GetDataViewItem()const
	{
		return m_Item;
	}
	//---------------------------------------------------------------------------
	virtual size_t  GetDataSize () const  
	{
		return sizeof(void*);
	}
	//---------------------------------------------------------------------------
	virtual bool  GetDataHere (void *buf) const  //Copy the data to the buffer, return true on success. 
	{
		errno_t err=memcpy_s(buf,sizeof(m_Item),&m_Item,sizeof(m_Item));
		if(err==0)
			return true;
		return false;
	}
	//---------------------------------------------------------------------------
	virtual bool  SetData (size_t len, const void *buf) //  Copy the data from the buffer, return true on success. 
	{
		if(!len || !buf)
			return false;
		
		errno_t err=memcpy_s(&m_Item,sizeof(m_Item),buf,sizeof(m_Item));
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











//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// DataModel
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
DataModel::DataModel()
	:wxDataViewModel()
	,mData(new LinkNode)	
{
	mData->mID=1;
	mData->mPID=0;
}
//----------------------------------------------------------------------------
DataModel::~DataModel()
{
	delete 	mData;
}
//----------------------------------------------------------------------------

void DataModel::GetValue( wxVariant &variant,const wxDataViewItem &item, unsigned int col ) const
{
	
	if(item.IsOk() )
	{
		Link* data = static_cast<Link*> (item.GetID());
		
		const wxIcon*		ico=&wxNullIcon;
		wxString			str=data->GetLabel();

		switch( data->GetLinkType() )
		{
			default:	break;
			case Link::lnkFolder:		ico=&(GetResMgr()->m_ico_folder24);	break;
			case Link::lnkClass :		ico=&(GetResMgr()->m_ico_type24);	break;
			case Link::lnkObjNum :		ico=&(GetResMgr()->m_ico_obj24);	break;
			case Link::lnkObjNumFilter:	ico=&(GetResMgr()->m_ico_objtype24);break;
		}//switch
		variant << wxDataViewIconText(str,*ico);				

	}//if(item.IsOk() )
	
}
//----------------------------------------------------------------------------

unsigned int DataModel::LoadNode(unsigned int pid,LinkNode* node,wxDataViewItemArray &array)const
{
	wxString query="SELECT	favorites_id,pid,label,\
							objname,objclass,classfilter\
							FROM w_favorites WHERE pid=%d";
	query=wxString::Format(query,pid);
			
	whTable::shared_ptr	t= whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if(t)
	{
		for(unsigned int i=0;i<t->GetRowCount(); ++i)
		{
			//Link * lnk=new Link;
			LinkNode * lnk=new LinkNode(node);

			lnk->mID	= t->GetAsInt(0,i);
			lnk->mPID	= t->GetAsInt(1,i);
			lnk->mLabel = t->GetAsString(2,i);
			lnk->mObjName	= t->GetAsString(3,i);
			lnk->mObjClass	= t->GetAsString(4,i);
			lnk->mClassFilter=t->GetAsString(5,i);
			
			if(node)
				node->mChild.push_back(lnk) ;	
			else
				mData->mChild.push_back(lnk) ;	
			//mNode->insert(lnk->mID , lnk );
			//mViewIDChild->insert( std::make_pair(lnk->mPID, lnk ) );

			array.Add( wxDataViewItem( (void*)lnk ) );

			
		}//for(unsigned int i=0;i<t->GetRowCount(); ++i)

		return t->GetRowCount();	
		
	}//if(t)
		

	return 0;	
}
//----------------------------------------------------------------------------
//
wxDataViewItem  DataModel::GetParent( const wxDataViewItem &item ) const
{
	if(item.IsOk() )
	{
		LinkNode* lnk = static_cast<LinkNode*> (item.GetID());
		if(lnk->mParent != mData )
			return	wxDataViewItem( (void*) lnk->mParent );
	}
	return wxDataViewItem(NULL);// the invisible root node has no parent
}//virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const
//----------------------------------------------------------------------------

unsigned int DataModel::GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const
{
	using namespace wh::favorites;
	
	LinkNode* parent_lnk = parent.IsOk()? static_cast<LinkNode*>(parent.GetID()) : NULL;
	unsigned int parent_id = parent_lnk ? parent_lnk->mID : 1 ;

	
	if(parent_lnk && parent_lnk->mChild.size() )
	{
		for (LinkNode::_IDChild::iterator it= parent_lnk->mChild.begin(); it!=parent_lnk->mChild.end(); ++it)
		{
			LinkNode * lnk= &(*it);
			array.Add( wxDataViewItem( (void*)lnk ) );
		}
		return parent_lnk->mChild.size();
	}
	else if(!parent_lnk && mData->mChild.size() )
	{
		for (LinkNode::_IDChild::iterator it= mData->mChild.begin(); it!=mData->mChild.end(); ++it)
		{
			LinkNode * lnk= &(*it);
			array.Add( wxDataViewItem( (void*)lnk ) );
		}
		return mData->mChild.size();
	}


	return LoadNode(parent_id,parent_lnk,array);
}//virtual unsigned int GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const
//----------------------------------------------------------------------------

int DataModel::MakeFolder(const wxDataViewItem &item,const wxString& label)
{
	int err=0;
	LinkNode* parent_lnk = item.IsOk() ? static_cast<LinkNode*>(item.GetID()) : NULL ;
	unsigned int parent_id = parent_lnk ? parent_lnk->mID : 1 ;

		
	wxString query="INSERT INTO t_favorites(pid,label)   VALUES (%d,'%s') RETURNING favorites_id" ;
	query=wxString::Format(query,parent_id,label);
	whTable::shared_ptr t=whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if(t && t->GetRowCount() )
	{
		LinkNode * lnk=new LinkNode(parent_lnk);
		lnk->mID   = t->GetAsInt(0,0);
		lnk->mPID  = parent_id;
		lnk->mLabel= label;
		

		wxDataViewItem child( (void*) lnk );
		wxDataViewItem parent;
		if(parent_lnk)
		{
			parent=wxDataViewItem((void*)parent_lnk);
			parent_lnk->mChild.push_back(lnk);
		}
		else
		{
			parent=wxDataViewItem((void*)NULL);
			mData->mChild.push_back(lnk );
		}
		
		err=ItemAdded( parent, child );
	}

	Resort();
	return err;
}//int MakeFolder(unsigned int pid,const wxString& label)
//----------------------------------------------------------------------------

int DataModel::InsertLink(const Link& lnk)
{
	int err=0;
	wxString query;


	switch( lnk.GetLinkType() )
	{
		default:	
		case Link::lnkFolder:
			err=1;
			break;
		
		case Link::lnkClass :
			query="INSERT INTO t_favorites(pid, label,classid )\
					VALUES(%d,'%s',		(SELECT classid FROM wh_class WHERE classname='%s') )\
					RETURNING favorites_id";
			query=wxString::Format(query,lnk.mPID,lnk.mLabel,lnk.mClassFilter);
			break;
		
		case Link::lnkObjNum :
			query="INSERT INTO t_favorites(pid, label,objnumid )\
					VALUES(%d,'%s',		(SELECT id FROM wh_objnum WHERE  objname='%s' AND classname='%s') )\
					RETURNING favorites_id";
			query=wxString::Format(query,lnk.mPID,lnk.mLabel,  lnk.mObjName,lnk.mObjClass);
			break;
			
		case Link::lnkObjNumFilter :
			query="INSERT INTO t_favorites(pid, label, classid,objnumid )\
					VALUES(%d,'%s', \
					(SELECT classid FROM wh_class WHERE classname='%s'),\
					(SELECT id FROM wh_objnum WHERE  objname='%s' AND classname='%s') )\
					RETURNING favorites_id";
			query=wxString::Format(query,lnk.mPID,lnk.mLabel,  lnk.mClassFilter, lnk.mObjName,lnk.mObjClass );
			break;
	}//switch

	whTable::shared_ptr t=whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if(t)
	{
		LinkNode * new_lnk=new LinkNode();
		new_lnk->copy(lnk);
		new_lnk->mID   = t->GetAsInt(0,0);
		new_lnk->mPID   = 1;
		mData->mChild.push_back(new_lnk );

		// notify control
		wxDataViewItem child( (void*) new_lnk );
		wxDataViewItem parent ( (void*) NULL );
		
		err=ItemAdded( parent, child );

	}
	Resort();

	return err;
}
//----------------------------------------------------------------------------
int DataModel::MoveLink	(const wxDataViewItem &item, const wxDataViewItem &new_parent_item,bool new_parent_expanded)
{
	if(item.IsOk() && new_parent_item.IsOk())
	{
		LinkNode* old_lnk =				static_cast<LinkNode*>(item.GetID());
		LinkNode* new_parent_lnk =	static_cast<LinkNode*>(new_parent_item.GetID());

		wxString query="UPDATE t_favorites SET pid=%d WHERE favorites_id=%d";
		query=wxString::Format(query,new_parent_lnk->mID, old_lnk->mID);
		int t=whDataMgr::GetDB().Exec(query);
		//int t=0;
		if(t==0)
		{
			LinkNode* old_parent = static_cast<LinkNode*>(GetParent(item).GetID());;
			if(!old_parent)
				old_parent=mData;
			

			LinkNode::_IDChild::auto_type old_it;
			for(LinkNode::_IDChild::iterator it=old_parent->mChild.begin();it!=old_parent->mChild.end();++it)
			{
				if( it->mID == old_lnk->mID )
				{
					old_it = old_parent->mChild.release(it);
					break;
				}
			}
			
			ItemDeleted( GetParent(item) , item );
			
			
			
			if(new_parent_expanded)
			{
				LinkNode* new_node = old_it.release();
				new_node->mPID= new_parent_lnk->mID;
				new_node->mParent = new_parent_lnk;
				
				wxDataViewItem new_child( new_node );
				new_parent_lnk->mChild.push_back( new_node );
				ItemAdded( new_parent_item, new_child );
			}
			else
			{
				new_parent_lnk->mChild.clear();
			}
			

			

			

			
			//this->ItemChanged(item);
		}

	
	}
	return 1;
}
//----------------------------------------------------------------------------

int DataModel::DeleteLink(const wxDataViewItem &item)
{
	LinkNode* lnk = item.IsOk() ? static_cast<LinkNode*>( item.GetID()) : NULL ;
	if(lnk )
	{
		unsigned int id = lnk->mID;
		wxString query="DELETE FROM t_favorites WHERE favorites_id=%d" ;
		query=wxString::Format(query,id);
		
		int res=whDataMgr::GetDB().Exec(query);
		if(res)
			return 1;

		LinkNode* parent_lnk = lnk->mParent;
			
		wxDataViewItem item( (void*) lnk  );
		wxDataViewItem parent( (void*) parent_lnk );
		ItemDeleted( parent, item );
	
		parent_lnk = lnk->mParent ?  lnk->mParent : mData;
			
		LinkNode::_IDChild::iterator it;
		for(it=parent_lnk->mChild.begin();it!=parent_lnk->mChild.end();++it)
		{
			LinkNode* lit_nk= &(*it);
			if( lit_nk == lnk)
			{
				parent_lnk->mChild.erase(it);
				break;
			}
		}
			
		return 0;//whDataMgr::GetDB().Exec(query);
	}
	return 1;
}
//----------------------------------------------------------------------------

int DataModel::UpdateLabel(const wxDataViewItem &item,const wxString& label)
{
	int err=1;
	if(item.IsOk() && !label.IsEmpty() )
	{
		LinkNode* lnk = static_cast<LinkNode*> (  item.GetID() ); 
		lnk->mLabel=label;
		wxString query="UPDATE t_favorites SET label='%s' WHERE favorites_id=%d ";
		query=wxString::Format(query,label,lnk->mID);
		
		int res=whDataMgr::GetDB().Exec(query);
		if(!res)
		{
			ItemChanged(item);
			err=0;
		}	
	}
	Resort();
	return err;
}
//----------------------------------------------------------------------------
wxString DataModel::GetLinkLabel(const wxDataViewItem &item)const
{
	wxString link_label = item.IsOk() ? (static_cast<LinkNode*>( item.GetID()))->mLabel : wxEmptyString ;
	return link_label;
}

//----------------------------------------------------------------------------
Link* DataModel::GetLink(const wxDataViewItem &item)const
{
	Link* lnk = item.IsOk() ? static_cast<Link*>( item.GetID()) : NULL ;
	return lnk;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// wh::favorites::Panel
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Panel::InitDataView()
{
	m_View = new wxDataViewCtrl( this, wxID_ANY,wxDefaultPosition,wxDefaultSize/*,wxDV_NO_HEADER*/);
	m_View->SetRowHeight(24);



	whDataMgr* mgr = 	whDataMgr::GetInstance();	
	m_Model = &mgr->mFavoritesModel;

	m_View->AssociateModel(m_Model );
	
	

	
	//m_View->AssociateModel(m_Model );
	//m_Model->DecRef();

	wxDataViewIconTextRenderer*	render0  = new wxDataViewIconTextRenderer();
    wxDataViewColumn*			column0 =  new wxDataViewColumn( "title", render0, 0, 400, wxALIGN_LEFT,wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE  );
    
	m_View->AppendColumn( column0 );
	
	
	
	
}
//---------------------------------------------------------------------------

void Panel::InitToolBar()
{
	m_ToolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL | wxTB_FLAT ); 
	//m_ToolBar->AddTool( wxID_ANY, wxT("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_ToolBar->AddTool( wxID_NEW_FOLDER,"add_folder", GetResMgr()->m_ico_newfolder16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_ToolBar->AddTool( wxID_EDIT,      "edit_link" , GetResMgr()->m_ico_edit16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_ToolBar->AddTool( wxID_DELETE ,   "delete"    , GetResMgr()->m_ico_delete16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	m_ToolBar->AddTool( wxID_REFRESH ,  "refresh"   , GetResMgr()->m_ico_refresh16, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL ); 
	
	m_ToolBar->Realize(); 
	
}
//---------------------------------------------------------------------------
Panel::Panel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style,const wxString& name  ) 
:wxPanel( parent, id, pos, size, style,name),m_View(NULL)//,m_Model(NULL) 
{
	InitToolBar();
	InitDataView();

	wxSizer* szrMain= new wxBoxSizer( wxVERTICAL );
	szrMain->Add( m_ToolBar, 0, wxEXPAND, 5 );
	szrMain->Add( m_View, 1, wxALL|wxEXPAND, 0 );

	this->SetSizer( szrMain );
	this->Layout(  );

	
	Bind(wxEVT_COMMAND_TOOL_CLICKED ,&Panel::OnToolBarBtn_MakeFoder,this,wxID_NEW_FOLDER);
	Bind(wxEVT_COMMAND_TOOL_CLICKED ,&Panel::OnToolBarBtn_Delete,this,wxID_DELETE);
	Bind(wxEVT_COMMAND_TOOL_CLICKED ,&Panel::OnToolBarBtn_UpdateLabel,this,wxID_EDIT);

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,&Panel::OnActivated,this);

	//Connect(wxID_NEW_FOLDER, wxCommandEventHandler(	whPnlFavorites::OnToolBarBtn_MakeFoder) );
	
	//Connect(wxID_NEW, wxEVT_COMMAND_MENU_SELECTED,      wxCommandEventHandler(whTableViewPnl::OnAdd));
	//Bind(wxEVT_COMMAND_MENU_SELECTED,&whTableViewPnl::OnAdd,this,wxID_NEW);
	//

	m_View->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_BEGIN_DRAG, wxDataViewEventHandler( Panel::OnBeginDrag ), NULL ,this );
	m_View->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP_POSSIBLE, wxDataViewEventHandler( Panel::OnDropPossible ), NULL ,this );
	m_View->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_DROP, wxDataViewEventHandler( Panel::OnDrop ), NULL ,this );

	m_View->EnableDragSource("FavoritesDndObject");
    m_View->EnableDropTarget("FavoritesDndObject");
	
}
//---------------------------------------------------------------------------
Panel::~Panel()
{
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &Panel::OnToolBarBtn_MakeFoder, this,wxID_NEW_FOLDER);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &Panel::OnToolBarBtn_Delete, this,wxID_DELETE);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &Panel::OnToolBarBtn_UpdateLabel, this,wxID_EDIT);
	
	Unbind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED,&Panel::OnActivated,this);
	//m_AuiMgr.UnInit();
	//whTypeListModel* model = GetModel();
	//delete m_View;
	//model->DecRef();  // avoid memory leak !!

}
//---------------------------------------------------------------------------

void Panel::OnToolBarBtn_MakeFoder(wxCommandEvent& evt)
{
    wxTextEntryDialog dialog(this,
							"Введите название для папки в каталоге 'Избранное'",
                             wxT("Создание новой папки"),
                             wxT("Новая папка"),
                             wxOK | wxCANCEL);

    if (dialog.ShowModal() == wxID_OK)
    {
		whDataMgr* mgr = 	whDataMgr::GetInstance();	

		wxDataViewItem item = m_View->GetSelection();
		wxDataViewItem parent = mgr->mFavoritesModel.GetParent(item);
		//m_View->GetCurrentItem

		
		mgr->mFavoritesModel.MakeFolder(parent,dialog.GetValue());
		//mgr->mFavoritesModel.Resort();

    }	
	
}//void whPnlFavorites::OnToolBarBtn_MakeFoder(wxCommandEvent& evt)
//---------------------------------------------------------------------------

void Panel::OnToolBarBtn_Delete(wxCommandEvent& evt)
{
	wxDataViewItem item = m_View->GetCurrentItem();
	whDataMgr* mgr = 	whDataMgr::GetInstance();	

	if(item.IsOk())
	{
		const wxString	str_favorites_confirm_del= "Вы действительно ходите удалить из избранного '%s'?";
		const wxString	str_confirm = "Подтверждение";

		int res = wxMessageBox( wxString::Format(str_favorites_confirm_del, mgr->mFavoritesModel.GetLinkLabel(item) ) ,
								str_confirm,
								wxYES_NO);
		if (res == wxYES)
			mgr->mFavoritesModel.DeleteLink(item);
	}//if(item.IsOk())

}//void whPnlFavorites::OnToolBarBtn_Delete(wxCommandEvent& evt)
//---------------------------------------------------------------------------

void Panel::OnToolBarBtn_UpdateLabel(wxCommandEvent& evt)
{
	wxDataViewItem item = m_View->GetCurrentItem();
	whDataMgr* mgr = 	whDataMgr::GetInstance();	
	if(item.IsOk())
	{
		wxTextEntryDialog dialog(this,
							"Измените название ярлыка 'Избранного'",
                             wxT("Изменение нимени ярлыка"),
                             mgr->mFavoritesModel.GetLinkLabel(item),
                             wxOK | wxCANCEL);
		if (dialog.ShowModal() == wxID_OK)
		{
			mgr->mFavoritesModel.UpdateLabel(item,dialog.GetValue());
			
		}
			
	}//if(item.IsOk())
}
//---------------------------------------------------------------------------

void Panel::OnActivated( wxDataViewEvent &event )
{
	wxDataViewItem  item = event.GetItem();
	
	whDataMgr* mgr = 	whDataMgr::GetInstance();	
	Link* lnk = mgr->mFavoritesModel.GetLink(item );
		
	if(lnk)
	{
		whDataMgr* mgr = 	whDataMgr::GetInstance();	
		switch( lnk->GetLinkType() )
		{
			default:	
				break;
			case Link::lnkFolder:
				m_View->Expand(item);
			break;
		
			case Link::lnkClass :
				mgr->m_MainFrame->CreateTypeCatalog(lnk->mClassFilter);
			break;
		
			case Link::lnkObjNum :
				mgr->m_MainFrame->CreateObjCatalog(lnk->mObjClass,lnk->mObjName);
			break;
			
			case Link::lnkObjNumFilter :
				//mgr->m_MainFrame->CreateObjCatalog(lnk->mObjClass,lnk->mObjName,lnk->mClassFilter);
			break;
		}//switch
				
	
	
	}//if(lnk)
}//void whPnlFavorites::OnActivated( wxDataViewEvent &event )
//---------------------------------------------------------------------------
void Panel::OnBeginDrag( wxDataViewEvent &event )
{
    wxDataViewItem item( event.GetItem() );
	FavoritesDndObject* obj = new FavoritesDndObject(item);
	event.SetDataObject( obj );

};
//---------------------------------------------------------------------------	
void Panel::OnDropPossible( wxDataViewEvent &event )
{
	wxDataFormat& format=event.GetDataFormat();
	if (format!="FavoritesDndObject")
	{
		event.Veto();
		return;
	}


	wxDataViewItem new_parent_item( event.GetItem() );
	if(new_parent_item.IsOk() )
	{
		if(!m_Model->IsContainer(new_parent_item))
		{
			event.Veto();
			return;
		}
	}
		
	FavoritesDndObject dndobj;

	if( dndobj.SetData(  event.GetDataSize(), event.GetDataBuffer() ) )
	{
		wxDataViewItem dnd_item= dndobj.GetDataViewItem();
		if(dnd_item==new_parent_item)
			event.Veto();		
	}

	
};
//---------------------------------------------------------------------------
void Panel::OnDrop( wxDataViewEvent &event )
{
	if (event.GetDataFormat() == "FavoritesDndObject")
	{
		FavoritesDndObject dndobj;
		dndobj.SetData(  event.GetDataSize(), event.GetDataBuffer() );

		wxDataViewItem dnd_item= dndobj.GetDataViewItem();
		wxDataViewItem new_parent_item( event.GetItem() );
		wxDataViewItem old_parent_item( m_Model->GetParent(dnd_item) );


		if(dnd_item!=new_parent_item && new_parent_item!=old_parent_item )
			m_Model->MoveLink(dnd_item,new_parent_item,m_View->IsExpanded(new_parent_item));
		else
			event.Veto();
	}
	else
		event.Veto();

	
};