#include "_pch.h"
#include "DetailActCtrl.h"
#include "dlg_move_view_Frame.h"
#include "dlg_act_view_Frame.h"


using namespace wh;
//-----------------------------------------------------------------------------
DetailActCtrl::DetailActCtrl()
	:VTableCtrl()
{
	fnOnCmdInsert = nullptr;
	fnOnCmdEdit = nullptr;

	fnOnCmdMove = MakeSafeFn(&DetailActCtrl::OnCmdMove, whID_MOVE,this);
	//fnOnCmdMoveHere = MakeSafeFn(&DetailActCtrl::OnCmdCmdMoveHere, whID_MOVE_HERE,this);
	fnOnCmdAction = MakeSafeFn(&DetailActCtrl::OnCmdAction, whID_ACTION, this);

	fnOnCmdLoad = MakeSafeFn(&DetailActCtrl::OnCmdLoad, wxID_REFRESH, this);
}
//-----------------------------------------------------------------------------
void DetailActCtrl::SetObjModel(std::shared_ptr<wh::detail::model::Obj> model)
{
	mObjModel = model;
}
//-----------------------------------------------------------------------------
void DetailActCtrl::SetObjView(wh::detail::view::ObjDetailPGView* objview)
{
	if (mObjDetailView)
	{
		UnbindCmd(mObjDetailView);
		mObjDetailView->SetModel(nullptr);

	}
	mObjDetailView = objview;
	if (mObjDetailView && mObjModel)
	{
		mObjDetailView->SetModel(mObjModel);
		BindCmd(mObjDetailView);


	}
	
}
//-----------------------------------------------------------------------------
void DetailActCtrl::SetActToolbar(DetailActToolBar* act_toolbar)
{
	if (mActToolBar)
	{
		UnbindCmd(mActToolBar);
		//mActToolBar->SetModel(nullptr);
	}
	mActToolBar = act_toolbar;
	if (mActToolBar)
	{
		mActToolBar->Build();
		mActToolBar->EnableTool(whID_MOVE, (bool)fnOnCmdMove);
		mActToolBar->EnableTool(whID_MOVE_HERE, (bool)fnOnCmdMoveHere);
		mActToolBar->EnableTool(whID_ACTION, (bool)fnOnCmdAction);
		//mActToolBar->SetModel(mTableModel);
		
		BindCmd(mActToolBar);
		if (mAuiMgr)
		{
			wxAuiPaneInfo&  pi = mAuiMgr->GetPane(mActToolBar);
			if (pi.IsOk())
				pi.BestSize(mActToolBar->GetClientSize());
		}

	}
}
//-----------------------------------------------------------------------------
void DetailActCtrl::OnCmdMove(wxCommandEvent& WXUNUSED(evt))
{
	if (!mObjModel || !mTableModel)
		return;
	
	rec::PathItem data = mObjModel->GetData();
	try
	{
		auto subj = std::make_shared<dlg_move::model::MovableObj>();
		subj->SetData(data, true);
		dlg_move::view::Frame dlg;
		dlg.SetModel(subj);
		if (wxID_OK == dlg.ShowModal())
		{
			
			auto movable = dlg.GetModel();
			auto dst = movable->GetDstObj();
			auto qty = movable->GetQty();

			const auto& movable_data = movable->GetData();
			const auto& dstobj_data = dst->GetData();
			const auto& qty_data = qty->GetData();
			
			if (movable_data.mObj.mQty.toStr() == qty_data)
			{
				auto odata = mObjModel->GetData();
				odata.mObj.mParent.mId = dstobj_data.mId;
			}
		}
		
		mObjModel->Load();
		mTableModel->Load();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Бла, бла - вобщем кто-то уже юзает этот объект");
	}
}
//-----------------------------------------------------------------------------
void DetailActCtrl::OnCmdMoveHere(wxCommandEvent& WXUNUSED(evt))
{

}
//-----------------------------------------------------------------------------
void DetailActCtrl::OnCmdAction(wxCommandEvent& WXUNUSED(evt))
{
	if (!mObjModel || !mTableModel)
		return;

	if (mObjModel->GetData().mCls.IsAbstract() )
		return;

	rec::PathItem data = mObjModel->GetData();
	try
	{
		auto subj = std::make_shared<dlg_act::model::Obj >();
		subj->SetData(data, true);
		dlg_act::view::Frame dlg;
		dlg.SetModel(subj);
		dlg.ShowModal();
		mObjModel->Load();
		mTableModel->Load();
	}
	catch (...)
	{
		// Transaction already rollbacked, dialog was destroyed, so nothinh to do
		wxLogError("Бла, бла - вобщем кто-то уже юзает этот объект");
	}

}
//-----------------------------------------------------------------------------
void DetailActCtrl::OnCmdLoad(wxCommandEvent& evt)
{
	if (mObjModel)
		mObjModel->Load();

	auto data_cls_id = mTableModel->mFieldVec->at(12)->GetData();
	auto data_obj_id = mTableModel->mFieldVec->at(13)->GetData();
	data_cls_id.mFilter.emplace_back(mObjModel->GetData().mCls.mId);
	data_obj_id.mFilter.emplace_back(mObjModel->GetData().mObj.mId);
	mTableModel->mFieldVec->at(12)->SetData(data_cls_id);
	mTableModel->mFieldVec->at(13)->SetData(data_obj_id);

	VTableCtrl::OnCmdLoad(evt);
}
//-----------------------------------------------------------------------------
wxAcceleratorTable DetailActCtrl::GetAcceleratorTable()const
{
	wxAcceleratorEntry entries[12];
	entries[0].Set(wxACCEL_CTRL, (int) 'R', wxID_REFRESH);
	entries[1].Set(wxACCEL_NORMAL, WXK_F5, wxID_REFRESH);
	entries[2].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
	entries[3].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
	entries[4].Set(wxACCEL_NORMAL, WXK_DELETE, wxID_REMOVE);
	entries[5].Set(wxACCEL_CTRL, (int) 'O', wxID_EDIT);
	entries[6].Set(wxACCEL_CTRL, (int) 'F', wxID_FIND);
	entries[7].Set(wxACCEL_NORMAL, WXK_PAGEUP, wxID_BACKWARD);
	entries[8].Set(wxACCEL_NORMAL, WXK_PAGEDOWN, wxID_FORWARD);
	entries[9].Set(wxACCEL_NORMAL, WXK_F6, whID_MOVE);
	entries[10].Set(wxACCEL_CTRL, WXK_F6, whID_MOVE_HERE);
	entries[11].Set(wxACCEL_NORMAL, WXK_F7, whID_ACTION);
	wxAcceleratorTable accel(12, entries);
	return accel;
}
//-----------------------------------------------------------------------------
void DetailActCtrl::BindCmd(wxWindow* wnd)
{
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdMove, whID_MOVE);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdMoveHere, whID_MOVE_HERE);
	wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdAction, whID_ACTION);

	VTableCtrl::BindCmd(wnd);
}
//-----------------------------------------------------------------------------
void DetailActCtrl::UnbindCmd(wxWindow* wnd)
{
	VTableCtrl::UnbindCmd(wnd);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdMove, whID_MOVE);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdMoveHere, whID_MOVE_HERE);
	wnd->Unbind(wxEVT_COMMAND_MENU_SELECTED, fnOnCmdAction, whID_ACTION);
}
