#ifndef __VIEW_FILTERLIST_H
#define __VIEW_FILTERLIST_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewFilterCtrl
{
public:
	virtual wxString GetSysTitle()const = 0;

	virtual void SetFilter(const wxString& title, const wxString& sys_title
		, FilterOp op, FieldType type
		, const std::vector<wxString>& val)=0;

	virtual void GetFilter(wxString& title, wxString& sys_title
		, FilterOp& op, FieldType& type
		, std::vector<wxString>& val)const = 0;

};
//-----------------------------------------------------------------------------
class ViewFilterList : public IViewFilterList
{
	struct extr_name
	{
		typedef const wxString result_type;
		inline result_type operator()(const ViewFilterCtrl* r)const
		{
			return r->GetSysTitle();
		}
	};

	using ViewFilterCtrlList =
	boost::multi_index_container
	<
		ViewFilterCtrl*,
		indexed_by
		<
			random_access<> //order
			, ordered_unique< extr_name >
		>
	>;
	
	ViewFilterCtrlList	mCtrlList;

	wxScrolledWindow*	mPanel;
	wxSizer*			mFilterSizer;
	wxButton*			mBtnApply;

	void Insert(const std::shared_ptr<const ModelFilter>& filter
		, const std::shared_ptr<const ModelFilter>& before = nullptr);
	void Delete(const std::shared_ptr<const ModelFilter>& filter);
	void Update(const std::shared_ptr<const ModelFilter>& new_filter
		, const std::shared_ptr<const ModelFilter>& old_filter);

public:
	using NotyfyItem = ModelFilterList::NotyfyItem;

	ViewFilterList(const std::shared_ptr<IViewWindow>& parent);
	ViewFilterList(wxWindow* parent);
	virtual wxWindow* GetWnd()const override;

	virtual void Update(const std::vector<NotyfyItem>& data) override;
	virtual void ViewToModel() override;
protected:
	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_UpdateAll(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_Apply(wxCommandEvent& evt = wxCommandEvent());

	virtual void OnShow()override;
private:
};
//-----------------------------------------------------------------------------
}//namespace wh{


#endif // __****_H