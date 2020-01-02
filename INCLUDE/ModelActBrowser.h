#ifndef __MODEL_ACTBROWSER_H
#define __MODEL_ACTBROWSER_H

#include "IModelWindow.h"
#include "ModelActTable.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelActBrowserWindow : public IModelWindow
{
	ModelActTable mActTable;

	
public:
	ModelActBrowserWindow();
	
	void DoRefresh();
	void UpdateExist();
	void SetActs(const std::set<int64_t>& act_idents);

	using SigRefresh = sig::signal<void(const std::vector<const IIdent64*>& ) >;
	using SigOperation = sig::signal<void(Operation op, const std::vector<const IIdent64*>&)>;

	SigRefresh		sigBeforeRefresh;
	SigRefresh		sigAfterRefresh;
	SigOperation	sigOperation;
	
};
//---------------------------------------------------------------------------





} //namespace wh{
#endif // __*_H