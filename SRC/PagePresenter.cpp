#include "_pch.h"
//#include "PagePresenter.h"
#include "NotebookPresenter.h"

#include "VGroupCtrlPanel.h"
#include "VUserCtrlPanel.h"
#include "VActCtrlPanel.h"
//#include "VPropCtrlPanel.h"
#include "detail_ctrlpnl.h"

#include "VTablePanel.h"
#include "MHistory.h"
#include "MProp2.h"

#include "VObjCatalogCtrl.h"

#include "config.h"
#include "AppViewConfig.h"

using namespace mvp;

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
PagePresenter::PagePresenter(IPresenter* presenter)
	:IPresenter(presenter)
{}
//---------------------------------------------------------------------------
PagePresenter::~PagePresenter()
{
	SetView(nullptr);
}

//---------------------------------------------------------------------------

IView* PagePresenter::MakeView()
{
	SetView(nullptr);
	
	if (mModel && mModel->GetWhModel())
	{
		auto notebook_presenter = this->GetParent();
		if (!notebook_presenter)
			return mView.get();
		auto notebook_view = notebook_presenter->GetView();
		if (!notebook_view)
			return mView.get();

		auto wh_model = mModel->GetWhModel();
		//auto new_page_view = IPageView::MakeView(this, wh_model);
		IPageView* pv = nullptr;
		auto notebook_wnd = notebook_view->GetWnd();
		if (notebook_wnd)
		{
			pv = new IPageView(this);

			using namespace wh;

			MakeWindow<MGroupArray, view::VGroupCtrlPanel>(*mModel, notebook_wnd, pv);
			MakeWindow<MUserArray, view::VUserCtrlPanel>(*mModel, notebook_wnd, pv);
			MakeWindow<MActArray, view::VActCtrlPanel>(*mModel, notebook_wnd, pv);
			MakeWindow<MPropTable, VTablePanel>(*mModel, notebook_wnd, pv);
			MakeWindow<MLogTable, VTablePanel>(*mModel, notebook_wnd, pv);
			MakeWindow<object_catalog::MObjCatalog, view::VObjCatalogCtrl>(*mModel, notebook_wnd, pv);

			MakeWindow<wh::detail::model::Obj, wh::detail::view::CtrlPnl>(*mModel, notebook_wnd, pv);

			/*
			if (std::dynamic_pointer_cast<wh::MGroupArray>(wh_model))
			{
			auto new_wnd = new wh::view::VGroupCtrlPanel(parent_wnd);
			new_wnd->SetModel(wh_model);
			pv->SetWnd(new_wnd);
			}
			*/

		}//if (notebook_wnd)
		SetView(pv);
		auto onUpdatePageCaption = std::bind(&PagePresenter::OnSigUpdatePageCaption,this
			,std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		connUpdatePageCaption = mModel->sigUpdateCaption.connect(onUpdatePageCaption);
	}
	return mView.get();
}
//---------------------------------------------------------------------------

void PagePresenter::SetView(IView* view) 
{
	mView.reset(dynamic_cast<IPageView*>(view));

	if (!mModel || !mView)
		return;

	auto wnd = dynamic_cast<wh::view::VObjCatalogCtrl*>(mView->GetWnd());
	if (wnd)
		wnd->HideCatalogSelect(true);
}
//---------------------------------------------------------------------------

void PagePresenter::SetModel(const std::shared_ptr<IModel>& model)
{
	mModel = std::dynamic_pointer_cast<PageModel>(model);
};
//---------------------------------------------------------------------------
std::shared_ptr<IModel> PagePresenter::GetModel() 
{ 
	return mModel; 
}
//---------------------------------------------------------------------------

IView* PagePresenter::GetView()
{ 
	return mView.get(); 
}
//---------------------------------------------------------------------------
void PagePresenter::OnSigUpdatePageCaption(const PageModel& pm, const wxString& lbl, const wxIcon& icon)
{
	auto notebook_presenter = dynamic_cast<NotebookPresenter*>(this->GetParent());
	notebook_presenter->OnModelSig_UpdateCaption(this, lbl, icon);
}