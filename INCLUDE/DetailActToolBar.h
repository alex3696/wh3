#ifndef __DETAILACTTOOLBAR_H
#define __DETAILACTTOOLBAR_H

#include "globaldata.h"
#include "DetailActCtrl.h"

namespace wh{

class DetailActToolBar:
	public wxAuiToolBar
	,public ctrlWithResMgr

{
public:
	DetailActToolBar(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxAUI_TB_DEFAULT_STYLE);

	void SetCtrl(const DetailActCtrl& ctrl);
	void Build();
private:
	bool ConnCmdCtrl(int cmd_id, const DetailActCtrl& ctrl);

};






}//namespace wh
#endif //__*_H