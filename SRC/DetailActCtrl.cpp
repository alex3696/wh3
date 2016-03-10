#include "_pch.h"
#include "DetailActCtrl.h"

using namespace wh;

//-----------------------------------------------------------------------------
DetailActCtrl::DetailActCtrl()
{
}
//-----------------------------------------------------------------------------
const DetailActCtrl::TCmdFn* DetailActCtrl::GetCmdFunction(int cmd_id)const
{
	auto it = mFn.find(cmd_id);
	if (it != mFn.end())
		return &(it->second);
	return nullptr;
}
//-----------------------------------------------------------------------------
void DetailActCtrl::SetCmdFunction(int cmd_id, TCmdFn& cmd_fn)
{
	mFn[cmd_id] = cmd_fn;
}
