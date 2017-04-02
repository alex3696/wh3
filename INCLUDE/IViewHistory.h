#ifndef __IVIEW_HISTORY_H
#define __IVIEW_HISTORY_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelHistoryData.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewHistory : public IViewWindow
{
public:
	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) = 0;

	sig::signal<void()> sigUpdate;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H