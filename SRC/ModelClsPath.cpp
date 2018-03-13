#include "_pch.h"
#include "ModelClsPath.h"
#include "ModelBrowser.h"

using namespace wh;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const std::shared_ptr<const ICls64> ModelClsPath::mRoot
= std::make_shared<ClsRec64>(1, "ClsRoot", ClsKind::Abstract, nullptr);
//-----------------------------------------------------------------------------
ModelClsPath::ModelClsPath()
{
}
//-----------------------------------------------------------------------------
std::shared_ptr<const ICls64> ModelClsPath::GetCurrent()
{
	return (mData.empty()) ? mRoot : mData.back();
}
//-----------------------------------------------------------------------------
void ModelClsPath::Refresh(const int64_t& cid)
{
	TEST_FUNC_TIME;
	wxLongLong ll_cid((cid <= 1) ? 1 : cid);
	auto id = ll_cid.ToString();

	mData.clear();

	wxString query = wxString::Format(
		"SELECT id, title,kind,pid, measure"
		" FROM get_path_cls_info(%s, 1)"
		, id);

	whDataMgr::GetDB().BeginTransaction();
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		size_t row = 0;

		unsigned int rowQty = table->GetRowCount();
		for (size_t i = rowQty; i > 0; --i)
		{
			row = i - 1;
			int64_t id;
			if (!table->GetAsString(0, row).ToLongLong(&id))
				throw;
			auto cls = std::make_shared<ClsRec64>(id, nullptr);
			table->GetAsString(1, row, cls->mTitle);
			ToClsKind(table->GetAsString(2, row), cls->mKind);
			cls->SetParentId(table->GetAsString(3, row));
			table->GetAsString(4, row, cls->mMeasure);
			cls->mObjQty.Clear();

			mData.emplace_back(cls);

		}
	}//if (table)
	whDataMgr::GetDB().Commit();
}

//-----------------------------------------------------------------------------
wxString ModelClsPath::AsString() const
{
	wxString ret = "/";
	std::for_each(mData.crbegin(), mData.crend(), [&ret]
	(const std::shared_ptr<const ICls64>& curr)
	{
		const auto& title = curr->GetTitle();

		if (wxNOT_FOUND == title.Find('/'))
			ret = wxString::Format("/%s%s", title, ret);
		else
			ret = wxString::Format("/[%s]%s", title, ret);
	});
	return ret;
}
//-----------------------------------------------------------------------------

void ModelClsPath::SetId(const wxString& str)
{
	if (!str.IsEmpty())
	{
		int64_t tmp;
		if (str.ToLongLong(&tmp))
		{
			SetId(tmp);
			return;
		}
	}
	SetId(1);
}
//-----------------------------------------------------------------------------
void ModelClsPath::SetId(const int64_t& val)
{
	Refresh(val);
}
