#include "_pch.h"
#include "ViewFilterList.h"
#include "globaldata.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class wxAuiPanel : public wxPanel
{
public:
	wxAuiPanel(wxWindow* wnd)
		:wxPanel(wnd)
	{
		mAuiMgr.SetManagedWindow(this);
	}
	~wxAuiPanel()
	{
		mAuiMgr.UnInit();
	}
	wxAuiManager	mAuiMgr;
};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(wxWindow* parent)
{
	auto panel = new wxAuiPanel(parent);

	mAuiMgr = &panel->mAuiMgr;
	mPanel = panel;

	auto face_colour = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE);
	//auto face_colour = *wxRED;
	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);

	panel->mAuiMgr.GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, face_colour);


	mPG = new wxPropertyGrid(mPanel);
	panel->mAuiMgr.AddPane(mPG, wxAuiPaneInfo().
		Name("PropertyGrid").CaptionVisible(false)
		.CenterPane()
		.PaneBorder(false)
		);

	mPG->Append(new wxStringProperty("test"));

	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_Update, this, wxID_REFRESH);
	mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED, &ViewFilterList::OnCmd_UpdateAll, this, wxID_SETUP);
	
	mAuiMgr->Update();
}
//-----------------------------------------------------------------------------
ViewFilterList::ViewFilterList(std::shared_ptr<IViewWindow> parent)
	:ViewFilterList(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------

wxWindow* ViewFilterList::GetWnd()const
{
	return mPanel;
}

//-----------------------------------------------------------------------------
//virtual 
void ViewFilterList::Update(const std::vector<NotyfyItem>& data) //override;
{
	auto p0 = GetTickCount();



	wxLogMessage(wxString::Format("%d \t ViewFilterList : \t Update", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
void ViewFilterList::OnCmd_UpdateAll(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	sigUpdateAll();
}
//-----------------------------------------------------------------------------
void ViewFilterList::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

	sigUpdateAll();
}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewFilterList::OnShow()//override 
{
	//OnCmd_Update(); 
}
