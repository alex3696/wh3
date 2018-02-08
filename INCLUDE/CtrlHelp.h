#ifndef __CTRL_HELP_H
#define __CTRL_HELP_H

//#include "_pch.h"

namespace wh {
//-----------------------------------------------------------------------------
class CtrlHelp final
{
	wxString mHelpPath;
public:
	CtrlHelp();
	void Show(const wxString& index);
};






//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H