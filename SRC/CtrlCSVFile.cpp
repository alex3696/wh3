#include "_pch.h"
#include "CtrlCSVFile.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

CtrlCSVFile::CtrlCSVFile()
{

	HKEY hKey;
	DWORD dwType = REG_SZ;
	wchar_t buf[255] = { 0 };
	DWORD dwBufSize = sizeof(buf);

	long lError = RegOpenKeyExW(HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\excel.exe",
		0,
		KEY_READ,
		&hKey);
	if (lError == ERROR_SUCCESS)
	{
		lError = RegQueryValueEx(hKey, L"Path", NULL, &dwType, (LPBYTE)buf, &dwBufSize);
		if (lError == ERROR_SUCCESS)
			mExcelApp = wxString(buf) + L"\\excel.exe";
		//wxString sexcel = "\"c:\\Program Files (x86)\\Microsoft Office\\Office14\\excel.exe\"";
	}
}
//---------------------------------------------------------------------------
void CtrlCSVFile::Open(const wxString& filename)
{
	wxFileName file;
	file.Assign(filename);

	wxString fpath = file.GetPath();
	wxString fname = file.GetFullName();

	HINSTANCE h;
	if (mExcelApp.IsEmpty())
		h = ShellExecuteW(NULL, L"open", filename, NULL, fpath, SW_SHOWNORMAL);
	else
		h = ShellExecuteW(NULL, L"open", mExcelApp, fname, fpath, SW_SHOWNORMAL);

	if (ERROR_FILE_NOT_FOUND == (long)h)
		return;

	if (ERROR_PATH_NOT_FOUND == (long)h)
		return;
}
