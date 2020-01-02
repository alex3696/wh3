#ifndef __CTRL_ACTBROWSER_H
#define __CTRL_ACTBROWSER_H

#include "CtrlWindowBase.h"
#include "ViewActBrowser.h"
#include "ModelActBrowser.h"

namespace wh{

//-----------------------------------------------------------------------------
class CtrlActBrowser final : public CtrlWindowBase<ViewActBrowser, ModelActBrowserWindow>
{
public:
	CtrlActBrowser(const std::shared_ptr<ViewActBrowser>& view
		, const  std::shared_ptr<ModelActBrowserWindow>& model);
	void Refresh();

};
//-----------------------------------------------------------------------------

} // namespace wh{
#endif // __*_H