#ifndef __RECENTDSROIDPRESENTER_H
#define __RECENTDSROIDPRESENTER_H

//#include "RecentDstOidModel.h"
#include "IRecentDstOidView.h"

namespace wh{
//-----------------------------------------------------------------------------
class RecentDstOidPresenter
{
public:
	RecentDstOidPresenter()
	{
		namespace ph = std::placeholders;
		auto fnAU = std::bind(&RecentDstOidPresenter::OnModelAfterUpdate, this, ph::_1);
		//connModelAfterUpdate = mModel->ConnAU(fnAU);
	}

	void Clear(std::size_t max_items = 10)	{ mModel->Clear(max_items); }
	bool Check(const wxString& item)const	{ return mModel->Check(item); }
	void Insert(const wxString& item)		{ mModel->Insert(item); }

	void GetFromConfig()		{ mModel->GetFromConfig(); }
	void SetToConfig()const		{ mModel->SetToConfig(); }


	char GetRecentEnable()const	{ return mModel->GetRecentEnable(); }
	char GetFilterEnable()const	{ return mModel->GetFilterEnable(); }

	void SetRecentEnable(char enable){ mModel->SetRecentEnable(enable); }
	void SetFilterEnable(char enable){ mModel->SetFilterEnable(enable); }
private:
	std::shared_ptr<wh::RecentDstOidModel> mModel = std::make_shared<RecentDstOidModel>();
	IRecentDstOidView* mView = nullptr;

	// View connector

	// Model connector
	sig::scoped_connection connModelAfterUpdate;
	void OnModelAfterUpdate(const rec::MruList& data)
	{
		if (mView)
			mView->Update(data);
	}


};//class RecentDstOidPresenter
//-----------------------------------------------------------------------------

}//namespace wh{
#endif // __****_H