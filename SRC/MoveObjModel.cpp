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
	TEST_FUNC_TIME;
	wxBusyCursor busyCursor;
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
		"   ,(substring(_dst_obj_label, '^[0-9]{1,9}')::INT, _dst_obj_label ) ASC "
		, mMoveble->mObj.mId.SqlVal()
		, mMoveble->mObj.mParent.mId.SqlVal()
		);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		wxString cid;
		wxString ctitle;
		wxString oid;
		wxString otitle;
		wxString opath;

		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			mDst.mObj.reserve(rowQty);
			for (unsigned int i = 0; i < rowQty; ++i)
			{
				table->GetAsString(0, i, cid);
				table->GetAsString(1, i, ctitle);
				table->GetAsString(2, i, oid);
				table->GetAsString(3, i, otitle);
				table->GetAsString(4, i, opath);
				
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
	mLock = true;
}
//-----------------------------------------------------------------------------
void Moveable::Unlock()
{
	if (!mLock)
		return;
	
	whDataMgr::GetDB().BeginTransaction();

	const rec::PathItem& movable = mMoveble.GetData();

	wxString query = wxString::Format(
		"SELECT lock_reset(%s,%s)"
		, movable.mObj.mId.SqlVal()
		, movable.mObj.mParent.mId.SqlVal());

	whDataMgr::GetDB().Exec(query);

	whDataMgr::GetDB().Commit();
	mLock = false;
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
//-----------------------------------------------------------------------------
void Moveable::SetMoveable(int64_t oid, int64_t parent_oid)
{
	TEST_FUNC_TIME;
	wxLongLong oid_ll(oid);
	wxLongLong parent_oid_ll(parent_oid);

	wxString query = wxString::Format(
		"SELECT obj.id, obj.title, obj.qty, obj.pid "
		" , acls.id, acls.title, acls.kind, acls.measure, acls.pid "
		" FROM obj "
		" LEFT JOIN acls ON acls.id = cls_id AND acls.kind=cls_kind"
		" WHERE obj.id = %s AND obj.pid = %s "
		, oid_ll.ToString()
		, parent_oid_ll.ToString()
	);
	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		unsigned int rowQty = table->GetRowCount();
		if (rowQty)
		{
			constexpr unsigned int row = 0;
			rec::PathItem movable;
			movable.mObj.mId		= table->GetAsString(0, row);
			movable.mObj.mLabel		= table->GetAsString(1, row);
			movable.mObj.mQty		= table->GetAsString(2, row);
			movable.mObj.mParent.mId= table->GetAsString(3, row);

			movable.mCls.mId		= table->GetAsString(4, row);
			movable.mCls.mLabel		= table->GetAsString(5, row);
			movable.mCls.mType		= table->GetAsString(6, row);
			movable.mCls.mMeasure	= table->GetAsString(7, row);
			movable.mCls.mParent.mId= table->GetAsString(8, row);

			SetMoveable(movable);
		}// if (rowQty)
	}//if (table)
	whDataMgr::GetDB().Commit();
}
//-----------------------------------------------------------------------------
inline void Moveable::SetMoveable(const rec::PathItem& movable)
{
	mMoveble.SetData(movable);
}
