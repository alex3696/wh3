#ifndef __IVIEW_HISTORY_SETUP_H
#define __IVIEW_HISTORY_SETUP_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelHistoryData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewHistorySetup : public IViewWindow
{
public:
	virtual void SetCfg(const rec::PageHistory&) = 0;
	virtual void ShowCfgWindow() = 0;

	sig::signal<void(const rec::PageHistory&)> sigSetCfg;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H