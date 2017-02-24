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
	wxString mRepId;
	sig::scoped_connection connListItemChange;
	sig::scoped_connection connListItemRemove;
	sig::scoped_connection connListItemUpdate;
public:
	ReportModel(std::shared_ptr<wxString> rep_id);

	void Update();
	void Export();
	void SetParam();
	//using SigUpdated = sig::signal<void(const rec::ReportList&)>;

	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Load(const boost::property_tree::ptree& page_val)override;
	virtual void Save(boost::property_tree::ptree& page_val)override;

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H