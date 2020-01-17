#include "_pch.h"
#include "MoveObjView.h"
#include <boost/algorithm/string.hpp>   
#include <wx/statline.h>
#include "RecentDstOidPresenter.h"

using namespace wh;
//---------------------------------------------------------------------------
class Node
	:public boost::noncopyable
{
public:
	Node(const Node* parent, int tid = 0, void* val = nullptr)
		:mParent(parent), mTypeId(tid), mVal(val)
	{}

	const Node*			mParent;
	int					mTypeId;
	void*				mVal;

	std::vector< std::shared_ptr<Node> >	mChilds;
};
//---------------------------------------------------------------------------
class DvModel
	: public wxDataViewModel
{

	

	void SetTree(const ObjTree* tree, std::shared_ptr<Node>& node)
	{
		if (!tree)
			return;

		auto view_node = node.get();

		view_node->mChilds.clear();
		view_node->mChilds.reserve(tree->mType.size());
		auto& types = view_node->mChilds;

		for (const auto& tp : tree->mType)
		{
			auto type_node = std::make_shared<Node>(view_node, 10, (void*)&tp);
			auto rng = tree->GetTypeRange(tp.mId);
			while (rng.first != rng.second)
			{
				const Obj* obj_data = rng.first.operator->();
				std::wstring obj_name = obj_data->mTitle.ToStdWstring();
				boost::algorithm::to_lower(obj_name);
				if (mFilterStr.empty() || std::wstring::npos != obj_name.find(mFilterStr))
				{
					auto obj_node = std::make_shared<Node>(type_node.get(), 20, (void*)obj_data);
					type_node->mChilds.emplace_back(obj_node);
				}
				++rng.first;
			}//while (rng.first != rng.second)

			if (type_node->mChilds.size())
				types.emplace_back(type_node);
		}//for
	}

	void RebuldTree()
	{
		if (!mFilterStr.empty() || !mRecentEnable) // mode filter
		{
			//mRootNode->mChilds.clear();
			//mRootNode->mChilds.emplace_back(mDstNode);
						
			mDstNode->mParent = nullptr;
			mRootNode = mDstNode;
		}
		else // mode all
		{
			//mRootNode->mChilds.clear();
			//mRootNode->mChilds.emplace_back(mRecentNode);
			//mRootNode->mChilds.emplace_back(mDstNode);
			mDstNode->mParent = mTopRootNode.get();
			mRootNode = mTopRootNode;
		}
		this->Cleared();
	}
public:
	const Node& GetNode()const{ return *mRootNode; };
	const Node& GetDstNode()const{ return *mDstNode; };

	DvModel()
		:wxDataViewModel(), mRootNode(new Node(nullptr))
	{
		mRecentNode = std::make_shared<Node>(mRootNode.get(), 1, (void*)&mRecentTree);
		mDstNode = std::make_shared<Node>(mRootNode.get(), 1, (void*)&mDstTree);
		mRootNode->mChilds.emplace_back(mRecentNode);
		mRootNode->mChilds.emplace_back(mDstNode);


		mTopRootNode = mRootNode;
	}
	
	void SetRecentEnable(bool enable)		
	{ 
		mRecentEnable = enable; 
		RebuldTree();
	}
	void SetFilter(const wxString& str)
	{
		auto new_ss = std::wstring(str.ToStdWstring());
		boost::algorithm::to_lower(new_ss);
		
		if (mFilterStr != new_ss)
		{
			mFilterStr = new_ss;
			SetTree(mDstTree, mDstNode);
			RebuldTree();

		}
	}
	void SetDstTree(const ObjTree& tree)	
	{	
		mDstTree = &tree;	
		SetTree(mDstTree, mDstNode);
		RebuldTree();
	}
	void SetRecentTree(const ObjTree& tree)	
	{	
		mRecentTree = &tree;
		std::wstring tmp;
		tmp.swap(mFilterStr);
		SetTree(mRecentTree, mRecentNode);
		tmp.swap(mFilterStr);
		RebuldTree();
	}
	

	virtual bool IsContainer(const wxDataViewItem &item)const override	
	{ 
		if (  !item.IsOk())
			return true;

		auto node = static_cast<Node*> (item.GetID());
		if (20 == node->mTypeId)
			return false;

		return true; 
	}
	virtual wxDataViewItem GetParent(const wxDataViewItem &item) const override 
	{
		if (!item.IsOk())
			wxDataViewItem();
		auto node = static_cast<Node*> (item.GetID());

		if (0 == node->mTypeId )
			return wxDataViewItem();
		if( 1 == node->mTypeId)
			return wxDataViewItem();
		if (nullptr==node->mParent)
			return wxDataViewItem();
		if (mRootNode.get() == node->mParent)
			return wxDataViewItem();


		return wxDataViewItem((void*)node->mParent);
	}
	virtual unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const override
	{
		const Node* parent_node = parent.IsOk() ?
			static_cast<Node*> (parent.GetID())
			: mRootNode.get();

		if (20 == parent_node->mTypeId)
			return 0;

		for (size_t i = 0; i < parent_node->mChilds.size();++i)
		{
			const Node* ch = parent_node->mChilds[i].get();
			wxDataViewItem item((void*)ch);
			array.Add(item);
		}
			
		return array.size();

	}
	virtual unsigned int	GetColumnCount() const override 
	{ 
		return 2; 
	}
	virtual wxString		GetColumnType(unsigned int col) const 
	{ 
		switch (col)
		{
		case 0: return "wxDataViewIconText";
		default: break;
		}
		return "string";
	}

	virtual void GetValue(wxVariant &  variant,
		const wxDataViewItem &  item,
		unsigned int  col
		)  const override
	{
		;
		switch (col)
		{
		case 0:
		{
			Node* node = static_cast<Node*> (item.GetID());
			switch (node->mTypeId)
			{
			case 1:
			{
				if (node->mVal == &mRecentTree)
					variant << wxDataViewIconText("Недавние", ResMgr::GetInstance()->m_ico_history24);
				else if (node->mVal == &mDstTree)
					variant << wxDataViewIconText("Все", ResMgr::GetInstance()->m_ico_folder_type24);
			}
			break;
			case 10:
			{
				auto cls = static_cast<Type*> (node->mVal);
				variant << wxDataViewIconText(cls->mTitle, ResMgr::GetInstance()->m_ico_type_num24);
			}
			break;
			case 20:
			{
				auto obj = static_cast<Obj*> (node->mVal);
				variant << wxDataViewIconText(obj->mTitle, wxNullIcon);
			}
			break;

			default: break;
			}
			if(variant.IsNull())
				variant << wxDataViewIconText("*ERROR*", wxNullIcon);
		}
		break;
		case 1:
		{
			auto node = static_cast<Node*> (item.GetID());
			if (20 == node->mTypeId)
			{
				auto obj = static_cast<Obj*> (node->mVal);
				variant = obj->mPath;
			}
		}//case 1:
		break;
		default: break;
		}
		if (variant.IsNull())
			variant << wxDataViewIconText("*ERROR*", wxNullIcon);

	}
	bool SetValue(const wxVariant &, const wxDataViewItem &, unsigned int)override
	{
		return false;
	}

protected:
	const ObjTree* mRecentTree = nullptr;
	const ObjTree* mDstTree = nullptr;

	std::shared_ptr<Node> mTopRootNode;
	std::shared_ptr<Node> mRootNode;
	std::shared_ptr<Node> mRecentNode;
	std::shared_ptr<Node> mDstNode;

	std::wstring	mFilterStr;
	bool			mRecentEnable=true;


};//class DwModel
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
XMoveObjView::XMoveObjView(wxWindow* parent)
{
	mFrame = new wxDialog(parent, wxID_ANY, "Перемещение"
		, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	const wxIcon  ico("ICO_MOVE_24", wxBITMAP_TYPE_ICO_RESOURCE, 24, 24);
	mFrame->SetIcon(ico);
	mFrame->SetMinSize(mFrame->GetSize());
	mFrame->SetSize(mFrame->GetSize()*1.8);
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);
	mInfo = new wxStaticText(mFrame, wxID_ANY, wxEmptyString);
	mInfo->Wrap(-1);
	szrMain->Add(mInfo, 0, wxALL | wxEXPAND, 5);

	//Panel Object list
	mObjListPanel = new wxPanel(mFrame);
	mObjBrowser = std::make_shared<ViewTableBrowser>(mObjListPanel);
	mObjBrowser->SetEditableQty(true);
	mObjBrowser->SetVisibleQty(true);
	
	wxSizer *objListSzr = new wxBoxSizer(wxVERTICAL);
	objListSzr->Add(mObjBrowser->GetWnd(), 1, wxEXPAND, 5);
	mObjListPanel->SetSizer(objListSzr);

	// destination panel
	mDstPanel = new wxPanel(mFrame);
	BuildToolBar(mDstPanel);
	BuildTree(mDstPanel);

	wxSizer *dstSzr = new wxBoxSizer(wxVERTICAL);
	dstSzr->Add(mToolBar, 0, wxALL | wxEXPAND, 0);
	dstSzr->Add(mTree, 1, wxALL | wxEXPAND, 0);
	mDstPanel->SetSizer(dstSzr);
	
	szrMain->Add(mObjListPanel, 1, wxEXPAND, 5);
	szrMain->Add(mDstPanel, 1, wxEXPAND, 5);

	wxBoxSizer* msdbSizer = new wxBoxSizer(wxHORIZONTAL);
	msdbSizer->Add(0, 0, 1, wxEXPAND, 5);
	mBtnBack = new wxButton(mFrame, wxID_BACKWARD, "< Назад");
	mBtnForward = new wxButton(mFrame, wxID_OK, "Далее >");
	auto mbtnCancel = new wxButton(mFrame, wxID_CANCEL, "Закрыть");
	msdbSizer->Add(mBtnBack, 0, wxALL, 5);
	msdbSizer->Add(mBtnForward, 0, wxALL, 5);
	msdbSizer->Add(mbtnCancel, 0, wxALL, 5);
	szrMain->Add(msdbSizer, 0, wxEXPAND, 10);

	mFrame->SetSizer(szrMain);
	mFrame->Layout();
	mFrame->Centre(wxBOTH);

	mFrame->Bind(wxEVT_DESTROY, [this](wxWindowDestroyEvent& evt)
	{
		if (evt.GetWindow() != mFrame)
			return;
		mTimer.Stop();
		mObjBrowser.reset();
		mFrame = nullptr;
	});

	mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &XMoveObjView::OnCancel, this, wxID_CANCEL);
	mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &XMoveObjView::OnOk, this, wxID_OK);
	mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &XMoveObjView::OnBack, this, wxID_BACKWARD);
	mTimer.Bind(wxEVT_TIMER, &XMoveObjView::OnTimer, this);

}
//---------------------------------------------------------------------------
XMoveObjView::XMoveObjView(const std::shared_ptr<IViewWindow>& parent)
	:XMoveObjView(parent->GetWnd())
{
}
//---------------------------------------------------------------------------
XMoveObjView::~XMoveObjView()
{
	//mFrame->Destroy();
	//mFrame = nullptr;
}
//-----------------------------------------------------------------------------
void XMoveObjView::BuildToolBar(wxWindow* parent)
{
	mToolBar = new wxAuiToolBar(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize
		, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_PLAIN_BACKGROUND /*| wxAUI_TB_TEXT */);

	{
		std::function<void()> itemFunc = [this]()
		{
			wxBusyCursor busyCursor;
			dynamic_cast<DvModel*>(mTree->GetModel())->SetFilter(wxEmptyString);
			mFindCtrl->SetValue(wxEmptyString);
			this->sigUpdate();
		};
		std::function<void(wxCommandEvent&)> eventFunctor
			= std::bind(SafeCallEvent(), itemFunc, std::placeholders::_1);
		const int		winid(wxID_REFRESH);
		const wxString	label(L"Обновить");
		const wxIcon&	ico24 = ResMgr::GetInstance()->m_ico_refresh24;
		mToolBar->AddTool(winid, label, ico24, label);
		parent->Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
	}

	{
		std::function<void()> itemFunc = [this]()
		{
			wxBusyCursor busyCursor;
			bool enable = this->mToolBar->GetToolToggled(wxID_PROPERTIES);
			this->sigEnableRecent(enable);
		};
		std::function<void(wxCommandEvent&)> eventFunctor
			= std::bind(SafeCallEvent(), itemFunc, std::placeholders::_1);
		const int winid = wxID_PROPERTIES;
		const wxString label = L"Показать/скрыть недавние";
		const wxIcon& ico24 = ResMgr::GetInstance()->m_ico_history24;
		mToolBar->AddTool(winid, label, ico24, label, wxITEM_CHECK);
		parent->Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
	}

	{
		mToolBar->AddSeparator();
		std::function<void()> itemFunc = [this]()
		{
			OnClickSearchBtn(wxCommandEvent());
		};
		std::function<void(wxCommandEvent&)> eventFunctor = std::bind(SafeCallEvent(), itemFunc, std::placeholders::_1);

		// поиск|фильтр

		//auto lbl = new wxStaticText(mToolBar, wxID_ANY, "Поиск по имени");
		//mToolBar->AddControl(lbl, "Поиск2");
		mFindCtrl = new wxTextCtrl(mToolBar, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
		mFindCtrl->Bind(wxEVT_COMMAND_TEXT_ENTER, eventFunctor);
		mFindCtrl->SetHint("фильтр");
		mToolBar->AddControl(mFindCtrl, "поиск|фильтр");



		const int winid = wxID_FIND;
		const wxString label = L"Выполнить поиск|фильтр";
		const wxIcon& ico24 = ResMgr::GetInstance()->m_ico_filter24;
		auto tool = mToolBar->AddTool(winid, label, ico24, label, wxITEM_NORMAL);
		tool->SetHasDropDown(true);

		parent->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [this, tool](wxAuiToolBarEvent& evt)
		{
			if (evt.IsDropDownClicked())
			{
				wxMenu menu;

				AppendBitmapMenu(&menu, wxID_FILE1, "Фильтр"
					, ResMgr::GetInstance()->m_ico_filter24);
				AppendBitmapMenu(&menu, wxID_FILE2, "Поиск"
					, wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR));

				//tool->SetSticky(true);
				wxRect rect = mToolBar->GetToolRect(tool->GetId());
				wxPoint pt = mToolBar->ClientToScreen(rect.GetBottomLeft());
				pt = mFrame->ScreenToClient(pt);
				mFrame->PopupMenu(&menu, pt);
				//tool->SetSticky(false);
				//tool->SetSticky(true);
			}
			else if (evt.IsCommandEvent())
			{
				OnClickSearchBtn(evt);
			}

		}, winid);

		parent->Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, wxID_FIND);

		auto mgr = whDataMgr::GetInstance();

		mFrame->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, tool, mgr](wxCommandEvent& evt)
		{
			tool->SetBitmap(ResMgr::GetInstance()->m_ico_filter24);
			mFindCtrl->SetHint("фильтр");
			mToolBar->Refresh();
			mgr->mRecentDstOidPresenter->SetFilterEnable(1);
			this->OnClickSearchBtn(wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_FIND));

		}
		, wxID_FILE1);

		mFrame->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &XMoveObjView::OnBack, this, wxID_BACKWARD);

		mFrame->Bind(wxEVT_COMMAND_MENU_SELECTED, [this, tool, mgr](wxCommandEvent& evt)
		{
			tool->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR));
			mFindCtrl->SetHint("поиск");
			mToolBar->Refresh();
			mgr->mRecentDstOidPresenter->SetFilterEnable(0);
			this->OnClickSearchBtn(wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_FIND));
		}
		, wxID_FILE2);



		if (mgr->mRecentDstOidPresenter->GetFilterEnable())
		{
			mFindCtrl->SetHint("фильтр");
			tool->SetBitmap(ResMgr::GetInstance()->m_ico_filter24);
			mToolBar->Refresh();
		}
		else
		{
			mFindCtrl->SetHint("поиск");
			tool->SetBitmap(wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR));
			mToolBar->Refresh();
		}




	}

	mToolBar->Realize();

}
//-----------------------------------------------------------------------------
void XMoveObjView::AutosizeColumns()
{
	//TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
	for (size_t i = 0; i < mTree->GetColumnCount(); i++)
	{
		auto col_pos = mTree->GetModelColumnIndex(i);
		auto col = mTree->GetColumn(col_pos);
		if (col)
		{
			unsigned int bs = mTree->GetBestColumnWidth(i);
			if (bs > 300)
				bs = 300;
			col->SetWidth(bs);
		}
	}
}
//-----------------------------------------------------------------------------
void XMoveObjView::BuildTree(wxWindow* parent)
{
	mTree = new wxDataViewCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxDV_ROW_LINES | wxDV_VERT_RULES /*| wxDV_HORIZ_RULES*/);

	auto dvModel = new DvModel();
	mTree->AssociateModel(dvModel);
	dvModel->DecRef();

	mTree->AppendIconTextColumn("Имя", 0, wxDATAVIEW_CELL_INERT, 200,
		wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);
	mTree->AppendTextColumn("Местоположение", 1, wxDATAVIEW_CELL_INERT, -1,
		wxALIGN_NOT, wxDATAVIEW_COL_RESIZABLE);

	//int ch = mTree->GetCharHeight();
	//mTree->SetRowHeight(ch * 2 + 0);
	mTree->SetRowHeight(26);

	parent->Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &XMoveObjView::OnActivated, this);

}
//-----------------------------------------------------------------------------
//virtual 
void XMoveObjView::SetShow()//override;
{
	StartCountdown();
	mFrame->ShowModal();
}
//---------------------------------------------------------------------------
//virtual 
void XMoveObjView::SetClose()//override;
{
	StopCountdown();
	mFrame->EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void XMoveObjView::OnClose(wxCloseEvent& evt)
{
	StopCountdown();
	this->sigUnlock();
	SetClose();
}
//-----------------------------------------------------------------------------
void XMoveObjView::OnCancel(wxCommandEvent& evt)
{
	OnClose(wxCloseEvent());
}
//-----------------------------------------------------------------------------
void XMoveObjView::OnBack(wxCommandEvent& evt)
{
	int page = 0;

	if (mDstPanel->IsShown())
		page = 0;
	else 
		page = 1;

	SetSelectPage(page);
}
//-----------------------------------------------------------------------------
void XMoveObjView::GetSelection(std::set<int64_t>& sel)const
{
	auto item = mTree->GetCurrentItem();
	if (!item.IsOk())
		return;
	auto sel_node = static_cast<Node*>(item.GetID());
	if (sel_node->mTypeId == 20)
	{
		auto sel_obj = static_cast<Obj*>(sel_node->mVal);
		unsigned long ival;
		
		if (sel_obj->mId.ToCULong(&ival))
			sel.emplace(ival);
	}
}
//-----------------------------------------------------------------------------
void XMoveObjView::OnOk(wxCommandEvent& evt)
{
	if (!mDstPanel->IsShown())
	{
		SetSelectPage(1);
		return;
	}
	sigExecute();

}
//---------------------------------------------------------------------------
void XMoveObjView::OnActivated(wxDataViewEvent& evt)
{
	OnOk();
}
//---------------------------------------------------------------------------
void XMoveObjView::ExpandAll()
{
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	const Node& root = dvmodel->GetNode();

	wxDataViewItem dvitem((void*)&root);
	ExpandTree(dvitem);
}
//---------------------------------------------------------------------------
void XMoveObjView::ExpandTree(const wxDataViewItem& dvitem, bool recursive)
{
	if (!dvitem.IsOk())
		return;

	const Node* node = static_cast<const Node*>((void*)dvitem);
	for (const auto& child : node->mChilds)
	{
		const Node* ch = child.get();

		wxDataViewItem type((void*)ch);
		mTree->Expand(type);
		if (recursive)
		{
			wxDataViewItem dvitem((void*)ch);
			ExpandTree(dvitem, recursive);
		}
			
	}
}
//---------------------------------------------------------------------------
void XMoveObjView::OnClickSearchBtn(wxCommandEvent& event)
{
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());

	if (mFindCtrl->GetValue().IsEmpty())
	{
		dvmodel->SetFilter(mFindCtrl->GetValue());
		ExpandAll();
		return;

	}

	if ("фильтр" == mFindCtrl->GetHint())
	{
		dvmodel->SetFilter(mFindCtrl->GetValue());
		ExpandAll();
		return;
	}

	dvmodel->SetFilter("");

	const auto& dst_node = dvmodel->GetDstNode();
	std::wstring ss = mFindCtrl->GetValue();
	boost::algorithm::to_lower(ss);
	bool isFind = false;

	for (const auto& tp : dst_node.mChilds)
	{
		for (const auto& obj : tp->mChilds)
		{
			const auto* obj_val = static_cast<const Obj*>(obj->mVal);
			std::wstring str = obj_val->mTitle.ToStdWstring();
			boost::algorithm::to_lower(str);

			if (std::wstring::npos != str.find(ss))
			{
				wxDataViewItem item(obj.get());
				mTree->Select(item);
				mTree->SetCurrentItem(item);
				mTree->EnsureVisible(item, mTree->GetColumn(0));
				//mTree->SetFocus();
				isFind = true;
				break;

			}
		}//for
		if (isFind)
			break;
	}//for (const auto& tp : dst_node.mChilds)
	//ExpandAll();
}
//-----------------------------------------------------------------------------
//virtual 
std::shared_ptr<ViewTableBrowser> XMoveObjView::GetViewObjBrowser()const
{
	return mObjBrowser;
}
//---------------------------------------------------------------------------
//virtual 
void XMoveObjView::UpdateRecent(const ObjTree& tree)//override
{ 
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetRecentTree(tree);
}
//---------------------------------------------------------------------------
//virtual 
void XMoveObjView::UpdateDst(const ObjTree& tree)//override
{ 
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetDstTree(tree);

	ExpandAll();
	AutosizeColumns();
}
//---------------------------------------------------------------------------
//virtual 
void XMoveObjView::EnableRecent(bool enable)//override
{ 
	mToolBar->ToggleTool(wxID_PROPERTIES, enable);

	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetRecentEnable(enable);
	ExpandAll();
}
//-----------------------------------------------------------------------------
void XMoveObjView::SetSelectPage(int page)
{
	switch (page)
	{
	default:
	case 0:
		mObjListPanel->Show();
		mDstPanel->Hide();
		mBtnBack->Hide();
		mBtnForward->SetLabel("Вперёд >");
		mInfo->SetLabel("Внимание! "
			"Выбранные объекты заблокированы на 10 минут для выполнения перемещения."
			"\nДля перехода к выбору действий нажмите кнопку 'Далее >'"
			"\nДля именения количества перемещаемых объектов один клик на количество.");
		mFrame->Layout();
		break;
	case 1:
		mObjListPanel->Hide();
		mDstPanel->Show();
		mBtnBack->Show();
		mBtnForward->SetLabel("Выполнить");
		mInfo->SetLabel(
			"Выберети объект, в который необходимо переместить выбранные объекты "
			"\nДля выполнения перемещения нажмите кнопку 'Выполнить'"
			"\nили двойной клик левой кнопкой мыши на необходимом объекте."
		);
		mFrame->Layout();
		break;
	}
}
//-----------------------------------------------------------------------------
void XMoveObjView::OnTimer(wxTimerEvent &evt)
{
	StepCountdown();
}
//-----------------------------------------------------------------------------
void XMoveObjView::StartCountdown()
{
	mMillSecLeft = 1000 * 60 * 10;//10min
	mTimer.Start(1000 * 10);//10sec
	mFrame->SetTitle("Перемещение");
}
//-----------------------------------------------------------------------------
void XMoveObjView::StepCountdown()
{
	if (mMillSecLeft > 0)
	{
		mMillSecLeft -= 1000 * 10;

		mFrame->SetTitle(wxString::Format(
			"Перемещение."
			" Автосброс блокировки объекта через %d:%d"
			, mMillSecLeft / 1000 / 60
			, (mMillSecLeft / 1000) % 60));
	}
	else
	{
		mTimer.Stop();
		mMillSecLeft = 0;
		mFrame->SetTitle("Выполнение перемещения невозможно. "
			"Перезапустите этот диалог ");
	}


}
//-----------------------------------------------------------------------------
void XMoveObjView::StopCountdown()
{
	mTimer.Stop();
}