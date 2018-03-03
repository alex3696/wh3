#ifndef __VIEW_FAV_H
#define __VIEW_FAV_H

#include "IViewFav.h"

namespace wh {
//-----------------------------------------------------------------------------
class ViewFav : public IViewFav
{
	wxDialog*		mPanel;
	wxAuiToolBar*	mToolBar;
	wxDataViewCtrl* mTable;

	const ICls64* GetSelectedItemCls()const;
protected:
	void OnCmd_AddClsProp(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddObjProp(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddPrevios(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddPeriod(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddNext(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_AddLeft(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Remove(wxCommandEvent& evt = wxCommandEvent());
public:
	ViewFav(wxWindow* parent);
	ViewFav(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}

	virtual void SetShow() override;
	virtual void SetUpdateTitle(const wxString&, const wxIcon&)override;

	virtual void SetBeforeUpdate(const std::vector<const ICls64*>&, const ICls64&) override;
	virtual void SetAfterUpdate(const std::vector<const ICls64*>&, const ICls64&) override;


};








//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H