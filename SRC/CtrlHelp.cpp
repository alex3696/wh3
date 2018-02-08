#include "_pch.h"
#include "CtrlHelp.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CtrlHelp::CtrlHelp()
	:mHelpPath(L"..\\..\\DOC\\Help\\")
{
	
}
//---------------------------------------------------------------------------
void CtrlHelp::Show(const wxString& index)
{
	const wxString full_path = mHelpPath + index + ".htm";

	const _TCHAR* path = full_path.wc_str();
	const _TCHAR* parametrs = L"";

	SHELLEXECUTEINFOW ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = path;
	ShExecInfo.lpParameters = parametrs;
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	BOOL ret = ShellExecuteExW(&ShExecInfo);
}
