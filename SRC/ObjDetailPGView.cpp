#include "_pch.h"
#include "ObjDetailPGView.h"
#include "PGClsPid.h"


using namespace wh;
using namespace wh::detail::view;

//-----------------------------------------------------------------------------
ObjDetailPGView::ObjDetailPGView(wxWindow *parent, wxWindowID id,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	:wxPropertyGrid(parent, id, pos, size, style, name)
{
	// ����������� ��������� �������� ��� ��������� ������� �������
	auto pg_cls = Append(new wxPropertyCategory("�������� ������"));
	AppendIn(pg_cls, new wxClsProperty("��������", "base_cls_prop"));
	AppendIn(pg_cls, new wxPropertyCategory("����������������", "user_cls_prop"));

	auto pg_obj = Append(new wxPropertyCategory("�������� �������"));
	AppendIn(pg_obj, new wxObjTitleProperty("��������", "base_obj_prop"));
	AppendIn(pg_obj, new wxPropertyCategory("����������������", "user_obj_prop"));

	SetPropertyReadOnly(pg_cls, true);
	SetPropertyReadOnly(pg_obj, true);


}
//-----------------------------------------------------------------------------
void ObjDetailPGView::SetModel(std::shared_ptr<wh::detail::model::Obj> model)
{
	mObj = model;

	namespace sph = std::placeholders;

	auto funcOnChange = std::bind(&ObjDetailPGView::OnChangeMainDetail,
		this, sph::_1, sph::_2);
	mChangeMainDetail = mObj->DoConnect(moAfterUpdate, funcOnChange);

	mConnClsPropAppend = mObj->GetClsPropArray()->ConnAfterInsert(
		std::bind(&ObjDetailPGView::OnClsPropAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnClsPropRemove = mObj->GetClsPropArray()->ConnectBeforeRemove(
		std::bind(&ObjDetailPGView::OnClsPropBeforeRemove, this, sph::_1, sph::_2));
	mConnClsPropChange = mObj->GetClsPropArray()->ConnectChangeSlot(
		std::bind(&ObjDetailPGView::OnClsPropChange, this, sph::_1, sph::_2));

	mConnObjPropAppend = mObj->GetObjPropArray()->ConnAfterInsert(
		std::bind(&ObjDetailPGView::OnObjPropAfterInsert, this, sph::_1, sph::_2, sph::_3));
	mConnObjPropRemove = mObj->GetObjPropArray()->ConnectBeforeRemove(
		std::bind(&ObjDetailPGView::OnObjPropBeforeRemove, this, sph::_1, sph::_2));
	mConnObjPropChange = mObj->GetObjPropArray()->ConnectChangeSlot(
		std::bind(&ObjDetailPGView::OnObjPropChange, this, sph::_1, sph::_2));

}


//-----------------------------------------------------------------------------
void ObjDetailPGView::OnChangeMainDetail(const IModel* model, const wh::detail::model::Obj::T_Data* data)
{
	if (!mObj)
		return;

	const model::Obj::T_Data& main_detail = mObj->GetData();

	SetPropVal("base_cls_prop", wxVariant(*(wh_rec_Cls*)&main_detail.mCls));
	SetPropVal("base_obj_prop", wxVariant(*(wh_rec_ObjTitle*)&main_detail.mObj));

	UpdatePGColour();
}
//-----------------------------------------------------------------------------
void ObjDetailPGView::UpdatePGColour()
{
	if (!mObj)
		return;
	wxWindowUpdateLocker	wndUpdateLocker(this);

	const model::Obj::T_Data& main_detail = mObj->GetData();
	wxColour cl;
	if (!main_detail.mCls.mType.IsNull())
		switch (main_detail.mCls.GetClsType())
	{
		default:
			cl = (wxColour(240, 240, 240));
			HideProperty("user_obj_prop");
			break;
		case ctQtyByFloat:	case ctQtyByOne:
			cl = (wxColour(210, 240, 250));
			HideProperty("user_obj_prop");
			break;
		case ctSingle:
			cl = (wxColour(250, 240, 210));
			HideProperty("user_obj_prop", false);
			break;
	}//switch

	SetPropertyBackgroundColour("�������� ������", cl);
	//mPropGrid->SetPropertyBackgroundColour("user_cls_prop", cl);


}
//---------------------------------------------------------------------------- -
void ObjDetailPGView::OnClsPropAfterInsert(const IModel& vec
, const std::vector<SptrIModel>& newItems
, const SptrIModel& itemBefore)
{
	auto propCategory = GetProperty("user_cls_prop");
	for (const auto& curr : newItems)
	{
		auto item = dynamic_pointer_cast<model::ClsProp>(curr);
		const auto& data = item->GetData();
		propCategory->AppendChild(new wxStringProperty(
			data.mProp.mLabel
			, wxString::Format("ClsProp_%s", data.mProp.mLabel.toStr())
			, data.mVal));
	}

	UpdatePGColour();
	SetPropertyReadOnly(propCategory, true);

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void ObjDetailPGView::OnClsPropBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	auto cls_prop_array = mObj->GetClsPropArray();

	auto propCategory = GetProperty("user_cls_prop");
	if (cls_prop_array->size() == remVec.size())
	{
		propCategory->DeleteChildren();
	}
	else
	{
		size_t pos = 0;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
			{
				DeleteProperty(pos);
			}

		}
	}

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void ObjDetailPGView::OnClsPropChange(const IModel& model, const std::vector<unsigned int>& vec)
{
	auto prop_array = mObj->GetClsPropArray();

	auto propCategory = GetProperty("user_cls_prop");

	for (const unsigned int& i : vec)
	{
		auto prop_model = prop_array->at(i);
		const auto& prop_data = prop_model->GetData();

		propCategory->Item(i)->SetValueFromString(prop_data.mVal);
	}
}
//-----------------------------------------------------------------------------
void ObjDetailPGView::OnObjPropAfterInsert(const IModel& vec
	, const std::vector<SptrIModel>& newItems
	, const SptrIModel& itemBefore)
{
	auto propCategory = GetProperty("user_obj_prop");

	for (const auto& curr : newItems)
	{
		auto item = dynamic_pointer_cast<model::ObjProp>(curr);
		const auto& data = item->GetData();
		propCategory->AppendChild(new wxStringProperty(
			data.mProp.mLabel
			, wxString::Format("ObjProp_%s", data.mProp.mLabel.SqlVal())
			, data.mVal));
	}

	SetPropertyReadOnly(propCategory, true);

	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void ObjDetailPGView::OnObjPropBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec)
{
	auto obj_prop_array = mObj->GetObjPropArray();

	auto propCategory = GetProperty("user_obj_prop");
	if (obj_prop_array->GetChildQty() == remVec.size())
	{
		propCategory->DeleteChildren();
	}
	else
	{
		size_t pos = 0;
		for (const auto& remItem : remVec)
		{
			if (vec.GetItemPosition(remItem, pos))
			{
				DeleteProperty(pos);
			}

		}
	}
	if (!propCategory->GetChildCount())
		propCategory->Hide(true);
	else
		propCategory->Hide(false);

}
//-----------------------------------------------------------------------------
void ObjDetailPGView::OnObjPropChange(const IModel& model, const std::vector<unsigned int>& vec)
{
	auto prop_array = mObj->GetObjPropArray();

	auto propCategory = GetProperty("user_obj_prop");

	for (const unsigned int& i : vec)
	{
		auto prop_model = prop_array->at(i);
		const auto& prop_data = prop_model->GetData();

		propCategory->Item(i)->SetValueFromString(prop_data.mVal);
	}
}