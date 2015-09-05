#include "_pch.h"
#include "MObjCatalog.h"

using namespace wh;
using namespace wh::object_catalog;

const std::vector<Field> gTypeItemFieldVec =
{
	{ "Тип", FieldType::ftName, true },
};


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
MTypeItem::MTypeItem(const char option)
	:TModelData<rec::Cls>(option)
	, mObjArray(new MObjArray)
	, mFavProp(new dlg::favprop::model::FavPropArray)
{
	this->AddChild(std::dynamic_pointer_cast<IModel>(mObjArray));
	this->AddChild(std::dynamic_pointer_cast<IModel>(mFavProp));
}




//-------------------------------------------------------------------------
bool MTypeItem::LoadThisDataFromDb(std::shared_ptr<whTable>& table, const size_t row)
{
	T_Data data;
	table->GetAsString(0, row, data.mLabel);
	table->GetAsString(1, row, mQty);
	table->GetAsString(2, row, data.mID);
	table->GetAsString(3, row, data.mType);
	table->GetAsString(4, row, data.mMeasure);
	table->GetAsString(5, row, data.mDefaultPid);
	SetData(data);
	return true;
};
//-------------------------------------------------------------------------
void MTypeItem::LoadChilds()
{
	//auto type_array = dynamic_cast<MTypeArray*>(this->GetParent());
	//auto catalog = dynamic_cast<MObjCatalog*>(type_array->GetParent());
	if ( "0" != GetData().mType)
		mObjArray->Load();
	
}
//-------------------------------------------------------------------------
wxString MTypeItem::GetQty()const
{
	wxString result;
	/*
	хреновый вариант - точность хромает, проще спросить у БД
	if ("1" != GetData().mClsType)
		result = wxString::Format("%d", mObjArray->GetChildQty());
	if ("2" != GetData().mClsType || "3" != GetData().mClsType)
	{
		double sum = 0.0;
		for (unsigned int i = 0; i < mObjArray->GetChildQty(); ++i)
		{
			auto child = mObjArray->GetChild(i);
			auto obj = std::dynamic_pointer_cast<MObjItem>(child);
			double tmp;
			if (obj->GetData().mQty.ToDouble(&tmp) )
				sum += tmp ;
		}
		result = wxString::Format("%f", sum);
	}
	*/	

	result = wxString::Format("%s", mQty);
	
	return result;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsPropArray> MTypeItem::GetClsPropArray()
{
	if (!mPropArray)
	{
		mPropArray.reset(new MClsPropArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mPropArray);
		this->AddChild(item);
	}
	return mPropArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsActArray> MTypeItem::GetClsActArray()
{
	if (!mActArray)
	{
		mActArray.reset(new MClsActArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mActArray);
		this->AddChild(item);
	}
	return mActArray;
}
//-------------------------------------------------------------------------
std::shared_ptr<MClsMoveArray> MTypeItem::GetClsMoveArray()
{
	if (!mMoveArray)
	{
		mMoveArray.reset(new MClsMoveArray);
		std::shared_ptr<IModel> item = std::dynamic_pointer_cast<IModel>(mMoveArray);
		this->AddChild(item);
	}
	return mMoveArray;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MTypeItem::GetInsertQuery(wxString& query)const
{
	const auto& cls = GetData();

	const wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"INSERT INTO t_cls "
		" (label, description, type, measurename, pid) VALUES "
		" ('%s', %s, %s, %s, '%s') "
		" RETURNING id, label, description, type, measurename, pid, vid"
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		);
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetUpdateQuery(wxString& query)const
{
	const auto& cls = GetData();

	wxString parent = cls.mParent.mId.IsNull() ? wxString("1") : cls.mParent.mId;

	wxString measure;
	if ("0" == cls.mType)
		measure = "NULL";
	else if ("1" == cls.mType)
		measure = "'ед.'";
	else
		measure = "'" + cls.mMeasure + "'";

	query = wxString::Format(
		"UPDATE t_cls SET "
		" label='%s', description=%s, type=%s, measurename=%s, pid=%s "
		" WHERE id = %s "
		, cls.mLabel
		, cls.mComment.IsEmpty() ? L"NULL" : wxString::Format(L"'%s'", cls.mComment)
		, cls.mType
		, measure
		, parent
		, cls.mID);
	return true;
}
//-------------------------------------------------------------------------
bool MTypeItem::GetDeleteQuery(wxString& query)const
{
	const auto& cls = GetData();
	query = wxString::Format(
		"DELETE FROM t_cls WHERE id = %s ",
		cls.mID);
	return true;
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
bool MTypeArray::GetSelectChildsQuery(wxString& query)const
{
	auto catalog = dynamic_cast<MObjCatalog*>(this->GetParent());
	if (!catalog)
		return false;

	const auto& root = catalog->GetData();

	if (catalog->mCfg->GetData().mObjCatalog)
	{
		query = wxString::Format(
			"SELECT cls_label, COALESCE(SUM(qty),0), "
			"cls_id,  type, measurename, cls_default_pid "
			" FROM w_obj "
			" WHERE obj_pid = %s "
			" GROUP BY cls_label, cls_id, type, measurename, cls_default_pid"
			, root.mObj.mID );
		return true;
	}
	else
	{
		query = wxString::Format(
			"SELECT t_cls.label, COALESCE(SUM(w_obj.qty),0), t_cls.id, t_cls.type, "
			"t_cls.measurename, t_cls.default_pid "
			" FROM t_cls "
			" LEFT JOIN w_obj ON w_obj.cls_id = t_cls.id "
			" WHERE t_cls.pid = %s "
			" GROUP BY t_cls.id "
			, root.mCls.mID );
		return true;

	}


	return false;
}


