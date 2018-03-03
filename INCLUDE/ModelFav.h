#ifndef __MODEL_FAV_H
#define __MODEL_FAV_H

#include "ModelBrowser.h"
#include "IModelWindow.h"

namespace wh {


//-----------------------------------------------------------------------------
class ModelFav : public IModelWindow
{
	int64_t						mCid;
	std::shared_ptr<ICls64>		mCls;
	std::unique_ptr<ClsCache>	mClsCache;
	std::unique_ptr<PropCache>	mPropCache;
	std::unique_ptr<ActCache>	mActCache;
	
	void LoadClsTree(std::vector<const ICls64*>& tree);
	void LoadFavCProp();
	void LoadFavOProp();
	void LoadFavAct();
public:
	ModelFav();
	void DoSetClsId(int64_t cid);
	void DoRefresh();

	void DoAddClsProp(int64_t clsId, int64_t propId);
	void DoAddObjProp(int64_t clsId, int64_t propId);
	void DoAddActProp(int64_t clsId, int64_t actId, FavAPropInfo);

	void DoRemoveClsProp(int64_t clsId, int64_t propId);
	void DoRemoveObjProp(int64_t clsId, int64_t propId);
	void DoRemoveActProp(int64_t clsId, int64_t actId, FavAPropInfo);
	
	using SigRefreshCls =
		sig::signal	<void(const std::vector<const ICls64*>& , const ICls64&)>;

	SigRefreshCls	sigBeforeRefresh;
	SigRefreshCls	sigAfterRefresh;

	// IModelWindow
	virtual void UpdateTitle()override;

};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
} //namespace wh{
#endif // __*_H