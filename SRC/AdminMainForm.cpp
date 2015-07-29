#include "_pch.h"
#include "AdminMainForm.h"



//#include "ObjNavigatorPanel.h"

#include "VPropCtrlPanel.h"
#include "VActCtrlPanel.h"
#include "VGroupCtrlPanel.h"
#include "VUserCtrlPanel.h"

#include "VClsTreeCtrl.h"

#include "VClsFavActPropCtrl.h"

using namespace wh;
using namespace wh::gui;
using namespace wh::view;
using namespace boost::signals2;


AdminMainForm::AdminMainForm(	wxWindow* parent, 
					wxWindowID id,
					const wxString& title,
					const wxPoint& pos,
					const wxSize& size,
					long style,
					const wxString& name)
		:wxDialog(parent,id,title,pos,size,style,name)
{
	m_AuiMgr.SetManagedWindow(this);


	m_ObjTree = new wxTreeCtrl(this,wxID_ANY,wxDefaultPosition, wxDefaultSize,
		wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxNO_BORDER|wxTR_NO_LINES);
	m_AuiMgr.AddPane(m_ObjTree,wxAuiPaneInfo().
										Name("CategoryTreeItem").Caption("Category tree")
											.Left()
											 //.Layer(1).Position(1)
											 .MinSize(150,130)
											 .CloseButton(false)
											 .Dockable(true)
											 .Floatable(true)
											 );



	auto pnl = new wxPanel(this);
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	pnl->SetSizer( bSizer1 );
	

	m_AuiMgr.AddPane(pnl, wxAuiPaneInfo().
					Name("ContentPanel_1").Caption("Content Panel")
					.CenterPane()
					//.Layer(0).Position(1)
					.CloseButton(false)
					.Dockable(true)
					.Floatable(true)
					.CaptionVisible()
					);	



	ResMgr* mgr=ResMgr::GetInstance();
	wxImageList *images = new wxImageList(24, 24, true);
	images->Add(mgr->m_ico_db24);			// 0
	images->Add(mgr->m_ico_usergroup24);	// 1
	images->Add(mgr->m_ico_user24);			// 2
	
	images->Add(mgr->m_ico_folder_props24);	// 3
	images->Add(mgr->m_ico_folder_acts24);	// 4
	images->Add(mgr->m_ico_folder_type24);	// 5
	
	m_ObjTree->AssignImageList(images);


	wxTreeItemId root=m_ObjTree->GetRootItem();
	if(!root.IsOk())
		root= m_ObjTree->AddRoot("root");


	m_ObjTree->AppendItem(root, "NULL",0, 0);


	m_ObjTree->AppendItem(root, "VProp", 3, 3);
	m_ObjTree->AppendItem(root, "VAct", 4, 4);
	m_ObjTree->AppendItem(root, "VGroup", 1, 1);
	m_ObjTree->AppendItem(root, "VUser", 2, 2);

	m_ObjTree->AppendItem(root, "VClsTree", 5, 5);

	m_ObjTree->AppendItem(root, "VClsFavoriteActProp", 5, 5);
	

	
	m_ObjTree->ExpandAll();


	m_AuiMgr.Update();


	 

	m_ObjTree->Bind(wxEVT_COMMAND_TREE_SEL_CHANGING, &AdminMainForm::OnSelectCategoryTreeItem,this);
}
//---------------------------------------------------------------------------
AdminMainForm::~AdminMainForm()
{
	m_ObjTree->Unbind(wxEVT_COMMAND_TREE_SEL_CHANGING, &AdminMainForm::OnSelectCategoryTreeItem,this);

	auto pane = m_AuiMgr.GetPane(L"ContentPanel_1");
	pane.window->GetSizer()->Clear(true);


	m_AuiMgr.UnInit();

}

//---------------------------------------------------------------------------
void AdminMainForm::OnSelectCategoryTreeItem(wxTreeEvent& evt)
{
	wxSetCursor( *wxHOURGLASS_CURSOR );
	try
	{
		auto pane = m_AuiMgr.GetPane(L"ContentPanel_1");

		wxWindowUpdateLocker	wndDisabler(pane.window);
		
		//pane.window->GetSizer()->Clear(true);
		pane.window->GetSizer()->DeleteWindows();
		
		wxWindow* viewpanel(NULL);

		if ("VProp" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			auto model = std::make_shared<MPropArray>();
			auto view = new VPropCtrlPanel(pane.window);
						
			model->Load();
			view->SetModel(model);
			
			viewpanel = view;
		}
		
		else if ("VAct" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			auto model = std::make_shared<MActArray>();
			auto view = new VActCtrlPanel(pane.window);

			model->Load();
			view->SetModel(model);

			viewpanel = view;
		}

		else if ("VGroup" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			auto model = std::make_shared<MGroupArray>();
			auto view = new VGroupCtrlPanel(pane.window);
			model->Load();
			view->SetModel(model);
			viewpanel = view;
		}
		else if ("VUser" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			auto model = std::make_shared<MUserArray>();
			auto view = new VUserCtrlPanel(pane.window);
			model->Load();
			view->SetModel(model);
			viewpanel = view;
		}

		else if ("VClsTree" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			rec::Cls root("1", "Object");
			auto model = std::make_shared<MClsNode>();
			model->SetData(root);
			model->MarkSaved();

			auto view = new VClsTreeCtrl(pane.window);
			model->Load();
			view->SetModel(model);
			viewpanel = view;
		}

		else if ("VClsFavoriteActProp" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			rec::Cls root("1", "Object");
			auto model = std::make_shared<MClsFavorite>();
			model->SetData(root);
			model->MarkSaved();

			auto view = new VClsFavActPropCtrl(pane.window);
			model->Load();
			view->SetModel(model);
			viewpanel = view;
		}


		

		else if ("NULL" == m_ObjTree->GetItemText(evt.GetItem()))
		{
			auto view = new wxPanel(pane.window);
			viewpanel = view;
		}



		if(viewpanel)
			pane.window->GetSizer()->Add( viewpanel, 1, wxEXPAND | wxALL, 5 );
		pane.window->Layout();
				
	
	}//try
	catch(boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)) );
	}///catch(boost::exception & e)
	catch(...)
	{
		wxLogWarning(wxString("Unhandled exception") );
	}//catch(...)

	wxSetCursor( *wxSTANDARD_CURSOR );
}//OnSelectCategoryTreeItem





