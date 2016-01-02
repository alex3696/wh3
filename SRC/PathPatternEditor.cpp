#include "_pch.h"
#include "PathPatternEditor.h"
#include "dlgselectcls_ctrlpnl.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// TmpStrPathItem
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TmpStrPathItem::TmpStrPathItem(wxWindow *parent,
	wxWindowID id,
	const wxString& label,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxStaticText(parent, id, label, pos, size, style, name)
{
	Bind(wxEVT_ENTER_WINDOW, &TmpStrPathItem::OnEnter, this);
	Bind(wxEVT_LEAVE_WINDOW, &TmpStrPathItem::OnLeave, this);
	SetToolTip(wxT("Нажмите для редактирования"));
}
//---------------------------------------------------------------------------
void TmpStrPathItem::OnEnter(wxMouseEvent& event)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	Refresh();
}
//---------------------------------------------------------------------------
void TmpStrPathItem::OnLeave(wxMouseEvent& event)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	Refresh();
}
//---------------------------------------------------------------------------
void TmpStrPathItem::SetModel(std::shared_ptr<temppath::model::Item>& newModel)
{
	if (newModel == mModel)
		return;
	mModel = newModel;
	if (!mModel)
		return;

	namespace ph = std::placeholders;
	//namespace cat = wh::object_catalog;

	auto onChange = std::bind(&TmpStrPathItem::OnChange, this, ph::_1, ph::_2);

	connChange = mModel->DoConnect(moAfterUpdate, onChange);
	OnChange(mModel.get(), &mModel->GetData());

}

//---------------------------------------------------------------------------
void TmpStrPathItem::OnChange(const IModel*, const temppath::model::Item::DataType* data)
{
	if (!data)
		return;
	wxString chStr(" /* ");
	if (!data->mCls.mId.IsNull() || !data->mObj.mId.IsNull())
	{
		const wxString clsStr = data->mCls.mId.IsNull() ? "*" : data->mCls.mLabel.toStr();
		const wxString objStr = data->mObj.mId.IsNull() ? "*" : data->mObj.mLabel.toStr();
		chStr = wxString::Format(L" /[%s]%s ", clsStr, objStr);//\u02C5 |v
	}
	SetLabel(chStr);
	
	
	// 
	auto parent = GetParent();
	while (parent)
	{
		parent->Layout();
		parent = parent == GetParent() ? nullptr : GetParent();
	}
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// PathPatternEditor
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PathPatternEditor::PathPatternEditor(wxWindow *parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxScrolledWindow(parent, winid, pos, size, style, name)
{
	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	this->SetScrollRate(5, 5);
	wxBoxSizer* szrPath = new wxBoxSizer(wxHORIZONTAL);
	this->SetMinSize(wxSize(-1, 50));

	this->SetSizer(szrPath);
	this->Layout();
	szrPath->Fit(this);

	auto rc = ResMgr::GetInstance();

	mMnuAddToLeft = AppendBitmapMenu(&mMenu, miAddToLeft, L"+ добавить слева", rc->m_ico_plus16);
	mMnuAddToRight = AppendBitmapMenu(&mMenu, miAddToRight, L"добавить справа +", rc->m_ico_plus16);
	mMnuRemove = AppendBitmapMenu(&mMenu, miRemove, L"удалить", rc->m_ico_delete16);
	mMenu.AppendSeparator();
	mMnuSetAny = mMenu.Append(miSetAny, L"* любой");
	mMnuSetCls = mMenu.Append(miSetCls, L"[?]* выбрать только класс,объект любой");
	mMnuSetClsObj= mMenu.Append(miSetClsObj, L"[?]? выбрать объект и соответственно его класс");

	//Bind(wxEVT_COMMAND_MENU_SELECTED, [](wxCommandEvent& evt){}, miAddToLeft);
	//Bind(wxEVT_COMMAND_MENU_SELECTED, &PathPatternEditor::PnlShowAct, this, miAddToLeft);

	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdAddToLeft, this, miAddToLeft);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdAddToRight, this, miAddToRight);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdRemove, this, miRemove);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetAny, this, miSetAny);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetCls, this, miSetCls);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetClsObj, this, miSetClsObj);
}
//---------------------------------------------------------------------------
bool PathPatternEditor::GetGiuItemIndex(TmpPathItem* ch, size_t& model_idx)
{
	const PtrIdx& ptrIdx = mPathChoice.get<1>();
	CPtrIterator ptrIt = ptrIdx.find(ch);
	if (ptrIdx.end() != ptrIt)
	{
		CRndIterator rndIt = mPathChoice.project<0>(ptrIt);
		CRndIterator rndBegin = mPathChoice.cbegin();
		model_idx = std::distance(rndBegin, rndIt);
	}
	else
		return false;
	return true;
}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdAddToLeft(wxCommandEvent& evt)
{
	size_t model_idx(0);

	if (GetGiuItemIndex(mSelectedItem, model_idx))
	{
		if (1 == mModel->GetChildQty() || 0 != model_idx)
			mModel->InsertChild(mModel->CreateChild(), mModel->GetChild(model_idx));

	}

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdAddToRight(wxCommandEvent& evt)
{
	size_t model_idx(0);

	if (GetGiuItemIndex(mSelectedItem, model_idx))
	{
		if (mPathChoice.size() - 1 != model_idx)
			mModel->InsertChild(mModel->CreateChild(), mModel->GetChild(model_idx + 1));

	}

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdRemove(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (GetGiuItemIndex(mSelectedItem, model_idx))
	{
		if (mDoNotDeleteFirst && 0 == model_idx)
			return;
		if (mDoNotDeleteLast && mModel->GetChildQty() == (model_idx + 1))
			return;

		mModel->DelChild(model_idx);
	}

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdSetAny(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;

	long cls_id = 0;
	if ((mModel->GetChildQty() - 1) == model_idx)
	{
		auto data = mModel->at(model_idx)->GetData();
		data.mObj = rec::Base();
		mModel->at(model_idx)->SetData(data);
	}
	else
	{
		temppath::model::Item::DataType emptyData;
		mModel->at(model_idx)->SetData(emptyData);
	}

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdSetCls(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;

	
	CatDlg dlg(nullptr);
	dlg.SetTargetObj(false);

	auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();
	catalog->SetCatalog(false);
	catalog->PropEnable(false);
	catalog->ObjEnable(false);
	catalog->SetFilterClsKind(ctQtyByOne, foLess, true);
	catalog->Load();

	dlg.SetModel(catalog);
	if (wxID_OK == dlg.ShowModal())
	{
		wh::rec::Cls cls;
		if (dlg.GetSelectedCls(cls))
		{
			temppath::model::Item::DataType data;
			data.mCls = cls;
			mModel->at(model_idx)->SetData(data);
		}
	}

}//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdSetClsObj(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;
	
	long cls_id = 0;
	if ((mModel->GetChildQty() - 1) == model_idx)
	{
		cls_id = mModel->at(model_idx)->GetData().mCls.mId;
	}



	CatDlg dlg(nullptr);
	dlg.SetTargetObj(true);

	auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();
	
	catalog->PropEnable(false);
	catalog->ObjEnable(true);
	catalog->SetFilterClsKind(ctQtyByOne, foLess, true);
	if (cls_id)
	{
		catalog->SetCatalog(false);
		catalog->SetFilterClsId(cls_id, foEq, true);
	}
	else
		catalog->SetCatalog(true);
	catalog->Load();

	dlg.SetModel(catalog);
	if (wxID_OK == dlg.ShowModal())
	{
		wh::rec::ObjInfo obj;
		if (dlg.GetSelectedObj(obj))
		{
			temppath::model::Item::DataType data;
			data.mCls = obj.mCls;
			data.mObj = obj.mObj;
			mModel->at(model_idx)->SetData(data);
		}
	}
}

//---------------------------------------------------------------------------
void PathPatternEditor::DoNotDeleteFirst(bool val)
{
	mDoNotDeleteFirst = val;
}

//---------------------------------------------------------------------------
void PathPatternEditor::DoNotDeleteLast(bool val)
{
	mDoNotDeleteLast = val;
}

//---------------------------------------------------------------------------
void PathPatternEditor::SetModel(std::shared_ptr<temppath::model::Array>& newModel)
{
	if (newModel == mModel)
		return;
	mModel = newModel;
	if (!mModel)
		return;

	namespace ph = std::placeholders;

	auto onAddNode = std::bind(&PathPatternEditor::OnAddNode, this, ph::_1, ph::_2);
	auto onDelNode = std::bind(&PathPatternEditor::OnDelNode, this, ph::_1, ph::_2);
	auto onAfterReset = std::bind(&PathPatternEditor::OnAfterReset, this, ph::_1);
	auto onAfterIns = std::bind(&PathPatternEditor::OnAfterInsert, this, ph::_1, ph::_2, ph::_3);

	connAdd = mModel->ConnectAppendSlot(onAddNode);
	connDel = mModel->ConnectBeforeRemove(onDelNode);
	connAfterReset = mModel->ConnAfterReset(onAfterReset);
	connAfterInsert = mModel->ConnAfterInsert(onAfterIns);

	mModel->Reset();



}
//---------------------------------------------------------------------------
void PathPatternEditor::MakeGuiItem(unsigned int pos)
{
	auto szrPath = this->GetSizer();
	auto itPos = mPathChoice.begin() + pos;

	auto ch = new TmpPathItem(this, wxID_ANY);
	ch->SetModel(mModel->at(pos));

	szrPath->Insert(pos, ch, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);
	mPathChoice.insert(itPos, ch);

	szrPath->Fit(this);
	this->GetParent()->Layout();

	// wxEVT_CONTEXT_MENU wxContextMenuEvent
	//auto popupMenu = [this, ch](wxMouseEvent& evt)
	auto popupMenu = [this, ch](wxContextMenuEvent& evt)
	{
		//TmpPathItem* ch = mSelectedItem;

		TmpPathItem* ch = dynamic_cast<TmpPathItem*>(evt.GetEventObject());

		size_t model_idx(0);
		if (GetGiuItemIndex(ch, model_idx))
		{
			bool isFirst = (0 == model_idx);
			bool isLast = (mModel->GetChildQty() - 1 == model_idx);
			bool isOne = (mModel->GetChildQty() == 1);

			mMnuAddToLeft->Enable(!isFirst || isOne);
			mMnuAddToRight->Enable(!isLast);
			mMnuRemove->Enable(!(mDoNotDeleteFirst && isFirst));
			mMnuRemove->Enable(!(mDoNotDeleteLast && isLast));
			//mMnuSetAny->Enable(!isLast);
			mMnuSetCls->Enable(!isLast);

		}
		mSelectedItem = ch;
		wxRect rect = ch->GetRect();
		wxPoint pt = this->ClientToScreen(rect.GetBottomLeft());
		pt = ScreenToClient(pt);
		PopupMenu(&mMenu, pt);
	};

	//ch->Bind(wxEVT_RIGHT_DOWN, popupMenu);
	//ch->Bind(wxEVT_LEFT_DOWN, popupMenu);
	ch->Bind(wxEVT_CONTEXT_MENU, popupMenu);

}


//---------------------------------------------------------------------------
void PathPatternEditor::OnDelNode(const IModel& model, const std::vector<unsigned int>& vec)
{
	std::vector<TmpPathItem*> to_del;
	for (const auto idx : vec)
	{
		auto it = mPathChoice.begin() + idx;
		to_del.emplace_back(*it);
	}

	for (const auto ch : to_del)
	{
		delete ch;
		PtrIdx& ptrIdx = mPathChoice.get<1>();
		PtrIterator ptrIt = ptrIdx.find(ch);
		if (ptrIdx.end() != ptrIt)
			ptrIdx.erase(ptrIt);
	}
		
	auto szrPath = this->GetSizer();
	szrPath->Fit(this);
	this->GetParent()->Layout();

}
//---------------------------------------------------------------------------
void PathPatternEditor
::OnAddNode(const IModel& model, const std::vector<unsigned int>& vec)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		size_t pos = vec[i];
		MakeGuiItem(pos);
	}
}

//---------------------------------------------------------------------------
void PathPatternEditor::OnAfterInsert(const IModel& vec
	, const std::shared_ptr<IModel>& itemBefore
	, std::shared_ptr<IModel>& newItem)
{
	size_t pos;
	auto item = std::dynamic_pointer_cast<temppath::model::Item>(newItem);

	if (!item || !vec.GetItemPosition(newItem, pos))
		return;

	MakeGuiItem(pos);

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnAfterReset(const IModel& model)
{
	auto szrPath = this->GetSizer();

	auto qty = model.GetChildQty();
	for (size_t i = 0; i < qty; i++)
		MakeGuiItem(i);

}

//-----------------------------------------------------------------------------

