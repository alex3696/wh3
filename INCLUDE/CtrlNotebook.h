#ifndef __CTRLNOTEBOOK_H
#define __CTRLNOTEBOOK_H


#include "IViewNotebook.h"
#include "ModelNotebook.h"
#include "ICtrlWindow.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlNotebook : public ICtrlWindow
{
	sig::scoped_connection connViewMkWindow;
	sig::scoped_connection connViewRmWindow;
	sig::scoped_connection connViewShowWindow;

	sig::scoped_connection connModelAfterMkWindow;
	sig::scoped_connection connModelAfterChWindow;
	sig::scoped_connection connModelBeforeRmWindow;
	sig::scoped_connection connModelShowWindow;
	
	
	std::shared_ptr<IViewNotebook> mView;
	std::shared_ptr<ModelNotebook> mModel;

public:
	CtrlNotebook(std::shared_ptr<IViewNotebook> view, std::shared_ptr<ModelNotebook> model);

	// command to model
	void MkWindow(const wxString& wi);
	void RmWindow(wxWindow* wnd);
	void ChWindow(wxWindow* wnd, const wxString& title, const wxIcon& ico);
	void ShowWindow(wxWindow* wnd);
	//std::shared_ptr<IViewNotebook> GetView()const{ return mView; }


	virtual void UpdateTitle()override {};
	virtual void Show()override {};
	virtual void Close()override {};
	virtual void Load(const boost::property_tree::ptree& val)override 
	{
		mModel->Load(val);
	}
	virtual void Save(boost::property_tree::ptree& val)override 
	{
		mModel->Save(val);
	};

	std::shared_ptr<IViewWindow> GetView()const override { return mView; }

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H