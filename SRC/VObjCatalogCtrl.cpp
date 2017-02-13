#include "_pch.h"
//#include "NotebookModel.h"
#include "NotebookPresenter.h"
//#include "NotebookView.h"

#include "VObjCatalogCtrl.h"
#include "MObjCatalog.h"

#include "dlg_act_view_Frame.h"
#include "dlg_mkobj_view_Frame.h"
#include "dlg_favprop_SelectFrame.h"

#include "DClsEditor.h"
#include "detail_ctrlpnl.h"
#include "MainFrame.h"
#include "config.h"



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

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mPathSring = new wxTextCtrl(this, wxID_ANY, "/", wxDefaultPosition, wxDefaultSize, 
		wxTE_READONLY);
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
	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_EXPANDING, &VObjCatalogCtrl::OnExpanding, this);
	
	mTableView->GetTargetWindow()->Bind(wxEVT_MIDDLE_UP, &VObjCatalogCtrl::OnMiddleUpTable, this);
	
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_TYPE, "по &типу"
		, m_ResMgr->m_ico_folder_type24);
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_PATH, "по &месту+типу"
		, m_ResMgr->m_ico_folder_obj24);

	BuildToolbar();
	
	Layout();

}//
//-----------------------------------------------------------------------------
VObjCatalogCtrl::~VObjCatalogCtrl()
{

}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::BuildToolbar(bool is_dlg)
{
	wxWindowUpdateLocker	wndLockUpdater(mToolBar);
	mToolBar->ClearTools();

	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();

	// делаем тулбар
	// ---------- Общедоступные кнопки
	if (!is_dlg)
	{
		mCatalogTool = mToolBar->AddTool(whID_CATALOG_SELECT, "Группировать",
			m_ResMgr->m_ico_folder_obj24, wxEmptyString, wxITEM_NORMAL);
		mCatalogTool->SetHasDropDown(true);
		Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [this](wxAuiToolBarEvent& evt)
		{
			mCatalogTool->SetSticky(true);
			wxRect rect = mToolBar->GetToolRect(whID_CATALOG_SELECT);
			wxPoint pt = mToolBar->ClientToScreen(rect.GetBottomLeft());
			pt = ScreenToClient(pt);
			PopupMenu(&mCatalogToolMenu, pt);
			mCatalogTool->SetSticky(false);
		}
		, whID_CATALOG_SELECT);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetPathDir, this, whID_CATALOG_PATH);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetTypeDir, this, whID_CATALOG_TYPE);
	}

	mReloadTool = mToolBar->AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdReload, this, wxID_REFRESH);

	//if (mCatalogModel && mCatalogModel->GetFilterClsId().mIsEnabled)
	{
		mUpTool = mToolBar->AddTool(wxID_BACKWARD, "Вверх", m_ResMgr->m_ico_back24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdUp, this, wxID_BACKWARD);
	}

	if (!is_dlg)
	{
		mUpTool = mToolBar->AddTool(wxID_PROPERTIES, "Выбрать свойства",
			m_ResMgr->m_ico_favprop_select24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdFavProp, this, wxID_PROPERTIES);

		mUpTool = mToolBar->AddTool(wxID_VIEW_DETAILS, "Подробно", m_ResMgr->m_ico_views24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdDetail, this, wxID_VIEW_DETAILS);

		// ---------- User кнопки ---------- 
		/*
		mToolBar->AddSeparator();

		if ((int)currBaseGroup >= (int)bgUser)
		{
			mMoveTool = mToolBar->AddTool(wxID_REPLACE, "Переместить", m_ResMgr->m_ico_move24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdMove, this, wxID_REPLACE);

			mActTool = mToolBar->AddTool(wxID_EXECUTE, "Выполнить", m_ResMgr->m_ico_act24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdAct, this, wxID_EXECUTE);
		}
		*/

		// ---------- TypeDesigner кнопки ---------- 
		mToolBar->AddSeparator();
		if ((int)currBaseGroup >= (int)bgTypeDesigner)
		{
			mMkClsTool = mToolBar->AddTool(wxID_MKCLS, "Создать класс", m_ResMgr->m_ico_add_type24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkCls, this, wxID_MKCLS);
		}

		// ---------- Type|ObjDesigner кнопки ---------- 
		if ((int)currBaseGroup >= (int)bgObjDesigner)
		{
			mMkObjTool = mToolBar->AddTool(wxID_MKOBJ, "Создать объект", m_ResMgr->m_ico_add_obj24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkObj, this, wxID_MKOBJ);

			mEditTool = mToolBar->AddTool(wxID_EDIT, "Редактировать", m_ResMgr->m_ico_edit24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnEdit, this, wxID_EDIT);

			mDeleteTool = mToolBar->AddTool(wxID_DELETE, "Удалить", m_ResMgr->m_ico_delete24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnDelete, this, wxID_DELETE);
		}
	}

	mToolBar->Realize();
	UpdateToolsStates();
}

//-----------------------------------------------------------------------------
void VObjCatalogCtrl::UpdateToolsStates()
{
	mToolDisable[whID_CATALOG_SELECT] = 0;
	mToolDisable[wxID_REFRESH] = 0;
	mToolDisable[wxID_BACKWARD] = 0;
	mToolDisable[wxID_VIEW_DETAILS] = 0;
	mToolDisable[wxID_PROPERTIES] = 0;
	mToolDisable[wxID_REPLACE] = 0;
	mToolDisable[wxID_EXECUTE] = 0;
	mToolDisable[wxID_MKCLS] = 0;
	mToolDisable[wxID_MKOBJ] = 0;
	mToolDisable[wxID_EDIT] = 0;
	mToolDisable[wxID_DELETE] = 0;
	// ограничения кнопок по логину
	const auto& bg = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	switch (bg)
	{
	default:
	case bgNull:
	case bgGuest:
		mToolDisable[wxID_REPLACE] += 1;
		mToolDisable[wxID_EXECUTE] += 1;
		mToolDisable[wxID_MKCLS] += 1;
		mToolDisable[wxID_MKOBJ] += 1;
		mToolDisable[wxID_EDIT] += 1;
		mToolDisable[wxID_DELETE] += 1;
		break;
	case bgUser:
		mToolDisable[wxID_MKCLS] += 1;
		mToolDisable[wxID_MKOBJ] += 1;
		mToolDisable[wxID_EDIT] += 1;
		mToolDisable[wxID_DELETE] += 1;
		break;
	case bgObjDesigner:
		mToolDisable[wxID_MKCLS] += 1;
		break;
	case bgAdmin:
	case bgTypeDesigner:
		break;

	}
	// ограничения кнопок по типу каталога


	
	if (mCatalogModel)
	{
		if ( mCatalogModel->IsObjTree() )
			mToolDisable[wxID_MKCLS] += 1; 
		
	}
	



	auto selectedItem = mTableView->GetSelection();
	if (selectedItem.IsOk())
	{
		using namespace object_catalog;

		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		MTypeItem* typeItem = dynamic_cast<MTypeItem*> (modelInterface);

		if (typeItem)
		{
			const auto& cls_data = typeItem->GetData();
			switch (cls_data.GetClsType())
			{
			case ctSingle:
				mToolDisable[wxID_REPLACE] += 1;
				mToolDisable[wxID_EXECUTE] += 1;
				break;
			case ctQtyByOne:
			case ctQtyByFloat:
				mToolDisable[wxID_EXECUTE] += 1;
			default://ctAbstract
				mToolDisable[wxID_REPLACE] += 1;
				mToolDisable[wxID_EXECUTE] += 1;
				//mToolDisable[wxID_PROPERTIES] += 1;
				break;
			}
		}
		else
		{
			MObjItem* objItem = dynamic_cast<MObjItem*> (modelInterface);
			MObjArray* objArray = dynamic_cast<MObjArray*> (objItem->GetParent());
			if (objArray)
			{
				typeItem = dynamic_cast<MTypeItem*> (objArray->GetParent());
				const auto& cls_data = typeItem->GetData();
				switch (cls_data.GetClsType())
				{
				case ctSingle:
					break;
				case ctQtyByOne:
				case ctQtyByFloat:
					break;
				default://ctAbstract
					mToolDisable[wxID_EXECUTE] += 1;
					mToolDisable[wxID_PROPERTIES] += 1;
					mToolDisable[wxID_REPLACE] += 1;
					mToolDisable[wxID_VIEW_DETAILS] += 1;
					break;
				}

			}//if (typeItem)
		}

	}




	for (const auto& tool : mToolDisable)
		mToolBar->EnableTool(tool.first, !tool.second);


	mToolBar->Refresh();

}

//-----------------------------------------------------------------------------
void VObjCatalogCtrl::HideCatalogSelect(bool hide)
{
	if (hide)
		mToolBar->DeleteTool(whID_CATALOG_SELECT);
	else
	{
		mCatalogTool = mToolBar->AddTool(whID_CATALOG_SELECT, "Группировать",
			m_ResMgr->m_ico_folder_obj24, wxEmptyString, wxITEM_NORMAL);
		mCatalogTool->SetHasDropDown(true);
		Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [this](wxAuiToolBarEvent& evt)
		{
			mCatalogTool->SetSticky(true);
			wxRect rect = mToolBar->GetToolRect(whID_CATALOG_SELECT);
			wxPoint pt = mToolBar->ClientToScreen(rect.GetBottomLeft());
			pt = ScreenToClient(pt);
			PopupMenu(&mCatalogToolMenu, pt);
			mCatalogTool->SetSticky(false);
		}
		, whID_CATALOG_SELECT);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetPathDir, this, whID_CATALOG_PATH);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdSetTypeDir, this, whID_CATALOG_TYPE);
	}
}

//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdSetTypeDir(wxCommandEvent& evt)
{
	wxWindowUpdateLocker	wndLockUpdater(mToolBar);
	wxWindowDisabler		wndDisabler(mToolBar);
	wxBusyCursor			busyCursor;
	mToolBar->SetToolBitmap(whID_CATALOG_SELECT, m_ResMgr->m_ico_folder_type24);
	//mToolBar->Refresh();
	if (mCatalogModel)
	{
		auto cfg = mCatalogModel->GetData();
		cfg.mCatType = rec::catCls;
		mCatalogModel->SetCfg(cfg);
		mCatalogModel->Load();
		mTableView->ExpandAll();
	}

	UpdateToolsStates();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdSetPathDir(wxCommandEvent& evt)
{
	wxWindowUpdateLocker	wndLockUpdater(mToolBar);
	wxWindowDisabler		wndDisabler(mToolBar);
	wxBusyCursor			busyCursor;
	mToolBar->SetToolBitmap(whID_CATALOG_SELECT, m_ResMgr->m_ico_folder_obj24);
	//mToolBar->Refresh();
	if (mCatalogModel)
	{
		auto cfg = mCatalogModel->GetData();
		cfg.mCatType = rec::catObj;
		mCatalogModel->SetCfg(cfg);
		mCatalogModel->Load();
		mTableView->ExpandAll();
	}

	UpdateToolsStates();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdReload(wxCommandEvent& evt)
{
	//wxBusyInfo		busyInfo("Please wait, working...");
	wxBusyCursor			busyCursor;
	wxWindowUpdateLocker	wndDisabler(mTableView);

	auto selectedItem = mTableView->GetSelection();

	if (!mCatalogModel)
		return;

	mCatalogModel->Load();
	mTableView->ExpandAll();

	//selectedItem = wxDataViewItem(nullptr);
	//mTableView->Select(selectedItem);
	UpdateToolsStates();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdUp(wxCommandEvent& evt)
{
	if (mCatalogModel)
	{
		mCatalogModel->DoUp();
		//wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
		//this->ProcessEvent(evt);
	}
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdDetail(wxCommandEvent& evt)
{
	using namespace object_catalog;

	auto objItem = GetSelectedObj();
	if (!objItem)
		return;
	MObjArray* objArray = dynamic_cast<MObjArray*> (objItem->GetParent());
	if (!objArray)
		return;
	auto typeItem = dynamic_cast<MTypeItem*> (objArray->GetParent());
	if (!typeItem)
		return;
	
	rec::ObjInfo oi;
	oi.mCls = typeItem->GetData();
	oi.mObj = objItem->GetData();

	whDataMgr::GetInstance()->mNotebookPresenter->DoAddPage(oi);

}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdFavProp(wxCommandEvent& evt)
{
	auto selectedItem = mTableView->GetSelection();
	if (!selectedItem.IsOk())
		return;
	auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
	namespace cat = object_catalog;

	
	cat::MTypeItem* selectedCls(nullptr);

	cat::MObjItem* objItem = dynamic_cast<cat::MObjItem*> (modelInterface);
	if (objItem)
	{
		cat::MObjArray* objArray = dynamic_cast<cat::MObjArray*> (objItem->GetParent());
		if (objArray)
			selectedCls = dynamic_cast<cat::MTypeItem*> (objArray->GetParent());
	}
	else
		selectedCls = dynamic_cast<cat::MTypeItem*> (modelInterface);
	
	if (!selectedCls)
		return;

	dlg::favprop::view::SelectFrame dlg(this);

	selectedCls->mFavProp->Load();
	dlg.SetModel(std::dynamic_pointer_cast<IModel>(selectedCls->mFavProp));
	dlg.ShowModal();
	OnCmdReload(wxCommandEvent(wxID_REFRESH));
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdMove(wxCommandEvent& evt)
{
	rec::PathItem data;
	if (GetSelectedObjKey(data))
	{
		try
		{


			OnCmdReload(wxCommandEvent(wxID_REFRESH));
		}
		catch (...)
		{ 
			// Transaction already rollbacked
			// dialog was destroyed
			// so nothinh to do
			wxLogError("Бла, бла - вобщем кто-то уже юзает этот объект");
		}

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
		try
		{
			auto subj = std::make_shared<model::Obj >();
			subj->SetData(data, true);

			view::Frame dlg;
			dlg.SetModel(subj);
			dlg.ShowModal();
			OnCmdReload(wxCommandEvent(wxID_REFRESH));
		}
		catch (...)
		{
			// Transaction already rollbacked
			// dialog was destroyed
			// so nothinh to do
			wxLogError("Бла, бла - вобщем кто-то уже юзает этот объект");
		}
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
	typeItem->mObjArray->Insert(newObjModel);

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
	if (!mCatalogModel->IsObjTree())
	{
		const auto& root = mCatalogModel->GetRoot();

		SptrIModel iparent;
		if (mCatalogModel->mPath->size())
			iparent = mCatalogModel->mPath->GetChild(0);
		auto parent = std::dynamic_pointer_cast<object_catalog::model::MPathItem>(iparent);

		rec::Cls cls_data;
		cls_data.mParent.mId = root.mCls.mId;
		cls_data.mParent.mLabel = parent ? parent->GetData().mCls.mLabel : root.mCls.mLabel;
		
		auto newItem = std::make_shared<object_catalog::MTypeItem>();
		newItem->SetData(cls_data);

		DClsEditor editor;
		editor.SetModel(std::dynamic_pointer_cast<IModel>(newItem));

		int error = 0;
		int result = 0;
		bool loop = true;
		do
		{
			try
			{
				error = 0;
				result = editor.ShowModal();
				editor.UpdateModel();
				if (wxID_OK == result)
					newItem->Save();
			}
			catch (...){ error=1; }
			
			switch (result)
			{
			case wxID_CANCEL:	loop = false;			break;
			case wxID_OK:		loop = error?true:false;break;
			default:			loop = true;			break;
			}

		} while (loop);

		OnCmdReload(wxCommandEvent(wxID_REFRESH));
	}
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
	}//if (GetSelectedObjKey(data))
	else
	{
		wxDataViewItem selectedItem = mTableView->GetSelection();
		if (selectedItem.IsOk())
		{
			auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
			auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
			if (!typeItem)
				return;

			SptrIModel iparent;
			if (mCatalogModel->mPath->size())
				iparent = mCatalogModel->mPath->GetChild(0);
			auto parent = std::dynamic_pointer_cast<object_catalog::model::MPathItem>(iparent);
			if (parent)
			{
				auto type_data = typeItem->GetData();
				if (type_data.mParent.mId == parent->GetData().mCls.mId)
				{
					type_data.mParent.mLabel = parent->GetData().mCls.mLabel;
					typeItem->SetData(type_data);
				}
			}
			
			DClsEditor editor;
			std::shared_ptr<IModel> model = typeItem->shared_from_this();
			editor.SetModel(model);

			if (wxID_OK == editor.ShowModal())
			{
				editor.UpdateModel();
				typeItem->Save();
			}
			OnCmdReload(wxCommandEvent(wxID_REFRESH));
		}
	}

}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnDelete(wxCommandEvent& evt)
{
	int res = wxMessageBox("Вы действительно ходите удалить?",
		"Подтверждение", wxYES_NO);
	if (wxYES != res)
		return;

	wxPasswordEntryDialog passDlg(this, "введите пароль для подтверждения", "Удаление");
	if(wxID_OK != passDlg.ShowModal())
		return;

	const auto& connect_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	if (passDlg.GetValue() != connect_cfg.mPass)
	{ 
		wxMessageBox("Неверный пароль","Подтверждение", wxOK);
		return;
	}
	
	try
	{

	auto subj = GetSelectedObj();
	if (subj)
	{

		subj->MarkDeleted();
		
		auto objArray = subj->GetParent();
		objArray->Save();
		//OnCmdReload(wxCommandEvent(wxID_REFRESH));
	}
	else
	{
		wxDataViewItem selectedItem = mTableView->GetSelection();
		if (!selectedItem.IsOk())
			return;
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
		if (!typeItem)
			return;

		typeItem->MarkDeleted();
		auto arr = typeItem->GetParent();
		arr->Save();

	}
	}
	catch (boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)));
	}///catch(boost::exception & e)
	catch (...)
	{
		wxLogWarning(wxString("Unhandled exception"));
	}//catch(...)
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::SetModel(std::shared_ptr<IModel> model)
{
	mCatalogModel = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(model);
	if (mCatalogModel)
	{
		if(mCatalogModel->IsObjTree())
			mToolBar->SetToolBitmap(whID_CATALOG_SELECT, m_ResMgr->m_ico_folder_obj24);
		else
			mToolBar->SetToolBitmap(whID_CATALOG_SELECT, m_ResMgr->m_ico_folder_type24);
				
		mTableView->SetModel(model);
	
		namespace sph = std::placeholders;

		mConnPathChange.disconnect();
		auto funcOnChange = std::bind(&VObjCatalogCtrl::OnChangePath,
			this, sph::_1, sph::_2, sph::_3);
		mConnPathChange = mCatalogModel->mPath->ConnAfterInsert(funcOnChange);

		std::vector<SptrIModel> vec;
		unsigned int qty = mCatalogModel->mPath->GetChildQty();
		for (unsigned int i = 0; i < qty; ++i)
			vec.emplace_back(mCatalogModel->mPath->GetChild(i));

		
		OnChangePath(*mCatalogModel->mPath.get(), vec, SptrIModel(nullptr));
			
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

		object_catalog::MTypeItem* typeItem(nullptr);
		rec::PathItem new_root;
		

		if (mCatalogModel->IsObjTree())
		{
			if (objItem)
			{
				auto objArray = dynamic_cast<object_catalog::MObjArray*> (objItem->GetParent());
				typeItem = dynamic_cast<object_catalog::MTypeItem*> (objArray->GetParent());
				if (!typeItem)
					return;
				const auto& typeData = typeItem->GetData();

				if ("1" == typeData.mType)
				{
					const auto& objData = objItem->GetData();
					new_root.mObj.mId = objData.mId;
					new_root.mObj.mParent.mId = objData.mParent.mId;
					new_root.mObj.mLabel = objData.mLabel;
					new_root.mCls.mId = typeData.mId;
					new_root.mCls.mLabel = typeData.mLabel;
					mCatalogModel->SetCatFilter(objData.mId);// setObjRoot
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					evt.SetString("OnActivated");
					this->ProcessEvent(evt);
				}
			}//if (objItem)
			else
				mTableView->IsExpanded(selectedItem) ?
					mTableView->Collapse(selectedItem):mTableView->Expand(selectedItem);
		}
		else
		{
			if (!objItem)
			{
				typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
				if(!typeItem)
					return;

				const auto& typeData = typeItem->GetData();
				if ("0" == typeData.mType)
				{
					new_root.mCls.mId = typeData.mId;
					//new_root.mCls.mParent = mCatalogModel->GetData().mCls.mParent;
					mCatalogModel->SetCatFilter(typeData.mId);
					//mCatalogModel->SetData(new_root);
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					evt.SetString("OnActivated");
					this->ProcessEvent(evt);
				}
				else
					mTableView->IsExpanded(selectedItem) ?
						mTableView->Collapse(selectedItem) : mTableView->Expand(selectedItem);
			}//if (!objItem)
		} //if (IsObjCatalog)
		
	}//if (selectedItem.IsOk())
}
//---------------------------------------------------------------------------
void VObjCatalogCtrl::OnMiddleUpTable(wxMouseEvent& evt)
{
	wxDataViewItem item;
	wxDataViewColumn* column(nullptr);
	mTableView->HitTest(evt.GetPosition(), item, column);
	if (item.IsOk() && column && 1 == column->GetModelColumn())
	{
		mTableView->Select(item);
		OnCmdDetail(wxCommandEvent());
	}
}
//---------------------------------------------------------------------------
void VObjCatalogCtrl::OnExpanding(wxDataViewEvent &evt)
{
	wxBusyCursor			busyCursor;

	wxDataViewItem selectedItem = evt.GetItem();

	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto typeItem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
		if (typeItem)
		{
			wxBusyCursor busyCursor;
			typeItem->mObjArray->Load();
		}
	}
}
//---------------------------------------------------------------------------
const IModel* VObjCatalogCtrl::GetSelectedItem()const
{
	auto selectedItem = mTableView->GetSelection();
	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		//auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		//auto typetem = dynamic_cast<object_catalog::MTypeItem*> (modelInterface);
		
		return modelInterface;
	}
	return nullptr;
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
		if (objItem)
			return objItem->shared_from_this();
	}
	return std::shared_ptr<object_catalog::MObjItem>();
}
//-------------------------------------------------------------------------
void VObjCatalogCtrl::OnSelect(wxDataViewEvent& evt)
{
	UpdateToolsStates();
}
//-------------------------------------------------------------------------
void VObjCatalogCtrl::OnChangePath(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)
{
	if (mCatalogModel)
		mPathSring->SetValue(mCatalogModel->mPath->GetPathStr());
}
//-------------------------------------------------------------------------
bool VObjCatalogCtrl::GetCurrentParent(rec::PathItem& root)const
{
	if (!mCatalogModel)
		return false;
	
	root = mCatalogModel->GetRoot();
	return true;

}