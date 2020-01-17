#ifndef __IMOVEOBJVIEW_H
#define __IMOVEOBJVIEW_H

#include "IViewWindow.h"
#include "MoveObjData.h"
#include "ViewBrowser.h"


namespace wh{

class IMoveObjView : public IViewWindow
{
public:
	virtual std::shared_ptr<ViewTableBrowser>	GetViewObjBrowser()const = 0;
	virtual void SetSelectPage(int) = 0;
	virtual void UpdateRecent(const ObjTree& tree) = 0;
	virtual void UpdateDst(const ObjTree& tree) = 0;
	virtual void EnableRecent(bool enable) = 0;
	virtual void GetSelection(std::set<int64_t>& sel)const = 0;

	sig::signal<void()> sigUpdate;
	sig::signal<void(bool)> sigEnableRecent;
	sig::signal<void(const wxString&)> sigFindObj;

	sig::signal<void()> sigUnlock;
	sig::signal<void()> sigExecute;


};










}//namespace wh{
#endif // __****_H