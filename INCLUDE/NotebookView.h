#ifndef __NOTEBOOKVIEW_H
#define __NOTEBOOKVIEW_H

#include "INotebookView.h"
namespace mvp{
//---------------------------------------------------------------------------
class NotebookView
	: public INotebookView
{
public:
	NotebookView(wxWindow* wnd)
	{
		SetWnd(wnd);
	}

	NotebookView()
	{
	}

	virtual wxWindow* GetWnd() override{ return mNotebook; }
	virtual void SetWnd(wxWindow* wnd)override
	{
		mNotebook = dynamic_cast<wxAuiNotebook*>(wnd);
		if (!mNotebook)
			return;
		auto fnOnClosePage = [this](wxAuiNotebookEvent& evt)
		{
			int page_to_close = evt.GetSelection();
			wxWindow* wnd_to_close = mNotebook->GetPage(page_to_close);
			sigClosePage(this, wnd_to_close);
			evt.Veto();
		};
		mNotebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, fnOnClosePage);
	}

	virtual void AddPage(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) override
	{
		mNotebook->AddPage(wnd, lbl, true, icon);
	}
	virtual void DelPage(wxWindow* wnd) override
	{
		auto page_idx = mNotebook->GetPageIndex(wnd);
		BOOST_ASSERT_MSG(wxNOT_FOUND != page_idx, "page not found?!");
		mNotebook->DeletePage(page_idx);
	}

	virtual void UpdatePageCaption(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) override
	{
		wxBusyCursor busyCursor;
		wxWindowUpdateLocker	wndUpdateLocker(mNotebook);

		auto page_idx = mNotebook->GetPageIndex(wnd);
		if (wxNOT_FOUND != page_idx)
		{
			mNotebook->SetPageBitmap(page_idx, icon);
			mNotebook->SetPageText(page_idx, lbl);
			mNotebook->Update();
		}
	}

	wxAuiNotebook* mNotebook = nullptr;
};




} //namespace mvp{
#endif // __INOTEBOOKVIEW_H