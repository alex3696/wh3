#include "_pch.h"
#include "MoveObjView.h"
#include <boost/algorithm/string.hpp>   
#include <wx/statline.h>
#include "RecentDstOidPresenter.h"

using namespace wh;




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
		return !col ? "wxDataViewIconText" : "string";
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

			default:
				variant << wxDataViewIconText("*ERROR*", wxNullIcon);
				break;
			}
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
		default:	
			variant = "*ERROR*";
			break;
		}

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










using namespace wh;

//---------------------------------------------------------------------------
MoveObjView::MoveObjView(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	//: wh::view::DlgBaseOkCancel(parent, id, title, pos, size, style, name)
	: wxDialog(parent, id, title, pos, size, style, name)
{
	this->SetTitle("Перемещение");
	wxSizer* szrMain = new wxBoxSizer(wxVERTICAL);

	mLblMovableObj = new wxStaticText(this, wxID_ANY, "[класс]объект/...[класс]объект(ед.изм)");
	szrMain->Add(mLblMovableObj, 0, wxALL, 5);

	mqtySpin = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
		wxDefaultSize, wxSP_ARROW_KEYS, 1, MAXINT, 1);
	mqtyCtrl = new wxTextCtrl(this, wxID_ANY, "1", wxDefaultPosition, wxDefaultSize, 0,
		wxTextValidator(wxFILTER_NUMERIC));
	szrMain->Add(mqtySpin, 0, wxALL | wxEXPAND, 0);
	szrMain->Add(mqtyCtrl, 0, wxALL | wxEXPAND, 0);

	//mLblDstObj = new wxStaticText(this, wxID_ANY, "В");
	//szrMain->Add(mLblDstObj, 0, wxALL, 5);

	auto staticline = new wxStaticLine(this, wxID_ANY);
	szrMain->Add(staticline, 0, wxEXPAND | wxALL, 5);

	BuildToolBar();
	szrMain->Add(mToolBar, 0, wxALL | wxEXPAND, 0);

	BuildTree();
	szrMain->Add(mTree, 1, wxALL | wxEXPAND, 0);


	msdbSizer = new wxStdDialogButtonSizer();
	mbtnOK = new wxButton(this, wxID_OK);//,"Сохранить и закрыть" );
	msdbSizer->AddButton(mbtnOK);
	mbtnCancel = new wxButton(this, wxID_CANCEL);//," Закрыть" );
	msdbSizer->AddButton(mbtnCancel);
	msdbSizer->Realize();
	Bind(wxEVT_CLOSE_WINDOW, &MoveObjView::OnClose, this);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MoveObjView::OnCancel, this, wxID_CANCEL);
	Bind(wxEVT_COMMAND_BUTTON_CLICKED, &MoveObjView::OnOk, this, wxID_OK);
	szrMain->Add(msdbSizer, 0, wxALL | wxEXPAND, 10);


	wxSize sz = GetSize();
	sz = sz * 1.3;
	SetSize(sz);
	
	this->SetSizer(szrMain);
	this->Layout();

}
//-----------------------------------------------------------------------------
void MoveObjView::BuildToolBar()
{
	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize
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
		const wxIcon&	ico24 = m_ResMgr->m_ico_refresh24;
		mToolBar->AddTool(winid, label, ico24, label);
		Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
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
		const wxIcon& ico24 = m_ResMgr->m_ico_history24;
		mToolBar->AddTool(winid, label, ico24, label, wxITEM_CHECK);
		Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, winid);
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
		const wxIcon& ico24 = m_ResMgr->m_ico_filter24;
		auto tool = mToolBar->AddTool(winid, label, ico24, label, wxITEM_NORMAL);
		tool->SetHasDropDown(true);
		
		Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, [this, tool](wxAuiToolBarEvent& evt)
		{
			if (evt.IsDropDownClicked())
			{
				wxMenu menu;

				AppendBitmapMenu(&menu, wxID_FILE1, "Фильтр"
					, m_ResMgr->m_ico_filter24);
				AppendBitmapMenu(&menu, wxID_FILE2, "Поиск"
					, wxArtProvider::GetBitmap(wxART_FIND, wxART_TOOLBAR));

				//tool->SetSticky(true);
				wxRect rect = mToolBar->GetToolRect(tool->GetId());
				wxPoint pt = mToolBar->ClientToScreen(rect.GetBottomLeft());
				pt = ScreenToClient(pt);
				PopupMenu(&menu, pt);
				//tool->SetSticky(false);
				//tool->SetSticky(true);
			}
			else if (evt.IsCommandEvent())
			{
				OnClickSearchBtn(evt);
			}
			
		}, winid);
		
		Bind(wxEVT_COMMAND_MENU_SELECTED, eventFunctor, wxID_FIND);
		
		auto mgr = whDataMgr::GetInstance();
		
		Bind(wxEVT_COMMAND_MENU_SELECTED, [this,tool,mgr](wxCommandEvent& evt)
			{
				tool->SetBitmap(m_ResMgr->m_ico_filter24);
				mFindCtrl->SetHint("фильтр");
				mToolBar->Refresh();
				mgr->mRecentDstOidPresenter->SetFilterEnable(1);
				this->OnClickSearchBtn(wxCommandEvent(wxEVT_COMMAND_MENU_SELECTED, wxID_FIND));
				
			}
			, wxID_FILE1);

		Bind(wxEVT_COMMAND_MENU_SELECTED, [this,tool,mgr](wxCommandEvent& evt)
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
			tool->SetBitmap(m_ResMgr->m_ico_filter24);
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
void MoveObjView::BuildTree()
{
	mTree = new wxDataViewCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
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

	Bind(wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, &MoveObjView::OnActivated, this);

}
//-----------------------------------------------------------------------------

void MoveObjView::OnClose(wxCloseEvent& evt)
{
	OnCancel();
}
//-----------------------------------------------------------------------------

void MoveObjView::OnCancel(wxCommandEvent& evt)
{
	this->sigClose();
	EndModal(wxID_CANCEL);
}
//-----------------------------------------------------------------------------
void MoveObjView::OnOk(wxCommandEvent& evt)
{
	auto selected = mTree->GetSelection();
	auto sel_node = static_cast<Node*>(selected.GetID());
	if (selected && sel_node->mTypeId==20)
	{
		auto sel_obj = static_cast<Obj*>(sel_node->mVal);
		wxString qty="1";
		if (mqtySpin->IsShown() && !mqtyCtrl->IsShown())
			qty = wxString::Format("%d", mqtySpin->GetValue());
		else if (!mqtySpin->IsShown() && mqtyCtrl->IsShown())
			qty = mqtyCtrl->GetValue();

		sigMove(sel_obj->mId, qty);
		EndModal(wxID_OK);
	}// if (selected)
}
//---------------------------------------------------------------------------
void MoveObjView::OnActivated(wxDataViewEvent& evt)
{
	OnOk();
}
//---------------------------------------------------------------------------
void MoveObjView::ExpandAll()
{
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	const Node& root = dvmodel->GetNode();
	ExpandAll(root);
}
//---------------------------------------------------------------------------
void MoveObjView::ExpandAll(const Node& node)
{
	for (const auto& child : node.mChilds)
	{
		const Node* ch = child.get();

		wxDataViewItem type((void*)ch);
		mTree->Expand(type);
		ExpandAll(*ch);
	}
}
//---------------------------------------------------------------------------
void MoveObjView::UpdateRecent(const ObjTree& tree)
{
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetRecentTree(tree);
	//ExpandAll(root);

}
//---------------------------------------------------------------------------
void MoveObjView::UpdateDst(const ObjTree& tree)
{
	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetDstTree(tree);

	const Node& root = dvmodel->GetNode();
	ExpandAll(root);

}
//---------------------------------------------------------------------------
void MoveObjView::UpdateMoveable(const rec::PathItem& moveable)
{
	const wxString movLabel = wxString::Format("[%s]%s"
		, moveable.mCls.mLabel.toStr()
		, moveable.mObj.mLabel.toStr()
		//,mMovable->GetData().mCls.mMeasure
		);

	mLblMovableObj->SetLabel(movLabel);

	if (moveable.mCls.mType.IsNull())
		return;
	switch (moveable.mCls.GetClsType())
	{
	case wh::ctQtyByOne:
		mqtySpin->Show();
		mqtyCtrl->Hide();
		break;
	case wh::ctQtyByFloat:
		mqtySpin->Hide();
		mqtyCtrl->Show();
		break;
	default:
		mqtySpin->Hide();
		mqtyCtrl->Hide();
		break;
	}//switch

}
//---------------------------------------------------------------------------
void MoveObjView::EnableRecent(bool enable)
{
	mToolBar->ToggleTool(wxID_PROPERTIES, enable);

	auto dvmodel = dynamic_cast<DvModel*>(mTree->GetModel());
	dvmodel->SetRecentEnable(enable);
	ExpandAll();
}
//---------------------------------------------------------------------------
void MoveObjView::OnClickSearchBtn(wxCommandEvent& event)
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
				if (isFind)
					break;
				
			}
		}//for
		if (isFind)
			break;
	}//for (const auto& tp : dst_node.mChilds)
	//ExpandAll();
}