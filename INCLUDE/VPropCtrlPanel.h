#ifndef __VPROPCTRLPANEL_H
#define __VPROPCTRLPANEL_H

#include "TViewCtrlPanel.h"
#include "DPropEditor.h"


//-------------------------------------------------------------------------------------------------
namespace wh{
namespace view{

typedef TViewCtrlPanel<CtrlTool::All, DPropEditor, true>	VPropCtrlPanel;

//-------------------------------------------------------------------------------------------------

}//namespace view
}//namespace wh
#endif // __****_H