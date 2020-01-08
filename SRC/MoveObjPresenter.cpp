#include "_pch.h"
#include "MoveObjPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
MoveObjPresenter::MoveObjPresenter(const std::shared_ptr<IMoveObjView>& view
								, const  std::shared_ptr<rec::PathItem>& moveable)
{
	SetView(view.get());

	if (!moveable)
		return;
	/*
	std::set<ObjectKey> obj;
	int64_t id = moveable->mObj.mId;
	int64_t parent_id = moveable->mObj.mParent.mId;

	obj.emplace(ObjectKey(id, parent_id));
	SetObjects(obj);
	*/
}

//-----------------------------------------------------------------------------
MoveObjPresenter::~MoveObjPresenter()
{
	connViewEnableRecent.disconnect();
	connViewFindObj.disconnect();
	connViewClose.disconnect();
	connViewMoveObj.disconnect();

	if (mModel)
		mModel->Unlock();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::SetView(IMoveObjView* view)
{
	//отцепляем все сигналы этой вьюшки от этого презентера
	connViewEnableRecent.disconnect();
	connViewFindObj.disconnect();
	connViewClose.disconnect();
	connViewMoveObj.disconnect();
	
	mView = view;
	
	if (mView)
	{
		namespace ph = std::placeholders;

		auto fnER = std::bind(&MoveObjPresenter::OnViewEnableRecent, this, ph::_1);
		connViewEnableRecent = mView->sigEnableRecent.connect(fnER);

		auto fnF = std::bind(&MoveObjPresenter::OnViewFindObj, this, ph::_1);
		connViewFindObj = mView->sigFindObj.connect(fnF);

		auto fnC = std::bind(&MoveObjPresenter::OnViewClose, this);
		connViewClose = mView->sigClose.connect(fnC);

		auto fnM = std::bind(&MoveObjPresenter::OnViewMove, this, ph::_1, ph::_2);
		connViewMoveObj = mView->sigMove.connect(fnM);

	}
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::SetObjects(const std::set<ObjectKey>& obj)
{
	mModel->LockObjects(obj);
	OnModelUpdate();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::Show()
{
	if (mView)
		mView->SetShow();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::OnViewMove(const wxString& oid, const wxString& qty)
{
	mModel->Move(oid, qty);
}

//-----------------------------------------------------------------------------
void MoveObjPresenter::OnViewEnableRecent(bool enable)
{ 
	mModel->SetRecentEnable(enable);
	OnModelUpdate();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::OnViewFindObj(const wxString& ss)
{
	mModel->FindObj(ss);
	OnModelUpdate();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::OnViewClose()
{
	mModel->Unlock();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::OnModelUpdate()
{
	TEST_FUNC_TIME;
	if (mView)
	{
		mView->EnableRecent(mModel->GetRecentEnable());
		mView->UpdateMoveable(mModel->GetMoveable());
		mView->UpdateRecent(mModel->GetRecent());
		mView->UpdateDst(mModel->GetDst());
		
	}
}
