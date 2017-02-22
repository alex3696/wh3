#ifndef __IMOVEOBJVIEW_H
#define __IMOVEOBJVIEW_H

#include "IViewWindow.h"
#include "MoveObjData.h"

namespace wh{

class IMoveObjView
{
public:
	virtual void ShowDialog() = 0;
	virtual void UpdateRecent(const ObjTree& tree) = 0;
	virtual void UpdateDst(const ObjTree& tree) = 0;
	virtual void UpdateMoveable(const rec::PathItem& moveable) = 0;
	virtual void EnableRecent(bool enable) = 0;

	using SigUpdate = sig::signal<void()>;
	SigUpdate sigUpdate;
	//virtual sig::connection ConnSigUpdate(const SigUpdate::slot_type& slot) = 0;

	using SigEnableRecent = sig::signal<void(bool)>;
	SigEnableRecent sigEnableRecent;
	//virtual sig::connection ConnSigEnableRecent(const SigEnableRecent::slot_type& slot) = 0;

	using SigFindObj = sig::signal<void(const wxString&)>;
	SigFindObj sigFindObj;
	//virtual sig::connection ConnSigFindObj(const SigFindObj::slot_type& slot) = 0;

	using SigUnlock = sig::signal<void()>;
	SigUnlock sigClose;

	using SigMove = sig::signal<void(const wxString&, const wxString&)>;
	SigMove sigMove;

};










}//namespace wh{
#endif // __****_H