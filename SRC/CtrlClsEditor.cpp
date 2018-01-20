#include "_pch.h"
#include "CtrlClsEditor.h"
#include "MObjCatalog.h"
#include "DClsEditor.h"
#include "config.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wh::CtrlClsEditor::CtrlClsEditor(const std::shared_ptr<IViewClsEditor>& view
								, const std::shared_ptr<ModelClsEditor>& model)
	: CtrlWindowBase(view, model)
{

}
//---------------------------------------------------------------------------
void wh::CtrlClsEditor::Insert(int64_t parent_cid)
{
	TEST_FUNC_TIME;
	if (!parent_cid)
		return;

	wxLongLong parent_cid_ll(parent_cid);
	rec::Cls cls_data;
	auto query = wxString::Format(
		"SELECT id, title "
		" FROM acls "
		" WHERE id = %s "
		, parent_cid_ll.ToString());

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	cls_data.mParent.mId = table->GetAsLong(0, 0);
	cls_data.mParent.mLabel = table->GetAsString(1, 0);
	whDataMgr::GetDB().Commit();

	auto newItem = std::make_shared<object_catalog::MTypeItem>();
	newItem->SetData(cls_data);

	view::DClsEditor editor;
	editor.SetModel(std::dynamic_pointer_cast<IModel>(newItem));

	int error = 0;
	int result = 0;
	bool loop = true;
	do
	{
		try
		{
			error = 0;
			result = editor.ShowModal();
			editor.UpdateModel();
			if (wxID_OK == result)
				newItem->Save();
		}
		catch (...) { error = 1; }

		switch (result)
		{
		case wxID_CANCEL:	loop = false;			break;
		case wxID_OK:		loop = error ? true : false; break;
		default:			loop = true;			break;
		}

	} while (loop);

}
//---------------------------------------------------------------------------
void wh::CtrlClsEditor::Delete(int64_t cid)
{
	TEST_FUNC_TIME;

	int res = wxMessageBox("Вы действительно ходите удалить?",
		"Подтверждение", wxYES_NO);
	if (wxYES != res)
		return;
	
	wxPasswordEntryDialog passDlg(nullptr, "введите пароль для подтверждения", "Удаление");
	if (wxID_OK != passDlg.ShowModal())
		return;

	const auto& connect_cfg = whDataMgr::GetInstance()->mConnectCfg->GetData();
	if (passDlg.GetValue() != connect_cfg.mPass)
	{
		wxMessageBox("Неверный пароль", "Подтверждение", wxOK);
		return;
	}

	if (cid < 1)
		return;

	wxLongLong cid_ll(cid);
	auto query = wxString::Format("DELETE FROM acls WHERE id = %s "
		, cid_ll.ToString() );
	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().ExecWithResultsSPtr(query);
	whDataMgr::GetDB().Commit();
}
//---------------------------------------------------------------------------
void wh::CtrlClsEditor::Update(int64_t cid)
{
	TEST_FUNC_TIME;

	wxLongLong cid_ll(cid);

	rec::Cls cls_data;
	cls_data.mId = cid_ll.ToString();
	//cls_data.mParent.mLabel = "Insert cls" + cls_data.mParent.mId;

	auto newItem = std::make_shared<object_catalog::MTypeItem>();

	auto query = wxString::Format(
		"SELECT cls.id, cls.title, cls.kind, cls.measure, cls.note "
		", cls.pid, parent.title as parent_title "
		", cls.dobj, obj.title  as dobj_title "
		"FROM acls cls "
		"LEFT JOIN acls parent ON parent.id = cls.pid "
		"LEFT JOIN obj_name obj ON obj.id = cls.dobj "
		"WHERE cls.id = %s "
		, cid_ll.ToString());

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	unsigned int row = 0;

	auto& data = cls_data;
	data.mId = table->GetAsLong(0, row);
	data.mLabel = table->GetAsString(1, row);
	data.mType = table->GetAsString(2, row);
	data.mMeasure = table->GetAsString(3, row);
	data.mComment = table->GetAsString(4, row);

	data.mParent.mId = table->GetAsString(5, row);
	data.mParent.mLabel = table->GetAsString(6, row);

	data.mDefaultObj.mId = table->GetAsString(7, row);
	data.mDefaultObj.mLabel = table->GetAsString(8, row);

	whDataMgr::GetDB().Commit();
	newItem->SetData(cls_data,true);
	

	view::DClsEditor editor;
	editor.SetModel(std::dynamic_pointer_cast<IModel>(newItem));

	int error = 0;
	int result = 0;
	bool loop = true;
	do
	{
		try
		{
			error = 0;
			result = editor.ShowModal();
			editor.UpdateModel();
			if (wxID_OK == result)
				newItem->Save();
		}
		catch (...) { error = 1; }

		switch (result)
		{
		case wxID_CANCEL:	loop = false;			break;
		case wxID_OK:		loop = error ? true : false; break;
		default:			loop = true;			break;
		}

	} while (loop);
}


