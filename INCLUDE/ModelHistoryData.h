#ifndef __MODEL_DATAHISTORY_H
#define __MODEL_DATAHISTORY_H

#include "globaldata.h"
#include "db_rec.h"

namespace wh{
//-----------------------------------------------------------------------------
class ActRec
{
public:
	wxString	mId;
	wxString	mTitle;
	wxString	mColour;
};

class ModelHistoryTableData
{
public:
	virtual ~ModelHistoryTableData(){}
	virtual size_t size()const = 0;
	virtual const wxString& GetLogId(const size_t row)const = 0;
	virtual const wxString& GetUser(const size_t row)const = 0;
	virtual const wxString& GetDate(const size_t row)const = 0;

	virtual const wxString& GetCId(const size_t row)const = 0;
	virtual const wxString& GetCTiltle(const size_t row)const = 0;
	virtual const wxString& GetCKind(const size_t row)const = 0;
	virtual const wxString& GetCMeasure(const size_t row)const = 0;

	virtual const wxString& GetOId(const size_t row)const = 0;
	virtual const wxString& GetOTiltle(const size_t row)const = 0;
	virtual const wxString& GetQty(const size_t row)const = 0;

	virtual const ActRec& GetActRec(const size_t row)const = 0;

	virtual const wxString& GetSrcPath(const size_t row)const { return wxEmptyString2; };
	virtual const wxString& GetDstPath(const size_t row)const { return wxEmptyString2; };
};
	

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __IMVP_H