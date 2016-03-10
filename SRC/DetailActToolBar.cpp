#include "_pch.h"
#include "DetailActToolBar.h"

using namespace wh;
//-----------------------------------------------------------------------------

DetailActToolBar::DetailActToolBar(wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style)
	:wxAuiToolBar(parent, id, pos, size, style)
{

}
//-----------------------------------------------------------------------------
bool DetailActToolBar::ConnCmdCtrl(int cmd_id, const DetailActCtrl& ctrl)
{
	namespace ph = std::placeholders;
	auto wnd = this->GetParent();

	const std::function<void(wxCommandEvent&)>* fn = ctrl.GetCmdFunction(cmd_id);
	if (fn && (*fn))
	{
		std::function<void(wxCommandEvent&)> safe_fn 
			= std::bind(SafeCallCommandEvent(), *fn, ph::_1);
		wnd->Bind(wxEVT_COMMAND_MENU_SELECTED, safe_fn, cmd_id);
		return true;
	}
	return false;
}
//-----------------------------------------------------------------------------
void DetailActToolBar::SetCtrl(const DetailActCtrl& ctrl)
{
	EnableTool(whID_MOVE, ConnCmdCtrl(whID_MOVE, ctrl));
	EnableTool(whID_MOVE_HERE, ConnCmdCtrl(whID_MOVE_HERE, ctrl));
	EnableTool(whID_ACTION, ConnCmdCtrl(whID_ACTION, ctrl));
}
//-----------------------------------------------------------------------------
void DetailActToolBar::Build()
{
	auto parent = this->GetParent();
	wxWindowUpdateLocker	wndLockUpdater(parent);

	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_NORMAL, WXK_F6, whID_MOVE);
	entries[0].Set(wxACCEL_CTRL, WXK_F6, whID_MOVE_HERE);
	entries[1].Set(wxACCEL_NORMAL, WXK_F7, whID_ACTION);
	wxAcceleratorTable accel(2, entries);
	SetAcceleratorTable(accel);

	ClearTools();

	auto tool_move = AddTool(whID_MOVE, "Переместить", m_ResMgr->m_ico_move24, "Переместить(F6)");
	auto tool_move_here = AddTool(whID_MOVE_HERE, "Переместить сюда", m_ResMgr->m_ico_movehere24, "Переместить сюда(CTRL+F6)");
	auto tool_action = AddTool(whID_ACTION, "Выполнить", m_ResMgr->m_ico_act24, "Выполнить(F7)");

	const auto currBaseGroup = whDataMgr::GetInstance()->mCfg.Prop.mBaseGroup;
	if ((int)currBaseGroup < (int)bgUser)
	{
		tool_move->SetActive(false);
		tool_move_here->SetActive(false);
		tool_action->SetActive(false);
	}

	Realize();
}
