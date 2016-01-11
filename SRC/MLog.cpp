#include "_pch.h"
#include "MLog.h"

using namespace wh;
//-------------------------------------------------------------------------
DbFieldInfo::DbFieldInfo()
	:mFilter(nullptr)
{

}
//-------------------------------------------------------------------------
DbFieldInfo::~DbFieldInfo()
{

};
//-------------------------------------------------------------------------
const DbFieldInfo::Filter& DbFieldInfo::SetFilter(const wxString& val, FilterOp fo
	, FilterConn fc, bool enable)
{
	if (!mFilter)
		mFilter.reset(new DbFieldInfo::Filter(*this));
	mFilter->mVal = val;
	mFilter->mOp = fo;
	mFilter->mConn = fc;
	mFilter->mEnable = enable;
	return *mFilter.get();
}
//-------------------------------------------------------------------------
bool DbFieldInfo::HasFilter()const
{ 
	return (bool)mFilter; 
}
//-------------------------------------------------------------------------
const DbFieldInfo::Filter& DbFieldInfo::GetFilter()const	
{ 
	return *mFilter; 
}
//-------------------------------------------------------------------------

void DbFieldInfo::EnabeFilter(bool enable)
{
	if (mFilter)
		mFilter->mEnable = enable;
}
//-------------------------------------------------------------------------
bool DbFieldInfo::IsEnabedFilter()const
{
	return mFilter ? mFilter->mEnable : false;
}
