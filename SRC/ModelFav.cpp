#include "_pch.h"
#include "ModelFav.h"
using namespace wh;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ModelFav::ModelFav()
	: mClsCache(std::make_unique<ClsCache>(nullptr))
	, mPropCache(std::make_unique<PropCache>(nullptr))
	, mActCache(std::make_unique<ActCache>(nullptr))
	, mCls(std::make_shared<ClsRec64>(0,mClsCache.get()) )
{
	
}
//---------------------------------------------------------------------------
void ModelFav::DoSetClsId(int64_t cid)
{
	mCid = cid;
	DoRefresh();
	
}
//---------------------------------------------------------------------------
void ModelFav::LoadClsTree(std::vector<const ICls64*>& tree)
{
	wxLongLong cid_ll(mCid);
	cid_ll.ToString();

	wxString query = wxString::Format(
		"SELECT id, title,kind,pid, measure"
		" FROM get_path_cls_info(%s, 1)"
		, cid_ll.ToString() );

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table)
		return;
	unsigned int rowQty = table->GetRowCount();
	tree.clear();

	size_t row = 0;
	const ClsCache::fnModify fn = [this, &table, &row](const std::shared_ptr<ClsRec64>& cls)
	{
		//cls->SetId(table->GetAsString(0, row));
		table->GetAsString(1, row, cls->mTitle);
		ToClsKind(table->GetAsString(2, row), cls->mKind);
		cls->SetParentId(table->GetAsString(3, row));
		table->GetAsString(4, row, cls->mMeasure);
		cls->mObjQty.Clear();
		cls->mFavProp.clear();
		cls->mFavObjProp.clear();
		cls->mFavActProp.clear();
	};

	if (rowQty)
		mCls = mClsCache->GetById(mCid, fn);
		
	for (size_t i = rowQty; i > 0; --i)
	{
		row = i - 1;
		int64_t id;
		if (!table->GetAsString(0, row).ToLongLong(&id))
			throw;
		const std::shared_ptr<ClsRec64>& value = mClsCache->GetById(id, fn);

		
		

		tree.emplace_back(value.get());
	}

}
//---------------------------------------------------------------------------
void ModelFav::LoadFavCProp()
{
	wxLongLong cid_ll(mCid);
	cid_ll.ToString();

	wxString query = wxString::Format(
		"SELECT prop.id, prop.title, prop.kind, var, var_strict, cls.id "
		" FROM get_path_cls_info(%s, 1) cls"
		" INNER JOIN fav_cprop ON fav_cprop.cid = cls.id AND usr = CURRENT_USER"
		" LEFT JOIN prop ON prop.id = fav_cprop.pid"
		, cid_ll.ToString());
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table)
		return;
	unsigned int rowQty = table->GetRowCount();

	size_t row = 0;
	const PropCache::fnModify fn = [this, &table, &row](const std::shared_ptr<IProp64>& irec)
	{
		auto record = std::dynamic_pointer_cast<PropRec64>(irec);
		//record->SetId(table->GetAsString(0, row));
		record->SetTitle(table->GetAsString(1, row));
		record->SetKind(table->GetAsString(2, row));
		record->SetVar(table->GetAsString(3, row));
		record->SetVarStrict(table->GetAsString(4, row));
	};

	for (size_t i = 0; i < rowQty; ++i)
	{
		row = i;
		int64_t id;
		if (!table->GetAsString(0, row).ToLongLong(&id))
			throw;
		const auto& value = mPropCache->InsertOrUpdate(id, fn);

		int64_t cid;
		if (!table->GetAsString(5, row).ToLongLong(&cid))
			throw;

		const std::shared_ptr<ClsRec64>& cls = mClsCache->GetById(cid, nullptr);
		
		auto pv = std::make_shared<const PropVal>(PropVal(value));
		cls->mFavProp.emplace(pv);

	}
}
//---------------------------------------------------------------------------
void ModelFav::LoadFavOProp()
{
	wxLongLong cid_ll(mCid);
	cid_ll.ToString();

	wxString query = wxString::Format(
		"SELECT prop.id, prop.title, prop.kind, var, var_strict, cls.id "
		" FROM get_path_cls_info(%s, 1) cls"
		" INNER JOIN fav_oprop ON fav_oprop.cid = cls.id AND usr = CURRENT_USER"
		" LEFT JOIN prop ON prop.id = fav_oprop.pid"
		, cid_ll.ToString());
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table)
		return;
	unsigned int rowQty = table->GetRowCount();

	size_t row = 0;
	const PropCache::fnModify fn = [this, &table, &row](const std::shared_ptr<IProp64>& irec)
	{
		auto record = std::dynamic_pointer_cast<PropRec64>(irec);
		//record->SetId(table->GetAsString(0, row));
		record->SetTitle(table->GetAsString(1, row));
		record->SetKind(table->GetAsString(2, row));
		record->SetVar(table->GetAsString(3, row));
		record->SetVarStrict(table->GetAsString(4, row));
	};

	for (size_t i = 0; i < rowQty; ++i)
	{
		row = i;
		int64_t id;
		if (!table->GetAsString(0, row).ToLongLong(&id))
			throw;
		const std::shared_ptr<const IProp64>& 
			value = mPropCache->InsertOrUpdate(id, fn);

		int64_t cid;
		if (!table->GetAsString(5, row).ToLongLong(&cid))
			throw;

		const std::shared_ptr<ClsRec64>& cls = mClsCache->GetById(cid, nullptr);

		auto pv = std::make_shared<ObjProp>(value);
		cls->mFavObjProp.emplace(pv);

	}
}
//---------------------------------------------------------------------------
void ModelFav::LoadFavAct()
{
	wxLongLong cid_ll(mCid);
	cid_ll.ToString();

	wxString query = wxString::Format(
		"SELECT act.id, act.title, fav_act.info, cls.id as cid "
		" FROM get_path_cls_info(%s, 1) cls"
		" INNER JOIN fav_act ON fav_act.cid = cls.id AND usr = CURRENT_USER "
		" LEFT JOIN act ON act.id = fav_act.aid "
		, cid_ll.ToString());
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (!table)
		return;
	unsigned int rowQty = table->GetRowCount();

	size_t row = 0;
	const ActCache::fnModify fn = [this, &table, &row](const std::shared_ptr<IAct64>& irec)
	{
		auto record = std::dynamic_pointer_cast<ActRec64>(irec);
		//record->SetId(table->GetAsString(0, row));
		record->SetTitle(table->GetAsString(1, row));
	};

	for (size_t i = 0; i < rowQty; ++i)
	{
		row = i;
		int64_t aid;
		if (!table->GetAsString(0, row).ToLongLong(&aid))
			throw;
		const auto& act = mActCache->InsertOrUpdate(aid, fn);

		int64_t cid;
		if (!table->GetAsString(3, row).ToLongLong(&cid))
			throw;
		long info;
		if (!table->GetAsString(2, row).ToLong(&info))
			throw;

		const std::shared_ptr<ClsRec64>& cls = mClsCache->GetById(cid, nullptr);

		auto pv = std::make_shared<FavAProp>(act, Int2FavAPropInfo(info));
		cls->mFavActProp.emplace(pv);
		//cls->mFavActProp.emplace_back(pv);

	}
}
//---------------------------------------------------------------------------
void ModelFav::DoRefresh()
{
	TEST_FUNC_TIME;
	try
	{
		std::vector<const ICls64*> tree;
		sigBeforeRefresh(tree, *mCls);
		whDataMgr::GetDB().BeginTransaction();
		LoadClsTree(tree);
		LoadFavCProp();
		LoadFavOProp();
		LoadFavAct();
		whDataMgr::GetDB().Commit();

		UpdateTitle();
		sigAfterRefresh(tree, *mCls );
	}
	catch (...)
	{
		whDataMgr::GetDB().RollBack();
	}
}
//---------------------------------------------------------------------------
void ModelFav::DoAddClsProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddObjProp(int64_t clsId, int64_t propId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddPrevios(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddPeriod(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddNext(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoAddLeft(int64_t clsId, int64_t actId)
{

}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveClsProp(int64_t clsId, int64_t propId)
{
	TEST_FUNC_TIME;
	try
	{
		whDataMgr::GetDB().BeginTransaction();
		wxLongLong cid(clsId);
		wxLongLong pid(propId);

		wxString query = wxString::Format(
			"DELETE FROM fav_cprop "
			" WHERE usr=CURRENT_USER AND cid=%s AND pid=%s"
			, cid.ToString()
			, pid.ToString()
		);

		auto res = whDataMgr::GetDB().Exec(query, false);
		whDataMgr::GetDB().Commit();
		DoRefresh();
	}
	catch (...)
	{
		whDataMgr::GetDB().RollBack();
	}
}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveObjProp(int64_t clsId, int64_t propId)
{
	TEST_FUNC_TIME;
	try
	{
		whDataMgr::GetDB().BeginTransaction();
		wxLongLong cid(clsId);
		wxLongLong pid(propId);

		wxString query = wxString::Format(
			"DELETE FROM fav_oprop "
			" WHERE usr=CURRENT_USER AND cid=%s AND pid=%s"
			, cid.ToString()
			, pid.ToString()
		);

		auto res = whDataMgr::GetDB().Exec(query, false);
		whDataMgr::GetDB().Commit();
		DoRefresh();
	}
	catch (...)
	{
		whDataMgr::GetDB().RollBack();
	}
}
//---------------------------------------------------------------------------
void ModelFav::DoRemoveActProp(int64_t clsId, int64_t actId, FavAPropInfo info)
{

}
//---------------------------------------------------------------------------
//virtual 
void ModelFav::UpdateTitle() //override;
{
	wxString title = wxString::Format(
		"Выберите атрибуты %s для отображения"
		, mCls? mCls->GetTitle(): "***" );
	const auto& ico = ResMgr::GetInstance()->m_ico_favprop_select24;

	sigUpdateTitle(title, ico);
}
