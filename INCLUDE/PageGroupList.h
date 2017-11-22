#ifndef __PAGEGROUPLIST_H
#define __PAGEGROUPLIST_H

#include "CtrlWindowBase.h"

//#include "MGroup.h"
#include "VGroupCtrlPanel.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageGroupList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_usergroup24;
	const wxString mTitle = "Группы";

	std::shared_ptr<MGroupArray> mWhModel = std::make_shared<MGroupArray>();
public:
	ModelPageGroupList(const std::shared_ptr<rec::PageGroup>& usr)
	{

	}

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void Load(const boost::property_tree::wptree& page_val)override
	{
		//using ptree = boost::property_tree::wptree;
		//ptree::value_type page = *page_val.begin();
		//auto name = page.first.c_str();
		//int val1 = page.second.get<int>("id", 0);
		//int val2 = page_val.get<int>("CtrlPageGroupList.id", 0);
	}
	virtual void Save(boost::property_tree::wptree& page_val)override
	{
		using ptree = boost::property_tree::wptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair(L"CtrlPageGroupList", content));
		//page_val.put("CtrlPageGroupList.id", 33);
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageGroupList : public IViewWindow
{
	view::VGroupCtrlPanel* mPanel;
public:
	ViewPageGroupList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new view::VGroupCtrlPanel(parent->GetWnd());
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageGroupList : public CtrlWindowBase<ViewPageGroupList, ModelPageGroupList>
{
public:
	CtrlPageGroupList(std::shared_ptr<ViewPageGroupList> view
		, std::shared_ptr<ModelPageGroupList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H