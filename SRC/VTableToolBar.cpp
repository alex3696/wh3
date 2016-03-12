#include "_pch.h"
#include "VTableToolbar.h"

using namespace wh;
//-----------------------------------------------------------------------------

VTableToolBar::VTableToolBar(wxWindow* parent,
							wxWindowID id ,
							const wxPoint& pos ,
							const wxSize& size ,
							long style )
	:wxAuiToolBar(parent, id, pos, size, style)
{

}
//-----------------------------------------------------------------------------
void VTableToolBar::SetModel(std::shared_ptr<ITable> model)
{
	mConnAfterInsert.disconnect();
	mConnAfterRemove.disconnect();
	mConnAfterChange.disconnect();

	//mConnChangePageLimit.disconnect();;
	//mConnChangePageNo.disconnect();;
	//mMTable = model;

	if (!model)
		return;
	namespace ph = std::placeholders;

	auto fnAI = std::bind(&VTableToolBar::OnAfterInsert, this, ph::_1, ph::_2, ph::_3);
	auto fnAR = std::bind(&VTableToolBar::OnAfterRemove, this, ph::_1, ph::_2);
	auto fnAC = std::bind(&VTableToolBar::OnAfterChange, this, ph::_1, ph::_2);

	mConnAfterInsert = model->ConnAfterInsert(fnAI);
	mConnAfterRemove = model->ConnectAfterRemove(fnAR);
	mConnAfterChange = model->ConnectChangeSlot(fnAC);

	BuildToolBar();
	OnTableChange(*model);
}
//-----------------------------------------------------------------------------
void VTableToolBar::BuildToolBar()
{
	ClearTools();
	
	if (mEnableFilter)
	{
		AddTool(wxID_FIND, "Фильтр", m_ResMgr->m_ico_filter24
			, "Показать фильтр(CTRL+F)", wxITEM_CHECK);
		//std::function<void(wxCommandEvent&)>
		//	fn = std::bind(&VTableToolBar::OnCmdFind, this, ph::_1);
		//auto fnFind = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnFind, wxID_FIND);
	}
	if (mEnableLoad)
	{
		AddTool(wxID_REFRESH, "Обновить", m_ResMgr->m_ico_refresh24, "Обновить(CTRL+R)");
		//std::function<void(wxCommandEvent&)> 
		//	fn = std::bind(&VTableToolBar::OnCmdLoad, this, ph::_1);
		//auto fnLoad = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnLoad, wxID_REFRESH);
	}
	if (mEnableSave)
	{
		AddTool(wxID_SAVE, "Сохранить", m_ResMgr->m_ico_save24, "Сохранить(CTRL+S)");
		//std::function<void(wxCommandEvent&)> 
		//	fn = std::bind(&VTableToolBar::OnCmdSave, this, ph::_1);
		//auto fnSave = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnSave, wxID_SAVE);
	}

	AddSeparator();

	if (mEnableInsert)
	{
		AddTool(wxID_NEW, "Добавить", m_ResMgr->m_ico_create24, "Добавить(CTRL+N)");
		//std::function<void(wxCommandEvent&)>
		//	fn = std::bind(&VTableToolBar::OnCmdInsert, this, ph::_1);
		//auto fnInsert = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnInsert, wxID_NEW);
	}
	if (mEnableRemove)
	{
		AddTool(wxID_REMOVE, "Удалить", m_ResMgr->m_ico_delete24, "Удалить(DELETE)");
		//std::function<void(wxCommandEvent&)>
		//	fn = std::bind(&VTableToolBar::OnCmdRemove, this, ph::_1);
		//auto fnRemove = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnRemove, wxID_REMOVE);
	}
	
	if (mEnableChange)
	{
		AddTool(wxID_EDIT, "Редактировать", m_ResMgr->m_ico_edit24, "Редактировать(CTRL+O)");
		//std::function<void(wxCommandEvent&)>
		//	fn = std::bind(&VTableToolBar::OnCmdChange, this, ph::_1);
		//auto fnChange = std::bind(SafeCallCommandEvent(), fn, ph::_1);
		//Bind(wxEVT_COMMAND_MENU_SELECTED, fnChange, wxID_EDIT);
	}

	AddSeparator();

	AddTool(wxID_BACKWARD, "назад"
		, wxArtProvider::GetBitmap(wxART_GO_BACK, wxART_MENU), "Предыдущая страница(PageUp)");
	mPageLabel = new wxStaticText(this, wxID_ANY, "MyLabel");
	//mPageLabel->Wrap(-1);

	AddControl(mPageLabel, "Показаны строки");
	AddTool(wxID_FORWARD, "вперёд"
		, wxArtProvider::GetBitmap(wxART_GO_FORWARD, wxART_MENU), "Следующая страница(PageDown)");


	Realize();
	
}
//-----------------------------------------------------------------------------
void VTableToolBar::OnTableChange(const IModel& vec)
{
	const ITable* table = dynamic_cast<const ITable*>(&vec);
	if (!table)
		return;

	bool exist = (msExist == table->GetState());
	EnableTool(wxID_SAVE, !exist);
	Refresh();


	const auto& limit = table->mPageLimit->GetData();
	const auto& no = table->mPageNo->GetData();
	const auto& curr_qty = table->GetChildQty();

	wxString page_label;
	page_label << " " << (no*limit) << " - " << (no*limit + curr_qty);
	mPageLabel->SetLabel(page_label);

	EnableTool(wxID_BACKWARD, no ? true : false);
	EnableTool(wxID_FORWARD, curr_qty >= limit);
	Refresh();
}
//-----------------------------------------------------------------------------
void VTableToolBar::OnAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	OnTableChange(vec);
}//OnAfterInsert
//-----------------------------------------------------------------------------
void VTableToolBar::OnAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	OnTableChange(vec);
}//OnAfterRemove
//-----------------------------------------------------------------------------
void VTableToolBar::OnAfterChange(const IModel& vec, const std::vector<unsigned int>& itemVec)
{
	OnTableChange(vec);
}//OnAfterChange