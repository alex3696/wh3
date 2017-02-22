#ifndef __REPORTMODEL_H
#define __REPORTMODEL_H

#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportModel 
{
public:
	void Update();
	void Export();
	void SetParam();
	//using SigUpdated = sig::signal<void(const rec::ReportList&)>;

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H