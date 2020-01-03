#ifndef __CTRL_ACTBROWSER_H
#define __CTRL_ACTBROWSER_H

#include "CtrlWindowBase.h"
#include "ViewActBrowser.h"
#include "ModelActBrowser.h"

namespace wh{

//-----------------------------------------------------------------------------
class CtrlActBrowser final : public CtrlWindowBase<ViewActBrowser, ModelActBrowserWindow>
{
	sig::scoped_connection connModel_BeforeRefresh;
	sig::scoped_connection connModel_AfterRefresh;

	sig::scoped_connection connViewCmd_Activate;
public:
	CtrlActBrowser(const std::shared_ptr<ViewActBrowser>& view
		, const  std::shared_ptr<ModelActBrowserWindow>& model);

	void Activate(int64_t aid);

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H