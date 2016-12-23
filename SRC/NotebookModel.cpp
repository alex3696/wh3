#include "_pch.h"
#include "NotebookModel.h"
#include "wx/txtstrm.h"
#include <wx/sstream.h>

using namespace mvp;

//---------------------------------------------------------------------------
NotebookModel::~NotebookModel()
{
	//CloseAll();
}
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageUser& cfg)
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

//---------------------------------------------------------------------------
void NotebookModel::MakePage(const wh::rec::PageGroup& cfg)
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
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageProp& cfg)
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
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageAct& cfg)
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
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageObjByType& cfg)
{
	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;

	auto model = std::make_shared<wh::object_catalog::MObjCatalog>();
	model->SetCfg(wh::rec::CatCfg(wh::rec::catCls, true, true, hideSysRoot));

	unsigned long cid;
	if (!cfg.mParent_Cid.ToCULong(&cid))
		throw;

	model->SetCatFilter(cid, true);
	//model->Load();

	PageInfo pi;
	pi.mType = mdlObjByType;
	pi.mPageModel->SetWhModel(model);
	pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_folder_type24);
	pi.mPageModel->SetTitle("Каталог по типам...");
	mPages.emplace_back(pi);
	sigAfterAddPage(*this, pi.mPageModel);
}
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageObjByPath& cfg)
{
	bool hideSysRoot = true;
	const auto& currBaseGroup = whDataMgr::GetInstance()->mDbCfg->mBaseGroup->GetData();
	if ((int)bgUser < (int)currBaseGroup)
		hideSysRoot = false;

	auto model = std::make_shared<wh::object_catalog::MObjCatalog>();
	model->SetCfg(wh::rec::CatCfg(wh::rec::catObj, true, true, hideSysRoot));
	unsigned long oid;
	if (!cfg.mParent_Oid.ToCULong(&oid))
		throw;
	
	model->SetCatFilter(oid, true);
	//model->Load();

	PageInfo pi;
	pi.mType = mdlObjByPath;
	pi.mPageModel->SetWhModel(model);
	pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_folder_obj24);
	pi.mPageModel->SetTitle("Каталог по местоположению...");
	mPages.emplace_back(pi);
	sigAfterAddPage(*this, pi.mPageModel);
}
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::ObjInfo& detail)
{
	auto model = std::make_shared<wh::detail::model::Obj>();
	model->SetObject(detail);

	PageInfo pi;
	pi.mType = mdlObjDetail;
	pi.mPageModel->SetWhModel(model);
	pi.mPageModel->SetIcon(&ResMgr::GetInstance()->m_ico_views24);
	pi.mPageModel->SetTitle("Подробно...");
	mPages.emplace_back(pi);
	sigAfterAddPage(*this, pi.mPageModel);
}
//---------------------------------------------------------------------------

void NotebookModel::MakePage(const wh::rec::PageHistory& cfg)
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
//---------------------------------------------------------------------------

void NotebookModel::DelPage(unsigned int page_index)
{
	const auto& page = mPages.at(page_index);
	sigBeforeDelPage(*this, page.mPageModel);
	mPages.erase(mPages.begin() + page_index);
}
//---------------------------------------------------------------------------

void NotebookModel::SelPage(unsigned int page_index)
{
	mSelectedPage = page_index;
}
//---------------------------------------------------------------------------

unsigned int NotebookModel::GetPageQty()const 
{ 
	return mPages.size(); 
}
//---------------------------------------------------------------------------

std::shared_ptr<PageModel> NotebookModel::GetPageModel(unsigned int idx)const
{
	return mPages.at(idx).mPageModel;
}
//---------------------------------------------------------------------------

void NotebookModel::Load()
{
try{
	using ptree = boost::property_tree::ptree;
	ptree	notepad_cfg;
	//boost::property_tree::read_json(std::string("notepad_cfg.txt"), notepad_cfg);
	whDataMgr::GetDB().BeginTransaction();
	wxString query = "SELECT cfg	FROM app_config WHERE usr = CURRENT_USER";
	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	wxString str_app_config;
	table->GetAsString(0, 0, str_app_config);
	whDataMgr::GetDB().Commit();
	std::stringstream ss(str_app_config.ToStdString());
	boost::property_tree::read_json(ss, notepad_cfg);

	auto active_page = notepad_cfg.get<int>("ActivePage");
	SelPage(active_page);

	for(ptree::value_type &v: notepad_cfg.get_child("Pages")) 
	{
		auto type_id = (ModelType)v.second.get<int>("Type.Id");
		switch (type_id)
		{
		case mdlUser:	MakePage(wh::rec::PageUser()); break;
		case mdlGroup:	MakePage(wh::rec::PageGroup()); break;
		case mdlProp:	MakePage(wh::rec::PageProp()); break;
		case mdlAct:	MakePage(wh::rec::PageAct()); break;
		case mdlObjByType:
		{
			wh::rec::PageObjByType rec;
			rec.mParent_Cid = v.second.get<std::string>("ParentCid");
			MakePage(rec);
		}
		break;
		case mdlObjByPath:
		{
			wh::rec::PageObjByPath rec;
			rec.mParent_Oid = v.second.get<std::string>("ParentOid");
			MakePage(rec);
		}
		break;
		case mdlObjDetail:
		{
			wh::rec::ObjInfo  detail;
			detail.mCls.mType = v.second.get<std::string>("CKind");
			detail.mObj.mId = v.second.get<std::string>("Oid");
			detail.mObj.mParent.mId = v.second.get<std::string>("ParentOid");
			MakePage(detail);
		}
		break;
		case mdlHistory:	MakePage(wh::rec::PageHistory()); break;
		default:break;
		}
		
	}

}//try
catch (boost::exception & e)
{
	whDataMgr::GetDB().RollBack();
	wxLogWarning(wxString(diagnostic_information(e)));
}///catch(boost::exception & e)
catch (...)
{
	wxLogWarning(wxString("Ошибка загрузки конфигурации"));
}//catch(...)	

}
//---------------------------------------------------------------------------

void NotebookModel::Save()
{
try{
	using ptree = boost::property_tree::ptree;
	
	ptree	notepad_cfg;
	ptree pages;
	
	for (unsigned int i = 0; i < mPages.size(); ++i)
	{
		const PageInfo& pi = mPages.at(i);
		ptree page;
		ptree pageType;
		pageType.put("Id", (int)pi.mType);

		switch (pi.mType)
		{
		case mdlUser:	pageType.put("Name", "Users");	break;
		case mdlGroup:	pageType.put("Name", "Groups");	break;
		case mdlProp:	pageType.put("Name", "Propetries");	break;
		case mdlAct:	pageType.put("Name", "Acts");	break;
		case mdlObjByType:	pageType.put("Name", "ObjByType");
		{
			auto by_type = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(pi.mPageModel->GetWhModel());
			if (by_type)
			{
				page.put("ParentCid", by_type->GetRoot().mCls.mId.toStr());
			}
		}
		break;
		case mdlObjByPath:	pageType.put("Name", "ObjByPath");
		{
			auto by_type = std::dynamic_pointer_cast<wh::object_catalog::MObjCatalog>(pi.mPageModel->GetWhModel());
			if (by_type)
			{
				page.put("ParentOid", by_type->GetRoot().mObj.mId.toStr());
			}
		}
		break;
		case mdlObjDetail:	pageType.put("Name", "ObjDetail");
		{
			auto detail = std::dynamic_pointer_cast<wh::detail::model::Obj>(pi.mPageModel->GetWhModel());
			if (detail)
			{
				const auto& obj = detail->GetData();
				page.put("CKind", (int)obj.mCls.GetClsType());
				page.put("Oid", obj.mObj.mId.toStr());
				page.put("ParentOid", obj.mObj.mParent.mId.toStr());
			}
		}
		break;
		case mdlHistory:	pageType.put("Name", "History");	break;
		default:break;
		}
		page.push_back(std::make_pair("Type", pageType));
		pages.push_back(std::make_pair("", page));
		
		
		
	}
	
	notepad_cfg.put("ActivePage", "0");
	notepad_cfg.add_child("Pages", pages);
	
	//boost::property_tree::write_json(std::string("notepad_cfg.txt"), notepad_cfg);

	std::ostringstream  ss;
	boost::property_tree::write_json(ss, notepad_cfg);

	wxString s;
	s = ss.str();

	whDataMgr::GetDB().BeginTransaction();
	wxString query = wxString::Format(
		"INSERT INTO app_config(cfg)VALUES('%s')"
		" ON CONFLICT(usr) DO UPDATE SET cfg = EXCLUDED.cfg "
		, s);
	whDataMgr::GetDB().Exec(query);
	whDataMgr::GetDB().Commit();

}//try
catch (boost::exception & e)
{
	whDataMgr::GetDB().RollBack();
	wxLogWarning(wxString(diagnostic_information(e)));
}///catch(boost::exception & e)
catch (...)
{
	wxLogWarning(wxString("Ошибка сохранения конфигурации"));
}//catch(...)	

}
//---------------------------------------------------------------------------
