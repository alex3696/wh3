#include "_pch.h"
#include "ModelExecAct.h"

using namespace wh;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelActExecWindow::ModelActExecWindow()
	:mModelActBrowser(std::make_shared<ModelActBrowserWindow>())
	, mModelObjBrowser(std::make_shared<ModelBrowser>())
{
}
//---------------------------------------------------------------------------
void ModelActExecWindow::LockObjects(const std::set<ObjectKey>& obj)
{
	mObjects = obj;
	mModelObjBrowser->DoSetObjects(obj);
	// Execut lock and transfer acts to ModelActBrowserModel

	wxString query;


	for (const auto& aid : obj)
	{
		query += wxString::Format(
			"INTERSECT"
			" SELECT id, title, note, color "
			"  FROM lock_for_act(%s, %s) "
			, aid.GetId_AsString()
			, aid.GetParentId_AsString());
	}
	if (query.IsEmpty())
		return;
	query.Remove(0, wxString("INTERSECT").size());
	
	auto act_table = std::make_shared<ModelActTable>();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		const unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ModelActTable::fnModify fn = [this, &table, &row]
			(const std::shared_ptr<ModelActTable::RowType>& irec)
		{
			auto record = std::dynamic_pointer_cast<ActRec64>(irec);
			//record->SetId(table->GetAsString(0, row));
			record->SetTitle(table->GetAsString(1, row));
			record->SetNote(table->GetAsString(2, row));
			record->SetColour(table->GetAsString(3, row));
		};
		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;

			act_table->InsertOrUpdate(id, fn);
		}//for
	}
	whDataMgr::GetDB().Commit();

	mModelActBrowser->DoSwap(act_table);
}
//---------------------------------------------------------------------------
void ModelActExecWindow::UnlockObjects()
{
	wxString query;
	whDataMgr::GetDB().BeginTransaction();
	for (const auto& obj : mObjects)
	{
		query = wxString::Format(
			"SELECT lock_reset(%s,%s)"
			, obj.GetId_AsString()
			, obj.GetParentId_AsString() );
		whDataMgr::GetDB().Exec(query);
	}
	whDataMgr::GetDB().Commit();
}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoShowActProperty()
{

}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoShowActList()
{

}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoExecute()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::UpdateTitle()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Show()
{ 
	sigShow(); 
}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Init()
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Load(const boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelActExecWindow::Save(boost::property_tree::wptree& page_val)
{

}
//---------------------------------------------------------------------------