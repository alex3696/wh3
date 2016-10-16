#include "_pch.h"
#include "DetailActToolBar.h"
#include "config.h"

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
void DetailActToolBar::Build()
{
	ClearTools();

	auto tool_move = AddTool(whID_MOVE, "Переместить", m_ResMgr->m_ico_move24, "Переместить(F6)");
	auto tool_move_here = AddTool(whID_MOVE_HERE, "Переместить сюда", m_ResMgr->m_ico_movehere24, "Переместить сюда(CTRL+F6)");
	auto tool_action = AddTool(whID_ACTION, "Выполнить", m_ResMgr->m_ico_act24, "Выполнить(F7)");

	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)currBaseGroup < (int)bgUser)
	{
		tool_move->SetActive(false);
		tool_move_here->SetActive(false);
		tool_action->SetActive(false);
	}

	Realize();
}
