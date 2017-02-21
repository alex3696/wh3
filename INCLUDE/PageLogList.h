#ifndef __PAGELOGLIST_H
#define __PAGELOGLIST_H

#include "CtrlWindowBase.h"

#include "MHistory.h"
#include "VTablePanel.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelPageLogList : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_history24;
	const wxString mTitle = "История";

	std::shared_ptr<MLogTable> mWhModel = std::make_shared<MLogTable>();
public:
	ModelPageLogList(std::shared_ptr<rec::PageHistory> usr)
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
		//int val2 = page_val.get<int>("CtrlPageLogList.id", 0);
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair("CtrlPageLogList", content));
		//page_val.put("CtrlPageLogList.id", 33);
	}

	wh::SptrIModel GetWhModel()const
	{
		return std::dynamic_pointer_cast<IModel>(mWhModel);
	}

};
//---------------------------------------------------------------------------
class ViewPageLogList : public IViewWindow
{
	VTablePanel* mPanel;
public:
	ViewPageLogList(std::shared_ptr<IViewNotebook> parent)
	{
		mPanel = new VTablePanel(parent->GetWnd());
		mPanel->mCtrl.fnOnCmdInsert = nullptr;
		mPanel->mCtrl.fnOnCmdEdit = nullptr;
		mPanel->mCtrl.fnOnCmdRemove = nullptr;
		mPanel->mCtrl.fnOnCmdSave = nullptr;
	}

	virtual wxWindow* GetWnd()const override			{ return mPanel; }
	void SetWhModel(const wh::SptrIModel& wh_model)const{ mPanel->SetModel(wh_model); }

};
//---------------------------------------------------------------------------

class CtrlPageLogList : public CtrlWindowBase<ViewPageLogList, ModelPageLogList>
{
public:
	CtrlPageLogList(std::shared_ptr<ViewPageLogList> view
		, std::shared_ptr<ModelPageLogList> model)
		:CtrlWindowBase(view, model)
	{
		view->SetWhModel(model->GetWhModel());
		model->GetWhModel()->Load();
	}

};
//---------------------------------------------------------------------------


} //namespace wh{
#endif // __IMVP_H