#ifndef __IVIEW_DETAIL_PAGE_H
#define __IVIEW_DETAIL_PAGE_H
//-----------------------------------------------------------------------------

#include "IViewHistory.h"

namespace wh{
//-----------------------------------------------------------------------------
class IViewToolbarAct : public IViewWindow
{
public:
	virtual void SetObj(const rec::ObjInfo& rt) = 0 ;

	sig::signal<void()>		sigMove;
	sig::signal<void()>		sigAct;
	sig::signal<void()>		sigRedo;

};

//-----------------------------------------------------------------------------
class IViewPageDetail : public IViewWindow
{
public:
public:
	virtual std::shared_ptr<IViewToolbarAct>		GetViewToolbarAct()const = 0;
	virtual std::shared_ptr<IViewToolbarHistory>	GetViewToolbarHistory()const = 0;
	virtual std::shared_ptr<IViewTableHistory>		GetViewTableHistory()const = 0;
	virtual std::shared_ptr<IViewFilterList>		GetViewFilterList()const = 0;
	virtual std::shared_ptr<IViewObjPropList>		GetViewObjPropList()const = 0;
	virtual void SetCfg(const rec::PageHistory&) = 0;

};





//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H