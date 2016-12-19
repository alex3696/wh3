#ifndef __MAINNOTEBOOKMVP_H
#define __MAINNOTEBOOKMVP_H

#include "NotebookModel.h"
#include "INotebookView.h"

#include "PagePresenter.h"


namespace mvp{

//---------------------------------------------------------------------------
class NotebookPresenter
	: public IPresenter
{
public:
	NotebookPresenter(IPresenter* presenter);
	~NotebookPresenter();

	virtual void SetView(IView* view)override;
	virtual void SetModel(const std::shared_ptr<IModel>& model)override;
	virtual std::shared_ptr<IModel> GetModel() override;
	virtual IView* GetView() override;

	// command to model
	template <class PAGE_INFO>
	void DoAddPage(const PAGE_INFO& pi)
	{
		if (mModel)
			mModel->MakePage(pi);
	}
	void DoDelPage(unsigned int page_index);
	void DoDelPage(wxWindow* wnd);

	void Load(){ mModel->Load(); };
	void Save(){ mModel->Save(); };

private:
	// On signal from MODEL -> command for update view
	void OnModelSig_AddPage(const NotebookModel& nb, const std::shared_ptr<PageModel>& pg);
	void OnModelSig_DelPage(const NotebookModel& nb, const std::shared_ptr<PageModel>& pg);

	int FindIndex(const std::function<bool(PagePresenter&)> &fn);
	int FindIndex(wxWindow* wnd);

	
	std::shared_ptr<NotebookModel> mModel = nullptr;
	INotebookView* mView = nullptr;

	
	std::vector< std::shared_ptr<PagePresenter>> mPagePresenters;

	std::map< std::shared_ptr<PagePresenter>, sig::scoped_connection> mPagePresentersConn;


	sig::scoped_connection connAI;
	sig::scoped_connection connBD;

	sig::scoped_connection view_connBD;
};
//---------------------------------------------------------------------------







} //namespace mvp{
#endif // __MAINNOTEBOOKMVP_H


