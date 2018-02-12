#ifndef __MODEL_FAV_H
#define __MODEL_FAV_H

#include "ModelBrowserData.h"
#include "IModelWindow.h"
#include "ModelFilterList.h"

namespace wh {
//-----------------------------------------------------------------------------
class ModelFav : public IModelWindow
{
	std::shared_ptr<ICls64>	mCls;
	
public:
	ModelFav();
	void DoSetClsId(int64_t cid);
	void DoRefresh();

	void DoAddClsProp(int64_t clsId, int64_t propId);
	void DoAddObjProp(int64_t clsId, int64_t propId);
	void DoAddPrevios(int64_t clsId, int64_t actId);
	void DoAddPeriod(int64_t clsId, int64_t actId);
	void DoAddNext(int64_t clsId, int64_t actId);
	void DoAddLeft(int64_t clsId, int64_t actId);

	void DoRemoveClsProp(int64_t clsId, int64_t propId);
	void DoRemoveObjProp(int64_t clsId, int64_t propId);
	void DoRemovePrevios(int64_t clsId, int64_t actId);
	void DoRemovePeriod(int64_t clsId, int64_t actId);
	void DoRemoveNext(int64_t clsId, int64_t actId);
	void DoRemoveLeft(int64_t clsId, int64_t actId);


	
	using SigRefreshCls =
		sig::signal	<void(const std::vector<const IIdent64*>&
			, const IIdent64*
			, const wxString&
			)
		>;

	SigRefreshCls	sigBeforeRefresh;
	SigRefreshCls	sigAfterRefresh;

	// IModelWindow
	virtual void UpdateTitle()override;

};
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H