#ifndef __DETAILACTCTRL_H
#define __DETAILACTCTRL_H

#include "VTable.h"
#include "dlg_move_model_MovableObj.h"
#include "dlg_act_model_Obj.h"

namespace wh{
//-----------------------------------------------------------------------------
class DetailActCtrl
{
public:
	DetailActCtrl();
	
	typedef std::function<void(wxCommandEvent&)> TCmdFn;

	const TCmdFn* GetCmdFunction(int cmd_id)const;
	void SetCmdFunction(int cmd_id, TCmdFn& cmd_fn);

private:
	std::map<int, std::function<void(wxCommandEvent&)> > mFn;

	wxWindow*		mPanel = nullptr;;
	wxAuiToolBar*	mToolBar = nullptr;
};

//-----------------------------------------------------------------------------
}//namespace wh
#endif //__*_H