#ifndef __REPORTDATA_H
#define __REPORTDATA_H

#include "_pch.h"
namespace wh{
namespace rec{
//---------------------------------------------------------------------------
class ReportItem
{
public:
	wxString mId;
	wxString mTitle;
	wxString mNote;
	wxString mScript;
};


typedef std::vector<ReportItem> ReportList;
//class ReportList : public std::vector<ReportItem>
//{
//
//};

class ReportTable
{
public:
	std::vector<wxString> mFields;
};




//---------------------------------------------------------------------------
} //namespace rec{
} //namespace wh{
#endif // __IMVP_H