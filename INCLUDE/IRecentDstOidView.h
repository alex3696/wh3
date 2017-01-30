#ifndef __IRECENTDSROIDVIEW_H
#define __IRECENTDSROIDVIEW_H

#include "RecentDstOidModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class IRecentDstOidView
{
public:
	virtual void Update(const rec::MruList& item) = 0;
};
//-----------------------------------------------------------------------------

}//namespace wh{
#endif // __****_H