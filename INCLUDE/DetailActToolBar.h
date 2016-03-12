#ifndef __DETAILACTTOOLBAR_H
#define __DETAILACTTOOLBAR_H

#include "globaldata.h"

namespace wh{
//-----------------------------------------------------------------------------
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

	void Build();

};
//-----------------------------------------------------------------------------

}//namespace wh
#endif //__*_H