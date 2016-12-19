#include "_pch.h"
#include "NotebookPresenter.h"
#include "ViewFactory.h"

using namespace mvp;

//---------------------------------------------------------------------------
NotebookPresenter::NotebookPresenter(IPresenter* presenter)
	:IPresenter(presenter)
{}
//---------------------------------------------------------------------------
NotebookPresenter::~NotebookPresenter()
{
	mPagePresenters.clear();
	delete mView;
}
//---------------------------------------------------------------------------
//virtual 
void NotebookPresenter::SetView(IView* view)//override
{ 
	mView = dynamic_cast<INotebookView*>(view); 
}
//---------------------------------------------------------------------------
//virtual 
void NotebookPresenter::SetModel(const std::shared_ptr<IModel>& model)//override
{ 
	namespace ph = std::placeholders;
	if (mModel)
	{

		if (mView)
		{
			for (size_t i = 0; i < mModel->GetPageQty(); i++)
			{
				auto page_model = mModel->GetPageModel(i);
				OnModelSig_DelPage(*mModel.get(), page_model);
			}
		}


		connAI.disconnect();
		connBD.disconnect();

		mPagePresenters.clear();
		mModel.reset();
	}


	mModel = std::dynamic_pointer_cast<NotebookModel>(model);
	if (mModel)
	{
		auto fnAI = std::bind(&NotebookPresenter::OnModelSig_AddPage, this, ph::_1, ph::_2);
		connAI = mModel->sigAfterAddPage.connect(fnAI);

		auto fnBD = std::bind(&NotebookPresenter::OnModelSig_DelPage, this, ph::_1, ph::_2);
		connBD = mModel->sigBeforeDelPage.connect(fnBD);
		if (mView)
		{
			for (size_t i = 0; i < mModel->GetPageQty(); i++)
			{
				OnModelSig_AddPage(*mModel.get(), mModel->GetPageModel(i));
			}//for 
		}//if (mView)
	}//if (mModel)
		

}
//---------------------------------------------------------------------------
//virtual 
std::shared_ptr<IModel> NotebookPresenter::GetModel() //override
{
	return mModel;
}
//---------------------------------------------------------------------------
//virtual 
IView* NotebookPresenter::GetView()// override
{
	return mView;
}
//---------------------------------------------------------------------------
void NotebookPresenter::DoDelPage(unsigned int page_index)
{
	if (mModel && page_index >= 0 && page_index<mPagePresenters.size())
		mModel->DelPage(page_index);
}
//---------------------------------------------------------------------------

void NotebookPresenter::DoDelPage(IView* view)
{
	DoDelPage(FindIndex(view));
}
//---------------------------------------------------------------------------

void NotebookPresenter::DoDelPage(wxWindow* wnd)
{
	DoDelPage(FindIndex(wnd));
}
//---------------------------------------------------------------------------

std::shared_ptr<IPresenter> NotebookPresenter::GetPagePresenter(int i)
{
	return mPagePresenters[i];
}
//---------------------------------------------------------------------------

void NotebookPresenter::OnModelSig_UpdateCaption(PagePresenter* pres, const wxString& lbl, const wxIcon& icon)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(mView->GetWnd());

	if (!pres || !pres->GetView() || !pres->GetView()->GetWnd() || !mView->GetWnd())
		return;

	mView->UpdatePageCaption(pres->GetView()->GetWnd(), lbl, icon);
}
//---------------------------------------------------------------------------
void NotebookPresenter::OnModelSig_AddPage(const NotebookModel& nb, const std::shared_ptr<PageModel>& pg)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(mView->GetWnd());
	if (!pg)
		return;

	auto pp = std::make_shared<PagePresenter>(this);
	pp->SetModel(pg);

	auto view = ViewFactory::MakePage(pp.get());

	//auto view = pp->MakeView();
	
	if (view && view->GetWnd())
	{
		mView->AddPage(view->GetWnd(), pg->GetTitle(), *pg->GetIcon());
		pg->GetWhModel()->Load();
		mPagePresenters.emplace_back(pp);
	}

	
}
//---------------------------------------------------------------------------

void NotebookPresenter::OnModelSig_DelPage(const NotebookModel&, const std::shared_ptr<PageModel>& pg)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(mView->GetWnd());

	auto it = mPagePresenters.begin();
	while (it != mPagePresenters.end())
	{
		//auto gmodel = std::dynamic_pointer_cast<PageModel>((*it)->GetModel());
		if (pg == (*it)->GetModel() )
			break;
		++it;
	}
	if (mView)
	{
		auto page_view = (*it)->GetView();
		if (page_view)
		{
			auto wnd = page_view->GetWnd();
			if (wnd)
				mView->DelPage(wnd);
		}
	}

	mPagePresenters.erase(it);
}
//---------------------------------------------------------------------------

int NotebookPresenter::FindIndex(IView* view)
{
	//find view
	int ret = -1;
	auto it = mPagePresenters.begin();
	while (it != mPagePresenters.end())
	{
		if ((*it)->GetView() == view)
		{
			ret = std::distance(mPagePresenters.begin(), it);
			break;
		}
		++it;
	}
	return ret;
}
//---------------------------------------------------------------------------
int NotebookPresenter::FindIndex(wxWindow* wnd)
{
	//find view
	int ret = -1;
	auto it = mPagePresenters.begin();
	while (it != mPagePresenters.end())
	{
		if ((*it)->GetView()->GetWnd() == wnd)
		{
			ret = std::distance(mPagePresenters.begin(), it);
			break;
		}
		++it;
	}
	return ret;
}




