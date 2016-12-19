#include "_pch.h"
#include "ViewFactory.h"

#include "PageModel.h"

#include "VGroupCtrlPanel.h"
#include "VUserCtrlPanel.h"
#include "VActCtrlPanel.h"
#include "detail_ctrlpnl.h"

#include "VTablePanel.h"
#include "MHistory.h"
#include "MProp2.h"

#include "VObjCatalogCtrl.h"

#include "config.h"



using namespace mvp;
//---------------------------------------------------------------------------


template <class PAGE_WHMODEL, class PAGE_WND>
void MakeWindow(const wh::SptrIModel& wh_model, wxWindow* parent_wnd, IPageView* pv)
{
	if (std::dynamic_pointer_cast<PAGE_WHMODEL>(wh_model))
	{
		auto wnd = new PAGE_WND(parent_wnd);
		wnd->SetModel(wh_model);
		pv->SetWnd(wnd);
	}
}
//---------------------------------------------------------------------------
IPageView* ViewFactory::MakePage(const wh::SptrIModel& wh_model, wxWindow* notebook_wnd)
{
	IPageView* pv = new IPageView();
	using namespace wh;
	
	MakeWindow<MGroupArray, view::VGroupCtrlPanel>(wh_model, notebook_wnd, pv);
	MakeWindow<MUserArray, view::VUserCtrlPanel>(wh_model, notebook_wnd, pv);
	MakeWindow<MActArray, view::VActCtrlPanel>(wh_model, notebook_wnd, pv);
	MakeWindow<MPropTable, VTablePanel>(wh_model, notebook_wnd, pv);
	MakeWindow<MLogTable, VTablePanel>(wh_model, notebook_wnd, pv);
	MakeWindow<object_catalog::MObjCatalog, view::VObjCatalogCtrl>(wh_model, notebook_wnd, pv);
	MakeWindow<wh::detail::model::Obj, wh::detail::view::CtrlPnl>(wh_model, notebook_wnd, pv);

	return pv;
}
