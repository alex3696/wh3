#include "_pch.h"
#include "ModelActBrowser.h"

using namespace wh;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelActBrowserWindow::ModelActBrowserWindow()
	:mActTable(std::make_shared<ModelActTable>())
{
}
//---------------------------------------------------------------------------
void wh::ModelActBrowserWindow::DoSwap(std::shared_ptr<ModelActTable> table)
{
	sigBeforeRefresh(mActTable);
	mActTable.swap(table);
	sigAfterRefresh(mActTable);
}
//---------------------------------------------------------------------------
void wh::ModelActBrowserWindow::UpdateExist()
{
	if (mActTable->empty())
		return;

	wxString str_id;
	for (const auto& it : mActTable->GetStorage())
	{
		if (it->GetTitle().empty())// do not load if title already exists
			str_id += wxString::Format(" OR id=%s", it->GetIdAsString());
	}
	if (str_id.empty())
		return;

	str_id.Remove(0, 3);

	wxString query = wxString::Format(
		"SELECT id, title, color, note "
		" FROM act WHERE %s"
		, str_id);

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ModelActTable::fnModify fn = [this, &table, &row]
			(const std::shared_ptr<ModelActTable::RowType>& irec)
		{
			auto record = std::dynamic_pointer_cast<ActRec64>(irec);
			//record->SetId(table->GetAsString(0, row));
			record->SetTitle(table->GetAsString(1, row));
			record->SetColour(table->GetAsString(2, row));
			record->SetNote(table->GetAsString(3, row));
		};


		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;

			mActTable->InsertOrUpdate(id, fn);
		}//for
	}//if (table)
	sigAfterRefresh(mActTable);
}
//---------------------------------------------------------------------------
void wh::ModelActBrowserWindow::SetActs(const std::set<int64_t>& act_idents)
{
	mActTable->Clear();
	const ModelActTable::fnModify empty_fn 
		= [](const std::shared_ptr<ModelActTable::RowType>&) {};
	for (const auto& aid : act_idents)
		mActTable->InsertOrUpdate(aid, empty_fn);
}
//---------------------------------------------------------------------------
void wh::ModelActBrowserWindow::DoActivate()
{
	std::set<int64_t> sel;
	sigGetSelection(sel);
	if (sel.empty())
		return;

	int64_t aid = *sel.begin();

	auto act = mActTable->GetById(aid);
	if(act && mFuncActivateCallback)
		mFuncActivateCallback(*act);
}
//---------------------------------------------------------------------------
void wh::ModelActBrowserWindow::SetActivateCallback(const FuncActivateCallback & fn)
{
	mFuncActivateCallback = fn;
}
