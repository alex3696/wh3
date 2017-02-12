#ifndef __REPORTLISTMODEL_H
#define __REPORTLISTMODEL_H

#include "PageModel.h"
#include "ReportData.h"

namespace wh{
//-----------------------------------------------------------------------------
class ReportListModel : public mvp::PageModel
{
	rec::ReportList mRepList;
public:
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