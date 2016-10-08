#include "_pch.h"
#include "detail_model.h"

using namespace wh;
using namespace wh::detail::model;

//-----------------------------------------------------------------------------
ObjProp::ObjProp(const char option)
	:TModelData<rec::PropVal>(option)
{
}
//-----------------------------------------------------------------------------
bool ObjProp::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	data.mProp.mId = table->GetAsLong(0, row);
	data.mProp.mLabel = table->GetAsString(1, row);
	data.mProp.mType = ToFieldType(table->GetAsString(2, row));
	//data.mVal = table->GetAsString(3, row);
	SetData(data);
	return true;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ObjPropArray::ObjPropArray(const char option)
	:TModelArray<ObjProp>()
{
	namespace ph = std::placeholders;
	auto fnAR = std::bind(&ObjPropArray::OnRowAfterRemove, this, ph::_1, ph::_2);
	mConnRowAR = ConnectAfterRemove(fnAR);

}
//-------------------------------------------------------------------------

void ObjPropArray::OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	if (0 == vec.size())
	{
		mActProps.clear();
		mUniqueIndex_PId.clear();;
	}
	else
	{
		BOOST_THROW_EXCEPTION(error() << wxstr("Не все свойства удалены"));
	}
}
//-----------------------------------------------------------------------------

bool ObjPropArray::LoadChildDataFromDb(std::shared_ptr<IModel>& child,
	std::shared_ptr<whTable>& db, const size_t pos)
{
	bool ret = TModelArray<ObjProp>::LoadChildDataFromDb(child,db,pos);
	if (ret)
	{
		auto new_mprop = std::dynamic_pointer_cast<ObjProp>(child);
		//wxString  new_mprop_title = new_mprop->GetData().mProp.mLabel;
		//wxString  new_mprop_pid = new_mprop->GetData().mProp.mId;
		//wxString  new_mprop_aid = new_mprop->GetData().mAId;
		if (new_mprop)
		{
			auto ins_pair = mUniqueIndex_PId.emplace(new_mprop);
			std::shared_ptr<ObjProp> inserted = *ins_pair.first;
			ret = ins_pair.second;

			const wxString aid_str = db->GetAsString(3, pos);
			std::shared_ptr<UniqueIndex_PId> act_props;
			auto it = mActProps.find(aid_str);
			if (mActProps.end() == it)
			{
				act_props.reset(new UniqueIndex_PId);
				it = mActProps.insert(std::make_pair(aid_str, act_props)).first;
			}
			else
				act_props = it->second;

			act_props->emplace(inserted);
		}
		else
			ret = false;

	} // if (ret)
	return ret;
}
//-----------------------------------------------------------------------------

void ObjPropArray::SetPropArray(const wxString& prop_str, const wxString& aid_str)
{
	auto obj = dynamic_cast<Obj*>(GetParent());
	if (!obj)
		return;
	const auto& obj_data = obj->GetData();
	if (obj_data.mCls.IsAbstract())
		return;

	auto it = mActProps.find(aid_str);
	if (mActProps.end() != it)
	{
		for (const auto& p : *it->second)
			p->SetSelected(true);
	}


	namespace pt = boost::property_tree;
	pt::wptree prop_arr;
	if (!prop_str.IsEmpty())
	{
		std::wstringstream ss;
		ss << prop_str;//ss << prop_str.ToUTF8();
		try
		{
			pt::read_json(ss, prop_arr);
		}
		catch (const pt::ptree_error &e)
		{
			wxString err= e.what() ;
			wxLogError(err + " JSON: " + prop_str);
		}
	} //if (!prop_str.IsEmpty())
	
	auto prop_qty = this->GetChildQty();
	for (size_t i = 0; i < prop_qty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<ObjProp>(GetChild(i));
		if (prop)
		{
			auto prop_data = prop->GetData();
			const std::wstring pid_str = prop_data.mProp.mId.toStr().wc_str();
			auto it = prop_arr.find(pid_str);
			if (it != prop_arr.not_found() )
			{
				prop_data.mVal = it->second.get_value<std::wstring>();
				//prop_data.mVal = wxString::FromUTF8(it->second.get_value<std::string>().c_str());
			}
			else
				prop_data.mVal.SetNull();
			prop->SetData(prop_data);
		}

	}//for (size_t i = 0; i < prop_qty; ++i)
}
//-----------------------------------------------------------------------------

void ObjPropArray::UnselectAll()
{
	auto prop_qty = this->GetChildQty();
	for (size_t i = 0; i < prop_qty; ++i)
	{
		auto prop = std::dynamic_pointer_cast<ObjProp>(GetChild(i));
		if (prop)
			prop->SetSelected(false);
	}
}
//-----------------------------------------------------------------------------

bool ObjPropArray::GetSelectChildsQuery(wxString& query)const
{
	auto parentCls = dynamic_cast<Obj*>(GetParent());
	if (!parentCls)
		return false;
	const auto& data = parentCls->GetData();
	if (data.mCls.mType.IsNull() || data.mCls.IsAbstract())
		return false;
	
	query = wxString::Format(
		"SELECT distinct(ref_act_prop.prop_id) , prop.title , prop.kind, ref_act_prop.act_id "
		"  FROM ref_cls_act "
		"  INNER JOIN ref_act_prop ON ref_act_prop.act_id = ref_cls_act.act_id "
		"  LEFT JOIN prop ON prop.id = ref_act_prop.prop_id "
		"  WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(%s, 0))" 
		"  ORDER BY ref_act_prop.prop_id "
		, data.mCls.mId.SqlVal() );
	return true;
}
