#include "_pch.h"
#include "ViewHistorySetup.h"
#include "globaldata.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewHistorySetup::ViewHistorySetup(std::shared_ptr<IViewWindow> parent)
{
	auto panel = new wxDialog(parent->GetWnd(), wxID_ANY, "Настройки истории");
	mPanel = panel;


	//mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED
	//		, &ViewHistorySetup::OnCmd_SetCfgToPage, this, wxID_SAVE);
	//mPanel->Bind(wxEVT_COMMAND_MENU_SELECTED
	//	, &ViewHistorySetup::OnCmd_SetCfgToGlobal, this, wxID_SAVEAS);



}
//-----------------------------------------------------------------------------
//virtual 
wxWindow* ViewHistorySetup::GetWnd()const //override;
{
	return mPanel;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistorySetup::OnShow()//override
{

}
//-----------------------------------------------------------------------------
//virtual 
void ViewHistorySetup::SetCfg(const rec::PageHistory&) //override;
{
	
}
//-----------------------------------------------------------------------------
void ViewHistorySetup::OnCmd_SetCfgToPage(wxCommandEvent& evt)
{

}
//-----------------------------------------------------------------------------
void ViewHistorySetup::OnCmd_SetCfgToGlobal(wxCommandEvent& evt)
{

}

