#ifndef __DETAILACTCTRL_H
#define __DETAILACTCTRL_H

#include "VTableCtrl.h"
#include "DetailActToolBar.h"
#include "detail_model.h"
#include "ObjDetailPGView.h"



namespace wh{
//-----------------------------------------------------------------------------
class DetailActCtrl
	:public VTableCtrl
{
public:
	DetailActCtrl();

	std::function<void(wxCommandEvent&)> fnOnCmdMove;
	std::function<void(wxCommandEvent&)> fnOnCmdMoveHere;
	std::function<void(wxCommandEvent&)> fnOnCmdAction;

	void OnCmdMove(wxCommandEvent& WXUNUSED(evt));
	void OnCmdMoveHere(wxCommandEvent& WXUNUSED(evt));
	void OnCmdAction(wxCommandEvent& WXUNUSED(evt));
	void OnCmdLoad(wxCommandEvent& WXUNUSED(evt));

	void SetObjModel(std::shared_ptr<detail::model::Obj> model);

	void SetObjView(detail::view::ObjDetailPGView* objview);
	void SetActToolbar(DetailActToolBar* act_toolbar);
protected:
	std::shared_ptr<detail::model::Obj> mObjModel;

	detail::view::ObjDetailPGView*	mObjDetailView = nullptr;
	DetailActToolBar*	mActToolBar = nullptr;

	virtual wxAcceleratorTable GetAcceleratorTable()const override;
	void BindCmd(wxWindow* wnd);
	void UnbindCmd(wxWindow* wnd);

};


//-----------------------------------------------------------------------------
}//namespace wh
#endif //__*_H