#ifndef __PAGEOBJBYTYPELIST_H
#define __PAGEOBJBYTYPELIST_H

#include "CtrlWindowBase.h"

//#include "MObjCatalog.h"
#include "VObjCatalogCtrl.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageObjByTypeList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_folder_type24;
	const wxString mTitle = "Каталог по типам...";

	std::shared_ptr<object_catalog::MObjCatalog> mWhModel 
				= std::make_shared<object_catalog::MObjCatalog>();


	sig::scoped_connection connPathClear;
	sig::scoped_connection connPathInsert;
	void OnSigPathClear(const wh::IModel& model, const std::vector<wh::SptrIModel>&)
	{
		sigUpdateTitle("/", mIco);
	}
	void OnSigPathInsert(const wh::IModel& model, const std::vector<wh::SptrIModel>& vec
		, const wh::SptrIModel&)
	{
		const auto& path_array = dynamic_cast<const wh::object_catalog::model::MPath&>(model);
		sigUpdateTitle(path_array.GetLastItemStr(), mIco);
	}


public:
	ModelPageObjByTypeList(std::shared_ptr<rec::PageObjByType> cfg)
	{
		bool hideSysRoot = true;
		const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
		if ((int)bgUser < (int)currBaseGroup)
			hideSysRoot = false;
		mWhModel->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, hideSysRoot));
		
		
		unsigned long cid = 0;
		if (cfg->mParent_Cid.IsEmpty() || !cfg->mParent_Cid.ToCULong(&cid))
		{
			cid = hideSysRoot ? 1 : 0;
		}

		mWhModel->SetCatFilter(cid, true);

		namespace ph = std::placeholders;
		auto onClearPath = std::bind(&ModelPageObjByTypeList::OnSigPathClear, this, ph::_1, ph::_2);
		auto onChangePath = std::bind(&ModelPageObjByTypeList::OnSigPathInsert, this, ph::_1, ph::_2, ph::_3);

		connPathInsert = mWhModel->mPath->ConnAfterInsert(onChangePath);
		connPathClear = mWhModel->mPath->ConnectAfterRemove(onClearPath);

	}

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }

	virtual void UpdateTitle()override
	{
		OnSigPathInsert(*mWhModel->mPath.get(), std::vector<wh::SptrIModel>(), nullptr);
	}

	virtual void Load(const boost::property_tree::ptree& page_val)override
	{
		unsigned long cid = page_val.get<int>("CtrlPageObjByTypeList.ParentCid", 0);
		mWhModel->SetCatFilter(cid, true);
		mWhModel->Load(); 
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		content.put("ParentCid", mWhModel->GetRoot().mCls.mId.toStr());
		page_val.push_back(std::make_pair("CtrlPageObjByTypeList", content));
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageObjByTypeList : public IViewWindow
{
	view::VObjCatalogCtrl* mPanel;
public:
	ViewPageObjByTypeList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new view::VObjCatalogCtrl(parent->GetWnd());
		mPanel->HideCatalogSelect(true);
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageObjByTypeList : public CtrlWindowBase<ViewPageObjByTypeList, ModelPageObjByTypeList>
{
public:
	CtrlPageObjByTypeList(std::shared_ptr<ViewPageObjByTypeList> view
		, std::shared_ptr<ModelPageObjByTypeList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H