#ifndef __VIEW_FILTERLIST_H
#define __VIEW_FILTERLIST_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewFilterList : public IViewFilterList
{
	wxPanel*		mPanel;
	wxAuiManager*   mAuiMgr;
	wxPropertyGrid* mPG;

	void Insert(const std::shared_ptr<const ModelFilter>& filter
		, const std::shared_ptr<const ModelFilter>& before = nullptr);
	void Delete(const std::shared_ptr<const ModelFilter>& filter);
	void Update(const std::shared_ptr<const ModelFilter>& new_filter
		, const std::shared_ptr<const ModelFilter>& old_filter);

	wxPGProperty* MakePGP(const std::shared_ptr<const ModelFilter>& filter);
public:
	using NotyfyItem = ModelFilterList::NotyfyItem;

	ViewFilterList(std::shared_ptr<IViewWindow> parent);
	ViewFilterList(wxWindow* parent);
	virtual wxWindow* GetWnd()const override;

	virtual void Update(const std::vector<NotyfyItem>& data) override;
protected:
	void OnCmd_Update(wxCommandEvent& evt = wxCommandEvent());
	void OnCmd_UpdateAll(wxCommandEvent& evt = wxCommandEvent());

	virtual void OnShow()override;
private:
};
//-----------------------------------------------------------------------------
}//namespace wh{

using whModelFilter = wh::ModelFilter;
WX_PG_DECLARE_VARIANT_DATA(whModelFilter)
//-----------------------------------------------------------------------------
class wxPGPFilterProperty : public wxPGProperty
{
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGPFilterProperty)
public:

	wxPGPFilterProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const whModelFilter& value = whModelFilter());
	virtual ~wxPGPFilterProperty();

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex, wxVariant& childValue) const;
	virtual void RefreshChildren();

protected:
	virtual wxString  ValueToString(wxVariant &  value, int  argFlags = 0)  const;

};








#endif // __****_H