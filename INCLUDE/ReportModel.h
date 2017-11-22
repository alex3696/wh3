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
	const wxString mTitle = "����� ...";
	wxString mRepId;
	sig::scoped_connection connListItemChange;
	sig::scoped_connection connListItemRemove;
	sig::scoped_connection connListItemUpdate;

	rec::ReportTable mReportTable;

	void DoUpdateView();
	void BuildFilterTable(rec::ReportFilterTable& ft);
public:
	ReportModel(const std::shared_ptr<wxString>& rep_id);

	void Update();
	void Execute(const std::vector<wxString>& filter_vec);
	void Export();
	sig::signal<void(const rec::ReportTable&)>			sigExecuted;
	sig::signal<void(const rec::ReportFilterTable&)>	sigSetFilterTable;
	sig::signal<void(const wxString&)>					sigSetNote;
	

	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;
	virtual void Show()override;
	virtual void Load(const boost::property_tree::wptree& page_val)override;
	virtual void Save(boost::property_tree::wptree& page_val)override;

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H