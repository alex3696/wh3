#ifndef __NOTEBOOKMODEL_H
#define __NOTEBOOKMODEL_H

#include "globaldata.h"
#include "config.h"

#include "PageModel.h"


#include "MUser2.h"
#include "MGroup.h"
#include "MAct.h"
#include "MProp2.h"
#include "MObjCatalog.h"
#include "MHistory.h"
#include "detail_model.h"
//---------------------------------------------------------------------------
namespace mvp{

//---------------------------------------------------------------------------
class NotebookModel
	:public IModel
{
public:
	~NotebookModel()
	{
		//CloseAll();
	}

	void MakePage(const wh::rec::PageUser& cfg)
	{
		auto model = std::make_shared<wh::MUserArray>();
		PageInfo pi;
		pi.mType = mdlUser;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_user24);
		pi.mPageModel->SetTitle("Пользователи");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageGroup& cfg)
	{
		auto model = std::make_shared<wh::MGroupArray>();
		PageInfo pi;
		pi.mType = mdlGroup;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_usergroup24);
		pi.mPageModel->SetTitle("Группы");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageProp& cfg)
	{
		auto model = std::make_shared<wh::MPropTable>();
		PageInfo pi;
		pi.mType = mdlProp;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_classprop24);
		pi.mPageModel->SetTitle("Свойства");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageAct& cfg)
	{
		auto model = std::make_shared<wh::MActArray>();
		PageInfo pi;
		pi.mType = mdlAct;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_act24);
		pi.mPageModel->SetTitle("Действия");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageObjByType& cfg)
	{
		bool hideSysRoot = true;
		const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
		if ((int)bgUser < (int)currBaseGroup)
			hideSysRoot = false;

		auto model = std::make_shared<wh::object_catalog::MObjCatalog>();
		model->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, hideSysRoot));
		model->SetCatFilter(cfg.mParent_Cid, true);
		//model->Load();

		PageInfo pi;
		pi.mType = mdlObjByType;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_folder_type24);
		pi.mPageModel->SetTitle("Каталог по типам...");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageObjByPath& cfg)
	{
		bool hideSysRoot = true;
		const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
		if ((int)bgUser < (int)currBaseGroup)
			hideSysRoot = false;

		auto model = std::make_shared<wh::object_catalog::MObjCatalog>();
		model->SetCfg(wh::rec::CatCfg(wh::rec::catObj, true, true, hideSysRoot));
		model->SetCatFilter(cfg.mParent_Oid, true);
		model->Load();

		PageInfo pi;
		pi.mType = mdlObjByPath;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_folder_obj24);
		pi.mPageModel->SetTitle("Каталог по местоположению...");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageObjDetail& cfg)
	{
		auto model = std::make_shared<wh::detail::model::Obj>();

		wh::rec::ObjInfo obj_info;
		obj_info.mObj.mId = cfg.mOid;
		obj_info.mObj.mParent.mId = cfg.mParentOid;

		model->SetObject(obj_info);

		PageInfo pi;
		pi.mType = mdlObjDetail;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_views24);
		pi.mPageModel->SetTitle("Подробно...");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}
	void MakePage(const wh::rec::PageHistory& cfg)
	{
		auto model = std::make_shared<wh::MLogTable>();
		PageInfo pi;
		pi.mType = mdlHistory;
		pi.mPageModel->SetWhModel(model);
		pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_history24);
		pi.mPageModel->SetTitle("Вся история");
		mPages.emplace_back(pi);
		sigAfterAddPage(*this, pi.mPageModel);
	}

	void DelPage(unsigned int page_index)
	{
		const auto& page = mPages.at(page_index);
		sigBeforeDelPage(*this, page.mPageModel);
		mPages.erase(mPages.begin() + page_index);
	}
	void SelPage(unsigned int page_index)
	{
		mSelectedPage = page_index;
	}

	unsigned int GetPageQty()const { return mPages.size(); }

	std::shared_ptr<PageModel> GetPageModel(unsigned int idx)const
	{
		return mPages.at(idx).mPageModel;
	}

	using SigNotebook = sig::signal<void(const NotebookModel& nb
		, const std::shared_ptr<PageModel>& pg)>;

	SigNotebook sigAfterAddPage;
	SigNotebook sigBeforeDelPage;
private:
	int mSelectedPage = -1;

	enum ModelType
	{
		mdlUser,
		mdlGroup,
		mdlProp,
		mdlAct,
		mdlObjByType,
		mdlObjByPath,
		mdlObjDetail,
		mdlHistory
	};

	struct PageInfo
	{
		PageInfo() 
			:mPageModel(std::make_shared<PageModel>())
			, mType(mdlGroup)
		{
		}
		ModelType					mType;
		std::shared_ptr<PageModel>	mPageModel;
	};

	std::vector<PageInfo> mPages;

};








} // namespace mvp{
#endif // __INOTEBOOKVIEW_H