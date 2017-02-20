#ifndef __PAGEDETAILOBJ_H
#define __PAGEDETAILOBJ_H

#include "CtrlWindowBase.h"

#include "detail_ctrlpnl.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageDetailObj : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_obj_num24;
	const wxString mTitle = "Подробности...";

	std::shared_ptr<detail::model::Obj> mWhModel = std::make_shared<detail::model::Obj>();
public:
	ModelPageDetailObj(std::shared_ptr<rec::ObjInfo> oi)
	{
		if (oi)
			mWhModel->SetObject(*oi);
	}

	virtual void Show()override
	{
		mWhModel->Load();
		sigShow();
	}

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void Load(const boost::property_tree::ptree& page_val)override
	{
		wh::rec::ObjInfo  detail;
		detail.mCls.mType = page_val.get<std::string>("CtrlPageDetailObj.CKind");
		detail.mObj.mId = page_val.get<std::string>("CtrlPageDetailObj.Oid");
		detail.mObj.mParent.mId = page_val.get<std::string>("CtrlPageDetailObj.ParentOid");
		mWhModel->SetObject(detail);
		mWhModel->Load();
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		const rec::ObjInfo& obj = mWhModel->GetData();
		content.put("CKind", (int)obj.mCls.GetClsType());
		content.put("Oid", obj.mObj.mId.toStr());
		content.put("ParentOid", obj.mObj.mParent.mId.toStr());
		page_val.push_back(std::make_pair("CtrlPageDetailObj", content));
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageDetailObj : public IViewWindow
{
	detail::view::CtrlPnl* mPanel;
public:
	ViewPageDetailObj(std::shared_ptr<IViewNotebook> parent)
	{
		wxWindowUpdateLocker lock(parent->GetWnd());
		mPanel = new detail::view::CtrlPnl(parent->GetWnd());
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageDetailObj : public CtrlWindowBase<ViewPageDetailObj, ModelPageDetailObj>
{
public:
	CtrlPageDetailObj(std::shared_ptr<ViewPageDetailObj> view
		, std::shared_ptr<ModelPageDetailObj> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H