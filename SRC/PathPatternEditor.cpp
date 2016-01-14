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
	mMnuSetAny = mMenu.Append(miSetAny, L"[*]* любой класс, любой объект");
	mMnuSetCls = mMenu.Append(miSetCls, L"[?]* выбрать только класс,любой объект");
	mMnuSetClsObj= mMenu.Append(miSetClsObj, L"[?]? выбрать объект и соответственно его класс");

	mMnuSetFixObj = mMenu.Append(miSetFixObj, L"[X]? выбрать объект");
	mMnuSetFixAny = mMenu.Append(miSetFixAny, L"[X]* любой объект");

	//Bind(wxEVT_COMMAND_MENU_SELECTED, [](wxCommandEvent& evt){}, miAddToLeft);
	//Bind(wxEVT_COMMAND_MENU_SELECTED, &PathPatternEditor::PnlShowAct, this, miAddToLeft);

	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdAddToLeft, this, miAddToLeft);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdAddToRight, this, miAddToRight);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdRemove, this, miRemove);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetAny, this, miSetAny);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetCls, this, miSetCls);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetClsObj, this, miSetClsObj);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetFixObj, this, miSetFixObj);
	Bind(wxEVT_MENU, &PathPatternEditor::OnCmdSetFixAny, this, miSetFixAny);
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
		return true;
	}
	return false;
}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdAddToLeft(wxCommandEvent& evt)
{
	size_t before_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, before_idx))
		return;
	const auto	qty = mModel->GetChildQty();

	auto new_item = mModel->CreateChild();
	auto before_item = (qty > before_idx) ? mModel->GetChild(before_idx) : SptrIModel(nullptr);
	mModel->Insert(new_item, before_item);
}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdAddToRight(wxCommandEvent& evt)
{
	size_t before_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, before_idx))
		return;
	const auto	qty = mModel->GetChildQty();
	
	before_idx++;

	auto new_item = mModel->CreateChild();
	auto before_item = (qty > before_idx) ? mModel->GetChild(before_idx) : SptrIModel(nullptr);
	mModel->Insert(new_item, before_item);
}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdRemove(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (GetGiuItemIndex(mSelectedItem, model_idx))
	{
		if (mModel->GetChildQty() == 1 )
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

	temppath::model::Item::DataType emptyData;
	mModel->at(model_idx)->SetData(emptyData);

}
//---------------------------------------------------------------------------
void PathPatternEditor::OnCmdSetCls(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;

	auto tv = [this](const wh::rec::Cls* cls, const wh::rec::Obj* obj)->bool
	{
		if (ReqOne_ReqCls == mMode || ReqOne_FixCls == mMode)
			return cls && !obj && 1 == (long)cls->mType;
		return cls && !obj;
	};

	CatDlg dlg(nullptr);
	dlg.SetTargetValidator(tv);

	auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();
	catalog->SetCfg(rec::catCls, false, false);
	//if (FixOne_ReqCls != mMode)
	//	catalog->SetFilterClsKind(ctQtyByOne, foLess, true);
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

	auto tv = [this](const wh::rec::Cls* cls, const wh::rec::Obj* obj)->bool
	{
		if (ReqOne_ReqCls == mMode || ReqOne_FixCls == mMode)
			return cls && obj && 1== (long)cls->mType;
		return cls && obj;
	};

	CatDlg dlg(nullptr);
	dlg.SetTargetValidator(tv);

	auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();
	
	catalog->SetCfg(rec::catCls, false, true);
	//catalog->SetCfg(rec::catObj, false, true);
	//if(FixOne_ReqCls!=mMode)
	//	catalog->SetFilterClsKind(ctQtyByOne, foLess, true);
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
void PathPatternEditor::OnCmdSetFixObj(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;

	auto m = mModel->at(model_idx);
	if (!m)
		return;

	const auto& cls_data = m->GetData();
	long cls_id = cls_data.mCls.mId.IsNull() ? 0 : cls_id = cls_data.mCls.mId;

	if (!cls_id)
		return;
	
	auto catalog = std::make_shared<wh::object_catalog::MObjCatalog>();

	catalog->SetCfg(rec::catCustom, false, true);
	catalog->SetFilterClsKind(ctQtyByOne, foLess, true);
	catalog->SetFilterClsId(cls_id, foEq, true);
	catalog->Load();

	auto tv = [](const wh::rec::Cls* cls, const wh::rec::Obj* obj)->bool
	{
		return cls && obj;
	};

	CatDlg dlg(nullptr);
	dlg.SetTargetValidator(tv);
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
void PathPatternEditor::OnCmdSetFixAny(wxCommandEvent& evt)
{
	size_t model_idx(0);
	if (!GetGiuItemIndex(mSelectedItem, model_idx))
		return;

	//if ((mModel->GetChildQty() - 1) == model_idx)
	{
		auto data = mModel->at(model_idx)->GetData();
		data.mObj = rec::Base();
		mModel->at(model_idx)->SetData(data);
	}

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

	auto onDelNode = std::bind(&PathPatternEditor::OnDelNode, this, ph::_1, ph::_2);
	auto onAfterIns = std::bind(&PathPatternEditor::OnAfterInsert, this, ph::_1, ph::_2, ph::_3);

	connDel = mModel->ConnectBeforeRemove(onDelNode);
	connAfterInsert = mModel->ConnAfterInsert(onAfterIns);
	
	ResetGui();

}
//---------------------------------------------------------------------------
void PathPatternEditor::MakeGuiItem(unsigned int pos)
{
	auto szrPath = this->GetSizer();
	auto itPos = mPathChoice.begin() + pos;		// получаем позицию GUI элемента

	auto ch = new TmpPathItem(this, wxID_ANY);	// создаём новый GUI элемент
	ch->SetModel(mModel->at(pos));				// устанавливаем модель в GUI

	szrPath->Insert(pos, ch, 0, wxALL | wxALIGN_CENTER_VERTICAL, 1);//вставляем в GUI массив
	mPathChoice.insert(itPos, ch);				// вставляем в перечень

	szrPath->Fit(this);
	this->GetParent()->Layout();

	// меню элемента
	auto popupMenu = [this, ch](wxContextMenuEvent& evt)
	{
		//TmpPathItem* ch = mSelectedItem;

		TmpPathItem* ch = dynamic_cast<TmpPathItem*>(evt.GetEventObject());

		size_t model_idx(0);
		if (GetGiuItemIndex(ch, model_idx))
		{
			//bool isFirst = (0 == model_idx);
			bool isLast = (mModel->GetChildQty() - 1 == model_idx);
			//bool isOne = (mModel->GetChildQty() == 1);

			mMnuAddToLeft->Enable(false);
			mMnuAddToRight->Enable(false);
			mMnuRemove->Enable(false);
			mMnuSetAny->Enable(false);
			mMnuSetCls->Enable(false);
			mMnuSetClsObj->Enable(false);
			mMnuSetFixObj->Enable(false);
			mMnuSetFixAny->Enable(false);

			if (isLast && FixOne_ReqCls == mMode)
			{
				mMnuSetCls->Enable(true);
				mMnuSetClsObj->Enable(true);
			}
			else if (isLast && ReqOne_ReqCls == mMode)
			{
				mMnuAddToLeft->Enable(true);
				mMnuSetCls->Enable(true);
				mMnuSetClsObj->Enable(true);
			}
			else if (isLast && ReqOne_FixCls == mMode)
			{
				mMnuAddToLeft->Enable(true);
				mMnuSetFixObj->Enable(true);
				mMnuSetFixAny->Enable(true);
			}
			else
			{
				mMnuAddToLeft->Enable(true);
				mMnuAddToRight->Enable(true);
				mMnuRemove->Enable(true);
				mMnuSetAny->Enable(true);
				mMnuSetCls->Enable(true);
				mMnuSetClsObj->Enable(true);
				mMnuSetFixObj->Enable(true);
				mMnuSetFixAny->Enable(true);

			}

		}
		mSelectedItem = ch;
		wxRect rect = ch->GetRect();
		wxPoint pt = this->ClientToScreen(rect.GetBottomLeft());
		pt = ScreenToClient(pt);
		PopupMenu(&mMenu, pt);
	};

	//ch->Bind(wxEVT_RIGHT_DOWN, popupMenu);
	//ch->Bind(wxEVT_LEFT_DOWN, popupMenu);
	ch->Bind(wxEVT_CONTEXT_MENU, popupMenu); // привязывем меню к элементу

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
void PathPatternEditor::OnAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore)
{
	size_t pos;

	for (const auto& curr : newItems)
	{
		auto item = std::dynamic_pointer_cast<temppath::model::Item>(curr);
		if (!item || !vec.GetItemPosition(item, pos))
			return;
		MakeGuiItem(pos);
		pos++;
	}

}
//---------------------------------------------------------------------------
void PathPatternEditor::ResetGui()
{
	for (const auto ch : mPathChoice)
		delete ch;
	mPathChoice.clear();

	auto qty = mModel->GetChildQty();
	for (size_t i = 0; i < qty; i++)
		MakeGuiItem(i);

	auto szrPath = this->GetSizer();
	szrPath->Fit(this);
	this->GetParent()->Layout();

}

//-----------------------------------------------------------------------------

