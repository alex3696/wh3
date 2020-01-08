#include "_pch.h"
#include "MoveObjPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
MoveObjPresenter::MoveObjPresenter(const std::shared_ptr<IMoveObjView>& view
								, const  std::shared_ptr<rec::PathItem>& moveable)
{
	SetMoveable(*moveable);
	SetView(view.get());
}

//-----------------------------------------------------------------------------
MoveObjPresenter::~MoveObjPresenter()
{
	connViewUpdate.disconnect();
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
	//��������� ��� ������� ���� ������ �� ����� ����������
	connViewUpdate.disconnect();
	connViewEnableRecent.disconnect();
	connViewFindObj.disconnect();
	connViewClose.disconnect();
	connViewMoveObj.disconnect();
	
	mView = view;
	
	if (mView)
	{
		namespace ph = std::placeholders;

		auto fnU = std::bind(&MoveObjPresenter::OnViewUpdate, this);
		connViewUpdate = mView->sigUpdate.connect(fnU);

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
void MoveObjPresenter::SetMoveable(const rec::PathItem& moveable)
{
	mModel->SetMoveable(moveable); 
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::SetMoveable(int64_t oid, int64_t parent_oid)
{
	mModel->SetMoveable(oid, parent_oid);
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::SetObjects(const std::set<ObjectKey>& obj)
{
	mModel->LockObjects(obj);
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::ShowDialog()
{
	if (mView)
		mView->SetShow();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::OnViewUpdate()
{ 
	mModel->Load(); 
	OnModelUpdate();
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
	auto p0 = GetTickCount();
	if (mView)
	{
		mView->EnableRecent(mModel->GetRecentEnable());
		mView->UpdateMoveable(mModel->GetMoveable());
		mView->UpdateRecent(mModel->GetRecent());
		mView->UpdateDst(mModel->GetDst());
		
	}
	wxLogMessage(wxString::Format("MoveObjPresenter:\t%d\t MoveObj update view", GetTickCount() - p0));
}
