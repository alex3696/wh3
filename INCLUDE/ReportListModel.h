#ifndef __REPORTLISTMODEL_H
#define __REPORTLISTMODEL_H

#include "IModelWindow.h"
#include "ReportData.h"
#include "ResManager.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListModel : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_report_list24;
	const wxString mTitle = "Îò÷¸òû";


	rec::ReportList mRepList;
public:
	ReportListModel();
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void Load(const boost::property_tree::ptree& page_val)override
	{
	}
	virtual void Save(boost::property_tree::ptree& page_val)override
	{
		using ptree = boost::property_tree::ptree;
		ptree content;
		//content.put("id", (int)-1);
		page_val.push_back(std::make_pair("CtrlPageReportList", content));
	}

	size_t Size()const { return mRepList.size();  }
	void UpdateList();
	std::shared_ptr<const rec::ReportItem> LoadAll(const wxString& rep_id);

	std::shared_ptr<const rec::ReportItem> GetRep(const wxString& rep_id)const;

	void Mk(const std::shared_ptr<rec::ReportItem>& rep);
	void Rm(const wxString& rep_id);
	void Ch(const wxString& rep_id, const std::shared_ptr<rec::ReportItem>& rep);

	
	sig::signal<void(const rec::ReportList&)>					sigListUpdated;

	sig::signal<void(const std::shared_ptr<const rec::ReportItem>&)>	sigMkReport;
	sig::signal<void(const std::shared_ptr<const rec::ReportItem>&)>	sigRmReport;
	sig::signal<void(const std::shared_ptr<const rec::ReportItem>&, const wxString& old_rep_id)> sigChReport;

};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ReportItemModel
{
	std::shared_ptr<ReportListModel> mList;
	wxString mRepId;

public:
	ReportItemModel(std::shared_ptr<ReportListModel> list)
		:mList(list)
	{

	}

	inline const wxString& GetRepId()const
	{
		return mRepId;
	}

	inline void SetRepId(const wxString& rep_id)
	{
		mRepId = rep_id;
	}

	inline std::shared_ptr<const rec::ReportItem> GetValue()const
	{
		return mList->LoadAll(mRepId);
	}

	void SetValue(const std::shared_ptr<rec::ReportItem>& rep)
	{
		const auto& ri = mList->GetRep(mRepId);
		if (ri)
			mList->Ch(mRepId, rep);
		else
			mList->Mk(rep);
	}
};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H