#ifndef __INOTEBOOKVIEW_H
#define __INOTEBOOKVIEW_H

#include "IMVP.h"
#include "db_rec.h"

namespace mvp{
//---------------------------------------------------------------------------
class INotebookView :public IView
{
public:

	virtual void AddPage(wxWindow*, const wxString& lbl, const wxIcon&) = 0;
	virtual void DelPage(wxWindow*) = 0;

	virtual void UpdatePageCaption(wxWindow* wnd, const wxString& lbl, const wxIcon& icon) = 0;

	using SigClosePage = sig::signal<void(const INotebookView* pm, wxWindow* page)>;
	SigClosePage sigClosePage;

	template <class PAGE_INFO>
	using SigMakePage = sig::signal<void(const PAGE_INFO&)>;

	SigMakePage<wh::rec::PageUser>		sigMakePageUser;
	SigMakePage<wh::rec::PageGroup>		sigMakePageGroup;
	SigMakePage<wh::rec::PageProp>		sigMakePageProp;
	SigMakePage<wh::rec::PageAct>		sigMakePageAct;
	SigMakePage<wh::rec::PageObjByPath> sigMakePageObjByPath;
	SigMakePage<wh::rec::PageObjByType> sigMakePageObjByType;
	SigMakePage<wh::rec::PageHistory>	sigMakePageHistory;
	SigMakePage<wh::rec::PageReport>	sigMakePageReport;

};
} //namespace mvp{

#endif // __INOTEBOOKVIEW_H