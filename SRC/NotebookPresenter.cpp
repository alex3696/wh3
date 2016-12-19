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
	mPagePresentersConn.clear();
	mPagePresenters.clear();
	delete mView;
}
//---------------------------------------------------------------------------
//virtual 
void NotebookPresenter::SetView(IView* view)//override
{ 
	view_connBD.disconnect();
	mView = dynamic_cast<INotebookView*>(view); 
	if (!mView)
		return;

	auto fnDoClosePage = [this](const INotebookView* pm, wxWindow* page)
	{
		DoDelPage(page);
	};
	view_connBD = mView->ConnectSigDelPage(fnDoClosePage);
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
	if (mModel && page_index<mPagePresenters.size())
		mModel->DelPage(page_index);
}
//---------------------------------------------------------------------------

void NotebookPresenter::DoDelPage(wxWindow* wnd)
{
	int idx = FindIndex(wnd);
	if(idx >= 0)
		DoDelPage(idx);
}
//---------------------------------------------------------------------------

void NotebookPresenter::OnModelSig_AddPage(const NotebookModel& nb, const std::shared_ptr<PageModel>& pg)
{
	wxBusyCursor busyCursor;
	wxWindowUpdateLocker	wndUpdateLocker(mView->GetWnd());
	if (!pg)
		return;
	auto wh_model = pg->GetWhModel();
	if (!wh_model)
		return;
	auto notebook_wnd = mView ? mView->GetWnd() : nullptr;
	if (!notebook_wnd)
		return;

	auto pp = std::make_shared<PagePresenter>(this);
	auto view = ViewFactory::MakePage(wh_model, notebook_wnd);

	if (view && view->GetWnd())
	{
		pp->SetModel(pg);
		pp->SetView(view);

		mView->AddPage(view->GetWnd(), pg->GetTitle(), *pg->GetIcon());
		mPagePresenters.emplace_back(pp);

		// цепляем сигнал обновления от каждой странички
		auto page_wnd = view->GetWnd();
		auto fn1 = [this, page_wnd](const PageModel& pm, const wxString& lbl, const wxIcon& icon)
		{
			mView->UpdatePageCaption(page_wnd, lbl, icon);
		};
		mPagePresentersConn.emplace(std::make_pair(pp, pg->sigUpdateCaption.connect(fn1)));
		// загружаем все сведения
		wh_model->Load();

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

	mPagePresentersConn.erase(*it);
	mPagePresenters.erase(it);
}
//---------------------------------------------------------------------------

int NotebookPresenter::FindIndex(const std::function<bool(PagePresenter&)>& fn)
{
	int ret = -1;
	auto it = mPagePresenters.begin();
	while (it != mPagePresenters.end())
	{
		if (fn(**it))
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
	auto fn = [wnd](PagePresenter& pp)
	{
		return wnd == pp.GetView()->GetWnd() ;
	};
	return FindIndex(fn);
}


