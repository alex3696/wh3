#include "_pch.h"
#include "MoveObjModel.h"
#include "globaldata.h"
#include "RecentDstOidPresenter.h"

using namespace wh;



//-----------------------------------------------------------------------------
Moveable::Moveable()
{
	//auto x = make_shared_model<bool>(true);
	
	//mEnableRecent.SetData(true);
	//mMoveble.SetData(rec::PathItem());
	//mDst.SetData(ObjTree());
	//mRecent.SetData(ObjTree());


	
}
//-----------------------------------------------------------------------------
const ObjStore::iterator Moveable::FindObj(const wxString& str)const
{
	std::function<bool(const Obj&)>
		isInTitle = [&str](const Obj& obj)
	{
		return obj.mId.Find(str) != wxNOT_FOUND;
	};

	return std::find_if(mDst.mObj.cbegin(), mDst.mObj.cend(), isInTitle);
}
//-----------------------------------------------------------------------------
void Moveable::Load()
{
	mDst.Clear();
	mRecent.Clear();
	const auto& global_recent = whDataMgr::GetInstance()->mRecentDstOidPresenter;

	wxString query = wxString::Format(
		" SELECT _dst_cls_id, acls.title "
		", _dst_obj_id, _dst_obj_label , get_path_objnum(_dst_obj_pid,1) AS DST_PATH "
		" FROM lock_for_move(%s,%s) "
		" LEFT JOIN acls ON acls.id = _dst_cls_id "
		" ORDER BY "
		"   acls.title ASC "
		"   ,(substring(_dst_obj_label, '^[0-9]+')::INT, _dst_obj_label ) ASC "
		, mMoveble->mObj.mId.SqlVal()
		, mMoveble->mObj.mParent.mId.SqlVal()
		);

	whDataMgr::GetDB().BeginTransaction();
	
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			mDst.mObj.reserve(rowQty);
			for (unsigned int i = 0; i < rowQty; ++i)
			{
				wxString cid = table->GetAsString(0, i);
				wxString ctitle = table->GetAsString(1, i);
				wxString oid = table->GetAsString(2, i);
				wxString otitle = table->GetAsString(3, i);
				wxString opath = table->GetAsString(4, i);

				auto type_IterBool = mDst.mType.emplace_back(cid, ctitle);
				const Type* type = &(*type_IterBool.first);
				auto obj_IterBool = mDst.mObj.emplace_back(type, oid, otitle, opath);

				if (global_recent->Check(oid))
				{
					auto type_recent_itbool = mRecent.mType.emplace_back(cid, ctitle);
					const Type* type = &(*type_recent_itbool.first);
					mRecent.mObj.emplace_back(type, oid, otitle, opath);
				}
			}//for
		}// if (rowQty)

	}//if (table)

	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void Moveable::Unlock()
{
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = mMoveble.GetData();

	wxString query = wxString::Format(
		"SELECT lock_reset(%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal());

	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
void Moveable::Move(const wxString& oid, const wxString& qty)
{
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = mMoveble.GetData();

	wxString query = wxString::Format(
		"SELECT do_move(%s,%s,%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal()
		, oid
		, qty
		);
	whDataMgr::GetDB().Exec(query);

	query = wxString::Format(
		"SELECT lock_reset(%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal());
	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();

	whDataMgr::GetInstance()->mRecentDstOidPresenter->Insert(oid);


}
//-----------------------------------------------------------------------------
bool Moveable::GetRecentEnable()const
{
	return whDataMgr::GetInstance()->mRecentDstOidPresenter->GetRecentEnable()==1 ;
}
//-----------------------------------------------------------------------------
void Moveable::SetRecentEnable(bool enable)
{
	char i = enable ? 1 : 0;
	whDataMgr::GetInstance()->mRecentDstOidPresenter->SetRecentEnable(i);
}