#ifndef __CTRL_CSVFile_H
#define __CTRL_CSVFile_H

//#include "_pch.h"

namespace wh {
//-----------------------------------------------------------------------------
class CtrlCSVFile final
{
	wxString mExcelApp;
public:
	CtrlCSVFile();
	void Open(const wxString& filename);
};


//-----------------------------------------------------------------------------
class CtrlCSVFileOpen
{
public:
	CtrlCSVFileOpen(const std::shared_ptr<CtrlCSVFile>& ctrl
		,const std::shared_ptr<wxString>& fn)
	{
		ctrl->Open(*fn);
	}
};





//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H