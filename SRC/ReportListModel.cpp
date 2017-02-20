#include "_pch.h"
#include "ReportListModel.h"
#include "globaldata.h"

using namespace wh;

//-----------------------------------------------------------------------------
void ReportListModel::UpdateList()
{
	{
		auto p0 = GetTickCount();
		mRepList.clear();
		wxLogMessage(wxString::Format("%d \t ReprtListModel \t clear list", GetTickCount() - p0));
	}

	wxString query =
		" SELECT id, title, note "//, script "
		" FROM report ORDER BY title "
		;

	whDataMgr::GetDB().BeginTransaction();

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);

	auto p0 = GetTickCount();
	if (table)
	{
		//wxString rid;
		//wxString rtitle;
		//wxString rnote;
		//wxString rscript;
		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			mRepList.reserve(rowQty);
			for (unsigned int i = 0; i < rowQty; ++i)
			{
				rec::ReportItem item;
				table->GetAsString(0, i, item.mId);
				table->GetAsString(1, i, item.mTitle);
				table->GetAsString(2, i, item.mNote);
				//table->GetAsString(3, i, item.mScript);
				mRepList.emplace_back(std::move(item));
			}//for
		}//if (rowQty)
	}//if (table)

	whDataMgr::GetDB().Commit();
	wxLogMessage(wxString::Format("%d \t ReprtListModel \t download results", GetTickCount() - p0));

	sigListUpdated(mRepList);
}
//-----------------------------------------------------------------------------
void ReportListModel::LoadAll(const size_t pos, const rec::ReportItem*& item)
{
	GetItemByIdx(pos, item);
	if (!item)
		return;

	wxString query = wxString::Format(
		" SELECT script "
		" FROM report "
		" WHERE id = %s "
		, item->mId
		);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
		table->GetAsString(0, 0, mRepList.at(pos).mScript);
	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void ReportListModel::GetItemByIdx(const size_t idx, const rec::ReportItem*& item)const
{
	item = (idx <= mRepList.size()) ? &mRepList.at(idx) : nullptr;
}
//-----------------------------------------------------------------------------
void ReportListModel::Mk(const rec::ReportItem& item)
{
	rec::ReportItem new_item(item);
	
	wxString query = wxString::Format(
		" INSERT INTO public.report(title, note, script) "
		" VALUES('%s', '%s', '%s') RETURNING id"
		, new_item.mTitle, new_item.mNote, new_item.mScript
		);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
		table->GetAsString(0, 0, new_item.mId);
	whDataMgr::GetDB().Commit();
	
	mRepList.emplace_back(std::move(new_item));
	sigMkReport(new_item);
	
}
//-----------------------------------------------------------------------------
void ReportListModel::Rm(const size_t idx)
{
	wxString query = wxString::Format(
		" DELETE FROM report WHERE id = %s "
		, mRepList[idx].mId
		);

	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();

	sigRmReport(idx);
	mRepList.erase(mRepList.begin() + idx);
}
//-----------------------------------------------------------------------------
void ReportListModel::Ch(const size_t idx, const rec::ReportItem& item)
{
	wxString query = wxString::Format(
		" UPDATE report "
		" SET title = '%s' , note = '%s' , script = '%s' "
		" WHERE id = %s "
		, item.mTitle, item.mNote, item.mScript
		, item.mId
		);

	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();

	mRepList[idx] = item;
	sigChReport(idx, item);
}
//-----------------------------------------------------------------------------
