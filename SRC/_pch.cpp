#include "_pch.h"
//---------------------------------------------------------------------------
wxMenuItem* AppendBitmapMenu(wxMenu* menu,int id,const wxString& label,const wxBitmap &bmp)
{
	auto item = new wxMenuItem(menu,id,label);
	item->SetBitmap(bmp);	
	menu->Append(item);
	return item;
}
//---------------------------------------------------------------------------
wxString GetAppVersion()
{
	wxStandardPathsBase& stdp = wxStandardPaths::Get();
	auto wxFileName = stdp.GetExecutablePath();

	const wchar_t *pszFilePath = wxFileName.wc_str();

	DWORD               dwSize = 0;
	BYTE                *pbVersionInfo = NULL;
	VS_FIXEDFILEINFO    *pFileInfo = NULL;
	UINT                puLenFileInfo = 0;

	// get the version info for the file requested
	dwSize = GetFileVersionInfoSize(pszFilePath, NULL);
	if (dwSize == 0)
	{
		printf("Error in GetFileVersionInfoSize: %d\n", GetLastError());
		return "version error";
	}

	pbVersionInfo = new BYTE[dwSize];

	if (!GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo))
	{
		printf("Error in GetFileVersionInfo: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return "version error";
	}

	if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
	{
		printf("Error in VerQueryValue: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return "version error";
	}


	DWORD v1 = (pFileInfo->dwFileVersionMS >> 16) & 0xFFFF;
	DWORD v2 = (pFileInfo->dwFileVersionMS >> 0) & 0xFFFF;
	DWORD v3 = (pFileInfo->dwFileVersionLS >> 16) & 0xFFFF;
	DWORD v4 = (pFileInfo->dwFileVersionLS >> 0) & 0xFFFF;

	delete[] pbVersionInfo;

	// pFileInfo->dwFileVersionMS is usually zero. However, you should check
	// this if your version numbers seem to be wrong

	//printf("File Version: %d.%d.%d.%d\n",
	//	(pFileInfo->dwFileVersionLS >> 24) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 16) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 8) & 0xff,
	//	(pFileInfo->dwFileVersionLS >> 0) & 0xff
	//	);

	//// pFileInfo->dwProductVersionMS is usually zero. However, you should check
	//// this if your version numbers seem to be wrong

	//printf("Product Version: %d.%d.%d.%d\n",
	//	(pFileInfo->dwProductVersionLS >> 24) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 16) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 8) & 0xff,
	//	(pFileInfo->dwProductVersionLS >> 0) & 0xff
	//	);
	return	wxString::Format("%d.%d.%d.%d", v1, v2, v3, v4);

}