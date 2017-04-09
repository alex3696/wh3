#include "_pch.h"
#include "ViewNotebook.h"

using namespace wh;

//-----------------------------------------------------------------------------
ViewNotebook::ViewNotebook(const std::shared_ptr<IViewWindow>& parent)
	:IViewNotebook(), mNotebook(new wxAuiNotebook(parent->GetWnd()))
{
	mNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, 
		&ViewNotebook::OnEvt_ClosePage, this);
}
//-----------------------------------------------------------------------------

ViewNotebook::ViewNotebook(wxWindow* parent_wnd)
	:IViewNotebook(), mNotebook(new wxAuiNotebook(parent_wnd))
{
	mNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
	&ViewNotebook::OnEvt_ClosePage, this);

}
//-----------------------------------------------------------------------------

wxWindow* ViewNotebook::GetWnd()const
{
	return mNotebook;
}
//-----------------------------------------------------------------------------

void ViewNotebook::OnCmd_MkWindow(wxCommandEvent& evt)
{
	sigMkWindow("FactoryWindowUser");
}
//-----------------------------------------------------------------------------

void ViewNotebook::OnEvt_ClosePage(wxAuiNotebookEvent& evt)
{
	int page_to_close = evt.GetSelection();
	wxWindow* wnd_to_close = mNotebook->GetPage(page_to_close);
	sigRmWindow(wnd_to_close);
	evt.Veto();
}
//-----------------------------------------------------------------------------

void ViewNotebook::MkPage(wxWindow* wnd)
{
	mNotebook->AddPage(wnd, wxEmptyString);
}
//-----------------------------------------------------------------------------

void ViewNotebook::ChPage(wxWindow* wnd, const wxString& lbl, const wxIcon& icon)
{
	auto page_idx = mNotebook->GetPageIndex(wnd);
	if (wxNOT_FOUND != page_idx)
	{
		mNotebook->SetPageBitmap(page_idx, icon);
		mNotebook->SetPageText(page_idx, lbl);
		mNotebook->Update();
	}
}
//-----------------------------------------------------------------------------

void ViewNotebook::RmPage(wxWindow* wnd)
{
	auto page_idx = mNotebook->GetPageIndex(wnd);
	BOOST_ASSERT_MSG(wxNOT_FOUND != page_idx, "page not found?!");
	mNotebook->DeletePage(page_idx);
}
//-----------------------------------------------------------------------------

void ViewNotebook::ShowPage(wxWindow* wnd)
{
	auto page_idx = mNotebook->GetPageIndex(wnd);
	BOOST_ASSERT_MSG(wxNOT_FOUND != page_idx, "page not found?!");
	mNotebook->ChangeSelection(page_idx);
}