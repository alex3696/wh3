#include "_pch.h"
#include "detail_ctrlpnl.h"

#include "MainFrame.h"
#include "dlg_move_view_Frame.h"
#include "dlg_act_view_Frame.h"


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

	// Создаём тулбар действий
	mActToolBar = new DetailActToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT);;
	mAuiMgr.AddPane(mActToolBar, wxAuiPaneInfo().
		Name("ActToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);

	// Создаём тулбар истории
	mLogToolBar = new VTableToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_PLAIN_BACKGROUND | wxAUI_TB_TEXT);
	mAuiMgr.AddPane(mLogToolBar, wxAuiPaneInfo().
		Name("ToolBarPane")
		.ToolbarPane().Top().Floatable(false)
		.PaneBorder(false)
		);

	// Создаём панель свойств
	mObjView = new ObjDetailPGView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_SPLITTER_AUTO_CENTER);
	mAuiMgr.AddPane(mObjView, wxAuiPaneInfo().
		Name("ClsPropGrid").Caption("Свойства")
		.CaptionVisible(true)
		//.ToolbarPane()
		.Left()
		.CloseButton(false)
		.MinSize(300, 400)
		//.Fixed()
		.Dockable(true)
		.PaneBorder(false)
		.Layer(2)
		//.Position(1)
		);

	// Создаём панель фильтров истории
	mLogTableFilter = new FilterArrayEditor(this);
	mAuiMgr.AddPane(mLogTableFilter, wxAuiPaneInfo().
		Name("FilterPane").Caption("Фильтр")
		.Left().Layer(1)//.Position(2)
		.MinSize(250, 200)
		.CloseButton(false).Dockable(false).Floatable(false)
		.Hide()
		.PaneBorder(false)
		);

	// Создаём панель таблицы истории
	mLogTable = new wh::VTable(this);
	//mLogTable->SetRowHeight(32);
	mAuiMgr.AddPane(mLogTable, wxAuiPaneInfo().
		Name("LogPane").Caption("LogPane").
		CenterPane().Layer(1).Position(1)
		.CloseButton(true).MaximizeButton(true).PaneBorder(false));
	

	mLogModel = std::make_shared<wh::MLogTable>();
	mObj = std::make_shared<model::Obj>();


	mCtrl.SetModel(mLogModel);
	mCtrl.SetAuiMgr(&mAuiMgr);
	mCtrl.SetViewTable(mLogTable);
	mCtrl.SetViewToolBar(mLogToolBar);
	mCtrl.SetViewFilter(mLogTableFilter);

	mCtrl.SetObjModel(mObj);
	mCtrl.SetObjView(mObjView);
	mCtrl.SetActToolbar(mActToolBar);


	mAuiMgr.Update();
}
//-----------------------------------------------------------------------------
CtrlPnl::~CtrlPnl()
{
	mAuiMgr.UnInit();
}
//-----------------------------------------------------------------------------
void CtrlPnl::SetObject(const rec::ObjInfo& oi)
{
	mObj->SetObject(oi);

	mCtrl.OnCmdLoad(wxCommandEvent(wxID_REFRESH));
	UpdateTab();
	mAuiMgr.Update();
	
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
			ico = &GetResMgr()->m_ico_obj_qty24;
			break;
		case ctSingle:		
			ico = &GetResMgr()->m_ico_obj_num24;
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
