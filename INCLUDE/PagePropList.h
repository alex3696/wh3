#ifndef __PAGEPROPLIST_H
#define __PAGEPROPLIST_H

#include "CtrlWindowBase.h"

#include "MProp2.h"
#include "VTablePanel.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPagePropList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_classprop24;
	const wxString mTitle = "Список свойств";

	std::shared_ptr<MPropTable> mWhModel = std::make_shared<MPropTable>();
public:
	ModelPagePropList(std::shared_ptr<rec::PageProp> usr)
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
		//int val2 = page_val.get<int>("CtrlPagePropList.id", 0);
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair("CtrlPagePropList", content));
		//page_val.put("CtrlPagePropList.id", 33);
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPagePropList : public IViewWindow
{
	VTablePanel* mPanel;
public:
	ViewPagePropList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new VTablePanel(parent->GetWnd());
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPagePropList : public CtrlWindowBase<ViewPagePropList, ModelPagePropList>
{
public:
	CtrlPagePropList(std::shared_ptr<ViewPagePropList> view
		, std::shared_ptr<ModelPagePropList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H