#include "_pch.h"
#include "detail_ctrlpnl.h"
#include "PGClsPid.h"
#include "MainFrame.h"

using namespace wh;
using namespace wh::detail::view;

//-----------------------------------------------------------------------------
CtrlPnl::CtrlPnl(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, id, pos, size, style, name)
{
	mAuiMgr.SetManagedWindow(this);

	mToolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_TEXT | wxAUI_TB_OVERFLOW);
	
	mToolBar->AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &CtrlPnl::OnCmdReload, this, wxID_REFRESH);
	
	mToolBar->Realize();

	mAuiMgr.AddPane(mToolBar, wxAuiPaneInfo().
		Name(wxT("m_MainToolBar")).Caption(wxT("Main toolbar"))
		.CaptionVisible(false)
		.ToolbarPane()
		.Top()
		.Fixed()
		.Dockable(false)
		.PaneBorder(false)
		.Layer(1)
		.Position(1)
		//.LeftDockable(false)
		//.RightDockable(false)
		);


	
	
	
	mPropGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	mAuiMgr.AddPane(mPropGrid, wxAuiPaneInfo().
		Name("ClsPropGrid").Caption("Свойства")
		.CaptionVisible(true)
		//.ToolbarPane()
		.Left()
		.CloseButton(false)
		.MinSize(300, 400)
		//.Fixed()
		.Dockable(true)
		.PaneBorder(false)
		.Layer(1)
		.Position(1)
		);

	auto pg_cls = mPropGrid->Append(new wxPropertyCategory("Свойства класса"));
	mPropGrid->AppendIn(pg_cls, new wxClsProperty("Основные", "base_cls_prop"));
	mPropGrid->AppendIn(pg_cls, new wxPropertyCategory("Пользовательские", "user_cls_prop"));

	auto pg_obj = mPropGrid->Append(new wxPropertyCategory("Свойства объекта"));
	mPropGrid->AppendIn(pg_obj, new wxObjTitleProperty("Основные", "base_obj_prop"));
	mPropGrid->AppendIn(pg_obj, new wxPropertyCategory("Пользовательские", "user_obj_prop"));

	mPropGrid->SetPropertyReadOnly(pg_cls, true);
	mPropGrid->SetPropertyReadOnly(pg_obj, true);

	mAuiMgr.Update();

	namespace sph = std::placeholders;

	auto funcOnChange = std::bind(&CtrlPnl::OnChangeMainDetail,
		this, sph::_1, sph::_2);
	mChangeMainDetail = mObj->DoConnect(moAfterUpdate, funcOnChange);

	mConnClsPropAppend = mObj->GetClsPropArray()->ConnAfterInsert(
		std::bind(&CtrlPnl::OnClsPropAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnClsPropRemove = mObj->GetClsPropArray()->ConnectBeforeRemove(
		std::bind(&CtrlPnl::OnClsPropBeforeRemove, this, sph::_1, sph::_2));
	mConnClsPropChange = mObj->GetClsPropArray()->ConnectChangeSlot(
		std::bind(&CtrlPnl::OnClsPropChange, this, sph::_1, sph::_2));

	mConnObjPropAppend = mObj->GetObjPropArray()->ConnAfterInsert(
		std::bind(&CtrlPnl::OnObjPropAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnObjPropRemove = mObj->GetObjPropArray()->ConnectBeforeRemove(
		std::bind(&CtrlPnl::OnObjPropBeforeRemove, this, sph::_1, sph::_2));
	mConnObjPropChange = mObj->GetObjPropArray()->ConnectChangeSlot(
		std::bind(&CtrlPnl::OnObjPropChange, this, sph::_1, sph::_2));
}
//-----------------------------------------------------------------------------
CtrlPnl::~CtrlPnl()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void CtrlPnl::SetObject(const wxString& cls_id, const wxString& obj_id, const wxString& obj_pid)
{
	mObj->SetObject(cls_id, obj_id, obj_pid);
	OnCmdReload(wxCommandEvent());
}
//-----------------------------------------------------------------------------
void CtrlPnl::OnCmdReload(wxCommandEvent& evt)
{
	mObj->Load();
}
//-----------------------------------------------------------------------------
void CtrlPnl::OnChangeMainDetail(const IModel* model, const wh::detail::model::Obj::T_Data* data)
{
	if (!mObj)
		return;

	const model::Obj::T_Data& main_detail = mObj->GetData();

	mPropGrid->SetPropVal("base_cls_prop", wxVariant(*(wh_rec_Cls*)&main_detail.mCls));
	mPropGrid->SetPropVal("base_obj_prop", wxVariant(*(wh_rec_ObjTitle*)&main_detail.mObj));

	UpdateTab();
}
//-----------------------------------------------------------------------------
void CtrlPnl::UpdatePGColour()
{
	if (!mObj)
		return;
	wxWindowUpdateLocker	wndUpdateLocker(this);

	const model::Obj::T_Data& main_detail = mObj->GetData();
	wxColour cl;
	if (!main_detail.mCls.mType.IsNull())
		switch (main_detail.mCls.GetClsType())
		{
			default:
				cl = (wxColour(240, 240, 240));
				mPropGrid->HideProperty("user_obj_prop");
				break;
			case ctQtyByFloat:	case ctQtyByOne:
				cl = (wxColour(210, 240, 250));
				mPropGrid->HideProperty("user_obj_prop");
				break;
			case ctSingle:
				cl = (wxColour(250, 240, 210));
				mPropGrid->HideProperty("user_obj_prop", false);
				break;
		}//switch

	mPropGrid->SetPropertyBackgroundColour("Свойства класса", cl);
	//mPropGrid->SetPropertyBackgroundColour("user_cls_prop", cl);


}
//-----------------------------------------------------------------------------
void CtrlPnl::UpdateTab()
{
	if (!mObj)
		return;
	wxWindowUpdateLocker	wndUpdateLocker(this);

	const model::Obj::T_Data& main_detail = mObj->GetData();
	const wxIcon*  ico(&wxNullIcon);
	if (!main_detail.mCls.mType.IsNull())
		switch (main_detail.mCls.GetClsType())
	{
		default:			
			ico = &GetResMgr()->m_ico_type_abstract24;
			break;
		case ctQtyByFloat:	case ctQtyByOne:	
			ico = &GetResMgr()->m_ico_type_qty24;
			break;
		case ctSingle:		
			ico = &GetResMgr()->m_ico_type_num24;
			break;
	}//switch

	wxWindow* notebook = this->GetParent();
	MainFrame* main_farame(nullptr);
	if (notebook)
		main_farame = dynamic_cast<MainFrame*>(notebook->GetParent());
	
	if (!main_farame)
		return;

	const wxString lbl = wxString::Format("[%s]%s (%s %s)"
		, main_detail.mCls.mLabel.toStr()
		, main_detail.mObj.mLabel.toStr()
		, main_detail.mObj.mQty.toStr()
		, main_detail.mCls.mMeasure.toStr()
		);
	main_farame->UpdateTab(this, lbl, *ico);
	//mPropGrid->SetCaptionBackgroundColour(wxColour(255, 200, 200));

}
//-----------------------------------------------------------------------------
void CtrlPnl::OnClsPropAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	auto propCategory = mPropGrid->GetProperty("user_cls_prop");
	for (const auto& curr : newItems)
	{
		auto item = dynamic_pointer_cast<model::ClsProp>(curr);
		const auto& data = item->GetData();
		propCategory->AppendChild(new wxStringProperty(
			data.mProp.mLabel
			, wxString::Format("ClsProp_%s", data.mProp.mLabel.toStr())
			, data.mVal));
	}

	UpdatePGColour();
	mPropGrid->SetPropertyReadOnly(propCategory, true);

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void CtrlPnl::OnClsPropBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	auto cls_prop_array = mObj->GetClsPropArray();
	
	auto propCategory = mPropGrid->GetProperty("user_cls_prop");
	if (cls_prop_array->size() == remVec.size())
	{
		propCategory->DeleteChildren();
	}
	else
	{
		size_t pos = 0;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
			{
				mPropGrid->DeleteProperty(pos);
			}

		}
	}

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void CtrlPnl::OnClsPropChange(const IModel& model, const std::vector<unsigned int>& vec)
{
	auto prop_array = mObj->GetClsPropArray();

	auto propCategory = mPropGrid->GetProperty("user_cls_prop");

	for (const unsigned int& i : vec)
	{
		auto prop_model = prop_array->at(i);
		const auto& prop_data = prop_model->GetData();

		propCategory->Item(i)->SetValueFromString(prop_data.mVal);
	}
}
//-----------------------------------------------------------------------------
void CtrlPnl::OnObjPropAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	auto propCategory = mPropGrid->GetProperty("user_obj_prop");

	for (const auto& curr : newItems)
	{
		auto item = dynamic_pointer_cast<model::ObjProp>(curr);
		const auto& data = item->GetData();
		propCategory->AppendChild(new wxStringProperty(
			data.mProp.mLabel
			, wxString::Format("ObjProp_%s", data.mProp.mLabel.SqlVal())
			, data.mVal));
	}

	mPropGrid->SetPropertyReadOnly(propCategory, true);

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void CtrlPnl::OnObjPropBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	auto obj_prop_array = mObj->GetObjPropArray();

	auto propCategory = mPropGrid->GetProperty("user_obj_prop");
	if (obj_prop_array->GetChildQty() == remVec.size())
	{
		propCategory->DeleteChildren();
	}
	else
	{
		size_t pos = 0;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
			{
				mPropGrid->DeleteProperty(pos);
			}

		}
	}
	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void CtrlPnl::OnObjPropChange(const IModel& model, const std::vector<unsigned int>& vec)
{
	auto prop_array = mObj->GetObjPropArray();

	auto propCategory = mPropGrid->GetProperty("user_obj_prop");

	for (const unsigned int& i : vec)
	{
		auto prop_model = prop_array->at(i);
		const auto& prop_data = prop_model->GetData();
		
		propCategory->Item(i)->SetValueFromString(prop_data.mVal);
	}
}