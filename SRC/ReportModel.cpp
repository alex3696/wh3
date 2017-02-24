#include "_pch.h"
#include "ReportModel.h"
#include "ReportListModel.h"


using namespace wh;
//-----------------------------------------------------------------------------
ReportModel::ReportModel(std::shared_ptr<wxString> rep_id)
	:IModelWindow(), mRepId(*rep_id)
{
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	if(!modelRepList->Size())
		modelRepList->UpdateList();

	connListItemChange = modelRepList->sigChReport.connect
		([this](const std::shared_ptr<const rec::ReportItem>& ri, const wxString& old_rep_id)
	{
		Update();
	});

	connListItemRemove = modelRepList->sigRmReport.connect
		([this](const std::shared_ptr<const rec::ReportItem>& ri)
	{
		
		sigUpdateTitle("Îò÷¸ò óäàë¸í", mIco);
	});


	connListItemUpdate = modelRepList->sigListUpdated.connect
		([this](const rec::ReportList& rl)
	{
		Update();
	});

	sig::scoped_connection connListItemChange;
	sig::scoped_connection connListItemRemove;
	sig::scoped_connection connListItemUpdate;


}
//-----------------------------------------------------------------------------
void ReportModel::Update()
{
	UpdateTitle();
}
//-----------------------------------------------------------------------------
void ReportModel::Export()
{

}
//-----------------------------------------------------------------------------
void ReportModel::SetParam()
{

}
//-----------------------------------------------------------------------------
void ReportModel::Load(const boost::property_tree::ptree& page_val)
{
	mRepId = page_val.get<std::string>("CtrlPageReport.RepId");

}
//-----------------------------------------------------------------------------
void ReportModel::Save(boost::property_tree::ptree& page_val)
{
	using ptree = boost::property_tree::ptree;
	ptree content;
	content.put("RepId", mRepId.c_str());
	page_val.push_back(std::make_pair("CtrlPageReport", content));
	//page_val.put("CtrlPageGroupList.id", 33);
}
//-----------------------------------------------------------------------------
void ReportModel::UpdateTitle()
{
	wxString lbl = mTitle;
	
	auto container = whDataMgr::GetInstance()->mContainer;
	auto modelRepList = container->GetObject<ReportListModel>("ModelPageReportList");
	
	auto ri = modelRepList->GetRep(mRepId);
	if (ri)
	{
		lbl = ri->mTitle;
	}

	sigUpdateTitle(lbl, mIco);

}
