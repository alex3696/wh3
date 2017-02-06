#ifndef __MOVEOBJPRESENTER_H
#define __MOVEOBJPRESENTER_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IMoveObjView.h"
//#include "MoveObjModel.h"

namespace wh{
//-----------------------------------------------------------------------------
class MoveObjPresenter
{
public:
	MoveObjPresenter(std::shared_ptr<IMoveObjView> view, std::shared_ptr<rec::PathItem> moveable);
	~MoveObjPresenter();
	
	void ShowDialog();
	
	// View connector functions
	void OnViewUpdate();
	void OnViewMove(const wxString& oid, const wxString& qty);
	void OnViewEnableRecent(bool enable);
	void OnViewFindObj(const wxString& ss);
	void OnViewClose();
private:
	void SetView(IMoveObjView* view);
	void SetMoveable(const rec::PathItem& moveable){ mModel->SetMoveable(moveable); }

	std::unique_ptr<Moveable>	mModel = std::make_unique<Moveable>();
	IMoveObjView*				mView = nullptr;
	// View connector
	sig::scoped_connection connViewUpdate;
	sig::scoped_connection connViewEnableRecent;
	sig::scoped_connection connViewFindObj;
	sig::scoped_connection connViewClose;
	sig::scoped_connection connViewMoveObj;
	
	// Model connector
	sig::scoped_connection connModelUpdate;
	// Model connector functions
	void OnModelUpdate();



};




//-----------------------------------------------------------------------------
}//namespace wh{

#endif // __****_H