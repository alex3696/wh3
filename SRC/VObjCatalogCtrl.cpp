#include "_pch.h"
#include "VObjCatalogCtrl.h"
#include "MObjCatalog.h"

#include "dlg_move_view_Frame.h"
#include "dlg_act_view_Frame.h"
#include "dlg_mkobj_view_Frame.h"


using namespace wh;
using namespace view;

VObjCatalogCtrl::VObjCatalogCtrl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{

	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_TEXT);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mPathSring = new wxStaticText(this, wxID_ANY, "/[CLS]OBJ/");
	GetSizer()->Add(mPathSring, 0, wxALL | wxEXPAND, 0);

	mTableView = new VObjCatalogTable(this);
	GetSizer()->Add(mTableView, 1, wxALL | wxEXPAND, 0);
	
	wxAcceleratorEntry entries[1];
	entries[0].Set(wxACCEL_NORMAL, WXK_BACK, wxID_BACKWARD); // , KEY_SPACE);
	wxAcceleratorTable accel(1, entries);
	SetAcceleratorTable(accel);

	//Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &VObjCatalogCtrl::OnContextMenu, this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &VObjCatalogCtrl::OnSelect, this);
	Bind(wxEVT_MENU, &VObjCatalogCtrl::OnCmdUp, this, wxID_BACKWARD);
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &VObjCatalogCtrl::OnActivated, this);
	

	// делаем тулбар
	
	mCatalogTool = mToolBar->AddTool(whID_CATALOG_SELECT, "Группировать", 
		m_ResMgr->m_ico_folder_obj24,wxEmptyString, wxITEM_NORMAL);
	mCatalogTool->SetHasDropDown(true);
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_TYPE, "по &типу"
		, m_ResMgr->m_ico_folder_type24);
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_PATH, "по &месту+типу"
		, m_ResMgr->m_ico_folder_obj24);
	Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [this](wxAuiToolBarEvent& evt)
		{
			mCatalogTool->SetSticky(true);
			wxRect rect = mToolBar->GetToolRect(mCatalogTool->GetId());
			wxPoint pt = mToolBar->ClientToScreen(rect.GetBottomLeft());
			pt = ScreenToClient(pt);
			PopupMenu(&mCatalogToolMenu, pt);
			mCatalogTool->SetSticky(false);
		}
		, whID_CATALOG_SELECT);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetPathDir,this, whID_CATALOG_PATH);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetTypeDir,this, whID_CATALOG_TYPE);
	
	mReloadTool = mToolBar->AddTool(wxID_REFRESH, "Обновить",
		m_ResMgr->m_ico_refresh24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdReload, this, wxID_REFRESH);
	
	mUpTool = mToolBar->AddTool(wxID_BACKWARD, "Назад",
		m_ResMgr->m_ico_back24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdUp, this, wxID_BACKWARD);

	mMoveTool = mToolBar->AddTool(wxID_REPLACE, "Переместить",
		m_ResMgr->m_ico_move24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdMove, this, wxID_REPLACE);

	mActTool = mToolBar->AddTool(wxID_EXECUTE, "Выполнить",
		m_ResMgr->m_ico_act24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdAct, this, wxID_EXECUTE);

	mMkObjTool = mToolBar->AddTool(wxID_MKOBJ, "Создать объект",
		m_ResMgr->m_ico_add_obj24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkObj, this, wxID_MKOBJ);

	mMkClsTool = mToolBar->AddTool(wxID_MKCLS, "Создать класс",
		m_ResMgr->m_ico_add_type24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkCls, this, wxID_MKCLS);

	mEditTool = mToolBar->AddTool(wxID_EDIT, "Редактировать",
		m_ResMgr->m_ico_edit24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnEdit, this, wxID_EDIT);

	mDeleteTool = mToolBar->AddTool(wxID_DELETE, "Удалить",
		m_ResMgr->m_ico_delete24, wxEmptyString, wxITEM_NORMAL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnDelete, this, wxID_DELETE);




	mToolBar->Realize();
	Layout();
}//

//-----------------------------------------------------------------------------
VObjCatalogCtrl::~VObjCatalogCtrl()
{

}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdSetTypeDir(wxCommandEvent& evt)
{
	if (mCatalogModel)
	{
		mCatalogModel->SetClsCatalog(1);
		mTableView->ExpandAll();
	}
	mCatalogTool->SetBitmap(m_ResMgr->m_ico_folder_type24);
	mToolBar->Refresh();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdSetPathDir(wxCommandEvent& evt)
{
	if (mCatalogModel)
	{
		mCatalogModel->SetObjCatalog(1);
		mTableView->ExpandAll();
	}
	mCatalogTool->SetBitmap(m_ResMgr->m_ico_folder_obj24);
	mToolBar->Refresh();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdReload(wxCommandEvent& evt)
{
	auto selectedItem = mTableView->GetSelection();

	if (!mCatalogModel)
		return;
	auto clsIdx = std::make_pair(false, size_t(0));
	auto objIdx = std::make_pair(false, size_t(0));

	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		if (objItem)
		{
			auto objArray = dynamic_cast<object_catalog::MObjArray*> (objItem->GetParent());
				
			auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());
			auto typeArray=dynamic_cast<object_catalog::MTypeArray*> (typeItem->GetParent());

			clsIdx.first = typeArray->GetItemPosition(typeItem, clsIdx.second);
			objIdx.first = objArray->GetItemPosition(objItem, objIdx.second);

		}
		else
		{
			auto typeItem=dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
			auto typeArray=dynamic_cast<object_catalog::MTypeArray*> (typeItem->GetParent());
			clsIdx.first = typeArray->GetItemPosition(typeItem, clsIdx.second);
		}
	}
		


		
	mTableView->Freeze();
	mCatalogModel->Load();
	mTableView->ExpandAll();

	std::shared_ptr<object_catalog::MTypeItem> selCls;
	std::shared_ptr<object_catalog::MObjItem> selObj;


	if (clsIdx.first)
	{
		while (!selCls && mCatalogModel->mTypeArray->GetChildQty()>clsIdx.second)
		{
			selCls = std::dynamic_pointer_cast<object_catalog::MTypeItem>
				(mCatalogModel->mTypeArray->GetChild(clsIdx.second));
			clsIdx.second--;
		}
	}

	if (selCls && objIdx.first)
	{
		while (!selObj && selCls->mObjArray->GetChildQty()>objIdx.second )
		{
			selObj = std::dynamic_pointer_cast<object_catalog::MObjItem>
				(selCls->mObjArray->GetChild(objIdx.second));
			objIdx.second--;
		}
	}

	selectedItem = (selObj) ? wxDataViewItem(selObj.get()) : wxDataViewItem(selCls.get());

	if (selectedItem.IsOk())
		mTableView->SetCurrentItem(selectedItem);

	mTableView->Thaw();

}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdUp(wxCommandEvent& evt)
{
	if (mCatalogModel)
	{
		const auto& rootObj = mCatalogModel->GetData();
		unsigned long pid(0);
		rec::PathItem new_root = rootObj;

		if (mCatalogModel->mCfg->GetData().mObjCatalog)
		{
			if (rootObj.mObj.mPID.ToCULong(&pid) && 0 < pid)
				new_root.mObj.mID = rootObj.mObj.mPID;
		}
		else
		{
			if (rootObj.mCls.mParent.ToCULong(&pid) && pid)
				new_root.mCls.mID = rootObj.mCls.mParent;
		}
		mCatalogModel->SetData(new_root);
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
		this->ProcessEvent(evt);
	}
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdMove(wxCommandEvent& evt)
{
	rec::PathItem data;
	if (GetSelectedObjKey(data))
	{
		using namespace dlg_move;
		namespace view = dlg_move::view;

		auto subj = std::make_shared<model::MovableObj>();
		subj->SetData(data, true);
		view::Frame dlg;
		dlg.SetModel(subj);
		dlg.ShowModal();
		OnCmdReload(wxCommandEvent(wxID_REFRESH));
	}
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdAct(wxCommandEvent& evt)
{
	rec::PathItem data;
	if (GetSelectedObjKey(data))
	{
		using namespace dlg_act;
		namespace view = dlg_act::view;

		auto subj = std::make_shared<model::Obj >();
		subj->SetData(data, true);

		view::Frame dlg;
		dlg.SetModel(subj);
		dlg.ShowModal();
		OnCmdReload(wxCommandEvent(wxID_REFRESH));

	}
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnMkObj(wxCommandEvent& evt)
{
	using namespace object_catalog;
	
	
	MTypeItem* typeItem(nullptr);

	auto selectedItem = mTableView->GetSelection();
	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<MObjItem*> (modelInterface);
		
		if (objItem)
		{
			auto objArray = dynamic_cast<MObjArray*> (objItem->GetParent());
			typeItem = dynamic_cast<MTypeItem*> (objArray->GetParent());
		}
		else
			typeItem = dynamic_cast<MTypeItem*> (modelInterface);
	}

	if (!typeItem)
		return;

	auto newObjModel = typeItem->mObjArray->CreateChild();
	typeItem->mObjArray->AddChild(newObjModel);

	auto newObj = std::dynamic_pointer_cast<MObjItem>(newObjModel);

	object_catalog::view::Frame dlg;
	dlg.SetModel(newObj);
	auto result = dlg.ShowModal();
	if (wxID_OK != result)
		typeItem->mObjArray->DelChild(newObjModel);

	OnCmdReload(wxCommandEvent(wxID_REFRESH));
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnMkCls(wxCommandEvent& evt)
{
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnEdit(wxCommandEvent& evt)
{
	rec::PathItem data;
	if (GetSelectedObjKey(data))
	{


		auto subj = GetSelectedObj();
		if (subj)
		{
			object_catalog::view::Frame dlg;
			dlg.SetModel(subj);
			dlg.ShowModal();
			OnCmdReload(wxCommandEvent(wxID_REFRESH));
		}


	}
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnDelete(wxCommandEvent& evt)
{
	auto subj = GetSelectedObj();
	if (subj)
	{
		int res = wxMessageBox("Вы действительно ходите удалить?",
			"Подтверждение", wxYES_NO);
		if (wxYES != res)
			return;

		subj->MarkDeleted();
		
		auto objArray = subj->GetParent();
		objArray->Save();
		//OnCmdReload(wxCommandEvent(wxID_REFRESH));
	}
}
//-----------------------------------------------------------------------------

void VObjCatalogCtrl::SetModel(std::shared_ptr<IModel> model)
{
	mCatalogModel = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(model);
	if (mCatalogModel)
	{
		mTableView->SetModel(model);

		if (mCatalogModel->mCfg->GetData().mObjCatalog)
			mCatalogTool->SetBitmap(m_ResMgr->m_ico_folder_obj24);
		else
			mCatalogTool->SetBitmap(m_ResMgr->m_ico_folder_type24);
	
		mConnPathChange.disconnect();
		auto funcOnChange = std::bind(&VObjCatalogCtrl::OnChangePath,
			this, std::placeholders::_1, std::placeholders::_2);
		mConnPathChange = mCatalogModel->mPath->ConnectAppendSlot(funcOnChange);

		std::vector<unsigned int> vec;
		unsigned int qty = mCatalogModel->mPath->GetChildQty();
		for (unsigned int i = 0; i < qty; ++i)
			vec.push_back(i);

		
		OnChangePath(*mCatalogModel->mPath.get(), vec);
	
	}
}
//---------------------------------------------------------------------------
void VObjCatalogCtrl::OnActivated(wxDataViewEvent& evt)
{
	
	wxDataViewItem selectedItem = evt.GetItem();

	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);

		bool IsObjCatalog = mCatalogModel->mCfg->GetData().mObjCatalog;
		object_catalog::MTypeItem* typeItem(nullptr);
		rec::PathItem new_root;
		

		if (IsObjCatalog)
		{
			if (objItem)
			{
				auto objArray = dynamic_cast<object_catalog::MObjArray*> (objItem->GetParent());
				typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());
				if(!typeItem)
					return;
				const auto& typeData = typeItem->GetData();
				
				if ("1" == typeData.mType )
				{
					const auto& objData = objItem->GetData();
					new_root.mObj.mID = objData.mID;
					new_root.mObj.mPID = objData.mPID;
					new_root.mObj.mLabel = objData.mLabel;
					new_root.mCls.mID = typeData.mID;
					new_root.mCls.mLabel = typeData.mLabel;
					mCatalogModel->SetData(new_root);
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					this->ProcessEvent(evt);
				}
			}//if (objItem)
		}
		else //if (IsObjCatalog)
		{
			if (!objItem)
			{
				typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
				if(!typeItem)
					return;

				const auto& typeData = typeItem->GetData();
				if ("0" == typeData.mType)
				{
					new_root.mCls.mID = typeData.mID;
					new_root.mCls.mParent = mCatalogModel->GetData().mCls.mParent;
					mCatalogModel->SetData(new_root);
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					this->ProcessEvent(evt);
				}


			}//if (!objItem)
		} //if (IsObjCatalog)
		
	}//if (selectedItem.IsOk())
}

//---------------------------------------------------------------------------
void VObjCatalogCtrl::GetSelected(std::vector<unsigned int>& selected)
{
	if (mTableView)
		mTableView->GetSelected(selected);
}

//-------------------------------------------------------------------------
bool VObjCatalogCtrl::GetSelectedObjKey(rec::PathItem& objKey)const
{
	auto selectedItem = mTableView->GetSelection();
	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		if (objItem)
		{
			auto objArray = dynamic_cast<object_catalog::MObjArray*> (objItem->GetParent());
			auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());
			if (!typeItem)
				return false;

			objKey.mObj = (const rec::ObjTitle)objItem->GetData();
			objKey.mCls = typeItem->GetData();

			return true;
		}
	}
	
	

	return false;
}
//-------------------------------------------------------------------------
std::shared_ptr<object_catalog::MObjItem> VObjCatalogCtrl::GetSelectedObj()const
{
	auto selectedItem = mTableView->GetSelection();
	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		return objItem->shared_from_this();
	}
	
	return std::shared_ptr<object_catalog::MObjItem>();
}


//-------------------------------------------------------------------------
void VObjCatalogCtrl::OnSelect(wxDataViewEvent& evt)
{
	wxDataViewItem item = evt.GetItem();
	if (!item.IsOk())
		return;
	
	const auto modelInterface = static_cast<const IModel*> (item.GetID());
	const auto objItem = dynamic_cast<const object_catalog::MObjItem*> (modelInterface);
	const object_catalog::MTypeItem* typeItem(nullptr);
	if (objItem)
	{
		typeItem = objItem->GetCls();
		const auto clsData = typeItem->GetData();

		
			

		ClsType ct;
		clsData.GetClsType(ct);
		switch (ct)
		{
		case ctSingle:
			mToolBar->EnableTool(mMoveTool->GetId(), true);
			mToolBar->EnableTool(mActTool->GetId(), true);
			break;
		case ctQtyByOne:
		case ctQtyByFloat:
			mToolBar->EnableTool(mMoveTool->GetId(), true);
			mToolBar->EnableTool(mActTool->GetId(), false);
			break;
		
		default://ctAbstract
			mToolBar->EnableTool(mMoveTool->GetId(), false);
			mToolBar->EnableTool(mActTool->GetId(), false);
				break;
		}
		
		mToolBar->Refresh();
	}
	else // MTypeItem*
	{
		mToolBar->EnableTool(mMoveTool->GetId(), false);
		mToolBar->EnableTool(mActTool->GetId(), false);
		mToolBar->Refresh();

		//typeItem = dynamic_cast<const object_catalog::MTypeItem*> (modelInterface);
	}

}
//-------------------------------------------------------------------------
void VObjCatalogCtrl::OnChangePath(const IModel& model, const std::vector<unsigned int>& vec)
{
	if (mCatalogModel)
		mPathSring->SetLabel(mCatalogModel->mPath->GetPathStr());
}