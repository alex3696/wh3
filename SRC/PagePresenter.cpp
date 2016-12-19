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
	mModel = std::dynamic_pointer_cast<PageModel>(model);

	if (!mModel)
		return;
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
