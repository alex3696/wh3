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
	bool			mShowDebugColumns = #ifdef __DEBUG	true #elif 	false #endif;
	/*
		#ifdef __DEBUG 
			true 
		#elif 
			false 
		#endif;
		*/
};//class LogCfg

//-------------------------------------------------------------------------
class MLog
	: public TModelData<LogCfg>
{
	// 
	std::shared_ptr<MLogArray>		mLogArray;
	std::shared_ptr<MFilterArray>	mFilterArray;

public:
	MLog(const char option = ModelOption::EnableParentNotify);




















}; //class MLog


//-------------------------------------------------------------------------	
} //namespace wh

#endif // __****_H