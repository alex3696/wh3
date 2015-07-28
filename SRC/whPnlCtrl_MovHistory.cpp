#include "_pch.h"
#include "globaldata.h"
#include "whPnlCtrl_MovHistory.h"


BEGIN_EVENT_TABLE(whPnlCtrl_MovHistory, wxPanel)

	EVT_MENU( wxID_REFRESH ,	whPnlCtrl_MovHistory::OnUpdateData )	
	EVT_MENU( wxID_ADD ,		whPnlCtrl_MovHistory::OnCreateFilterGroup )	
	EVT_MENU( wxID_MORE ,		whPnlCtrl_MovHistory::OnMore )	

	EVT_AUI_PANE_CLOSE(			whPnlCtrl_MovHistory::OnPaneClose)

	
/*	EVT_MENU( wxID_NEW_FOLDER,	ListPanel::OnNewFolder)
	EVT_MENU( wxID_NEW_TYPE,	ListPanel::OnNewType)
	EVT_MENU( wxID_DELETE,		ListPanel::OnDelete )
	EVT_MENU( wxID_CUT,			ListPanel::OnCut )
	EVT_MENU( wxID_PASTE,		ListPanel::OnPaste )
	EVT_MENU( wxID_UP,			ListPanel::OnUp )
	EVT_MENU( wxID_COPY,		ListPanel::OnMove )
	EVT_MENU( wxID_EDIT,		ListPanel::OnEdit )
*/
	//EVT_DATAVIEW_ITEM_CONTEXT_MENU ( TypeTreeCtrl::OnContextMenu )
END_EVENT_TABLE()

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// �������������� ������ � 2 ��������  *�������� ������* +  *�������*
void whPnlCtrl_MovHistory::InitToolBar()
{
	m_ToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,wxAUI_TB_DEFAULT_STYLE );
	
	m_ToolBar->AddTool(wxID_ADD, wxT("�������� ������"),GetResMgr()->m_ico_addfilter24, wxT("�������� ������ (?)"));
	m_ToolBar->AddTool(wxID_REFRESH, wxT("�������"),GetResMgr()->m_ico_refresh24, wxT("������� (CTRL+R)"));
	
	wxAuiToolBarItem* vc=m_ToolBar->AddTool(wxID_MORE, wxT("���������� ��� ���/����"),GetResMgr()->m_ico_views24, wxT("���������� ��� ���/����(CTRL+M)"),wxITEM_CHECK);
	vc->SetState(wxAUI_BUTTON_STATE_CHECKED);
	
	// 
	m_ToolBar->Realize();

	m_AuiMgr.AddPane(m_ToolBar, wxAuiPaneInfo().
				     Name(wxT("m_MovHistoryToolBar")).Caption(wxT("type dir toolbar"))
					 .CaptionVisible(false)
					 .Top()
					 .Row(0)
					 //.Position(0)
					 .DockFixed()
					 .PaneBorder(false));
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::InitFilterBar()
{
	m_ctrlSorter=new whViewCtrl_Sorter(this,wxID_ANY,wxDefaultPosition,wxSize(220,100),wxSTATIC_BORDER);
	m_AuiMgr.InsertPane(m_ctrlSorter, wxAuiPaneInfo().Caption("����������").Name("����������")
								.Top().Row(1).MinSize(250,100)
								);

	for(unsigned int i=0;i<m_LogQueryData.m_Filter.size();++i)
		CreateFilterGroupGUI(i);
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::InitDataView()
{
	m_View = new whViewCtrl_MovHistory( this, wxID_ANY,wxDefaultPosition,wxDefaultSize,wxSTATIC_BORDER|wxDV_HORIZ_RULES);//|wxDV_VERT_RULES
	//m_View->SetRowHeight(24);
	m_AuiMgr.AddPane(m_View, wxAuiPaneInfo().CenterPane().PaneBorder(false));

	m_View->UpdateView();

}
//---------------------------------------------------------------------------
// ������� ��������� ������� �������
//public:
whLogQueryData* whPnlCtrl_MovHistory::SetLogQueryDataDefault()
{
	m_LogQueryData.m_field_table.clear();
	m_LogQueryData.m_Filter.clear();
	// �������������� ��������� �������
	m_LogQueryData.m_field_table.push_back( whDbField("movlogid","ID",whSORT_NO));
	m_LogQueryData.m_field_table.push_back( whDbField("movlogtime","�����",whSORT_DESC));
	m_LogQueryData.m_field_table.push_back( whDbField("rolname","������������",whSORT_NO));

	m_LogQueryData.m_field_table.push_back( whDbField("classname","���",whSORT_NO));
	m_LogQueryData.m_field_table.push_back( whDbField("objname","���",whSORT_NO));
	m_LogQueryData.m_field_table.push_back( whDbField("qty","����������",whSORT_NO));
	m_LogQueryData.m_field_table.push_back( whDbField("classtype","������ ������",whSORT_NO));

	m_LogQueryData.m_field_table.push_back( whDbField("movlogcomment","�����������",whSORT_NO));

	m_LogQueryData.m_field_table.push_back( whDbField("src_path","���� ��������",whSORT_NO));
	m_LogQueryData.m_field_table.push_back( whDbField("dst_path","���� ��������",whSORT_NO));

	SetLogQueryData(m_LogQueryData);
	return &m_LogQueryData;
}
//---------------------------------------------------------------------------
//public:
whPnlCtrl_MovHistory::whPnlCtrl_MovHistory( wxWindow* parent, wxWindowID id, const wxPoint& pos, 
						const wxSize& size, long style)
	:wxPanel( parent, id, pos, size, style ),m_View(NULL)
{



	m_AuiMgr.SetManagedWindow(this);
	InitToolBar();
	InitFilterBar();
	InitDataView();

	whDataMgr* mgr = 	whDataMgr::GetInstance();
	if(mgr->mDb.IsOpen() )
		ConnectDB(&mgr->mDb);
	

	m_AuiMgr.Update();

}
//---------------------------------------------------------------------------
//public:
whPnlCtrl_MovHistory::~whPnlCtrl_MovHistory()
{
	wxAuiPaneInfo& pi=	m_AuiMgr.GetPane( "����������" );
	m_AuiMgr.ClosePane(pi);


	m_AuiMgr.UnInit();
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::ClearFilterGroup()
{
	unsigned int i=m_LogQueryData.m_Filter.size();
	while(i)
	{
		wxAuiPaneInfo& pi=	m_AuiMgr.GetPane( wxString::Format("Filter group %d",i-1) );
		m_AuiMgr.ClosePane(pi);
		i--;
	
	}
}
//---------------------------------------------------------------------------
//public:
void whPnlCtrl_MovHistory::OnUpdateData(wxCommandEvent& evt )
{
	m_View->UpdateData();
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::UpdateData()
{
	m_View->UpdateData();
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::CreateFilterGroupGUI(unsigned int gid)
{
	if(gid<m_LogQueryData.m_Filter.size() /*&& TODO �� ��������� ���� ����� ��� ����*/ )
	{
		this->Freeze();
		whPanel_Filter* pnl=new whPanel_Filter(this);
		pnl->SetLogQueryData(&m_LogQueryData,gid);
		m_AuiMgr.AddPane(pnl, wxAuiPaneInfo().Caption(pnl->GetName()).Name(pnl->GetName()).Top()
						.Row(1).Position(gid+1).MinSize(350,100).DestroyOnClose(true)        );
		m_AuiMgr.Update();
		this->Thaw();
	}
}
//---------------------------------------------------------------------------
// ��������� ������ �������� ������
void whPnlCtrl_MovHistory::OnCreateFilterGroup(wxCommandEvent& evt )	
{	
	unsigned int gid=m_LogQueryData.m_Filter.size();
	m_LogQueryData.m_Filter.push_back(whGroupFilter());
	CreateFilterGroupGUI(gid);
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::OnPaneClose(wxAuiManagerEvent& evt)
{
	
	wxString str= evt.pane->name.substr(0,12);
	if (str == "Filter group")
    {
        int res = wxMessageBox(wxT("Are you sure you want to close/hide this pane?"),
                               wxT("wxAUI"),
                               wxYES_NO,
                               this);
        if (res != wxYES)
            evt.Veto();
		else
		{
			
			/*
			str = evt.pane->name.AfterLast(' ');
			unsigned long n=0;
			if ( str.ToULong(&n))
			{
				whQueryFilter &filter=	m_View->GetQueryFilterRef(); 
				filter.eraseFilterGroup(n);

				whQueryFilter &filter2=	m_View->GetQueryFilterRef(); 
			}
			*/

		
		}//if (res != wxYES)
    }//if (str == "Filter group")
	
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::SetLogQueryData(const whLogQueryData& data)
{
	m_LogQueryData = data;	// ������� ����� ������
	UpdateView();
}
//---------------------------------------------------------------------------
void whPnlCtrl_MovHistory::UpdateView()
{
	this->Freeze();
	ClearFilterGroup();		// ������� ��� ������������ �������

	//������ �������� ��� ���� �����
	for(unsigned int i=0;i<m_LogQueryData.m_Filter.size();++i)
		CreateFilterGroupGUI(i);

	m_ctrlSorter->SetLogQueryData(&m_LogQueryData);	//��������� ������ � �������� ����������
	m_View->SetLogQueryData(&m_LogQueryData);		//��������� ������ � ������� ?? �� ������ ������ :), ��� �������
	m_View->UpdateView();
	
	m_AuiMgr.Update();
	this->Thaw();
}
//---------------------------------------------------------------------------
// ��������� ������ �������� ������
void whPnlCtrl_MovHistory::OnMore(wxCommandEvent& evt )	
{	
	m_View->SetViewCompact( evt.IsChecked() );
	m_View->UpdateView();
}
