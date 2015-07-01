#ifndef __TVIEWCTRLPANEL_H
#define __TVIEWCTRLPANEL_H


#include "BaseOkCancelDialog.h"
#include "BaseRefControlPanel.h"
#include "BaseControlPanel.h"
#include "BaseTable.h"

#include "TViewTable.h"

#include "TPresenter.h"

namespace wh{
namespace view{



//-----------------------------------------------------------------------------
namespace CtrlTool
{
	const char Load = 0x01;
	const char Save = 0x02;
	const char Append	= 0x04;
	const char Edit		= 0x08;
	const char Remove	= 0x10;
	const char Back		= 0x40;

	const char All = 0x1F;
	const char AddDel = 0x14;
};

template 
< 
	char		mTools = CtrlTool::All,
	typename T_ItemEditor = void,
	bool AutoSaveEdited = true
>
class TViewCtrlPanel
	: public wxPanel
	, public ctrlWithResMgr
{
public:
	typedef TViewTable	T_TableType;

	TViewCtrlPanel(wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxTAB_TRAVERSAL,
		const wxString& name = wxPanelNameStr);

	void SetModel(std::shared_ptr<IModel> model);

	void GetSelected(std::vector<unsigned int>& selected)
	{
		if (mTableView)
			mTableView->GetSelected(selected);
	}

	bool GetAutoSaveEdited()const
	{
		return mAutoSaveEdited;
	}
	void SetAutoSaveEdited(bool val=true)const
	{
		mAutoSaveEdited = val;
	}

protected:
	wxAuiToolBar*		mToolBar = nullptr;
	wxMenu				mContextMenu;
	T_TableType*		mTableView = nullptr;

	bool				mAutoSaveEdited = AutoSaveEdited;

	std::shared_ptr<IModel> mVecModel;

	void OnSelectChange(wxDataViewEvent &event);
	void OnContextMenu(wxDataViewEvent &event);

	void BuildCtrlItem(const int winid, const wxString& label,
		const std::function<void()>& itemFunc,
		const wxBitmap&	ico16, const wxBitmap&	ico24)
	{
		std::function<void(wxCommandEvent&)> eventFunctor =
			std::bind(SafeCallEvent(), itemFunc, std::placeholders::_1);

		mToolBar->AddTool(winid, label, ico24, label);
		Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
		AppendBitmapMenu(&mContextMenu, winid, label, ico16);
	}

	void BuildSaveItem()
	{
		if (!(mTools & CtrlTool::Save))
			return;
		
		if (!mAutoSaveEdited && mVecModel->GetCanCommitSave() )
		{
			std::function<void()> itemFunc =
				[this]() {	mVecModel->Save();	};

			const int		winid(wxID_SAVE);
			const wxString	label(L"Сохранить");
			const wxBitmap&	ico16 = m_ResMgr->m_ico_save16;
			const wxBitmap&	ico24 = m_ResMgr->m_ico_save24;

			BuildCtrlItem(winid, label, itemFunc, ico16, ico24);
		}//if (mAutoSaveEdited)
	}
	void BuildLoadItem()
	{
		if (!(mTools & CtrlTool::Load))
			return;
		
		std::function<void()> itemFunc =
			[this]() {	mVecModel->Load();	};

		const int		winid(wxID_REFRESH);
		const wxString	label(L"Обновить");
		const wxBitmap&	ico16 = m_ResMgr->m_ico_refresh16;
		const wxBitmap&	ico24 = m_ResMgr->m_ico_refresh24;

		BuildCtrlItem(winid, label, itemFunc, ico16, ico24);
	}

	void BuildAddItem()
	{
		if (!(mTools & CtrlTool::Append))
			return;

		std::function<void()> itemFunc = [this]()
		{
			//T_ItemEditor editor(nullptr);
			T_ItemEditor editor;
			T_AddFunctor()(mVecModel, editor, mAutoSaveEdited);
			mTableView->OnChangeVecState(mVecModel->GetState());
		};
		
		const int		winid(wxID_ADD);
		const wxString	label(L"Добавить");
		const wxBitmap&	ico16 = m_ResMgr->m_ico_create16;
		const wxBitmap&	ico24 = m_ResMgr->m_ico_create24;

		BuildCtrlItem(winid, label, itemFunc, ico16, ico24);
	}
	void BuildEditItem()
	{
		if (!(mTools & CtrlTool::Edit))
			return;
		
		std::function<void()> itemFunc = [this]()
		{
			//T_ItemEditor editor(nullptr);
			T_ItemEditor editor;

			T_EditFunctor()(mVecModel, editor, *mTableView, mAutoSaveEdited);
			
			mTableView->OnChangeVecState(mVecModel->GetState());
		};
		const int		winid(wxID_EDIT);
		const wxString	label(L"Редактировать");
		const wxBitmap&	ico16 = m_ResMgr->m_ico_edit16;
		const wxBitmap&	ico24 = m_ResMgr->m_ico_edit24;
		BuildCtrlItem(winid, label, itemFunc, ico16, ico24);
	}
	void BuildDelItem()
	{
		if (!(mTools & CtrlTool::Remove))
			return;

		std::function<void()> itemFunc = [this]()
		{
			T_RemoveFunctor()(mVecModel, *mTableView, mAutoSaveEdited);
			mTableView->OnChangeVecState(mVecModel->GetState());
		};
		const int		winid(wxID_REMOVE);
		const wxString	label(L"Удалить");
		const wxBitmap&	ico16 = m_ResMgr->m_ico_delete16;
		const wxBitmap&	ico24 = m_ResMgr->m_ico_delete24;
		BuildCtrlItem(winid, label, itemFunc, ico16, ico24);
	}




};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template < char mTools, typename T_ItemEditor = void, bool AutoSaveEdited = true >
TViewCtrlPanel < mTools, T_ItemEditor, AutoSaveEdited >
::TViewCtrlPanel
(wxWindow* parent,
wxWindowID id,
const wxPoint& pos,
const wxSize& size,
long style,
const wxString& name)
: wxPanel(parent, id, pos, size, style, name)
{
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	SetSizer(szrMain);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
	GetSizer()->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	mTableView = new T_TableType(this);
	GetSizer()->Add(mTableView, 1, wxALL | wxEXPAND, 0);

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_CONTEXT_MENU, &TViewCtrlPanel::OnContextMenu, this);
	Bind(wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, &TViewCtrlPanel::OnSelectChange, this);

	Layout();
}//
//-----------------------------------------------------------------------------
template < char mTools, typename T_ItemEditor = void, bool AutoSaveEdited = true >
void TViewCtrlPanel < mTools, T_ItemEditor, AutoSaveEdited >
::SetModel(std::shared_ptr<IModel> model)
{
	if (mTableView)
		mTableView->SetModel(model);
	
	mVecModel = model;

	mToolBar->Clear();
	mContextMenu.ClearEventHashTable();

	//Unbind(wxEVT_COMMAND_MENU_SELECTED, wxID_REFRESH);


	BuildSaveItem();
	BuildLoadItem();
	if (mToolBar->GetToolCount())
		mToolBar->AddSeparator();

	BuildAddItem();
	BuildEditItem();
	BuildDelItem();


	mToolBar->Realize();
	//this->Layout();

}
//-----------------------------------------------------------------------------
template < char mTools, typename T_ItemEditor = void, bool AutoSaveEdited = true >
void TViewCtrlPanel < mTools, T_ItemEditor, AutoSaveEdited >
::OnSelectChange(wxDataViewEvent &event)
{
	wxDataViewItem item = event.GetItem();
	if (item.IsOk()) {
		auto editContextMenu = mContextMenu.FindChildItem(wxID_EDIT);
		if (editContextMenu)
			editContextMenu->Enable();
		auto removeContextMenu = mContextMenu.FindChildItem(wxID_REMOVE);
		if (removeContextMenu)
			removeContextMenu->Enable();
		mToolBar->EnableTool(wxID_EDIT, true);
		mToolBar->EnableTool(wxID_REMOVE, true);

		mTableView->Select(item);
	}
	else
	{
		auto editContextMenu = mContextMenu.FindChildItem(wxID_EDIT);
		if (editContextMenu)
			editContextMenu->Enable(false);
		auto removeContextMenu = mContextMenu.FindChildItem(wxID_REMOVE);
		if (removeContextMenu)
			removeContextMenu->Enable(false);
		mToolBar->EnableTool(wxID_EDIT, false);
		mToolBar->EnableTool(wxID_REMOVE, false);
		mTableView->UnselectAll();
	}
	mToolBar->Refresh();
}//void OnSelectChange
//-----------------------------------------------------------------------------
template < char mTools, typename T_ItemEditor = void, bool AutoSaveEdited = true >
void TViewCtrlPanel < mTools, T_ItemEditor, AutoSaveEdited >
::OnContextMenu(wxDataViewEvent &event)
{
	OnSelectChange(event);
	PopupMenu(&mContextMenu, event.GetPosition());
}//void OnContextMenu( wxDataViewEvent &event )











}//namespace view
}//namespace wh
#endif //__*_H