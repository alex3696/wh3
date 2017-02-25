#include "_pch.h"
#include "ReportListModel.h"
#include "globaldata.h"

using namespace wh;
//-----------------------------------------------------------------------------

ReportListModel::ReportListModel()
	//:IModelWindow()
{

}
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
				auto item = std::make_shared<rec::ReportItem>();
				table->GetAsString(0, i, item->mId);
				table->GetAsString(1, i, item->mTitle);
				table->GetAsString(2, i, item->mNote);
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
std::shared_ptr<const rec::ReportItem> ReportListModel::LoadAll(const wxString& rep_id)
{
	auto& idxRepId = mRepList.get<1>();
	auto& it = idxRepId.find(rep_id);
	if (idxRepId.end() == it)
		return nullptr;

	auto rep = std::make_shared<rec::ReportItem>();
	rep->mId = rep_id;

	wxString query = wxString::Format(
		" SELECT title, note, script "
		" FROM report "
		" WHERE id = %s "
		, rep_id
		);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
	{
		
		table->GetAsString(0, 0, rep->mTitle);
		table->GetAsString(1, 0, rep->mNote);
		table->GetAsString(2, 0, rep->mScript);
	}
	whDataMgr::GetDB().Commit();
	
	idxRepId.replace(it, rep );
	sigChReport(rep, rep_id);
	return rep;	
}
//-----------------------------------------------------------------------------
std::shared_ptr<const rec::ReportItem> ReportListModel::GetRep(const wxString& rep_id)const
{
	const auto& idxRepId = mRepList.get<1>();
	const auto& it = idxRepId.find(rep_id);
	if (idxRepId.end() == it)
		return nullptr;
	return *it;
}
//-----------------------------------------------------------------------------
void ReportListModel::Mk(const std::shared_ptr<rec::ReportItem>& rep)
{
	wxString s = rep->mScript;
	s.Replace("'", "''");

	wxString query = wxString::Format(
		" INSERT INTO public.report(title, note, script) "
		" VALUES('%s', '%s', '%s') RETURNING id"
		, rep->mTitle, rep->mNote, s
		);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
		table->GetAsString(0, 0, rep->mId);
	whDataMgr::GetDB().Commit();
	
	auto ins_it = mRepList.emplace_back(rep);
	sigMkReport( (*ins_it.first) );
	
}
//-----------------------------------------------------------------------------
void ReportListModel::Rm(const wxString& rep_id)
{
	auto& idxRepId = mRepList.get<1>();
	auto& it = idxRepId.find(rep_id);
	if (idxRepId.end() == it)
		return;

	const auto& rep = *it;
	
	wxString query = wxString::Format(
		" DELETE FROM report WHERE id = %s "
		, rep->mId
		);

	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();

	sigRmReport(rep);
	idxRepId.erase(it);
}
//-----------------------------------------------------------------------------
void ReportListModel::Ch(const wxString& rep_id, const std::shared_ptr<rec::ReportItem>& rep)
{
	auto& idxRepId = mRepList.get<1>();
	auto& it = idxRepId.find(rep_id);
	if (idxRepId.end() == it)
		return;

	wxString s = rep->mScript;
	s.Replace("'", "''");

	wxString query = wxString::Format(
		" UPDATE report "
		" SET title = '%s' , note = '%s' , script = '%s' "
		" WHERE id = %s "
		, rep->mTitle, rep->mNote, s
		, rep_id
		);

	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();

	idxRepId.replace(it, rep);

	sigChReport(rep, rep_id);
}
//-----------------------------------------------------------------------------
