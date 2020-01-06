#ifndef __MODEL_PROPPG_H
#define __MODEL_PROPPG_H

#include "ModelPropTable.h"
#include "IModelWindow.h"

namespace wh {
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class ModelPropPg : public IModelWindow
{
	std::shared_ptr<ModelPropTable> mPropTable;
public:
	ModelPropPg();

	void DoSwap(std::shared_ptr<ModelPropTable> table);
	
	using SigRefresh = sig::signal<void(std::shared_ptr<const ModelPropTable>)>;

	SigRefresh		sigBeforeRefresh;
	SigRefresh		sigAfterRefresh;

private:

};
//---------------------------------------------------------------------------






} //namespace mvp{
#endif // __IMVP_H