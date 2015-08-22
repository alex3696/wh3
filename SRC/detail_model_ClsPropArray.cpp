#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
ClsProp::ClsProp(const char option)
	:TModelData<rec::PropVal>(option)
{
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ClsPropArray::ClsPropArray(const char option)
	:TModelArray<ClsProp>()
{
}
//-----------------------------------------------------------------------------
bool ClsPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<Obj*>(GetParent());
	if (parentCls)
	{
		const auto& data = parentCls->GetData();

		query = wxString::Format(
			"SELECT prop_id, prop_label, prop_type, val, id "
			" FROM w_class_prop "
			" WHERE id = '%s' ", data.mCls.mID);
		return true;
	}
	return false;
}
