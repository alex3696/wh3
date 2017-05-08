#include "_pch.h"
#include "ViewObjPropList.h"

using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewObjPropList::ViewObjPropList(wxWindow* parent)
{
	wxSizer *mainSz = new wxBoxSizer(wxVERTICAL);
	mPanel = new wxPanel(parent, wxID_ANY);

	mPG = new wxPropertyGrid(mPanel, wxID_ANY);
	mainSz->Add(mPG, 1, wxALL | wxEXPAND, 0);
	
	mPanel->SetSizer(mainSz);
	mPanel->Layout();
}
//-----------------------------------------------------------------------------
ViewObjPropList::ViewObjPropList(std::shared_ptr<IViewWindow> parent)
	:ViewObjPropList(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------

wxWindow* ViewObjPropList::GetWnd()const
{
	return mPanel;
}
//-----------------------------------------------------------------------------
//virtual 
void ViewObjPropList::SetPropList(const PropValTable& rt, const IAct*) //override;
{
	auto p0 = GetTickCount();

	wxLogMessage(wxString::Format("%d \t SetPropList : ", GetTickCount() - p0));
}
//-----------------------------------------------------------------------------
void ViewObjPropList::OnCmd_Update(wxCommandEvent& evt)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker lock(mPanel);

}
//-----------------------------------------------------------------------------
//IViewWindow virtual 
void ViewObjPropList::OnShow()//override 
{
	//OnCmd_Update(); 
}
//-----------------------------------------------------------------------------
