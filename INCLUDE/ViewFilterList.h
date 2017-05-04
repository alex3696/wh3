#ifndef __VIEW_FILTERLIST_H
#define __VIEW_FILTERLIST_H
//-----------------------------------------------------------------------------
#include "_pch.h"
#include "IViewFilterList.h"

namespace wh{
//-----------------------------------------------------------------------------
class ViewFilterList : public IViewFilterList
{
	wxScrolledWindow*	mPanel;

	void Insert(const std::shared_ptr<const ModelFilter>& filter
		, const std::shared_ptr<const ModelFilter>& before = nullptr);
	void Delete(const std::shared_ptr<const ModelFilter>& filter);
	void Update(const std::shared_ptr<const ModelFilter>& new_filter
		, const std::shared_ptr<const ModelFilter>& old_filter);

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


#endif // __****_H