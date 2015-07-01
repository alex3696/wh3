#ifndef __VUSERCTRLPANEL_H
#define __VUSERCTRLPANEL_H

#include "TViewCtrlPanel.h"
#include "DUserEditor.h"


//-----------------------------------------------------------------------------
namespace wh{
namespace view{


typedef view::TViewCtrlPanel<CtrlTool::All, DUserEditor, true>	VUserCtrlPanel;

//-----------------------------------------------------------------------------

}//namespace view
}//namespace wh
#endif // __****_H