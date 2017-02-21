#ifndef __CTRLNOTEBOOK_H
#define __CTRLNOTEBOOK_H


#include "IViewNotebook.h"
#include "ModelNotebook.h"
#include "CtrlWindowBase.h"

namespace wh{
//-----------------------------------------------------------------------------
class CtrlNotebook : public CtrlWindowBase<IViewNotebook, ModelNotebook>
{
	sig::scoped_connection connViewMkWindow;
	sig::scoped_connection connViewRmWindow;
	sig::scoped_connection connViewShowWindow;

	sig::scoped_connection connModelAfterMkWindow;
	sig::scoped_connection connModelAfterChWindow;
	sig::scoped_connection connModelBeforeRmWindow;
	sig::scoped_connection connModelShowWindow;

	void OnSig_MkChild(ICtrlWindow*);
	void OnSig_RmChild(ICtrlWindow*);
	void OnSig_ShowChild(ICtrlWindow*);
	void OnSig_ChTitleChild(ICtrlWindow*, const wxString&, const wxIcon&);
	
public:
	CtrlNotebook(std::shared_ptr<IViewNotebook> view, std::shared_ptr<ModelNotebook> model);

	// command to model
	void MkWindow(const wxString& wi);
	void RmWindow(wxWindow* wnd);
	void ChWindow(wxWindow* wnd, const wxString& title, const wxIcon& ico);
	void ShowWindow(wxWindow* wnd);

	//ICtrlWindow
	virtual void Load(const boost::property_tree::ptree& val)override;
	//virtual void Save(boost::property_tree::ptree& val)override;

	

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __INOTEBOOKVIEW_H