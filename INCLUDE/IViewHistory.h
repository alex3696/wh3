#ifndef __IVIEW_HISTORY_H
#define __IVIEW_HISTORY_H
//-----------------------------------------------------------------------------

#include "IViewWindow.h"
#include "ModelHistoryData.h"
#include "IViewFilterList.h"
#include "IViewObjPropList.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewHistory : public IViewWindow
{
public:
	virtual void SetHistoryTable(const std::shared_ptr<const ModelHistoryTableData>& rt) = 0;
	virtual void SetRowsOffset(const size_t& offset) = 0;
	virtual void SetRowsLimit(const size_t& limit) = 0;
	virtual std::shared_ptr<IViewFilterList> GetViewFilterList()const = 0;
	virtual std::shared_ptr<IViewObjPropList> GetViewObjPropList()const = 0;
	virtual void SetCfg(const rec::PageHistory&) = 0;

	sig::signal<void()> sigUpdate;

	sig::signal<void()> sigPageForward;
	sig::signal<void()> sigPageBackward;
	sig::signal<void(bool)> sigShowFilterList;
	sig::signal<void(bool)>	sigShowObjPropList;

	sig::signal<void(const wxString&)>	sigSelectHistoryItem;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H