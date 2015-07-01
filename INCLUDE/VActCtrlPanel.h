#ifndef __VACTCTRLPANEL_H
#define __VACTCTRLPANEL_H

#include "TViewCtrlPanel.h"
#include "DActEditor.h"


//-----------------------------------------------------------------------------
namespace wh{
namespace view{


typedef view::TViewCtrlPanel<CtrlTool::All, DActEditor, true>	VActCtrlPanel;

//-----------------------------------------------------------------------------

}//namespace view
}//namespace wh
#endif // __****_H