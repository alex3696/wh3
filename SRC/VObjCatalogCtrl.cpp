#include "_pch.h"
#include "VObjCatalogCtrl.h"
#include "MObjCatalog.h"

#include "dlg_move_view_Frame.h"
#include "dlg_act_view_Frame.h"
#include "dlg_mkobj_view_Frame.h"
#include "dlg_favprop_SelectFrame.h"

#include "DClsEditor.h"
#include "detail_ctrlpnl.h"
#include "MainFrame.h"

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
		wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);
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
	
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_TYPE, "�� &����"
		, m_ResMgr->m_ico_folder_type24);
	AppendBitmapMenu(&mCatalogToolMenu, whID_CATALOG_PATH, "�� &�����+����"
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

	const auto currBaseGroup = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;

	// ������ ������
	// ---------- ������������� ������
	if (!is_dlg)
	{
		mCatalogTool = mToolBar->AddTool(whID_CATALOG_SELECT, "������������",
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

	mReloadTool = mToolBar->AddTool(wxID_REFRESH, "��������", m_ResMgr->m_ico_refresh24);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdReload, this, wxID_REFRESH);

	if (mCatalogModel && mCatalogModel->GetFilterClsId().mIsEnabled)
	{
		mUpTool = mToolBar->AddTool(wxID_BACKWARD, "�����", m_ResMgr->m_ico_back24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdUp, this, wxID_BACKWARD);
	}

	if (!is_dlg)
	{
		mUpTool = mToolBar->AddTool(wxID_VIEW_DETAILS, "��������", m_ResMgr->m_ico_views24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdDetail, this, wxID_VIEW_DETAILS);

		mUpTool = mToolBar->AddTool(wxID_PROPERTIES, "������� ��������",
			m_ResMgr->m_ico_favprop_select24);
		Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdFavProp, this, wxID_PROPERTIES);

		// ---------- User ������ ---------- 
		mToolBar->AddSeparator();

		if ((int)currBaseGroup >= (int)bgUser)
		{
			mMoveTool = mToolBar->AddTool(wxID_REPLACE, "�����������", m_ResMgr->m_ico_move24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdMove, this, wxID_REPLACE);

			mActTool = mToolBar->AddTool(wxID_EXECUTE, "���������", m_ResMgr->m_ico_act24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnCmdAct, this, wxID_EXECUTE);
		}

		// ---------- TypeDesigner ������ ---------- 
		mToolBar->AddSeparator();
		if ((int)currBaseGroup >= (int)bgTypeDesigner)
		{
			mMkClsTool = mToolBar->AddTool(wxID_MKCLS, "������� �����", m_ResMgr->m_ico_add_type24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkCls, this, wxID_MKCLS);
		}

		// ---------- Type|ObjDesigner ������ ---------- 
		if ((int)currBaseGroup >= (int)bgObjDesigner)
		{
			mMkObjTool = mToolBar->AddTool(wxID_MKOBJ, "������� ������", m_ResMgr->m_ico_add_obj24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnMkObj, this, wxID_MKOBJ);

			mEditTool = mToolBar->AddTool(wxID_EDIT, "�������������", m_ResMgr->m_ico_edit24);
			Bind(wxEVT_COMMAND_MENU_SELECTED, &VObjCatalogCtrl::OnEdit, this, wxID_EDIT);

			mDeleteTool = mToolBar->AddTool(wxID_DELETE, "�������", m_ResMgr->m_ico_delete24);
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
	// ����������� ������ �� ������
	BaseGroup bg = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
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
	// ����������� ������ �� ���� ��������


	
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
				mToolDisable[wxID_PROPERTIES] += 1;
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
void VObjCatalogCtrl::OnCmdSetTypeDir(wxCommandEvent& evt)
{
	wxWindowUpdateLocker	wndLockUpdater(mToolBar);
	wxWindowDisabler		wndDisabler(mToolBar);
	wxBusyCursor			busyCursor;
	mToolBar->SetToolBitmap(whID_CATALOG_SELECT, m_ResMgr->m_ico_folder_type24);
	
	//mToolBar->Refresh();
	if (mCatalogModel)
	{
		mCatalogModel->SetCatalog(false);
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
		mCatalogModel->SetCatalog(true);
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

	// ���� ����������, �� ��������� ������� ����� ���������� �� � ����� ����������
	// ����� ���������� 0-�������
	struct ClsIdx
	{
		ClsIdx(){}
		ClsIdx(const rec::Cls& cls, unsigned int pos)
			:mCls(cls), mPos(pos){}
		rec::Cls		mCls;
		unsigned int	mPos;
	};

	struct ObjIdx
	{
		ObjIdx(){}
		ObjIdx(const rec::ObjTitle& obj, unsigned int pos)
			:mObj(obj), mPos(pos){}

		rec::ObjTitle	mObj;
		unsigned int	mPos;
	};

	std::unique_ptr< ClsIdx >	ucls;
	std::unique_ptr< ObjIdx >	uobj;

	using namespace object_catalog;
	
	if (selectedItem.IsOk())
	{
		auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
		auto objItem = dynamic_cast<object_catalog::MObjItem*> (modelInterface);
		if (objItem)
		{
			auto obj_array = objItem->GetParent();
			unsigned int pos = 0;
			if (obj_array->GetItemPosition(objItem, pos))
			{
				const rec::ObjTitle& objData = objItem->GetData();;
				uobj.reset(new ObjIdx(objData, pos));

				auto typeItem = dynamic_cast<MTypeItem*>(obj_array->GetParent());
				auto typeArray = typeItem->GetParent();
				if (typeArray->GetItemPosition(typeItem, pos))
				{
					const rec::Cls& clsData = typeItem->GetData();;
					ucls.reset(new ClsIdx(clsData, pos));
				}
			}
		}
		else
		{
			unsigned int pos = 0;
			auto typeItem = dynamic_cast<MTypeItem*> (modelInterface);
			auto typeArray = typeItem->GetParent();
			if (typeArray->GetItemPosition(typeItem, pos))
			{
				const rec::Cls& clsData = typeItem->GetData();;
				ucls.reset(new ClsIdx(clsData, pos));
			}

		}
	}

	mCatalogModel->Load();
	mTableView->ExpandAll();

	selectedItem = wxDataViewItem(nullptr);

	if (ucls && "OnActivated" != evt.GetString() )
	{
		if (mCatalogModel->mTypeArray->GetChildQty() > ucls->mPos)
		{
			auto selCls = (mCatalogModel->mTypeArray->at(ucls->mPos));
			if (uobj)
			{
				if (selCls && selCls->mObjArray->GetChildQty()>uobj->mPos)
				{
					auto selObj = selCls->mObjArray->at(uobj->mPos);
					if (selObj)
						selectedItem = wxDataViewItem(selObj.get());
				}
			}
			else
			{
				if (selCls)
					selectedItem = wxDataViewItem(selCls.get());
			}
		}
	}
	else
	{
		auto qty = mCatalogModel->mTypeArray->GetChildQty();
		if (qty)
		{
			auto firstChild = mCatalogModel->mTypeArray->at(0);
			selectedItem = wxDataViewItem(firstChild.get());
		}
	}

	
	if (selectedItem.IsOk())
	{
		mTableView->Select(selectedItem);
		//mTableView->SetCurrentItem(selectedItem);
	}

	UpdateToolsStates();
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdUp(wxCommandEvent& evt)
{
	if (mCatalogModel)
	{
		mCatalogModel->DoUp();
		wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
		this->ProcessEvent(evt);
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
	wxWindow* notebook = this->GetParent();
	MainFrame* main_farame(nullptr);
	if (notebook)
		main_farame = dynamic_cast<MainFrame*>(notebook->GetParent());
	if (!main_farame)
		return;

	const auto& obj_data = objItem->GetData();
	const auto& cls_data = typeItem->GetData();

	detail::view::CtrlPnl* pnl = new detail::view::CtrlPnl(notebook);
	main_farame->AddTab(pnl);
	pnl->SetObject(cls_data.mId, obj_data.mId, obj_data.mParent.mId);
	
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdFavProp(wxCommandEvent& evt)
{
	auto selectedItem = mTableView->GetSelection();
	if (!selectedItem.IsOk())
		return;
	
	using namespace object_catalog;

	auto modelInterface = static_cast<IModel*> (selectedItem.GetID());
	MTypeItem* typeItem = dynamic_cast<MTypeItem*> (modelInterface);

	if (!typeItem)
	{
		MObjItem* objItem = dynamic_cast<MObjItem*> (modelInterface);
		if (objItem)
		{
			MObjArray* objArray = dynamic_cast<MObjArray*> (objItem->GetParent());
			if (objArray)
				typeItem = dynamic_cast<MTypeItem*> (objArray->GetParent());
		}
	}
	if (!typeItem)
		return;

	dlg::favprop::view::SelectFrame dlg(this);

	typeItem->mFavProp->Load();
	dlg.SetModel(std::dynamic_pointer_cast<IModel>(typeItem->mFavProp));
	dlg.ShowModal();
	OnCmdReload(wxCommandEvent(wxID_REFRESH));
}
//-----------------------------------------------------------------------------
void VObjCatalogCtrl::OnCmdMove(wxCommandEvent& evt)
{
	rec::PathItem data;
	if (GetSelectedObjKey(data))
	{
		using namespace dlg_move;
		namespace view = dlg_move::view;

		try
		{
			auto subj = std::make_shared<model::MovableObj>();
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
			wxLogError("���, ��� - ������ ���-�� ��� ����� ���� ������");
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
			wxLogError("���, ��� - ������ ���-�� ��� ����� ���� ������");
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
	if (!mCatalogModel->IsObjTree())
	{
		const auto& root = mCatalogModel->GetData();

		auto newItem = std::make_shared<object_catalog::MTypeItem>();

		rec::Cls cls_data;
		cls_data.mParent.mId = root.mCls.mId;
		cls_data.mParent.mLabel = root.mCls.mLabel;

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

			//const auto& edited_cls = typeItem->GetData();
			//auto newItem = std::make_shared<object_catalog::MTypeItem>();
			//newItem->SetData(edited_cls);
			//newItem->Load();

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
	auto subj = GetSelectedObj();
	if (subj)
	{
		int res = wxMessageBox("�� ������������� ������ �������?",
			"�������������", wxYES_NO);
		if (wxYES != res)
			return;

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

		object_catalog::MTypeItem* typeItem(nullptr);
		rec::PathItem new_root;
		

		if (mCatalogModel->IsObjTree())
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
					new_root.mObj.mId = objData.mId;
					new_root.mObj.mParent.mId = objData.mParent.mId;
					new_root.mObj.mLabel = objData.mLabel;
					new_root.mCls.mId = typeData.mId;
					new_root.mCls.mLabel = typeData.mLabel;
					mCatalogModel->SetData(new_root);
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					evt.SetString("OnActivated");
					this->ProcessEvent(evt);
				}
			}//if (objItem)
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
					new_root.mCls.mParent = mCatalogModel->GetData().mCls.mParent;
					mCatalogModel->SetData(new_root);
					wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);
					evt.SetString("OnActivated");
					this->ProcessEvent(evt);
				}


			}//if (!objItem)
		} //if (IsObjCatalog)
		
	}//if (selectedItem.IsOk())
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
void VObjCatalogCtrl::OnChangePath(const IModel& model, const std::vector<unsigned int>& vec)
{
	if (mCatalogModel)
		mPathSring->SetValue(mCatalogModel->mPath->GetPathStr());
}
//-------------------------------------------------------------------------
bool VObjCatalogCtrl::GetCurrentParent(rec::PathItem& root)const
{
	if (!mCatalogModel)
		return false;
	
	root = mCatalogModel->GetData();
	return true;

}