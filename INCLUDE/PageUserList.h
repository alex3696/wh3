#ifndef __PAGEUSERLIST_H
#define __PAGEUSERLIST_H

#include "CtrlWindowBase.h"

//#include "MUser2.h"
#include "VUserCtrlPanel.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageUserList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_user24;
	const wxString mTitle = "Пользователи";
public:
	ModelPageUserList(std::shared_ptr<rec::PageUser> usr)
	{

	}

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }

	virtual void Load(const boost::property_tree::ptree& page_val)override
	{
		
	}

	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		page_val.push_back(std::make_pair("CtrlPageUserList", content));
	}


	std::shared_ptr<MUserArray> mUserList = std::make_shared<MUserArray>();

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mUserList);
	}
};
//---------------------------------------------------------------------------
class ViewPageUserList : public IViewWindow
{
	view::VUserCtrlPanel* mPanel;
public:
	ViewPageUserList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new view::VUserCtrlPanel(parent->GetWnd());
	}

	virtual wxWindow* GetWnd()const override
	{ 
		return mPanel; 
	}

	void SetWhModel(const wh::SptrIModel& wh_model)const
	{
		mPanel->SetModel(wh_model);
	}



};
//---------------------------------------------------------------------------
//typedef CtrlWindowBase<ViewPageUserList, ModelPageUserList> CtrlPageUserList;


class CtrlPageUserList : public CtrlWindowBase<ViewPageUserList, ModelPageUserList>
{
public:
	CtrlPageUserList(std::shared_ptr<ViewPageUserList> view
		, std::shared_ptr<ModelPageUserList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H