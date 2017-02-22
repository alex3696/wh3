#ifndef __REPORTMODEL_H
#define __REPORTMODEL_H

#include "globaldata.h"
#include "ReportData.h"
#include "IModelWindow.h"


namespace wh{
//-----------------------------------------------------------------------------
class ReportModel : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_report24;
	const wxString mTitle = "Îò÷¸ò ...";
public:
	void Update();
	void Export();
	void SetParam();
	//using SigUpdated = sig::signal<void(const rec::ReportList&)>;

	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }

	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair("CtrlPageReport", content));
		//page_val.put("CtrlPageGroupList.id", 33);
	}

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H