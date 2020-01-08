#ifndef __MOVEOBJPRESENTER_H
#define __MOVEOBJPRESENTER_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IMoveObjView.h"
#include "MoveObjModel.h"
#include "CtrlBrowserRO.h"

namespace wh{
//-----------------------------------------------------------------------------
class MoveObjPresenter 
{
public:
	MoveObjPresenter(const std::shared_ptr<IMoveObjView>& view,const std::shared_ptr<rec::PathItem>& moveable);
	~MoveObjPresenter();
	
	void Show();
	
	// View connector functions
	void OnViewMove(const wxString& oid, const wxString& qty);
	void OnViewEnableRecent(bool enable);
	void OnViewFindObj(const wxString& ss);
	void OnViewClose();
	
	void SetObjects(const std::set<ObjectKey>& obj);
private:
	std::shared_ptr<CtrlTableObjBrowser_RO>	mCtrlObjBrowser;


	void SetView(IMoveObjView* view);

	std::unique_ptr<Moveable>	mModel = std::make_unique<Moveable>();
	IMoveObjView*				mView = nullptr;
	// View connector
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