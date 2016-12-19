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
void MakeWindow(const PageModel& model, wxWindow* parent_wnd, IPageView* pv)
{
	auto wh_model = model.GetWhModel();
	if (std::dynamic_pointer_cast<PAGE_WHMODEL>(wh_model))
	{
		auto wnd = new PAGE_WND(parent_wnd);
		wnd->SetModel(wh_model);
		pv->SetWnd(wnd);
	}
}
//---------------------------------------------------------------------------

IPageView* ViewFactory::MakePage(IPresenter* presenter)
{
	auto notebook_presenter = presenter->GetParent();
	if (!notebook_presenter)
		return nullptr;
	auto notebook_view = notebook_presenter->GetView();
	if (!notebook_view)
		return nullptr;
	auto notebook_wnd = notebook_view->GetWnd();
	if (!notebook_wnd)
		return nullptr;
	auto page_model = std::dynamic_pointer_cast<PageModel>(presenter->GetModel());
	if (!page_model)
		return nullptr;

	IPageView* pv = new IPageView(presenter);
	using namespace wh;

	MakeWindow<MGroupArray, view::VGroupCtrlPanel>(*page_model, notebook_wnd, pv);
	MakeWindow<MUserArray, view::VUserCtrlPanel>(*page_model, notebook_wnd, pv);
	MakeWindow<MActArray, view::VActCtrlPanel>(*page_model, notebook_wnd, pv);
	MakeWindow<MPropTable, VTablePanel>(*page_model, notebook_wnd, pv);
	MakeWindow<MLogTable, VTablePanel>(*page_model, notebook_wnd, pv);
	MakeWindow<object_catalog::MObjCatalog, view::VObjCatalogCtrl>(*page_model, notebook_wnd, pv);

	MakeWindow<wh::detail::model::Obj, wh::detail::view::CtrlPnl>(*page_model, notebook_wnd, pv);

	presenter->SetView(pv);

	return pv;
}