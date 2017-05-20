#ifndef __VIEW_HISTORY_H
#define __VIEW_HISTORY_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewHistorySetup.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewHistorySetup final : public IViewHistorySetup
{
	wxDialog*		mPanel;
	wxPropertyGrid*	mPG;

	wxIntProperty*	mPGPRowsLimit;
	wxIntProperty*	mPGPRowsOffset;
	wxIntProperty*	mPGPStrPerRow;
	wxBoolProperty*	mPGPPathInProperties;
	wxBoolProperty*	mPGPColAutosize;
	wxBoolProperty*	mPGPShowPropertyList;
	wxBoolProperty*	mPGPShowFilterList;
public:
	ViewHistorySetup(std::shared_ptr<IViewWindow> parent);
	virtual wxWindow* GetWnd()const override;
	
	virtual void SetCfg(const rec::PageHistory&) override;
	virtual void ShowCfgWindow() override;
			
protected:
	void OnCmd_SetCfgToPage(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_SetCfgToGlobal(wxCommandEvent& evt = wxCommandEvent());
	
private:
	rec::PageHistory mCurrentCfg;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H