#include "_pch.h"
#include "CtrlObjEditor.h"
#include "MObjCatalog.h"
#include "dlg_mkobj_view_Frame.h"
#include "config.h"

using namespace wh;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
wh::CtrlObjEditor::CtrlObjEditor(const std::shared_ptr<IViewObjEditor>& view
	, const std::shared_ptr<ModelObjEditor>& model)
	: CtrlWindowBase(view, model)
{

}
//---------------------------------------------------------------------------
void CtrlObjEditor::Insert(int64_t cid)
{
	TEST_FUNC_TIME;
	if (!cid)
		return;

	rec::Obj obj_data;
	rec::Cls cls_data;
	wxLongLong cid_ll(cid);

	auto query = wxString::Format(
		"SELECT cls.id, cls.title, cls.pid, cls.kind "
		" , place.id, place.title "
		" FROM acls cls "
		" LEFT JOIN obj_name place ON place.id = cls.dobj "
		" WHERE cls.id = %s "
		, cid_ll.ToString());

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
	{
		wxLogError("%d\t %s \t%s"
			, GetTickCount() - ftester.GetStartTickCount()
			, ftester.GetFuncName()
			, " class not found");
		whDataMgr::GetDB().RollBack();
		return;
	}
	unsigned int col = 0;
	cls_data.mId = table->GetAsString(col++, 0);
	cls_data.mLabel = table->GetAsString(col++, 0);
	cls_data.mParent.mId = table->GetAsString(col++, 0);
	cls_data.mType = table->GetAsString(col++, 0);
	cls_data.mDefaultObj.mId = table->GetAsString(col++, 0);
	cls_data.mDefaultObj.mLabel = table->GetAsString(col++, 0);

	//obj_data.mParent.mId = cls_data.mDefaultObj.mId;

	whDataMgr::GetDB().Commit();

	auto catalog = std::make_shared<object_catalog::MObjCatalog>();
	catalog->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, false));

	catalog->SetCatFilter(cls_data.mParent.mId, true);


	auto cls_array = catalog->mTypeArray;// std::make_shared<object_catalog::MTypeArray>();
	auto cls = cls_array->CreateItem(cls_data, true);
	cls_array->Insert(cls);
	//auto cls = std::make_shared<object_catalog::MTypeItem>();
	//cls->SetData(cls_data, true);

	auto newObj = cls->mObjArray->CreateItem(obj_data);
	cls->mObjArray->Insert(newObj);
	//auto newObj = std::make_shared<object_catalog::MObjItem>();
	//newObj->SetData(obj_data,true);
	
	object_catalog::view::Frame editor;
	editor.SetModel(newObj);

	int error = 0;
	int result = 0;
	bool loop = true;
	do
	{
		try
		{
			error = 0;
			result = editor.ShowModal();
			//editor.UpdateModel();
			if (wxID_OK == result)
				newObj->Save();
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
void CtrlObjEditor::Delete(int64_t oid, int64_t parent_oid)
{
	TEST_FUNC_TIME;
	if (oid < 2)
		return;


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

	wxLongLong oid_ll(oid), parent_oid_ll(parent_oid);
	
	auto query = wxString::Format(
		"DELETE FROM obj WHERE "
		" id=%s AND pid=%s "
		, oid_ll.ToString()
		, parent_oid_ll.ToString()	);

	whDataMgr::GetDB().BeginTransaction();
	whDataMgr::GetDB().ExecWithResultsSPtr(query);
	whDataMgr::GetDB().Commit();
}
//---------------------------------------------------------------------------
void CtrlObjEditor::Update(int64_t oid, int64_t parent_oid)
{
	TEST_FUNC_TIME;
	if (!oid || !parent_oid)
		return;

	rec::Obj obj_data;
	rec::Cls cls_data;
	wxLongLong oid_ll(oid), parent_oid_ll(parent_oid);

	auto query = wxString::Format(
		"SELECT obj.id, obj.title, obj.qty, obj.move_logid " //, get_path_objnum(obj.pid,1)  AS path  "
		" , obj.pid, place.title "
		" , cls.id, cls.title, cls.pid, cls.kind "
		" FROM obj"
		" LEFT JOIN acls cls ON cls.id = obj.cls_id "
		" LEFT JOIN obj_name place ON place.id = obj.pid "
		" WHERE obj.id = %s AND obj.pid = %s"
		, oid_ll.ToString()
		, parent_oid_ll.ToString());

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table || !table->GetRowCount())
	{
		wxLogError("%d\t %s \t%s"
			, GetTickCount() - ftester.GetStartTickCount()
			, ftester.GetFuncName()
			, " object not found");
		whDataMgr::GetDB().RollBack();
		return;
	}
	unsigned int col = 0;
	obj_data.mId = table->GetAsString(col++, 0);
	obj_data.mLabel = table->GetAsString(col++, 0);
	obj_data.mQty = table->GetAsString(col++, 0);
	obj_data.mLastMoveLogId = table->GetAsString(col++, 0);
	//table->GetAsString(col++, 0, obj_data.mParent.mLabel);
	obj_data.mParent.mId = table->GetAsString(col++, 0);
	obj_data.mParent.mLabel = table->GetAsString(col++, 0);
	
	cls_data.mId = table->GetAsString(col++, 0);
	cls_data.mLabel = table->GetAsString(col++, 0);
	cls_data.mParent.mId = table->GetAsString(col++, 0);
	cls_data.mType= table->GetAsString(col++, 0);
	whDataMgr::GetDB().Commit();

	auto catalog = std::make_shared<object_catalog::MObjCatalog>();
	catalog->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, false));

	catalog->SetCatFilter(cls_data.mParent.mId, true);


	auto cls_array = catalog->mTypeArray;// std::make_shared<object_catalog::MTypeArray>();
	auto cls = cls_array->CreateItem(cls_data, true);
	cls_array->Insert(cls);
	//auto cls = std::make_shared<object_catalog::MTypeItem>();
	//cls->SetData(cls_data, true);

	auto newObj = cls->mObjArray->CreateItem(obj_data, true);
	cls->mObjArray->Insert(newObj);
	//auto newObj = std::make_shared<object_catalog::MObjItem>();
	//newObj->SetData(obj_data,true);

	object_catalog::view::Frame editor;
	editor.SetModel(newObj);

	int error = 0;
	int result = 0;
	bool loop = true;
	do
	{
		try
		{
			error = 0;
			result = editor.ShowModal();
			//editor.UpdateModel();
			if (wxID_OK == result)
				newObj->Save();
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



