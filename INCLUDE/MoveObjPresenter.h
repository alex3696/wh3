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
	MoveObjPresenter();
	MoveObjPresenter(const rec::PathItem& moveable);

	
	void SetView(IMoveObjView* view);
	void SetMoveable(const rec::PathItem& moveable){ mModel->SetMoveable(moveable); }
	void ShowDialog();

	void Run();


	// View connector functions
	void OnViewUpdate();
	void OnViewMove(const wxString& oid, const wxString& qty);
	void OnViewEnableRecent(bool enable);
	void OnViewFindObj(const wxString& ss);
	void OnViewClose();
private:
	std::unique_ptr<Moveable>	mModel;
	IMoveObjView*				mView = nullptr;
	// View connector
	sig::scoped_connection connViewUpdate;
	sig::scoped_connection connViewEnableRecent;
	sig::scoped_connection connViewFindObj;
	
	// Model connector
	sig::scoped_connection connModelUpdate;
	// Model connector functions
	void OnModelUpdate();



};




//-----------------------------------------------------------------------------
}//namespace wh{

#endif // __****_H