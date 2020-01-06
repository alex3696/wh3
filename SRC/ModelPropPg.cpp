#include "_pch.h"
#include "ModelPropPg.h"

using namespace wh;
//-----------------------------------------------------------------------------
ModelPropPg::ModelPropPg()
{

}
//-----------------------------------------------------------------------------
void ModelPropPg::DoSwap(std::shared_ptr<ModelPropTable> table)
{
	sigBeforeRefresh(mPropTable);
	mPropTable.swap(table);
	sigAfterRefresh(mPropTable);
}
