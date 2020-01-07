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

	std::shared_ptr<const ModelPropTable> GetPropTable() { return mPropTable; }
	
	using SigRefresh = sig::signal<void(std::shared_ptr<const ModelPropTable>)>;
	using SigGetPropValues = sig::signal<void(std::map<int64_t, wxString>&)>;

	SigRefresh		sigBeforeRefresh;
	SigRefresh		sigAfterRefresh;
	SigGetPropValues sigGetPropValues;

private:

};
//---------------------------------------------------------------------------






} //namespace mvp{
#endif // __IMVP_H