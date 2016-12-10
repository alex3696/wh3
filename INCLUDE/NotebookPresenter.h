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

	virtual IView* MakeView()override;
	virtual void SetView(IView* view)override;
	virtual void SetModel(const std::shared_ptr<IModel>& model)override;
	virtual std::shared_ptr<IModel> GetModel() override;
	virtual IView* GetView() override;

	// On signal from GUI
	template <class PAGE_INFO>
	void DoAddPage(const PAGE_INFO& pi)
	{
		if (mModel)
			mModel->MakePage(pi);
	}
	void DoDelPage(unsigned int page_index);
	void DoDelPage(wxWindow* wnd);
	void DoDelPage(IView* wnd);

	std::shared_ptr<IPresenter> GetPagePresenter(int i);

	void OnModelSig_UpdateCaption(PagePresenter* pres, const wxString& lbl, const wxIcon& icon);
private:
	// On signal from MODEL
	void OnModelSig_AddPage(const NotebookModel& nb, const std::shared_ptr<PageModel>& pg);
	void OnModelSig_DelPage(const NotebookModel&, const std::shared_ptr<PageModel>& pg);

	int FindIndex(IView* view);
	int FindIndex(wxWindow* wnd);

	
	std::shared_ptr<NotebookModel> mModel = nullptr;
	INotebookView* mView = nullptr;

	
	std::vector< std::shared_ptr<PagePresenter>> mPagePresenters;

	sig::scoped_connection connAI;
	sig::scoped_connection connBD;
};
//---------------------------------------------------------------------------







} //namespace mvp{
#endif // __MAINNOTEBOOKMVP_H


