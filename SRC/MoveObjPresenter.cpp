#include "_pch.h"
#include "MoveObjPresenter.h"

using namespace wh;

//-----------------------------------------------------------------------------
MoveObjPresenter::MoveObjPresenter()
{
	mModel = std::make_unique<Moveable>();

}
//-----------------------------------------------------------------------------
MoveObjPresenter::MoveObjPresenter(const rec::PathItem& moveable)
	:MoveObjPresenter()
{
	mModel->SetMoveable(moveable);
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::Run()
{
	ShowDialog();
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::SetView(IMoveObjView* view)
{
	if (mView)
	{
		//отцепляем все сигналы этой вьюшки от этого презентера
		connViewUpdate.disconnect();
		connViewEnableRecent.disconnect();
		connViewFindObj.disconnect();
	}
	mView = view;
	
	if (mView)
	{
		namespace ph = std::placeholders;
		auto fnU = std::bind(&MoveObjPresenter::OnViewUpdate, this);
		//mView->ConnSigUpdate(fnU);
		mView->sigUpdate.connect(fnU);

		auto fnER = std::bind(&MoveObjPresenter::OnViewEnableRecent, this, ph::_1);
		//mView->ConnSigEnableRecent(fnER);
		mView->sigEnableRecent.connect(fnER);

		auto fnF = std::bind(&MoveObjPresenter::OnViewFindObj, this, ph::_1);
		//mView->ConnSigFindObj(fnF);
		mView->sigFindObj.connect(fnF);

		auto fnC = std::bind(&MoveObjPresenter::OnViewClose, this);
		mView->sigClose.connect(fnC);

		auto fnM = std::bind(&MoveObjPresenter::OnViewMove, this, ph::_1, ph::_2);
		mView->sigMove.connect(fnM);

	}
}
//-----------------------------------------------------------------------------
void MoveObjPresenter::ShowDialog()
{
	if (mView)
		mView->ShowDialog();
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
	if (mView)
	{
		mView->EnableRecent(mModel->GetRecentEnable());
		mView->UpdateMoveable(mModel->GetMoveable());
		mView->UpdateRecent(mModel->GetRecent());
		mView->UpdateDst(mModel->GetDst());
		
	}
}
