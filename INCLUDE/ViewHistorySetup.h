#ifndef __VIEW_HISTORY_H
#define __VIEW_HISTORY_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewHistorySetup.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewHistorySetup final : public IViewHistorySetup
{
	wxWindow*		mPanel;
	wxPropertyGrid*	mPG;
public:
	ViewHistorySetup(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override;
	
	virtual void SetCfg(const rec::PageHistory&) override;
			
protected:
	void OnCmd_SetCfgToPage(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_SetCfgToGlobal(wxCommandEvent& evt = wxCommandEvent());
	
	virtual void OnShow()override;
private:

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H