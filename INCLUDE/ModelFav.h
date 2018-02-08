#ifndef __MODEL_FAV_H
#define __MODEL_FAV_H

#include "ModelBrowserData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh {
//-----------------------------------------------------------------------------
class ModelFav : public IModelWindow
{
	const wxIcon& mIco = ResMgr::GetInstance()->m_ico_type24;
	const wxString mTitle = "Object Browser";



	std::shared_ptr<ICls64>	mCls;
	
public:
	ModelFav();
	void DoShowFavEditor(int64_t cid);

	
	using SigRefreshCls =
		sig::signal	<void(const std::vector<const IIdent64*>&
			, const IIdent64*
			, const wxString&
			, bool
			)
		>;

	SigRefreshCls	sigBeforeRefreshCls;
	SigRefreshCls	sigAfterRefreshCls;


	// IModelWindow
	virtual const wxIcon& GetIcon()const override { return mIco; }
	virtual const wxString& GetTitle()const override { return mTitle; }
	virtual void UpdateTitle()override;

};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H