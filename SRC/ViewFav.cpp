#include "_pch.h"
#include "ViewFav.h"
using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewFav::ViewFav(const std::shared_ptr<IViewWindow>& parent)
	:ViewFav(parent->GetWnd())
{
}
//-----------------------------------------------------------------------------
ViewFav::ViewFav(wxWindow* parent)
{
	auto mgr = ResMgr::GetInstance();

	mPanel = new wxDialog(parent, wxID_ANY, "Отметьте избранные свойства"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	mPanel->SetIcon(mgr->m_ico_favprop_select24);


}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::OnShow()
{
	mPanel->ShowModal();
}
//-----------------------------------------------------------------------------
//virtual 
void ViewFav::SetUpdate(const std::vector<const IIdent64*>&
	, const IIdent64*, bool) //override;
{

}