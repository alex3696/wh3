#include "_pch.h"
#include "ModelExecAct.h"

using namespace wh;


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ModelActExecWindow::ModelActExecWindow()
	:mModelActBrowser(std::make_shared<ModelActBrowserWindow>())
	, mModelObjBrowser(std::make_shared<ModelBrowser>())
	, mModelPropPg(std::make_shared<ModelPropPg>())
	, mAct(0,nullptr )

{
	mOnActivateAct = [this](const ModelActTable::RowType& act)->int
	{
		mAct.SetId(act.GetId());
		mAct.SetTitle(act.GetTitle());
		mAct.SetColour(act.GetColour());
		mAct.SetNote(act.GetNote());

		DoShowActProperty();
		return 0;
	};

	mModelActBrowser->SetActivateCallback(mOnActivateAct);
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
ModelActExecWindow::~ModelActExecWindow()
{

}
//---------------------------------------------------------------------------
void ModelActExecWindow::UnlockObjectsWithoutTransaction()
{
	wxString query;
	for (const auto& obj : mObjects)
	{
		query = wxString::Format(
			"SELECT lock_reset(%s,%s)"
			, obj.GetId_AsString()
			, obj.GetParentId_AsString());
		whDataMgr::GetDB().Exec(query);
	}

}
//---------------------------------------------------------------------------
void ModelActExecWindow::UnlockObjects()
{
	whDataMgr::GetDB().BeginTransaction();
	UnlockObjectsWithoutTransaction();
	whDataMgr::GetDB().Commit();
}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoShowActProperty()
{
	wxString query;
	whDataMgr::GetDB().BeginTransaction();

	query = wxString::Format(
			" SELECT prop.id, prop.title, prop.kind, prop.var, prop.var_strict "
			" FROM ref_act_prop "
			" LEFT JOIN prop ON ref_act_prop.prop_id = prop.id "
			" WHERE act_id = %s "
			" ORDER BY prop.title"
			, mAct.GetIdAsString() );

	
	whDataMgr::GetDB().Exec(query);
	
	auto prop_table = std::make_shared<ModelPropTable>();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		const unsigned int rowQty = table->GetRowCount();
		size_t row = 0;
		const ModelPropTable::fnModify fn = [this, &table, &row]
		(const std::shared_ptr<ModelPropTable::RowType>& irec)
		{
			auto record = std::dynamic_pointer_cast<PropRec64>(irec);
			//record->SetId(table->GetAsString(0, row));
			record->SetTitle(table->GetAsString(1, row));
			record->SetKind(table->GetAsString(2, row));
			record->SetVar(table->GetAsString(3, row));
			record->SetVarStrict(table->GetAsString(4, row));
		};
		for (; row < rowQty; row++)
		{
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;

			prop_table->InsertOrUpdate(id, fn);
		}//for
	}
	whDataMgr::GetDB().Commit();
	
	mCurrentPage = 2;
	sigSelectPage(mCurrentPage);
	mModelPropPg->DoSwap(prop_table);
}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoShowActList()
{
	mCurrentPage = 1;
	sigSelectPage(mCurrentPage);
}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoSelectAct()
{
	mModelActBrowser->DoActivate();
}
//---------------------------------------------------------------------------
int ModelActExecWindow::BuildExecQuery(wxString& query)
{
	int ret = 0;

	std::map<int64_t, wxString> propval;
	mModelPropPg->sigGetPropValues(propval);
	auto prop_table = mModelPropPg->GetPropTable();

	wxString propdata;

	for (const auto& it : propval)
	{
		wxString value = it.second;
		//value.Replace("\\", "\\\\", true);
		//value.Replace("\"", "\\\\"", true);
		value.Replace("%", "%%", true);
		
		if (value.empty())
			ret++;

		auto prop = prop_table->GetById(it.first);
		if (prop)
		{
			wxString format;
			switch (prop->GetKind())
			{
			case ftLong:
			case ftDouble:	format = "'%s',%s,";
				break;
			default:		format = "'%s','%s',";
				break;
			}

			propdata += wxString::Format(format
				, prop->GetIdAsString()
				, value);
		}

	}
	if (propdata.empty())
		return -1;
	propdata.RemoveLast();
	propdata = wxString::Format("jsonb_build_object(%s)", propdata);

	query = wxString::Format(
		"SELECT do_act(%%s, %s, %s)"
		, mAct.GetIdAsString()
		, propdata );

	return ret;
}
//---------------------------------------------------------------------------
void ModelActExecWindow::DoExecute()
{
	wxString query;
	int ret = BuildExecQuery(query);
	if (ret < 0)
		return;

	if (ret>0)
	{
		wxMessageDialog dialog(nullptr,
			"Не все поля заполнены! Вы уверены, в том что надо сохранить?",
			"Confirm\n",
			wxCENTER |
			wxNO_DEFAULT | wxYES_NO | wxICON_QUESTION);
		int modalResult = dialog.ShowModal();
		if (wxID_YES != modalResult)
			return;
	}

	try {
		whDataMgr::GetDB().BeginTransaction();

		wxString qexec;
		for (const auto& obj : mObjects)
		{
			qexec = wxString::Format(query
				, obj.GetId_AsString());
			whDataMgr::GetDB().Exec(qexec);
		}
		UnlockObjectsWithoutTransaction();
		whDataMgr::GetDB().Commit();
		sigClose();
	}
	catch (...)
	{
		whDataMgr::GetDB().RollBack();
		wxLogError("error DoExecute");
		
	}

	

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
	if (1 == mObjects.size())
		mCurrentPage = 1;
	else
		mCurrentPage = 0;

	sigSelectPage(mCurrentPage);
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