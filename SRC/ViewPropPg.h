#ifndef __VIEW_PROPPG_H
#define __VIEW_PROPPG_H

#include "CtrlWindowBase.h"
#include "ModelPropTable.h"

namespace wh {
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ViewPropPg : public IViewWindow
{
	
public:
	ViewPropPg(wxWindow* parent);
	ViewPropPg(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override;
	
	void SetBeforeRefresh(std::shared_ptr<const ModelPropTable>);
	void SetAfterRefresh(std::shared_ptr<const ModelPropTable>);
protected:

private:
	wxPropertyGrid*	mPG;
};







} //namespace mvp{
#endif // __IMVP_H