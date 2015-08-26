#include "_pch.h"
#include "detail_ctrlpnl.h"
#include "PGClsPid.h"


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
	
	mToolBar->AddTool(wxID_REFRESH, "��������", m_ResMgr->m_ico_refresh24);
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
		Name("ClsPropGrid").Caption("��������")
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

	wxPGChoices soc;
	soc.Add(L"�����������", 0);
	soc.Add(L"��������", 1);
	soc.Add(L"��������������(�������������)", 2);
	soc.Add(L"��������������(�������)", 3);

	auto pgObjMain = mPropGrid->Append(new wxPropertyCategory("������"));
	pgObjMain->AppendChild(new wxStringProperty("���", "ObjName"));
	pgObjMain->AppendChild(new wxFloatProperty("����������", wxPG_LABEL));
	pgObjMain->AppendChild(new wxStringProperty("ID", wxPG_LABEL));
	pgObjMain->AppendChild(new wxStringProperty("PID", wxPG_LABEL));
	pgObjMain->AppendChild(new wxStringProperty("��������������", wxPG_LABEL));

	auto pgObjUser = pgObjMain->AppendChild(
		new wxPropertyCategory("��������","pgObjUser"));
	pgObjUser->AppendChild(new wxStringProperty("Test1", wxPG_LABEL));
	pgObjUser->AppendChild(new wxStringProperty("Test2", wxPG_LABEL));

	
	auto pgClsMain = mPropGrid->Append(new wxPropertyCategory("�����"));
	pgClsMain->AppendChild(new wxStringProperty("���", "ClsName"));
	pgClsMain->AppendChild(new wxLongStringProperty(L"��������"));
	pgClsMain->AppendChild(new wxEnumProperty(L"��� �����������", wxPG_LABEL, soc, 0));
	pgClsMain->AppendChild(new wxStringProperty(L"��.���������", wxPG_LABEL));
	pgClsMain->AppendChild(new wxStringProperty(L"#"));
	pgClsMain->AppendChild(new wxStringProperty(L"VID"));
	pgClsMain->AppendChild(new wxClsParentProperty(L"������������ �����"));
	auto pgClsUser = pgClsMain->AppendChild(
		new wxPropertyCategory("��������", "pgClsUser"));
	pgClsUser->AppendChild(new wxStringProperty("Test3", wxPG_LABEL));
	pgClsUser->AppendChild(new wxStringProperty("Test4", wxPG_LABEL));

	mPropGrid->SetPropertyReadOnly(pgObjMain, true);
	mPropGrid->SetPropertyReadOnly(pgClsMain, true);

	mAuiMgr.Update();

	auto funcOnChange = std::bind(&CtrlPnl::OnChangeMainDetail,
		this, std::placeholders::_1, std::placeholders::_2);
	mChangeMainDetail = mObj->DoConnect(
		model::Obj::Op::AfterChange, funcOnChange);
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
	mObj->Load();
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

	mPropGrid->SetPropertyValueString("ObjName", main_detail.mObj.mLabel);
	mPropGrid->SetPropertyValueString("����������", main_detail.mObj.mQty);
	mPropGrid->SetPropertyValueString("ID", main_detail.mObj.mID);
	mPropGrid->SetPropertyValueString("PID", main_detail.mObj.mPID);
	mPropGrid->SetPropertyValueString("��������������", "qwe qwe qwe");
	
	mPropGrid->SetPropertyValueString("ClsName", main_detail.mCls.mLabel);
	mPropGrid->SetPropertyValueString("��������", main_detail.mCls.mComment);
	mPropGrid->SetPropertyValueString("��� �����������", main_detail.mCls.mType);
	mPropGrid->SetPropertyValueString("��.���������", main_detail.mCls.mMeasure);
	mPropGrid->SetPropertyValueString("#", main_detail.mCls.mID);
	mPropGrid->SetPropertyValueString("VID", main_detail.mCls.mVID);
	rec::Base cp(main_detail.mCls.mParent);
	mPropGrid->SetPropVal("������������ �����", wxVariant(cp));


}