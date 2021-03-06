#ifndef __PAGEOBJBYPATHLIST_H
#define __PAGEOBJBYPATHLIST_H

#include "CtrlWindowBase.h"

//#include "MObjCatalog.h"
#include "VObjCatalogCtrl.h"

namespace wh{
//---------------------------------------------------------------------------
// ������� �� ��������������
class ModelPageObjByPathList : public IModelWindow
{
	std::shared_ptr<object_catalog::MObjCatalog> mWhModel 
				= std::make_shared<object_catalog::MObjCatalog>();


	sig::scoped_connection connPathClear;
	sig::scoped_connection connPathInsert;
	void OnSigPathClear(const wh::IModel& model, const std::vector<wh::SptrIModel>&)
	{
		sigUpdateTitle("/", ResMgr::GetInstance()->m_ico_folder_obj24);
	}
	void OnSigPathInsert(const wh::IModel& model, const std::vector<wh::SptrIModel>& vec
		, const wh::SptrIModel&)
	{
		const auto& path_array = dynamic_cast<const wh::object_catalog::model::MPath&>(model);
		sigUpdateTitle(path_array.GetLastItemStr(), ResMgr::GetInstance()->m_ico_folder_obj24);
	}


public:
	ModelPageObjByPathList(const std::shared_ptr<rec::PageObjByPath>& cfg)
	{
		bool hideSysRoot = true;
		const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
		if ((int)bgUser < (int)currBaseGroup)
			hideSysRoot = false;
		mWhModel->SetCfg(wh::rec::CatCfg(wh::rec::catObj, true, true, hideSysRoot));
		
		
		unsigned long oid = 0;
		if (cfg->mParent_Oid.IsEmpty() || !cfg->mParent_Oid.ToCULong(&oid))
		{
			oid = hideSysRoot ? 1 : 0;
		}

		mWhModel->SetCatFilter(oid, true);

		namespace ph = std::placeholders;
		auto onClearPath = std::bind(&ModelPageObjByPathList::OnSigPathClear, this, ph::_1, ph::_2);
		auto onChangePath = std::bind(&ModelPageObjByPathList::OnSigPathInsert, this, ph::_1, ph::_2, ph::_3);

		connPathInsert = mWhModel->mPath->ConnAfterInsert(onChangePath);
		connPathClear = mWhModel->mPath->ConnectAfterRemove(onClearPath);

	}


	virtual void Show()override
	{
		mWhModel->Load();
		sigShow();
	}
	virtual void UpdateTitle()override
	{
		OnSigPathInsert(*mWhModel->mPath.get(), std::vector<wh::SptrIModel>(), nullptr);
	}

	virtual void Load(const boost::property_tree::wptree& page_val)override
	{
		unsigned long oid = page_val.get<int>(L"CtrlPageObjByPathList.ParentOid", 0);
		mWhModel->SetCatFilter(oid, true);
		mWhModel->Load(); 
	}
	virtual void Save(boost::property_tree::wptree& page_val)override
	{
		using ptree = boost::property_tree::wptree;
		ptree content;
		content.put(L"ParentOid", mWhModel->GetRoot().mObj.mId.toStr().wc_str());
		page_val.push_back(std::make_pair(L"CtrlPageObjByPathList", content));
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageObjByPathList : public IViewWindow
{
	view::VObjCatalogCtrl* mPanel;
public:
	ViewPageObjByPathList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new view::VObjCatalogCtrl(parent->GetWnd());
		mPanel->HideCatalogSelect(true);
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageObjByPathList : public CtrlWindowBase<ViewPageObjByPathList, ModelPageObjByPathList>
{
public:
	CtrlPageObjByPathList(std::shared_ptr<ViewPageObjByPathList> view
		, std::shared_ptr<ModelPageObjByPathList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H