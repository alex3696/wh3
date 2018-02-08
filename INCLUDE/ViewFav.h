#ifndef __VIEW_FAV_H
#define __VIEW_FAV_H

#include "IViewFav.h"

namespace wh {
//-----------------------------------------------------------------------------
class ViewFav : public IViewFav
{
	wxDialog*		mPanel;
protected:

public:
	ViewFav(wxWindow* parent);
	ViewFav(const std::shared_ptr<IViewWindow>& parent);
	virtual wxWindow* GetWnd()const override
	{
		return mPanel;
	}

	void OnShow() override;

	virtual void SetUpdate(const std::vector<const IIdent64*>&, const IIdent64*, bool) override;


};








//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H