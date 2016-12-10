#ifndef __PAGEMODEL_H
#define __PAGEMODEL_H

#include "IMVP.h"
//#include "TModel.h"
#include "MObjCatalog.h"
#include "detail_model.h"
namespace mvp{
//---------------------------------------------------------------------------
class PageModel : public IModel
{
public:
	wh::SptrIModel GetWhModel()const { return mModel; }
	void SetWhModel(const wh::SptrIModel& wh_model) 
	{ 
		connPathInsert.disconnect();
		connPathClear.disconnect();
		connUpdateObj.disconnect();

		mModel = wh_model;
		auto mcat = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(wh_model);
		if (mcat)
		{
			auto onClearPath = std::bind(&PageModel::OnSigPathClear, this
				, std::placeholders::_1, std::placeholders::_2);
			auto onChangePath = std::bind(&PageModel::OnSigPathInsert, this, 
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

			onClearPath(*mcat->mPath.get(), std::vector<wh::SptrIModel>());
			onChangePath(*mcat->mPath.get(), std::vector<wh::SptrIModel>(), nullptr);

			connPathInsert = mcat->mPath->ConnAfterInsert(onChangePath);
			connPathClear = mcat->mPath->ConnectAfterRemove(onClearPath);
			return;
		}
		auto det_obj = std::dynamic_pointer_cast<wh::detail::model::Obj>(wh_model);
		if (det_obj)
		{
			auto fnOnChObj = std::bind(&PageModel::OnSigDetailObjUpdate, this
				, std::placeholders::_1, std::placeholders::_2);
			connUpdateObj=det_obj->DoConnect(wh::ModelOperation::moAfterUpdate, fnOnChObj);
		}
	}

	const wxIcon* const GetIcon()const
	{
		return mIcon;
	}
	const wxString& GetTitle()const
	{
		return mTitle;
	}
	void SetIcon(wxIcon* icon)
	{
		mIcon = icon;
	}
	void SetTitle(const wxString& title)
	{
		mTitle = title;
	}


	using SigUpdateCaption = sig::signal<void(const PageModel& pm
		, const wxString& lbl, const wxIcon& icon)>;

	SigUpdateCaption sigUpdateCaption;
private:	
	wh::SptrIModel	mModel;
	wxIcon*			mIcon=nullptr;
	wxString		mTitle;

	sig::scoped_connection connPathClear;
	sig::scoped_connection connPathInsert;
	sig::scoped_connection connUpdateObj;

	void OnSigPathClear(const wh::IModel& model, const std::vector<wh::SptrIModel>&)
	{
		sigUpdateCaption(*this, "/", *mIcon);
	}
	void OnSigPathInsert(const wh::IModel& model, const std::vector<wh::SptrIModel>& vec
		, const wh::SptrIModel&)
	{
		const auto& path_array = dynamic_cast<const wh::object_catalog::model::MPath&>(model);
		sigUpdateCaption(*this, path_array.GetLastItemStr(), *mIcon);
	}

	void OnSigDetailObjUpdate(const wh::IModel*, const wh::rec::ObjInfo* const oi)
	{
		const wxString lbl = wxString::Format("[%s]%s (%s %s)"
			, oi->mCls.mLabel.toStr()
			, oi->mObj.mLabel.toStr()
			, oi->mObj.mQty.toStr()
			, oi->mCls.mMeasure.toStr()
			);
		const wxIcon*  ico(&wxNullIcon);
		if (!oi->mCls.mType.IsNull())
			switch (oi->mCls.GetClsType())
		{
			default:
				ico = &ResMgr::GetInstance()->m_ico_type_abstract24;
				break;
			case wh::ctQtyByFloat:	case wh::ctQtyByOne:
				ico = &ResMgr::GetInstance()->m_ico_obj_qty24;
				break;
			case wh::ctSingle:
				ico = &ResMgr::GetInstance()->m_ico_obj_num24;
				break;
		}//switch
		sigUpdateCaption(*this, lbl, *ico);
	}
	
};

}// namespace mvp{
#endif // __PAGEMODEL_H