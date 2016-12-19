#include "_pch.h"
//#include "PagePresenter.h"
#include "NotebookPresenter.h"

#include "VObjCatalogCtrl.h"

using namespace mvp;



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
	connUpdatePageCaption.disconnect();

	mModel = std::dynamic_pointer_cast<PageModel>(model);

	if (!mModel)
		return;

	auto onUpdatePageCaption = std::bind(&PagePresenter::OnSigUpdatePageCaption, this
		, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	connUpdatePageCaption = mModel->sigUpdateCaption.connect(onUpdatePageCaption);

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