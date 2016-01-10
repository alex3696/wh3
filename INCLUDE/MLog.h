#ifndef __MLOG_H
#define __MLOG_H

#include "MLogArray.h"
#include "db_filter.h"

namespace wh{
//-------------------------------------------------------------------------
class LogCfg
{
public:
	unsigned int	mRowsPerPage = 100;
	bool			mEnableFavProp = true; // else show all object properties
	bool			mCompactMode = true;
	bool			mShowDebugColumns = __DEBUG; /* #ifdef __DEBUG true #elif false #endif;	*/
	
};//class LogCfg


class FieldData
{
	wxString	mName;
	FieldType	mType;
	bool		mShow;

};


//-------------------------------------------------------------------------
class MLog
{
	// 
	std::shared_ptr<TModelData<LogCfg>>	mCfg;
	std::shared_ptr<MLogArray>			mLogArray;
	std::shared_ptr<MFilterArray>	mFilterArray;
	//std::shared_ptr<MFavPropArray>	mFavPropArray;
	SigArray mSig;
public:
	MLog();

	void Load();

	sig::connection Connect(SigArray::Op op, const SigArray::Slot slot);
	void            Disconnect(SigArray::Op op, const SigArray::Slot slot);
















}; //class MLog


//-------------------------------------------------------------------------	
} //namespace wh

#endif // __****_H