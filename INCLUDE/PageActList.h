#ifndef __PAGEACTLIST_H
#define __PAGEACTPLIST_H

#include "CtrlWindowBase.h"

//#include "MAct.h"
#include "VActCtrlPanel.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageActList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_classprop24;
	const wxString mTitle = "Список групп";

	std::shared_ptr<MActArray> mWhModel = std::make_shared<MActArray>();
public:
	ModelPageActList(std::shared_ptr<rec::PageAct> usr)
	{

	}

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void Load(const boost::property_tree::ptree& page_val)override
	{
		//using ptree = boost::property_tree::ptree;
		//ptree::value_type page = *page_val.begin();
		//auto name = page.first.c_str();
		//int val1 = page.second.get<int>("id", 0);
		//int val2 = page_val.get<int>("CtrlPageActList.id", 0);
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair("CtrlPageActList", content));
		//page_val.put("CtrlPageActList.id", 33);
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageActList : public IViewWindow
{
	view::VActCtrlPanel* mPanel;
public:
	ViewPageActList(std::shared_ptr<IViewNotebook> parent)
	{
		wxWindowUpdateLocker lock(parent->GetWnd());
		mPanel = new view::VActCtrlPanel(parent->GetWnd());
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageActList : public CtrlWindowBase<ViewPageActList, ModelPageActList>
{
public:
	CtrlPageActList(std::shared_ptr<ViewPageActList> view
		, std::shared_ptr<ModelPageActList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H