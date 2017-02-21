#ifndef __REPORTLISTMODEL_H
#define __REPORTLISTMODEL_H

#include "IModelWindow.h"
#include "ReportData.h"
#include "ResManager.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListModel : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_report24;
	const wxString mTitle = "Îò÷¸òû";


	rec::ReportList mRepList;
public:
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
	void LoadAll(const size_t pos, const rec::ReportItem*& ret_item);

	void GetItemByIdx(const size_t idx, const rec::ReportItem*& item)const;

	void Mk(const rec::ReportItem& item);
	void Rm(const size_t idx);
	void Ch(const size_t idx, const rec::ReportItem& item);

	
	using SigListUpdated = sig::signal<void(const rec::ReportList&)>;
	SigListUpdated sigListUpdated;

	sig::signal<void(const rec::ReportItem&)>				sigMkReport;
	sig::signal<void(const size_t)>							sigRmReport;
	sig::signal<void(const size_t, const rec::ReportItem&)> sigChReport;

};

class ReportItemModel
{
	std::shared_ptr<ReportListModel> mList;
	size_t mPosition = -1;
public:
	ReportItemModel(std::shared_ptr<ReportListModel> list)
		:mList(list)
	{
	}

	const size_t GetPosition()const
	{
		return mPosition;
	}

	void SetPosition(const size_t& pos)
	{
		mPosition = pos;
	}

	bool IsOk()
	{
		return mList->Size() > mPosition;
	}

	const rec::ReportItem& GetValue()const
	{
		const rec::ReportItem* item = nullptr;
		mList->LoadAll(mPosition, item);
		//mList->GetItemByIdx(mPosition, item);
		return *item;
	}

	void SetValue(const rec::ReportItem& item)
	{
		if (IsOk())
			mList->Ch(mPosition, item);
		else
			mList->Mk(item);
	}
};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H