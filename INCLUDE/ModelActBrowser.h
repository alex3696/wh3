#ifndef __MODEL_ACTBROWSER_H
#define __MODEL_ACTBROWSER_H

#include "IModelWindow.h"
#include "ModelActTable.h"

namespace wh{
//---------------------------------------------------------------------------
class ModelActBrowserWindow : public IModelWindow
{
public:
	using FuncActivateCallback 
		= std::function<int(const ModelActTable::RowType& act)>;

	ModelActBrowserWindow();
	
	void DoSwap(std::shared_ptr<ModelActTable> table);
	void UpdateExist();
	void SetActs(const std::set<int64_t>& act_idents);
	void DoActivate();

	void SetActivateCallback(const FuncActivateCallback& fn);

	using SigRefresh = sig::signal<void(std::shared_ptr<const ModelActTable>)>;
	using SigGetSelection = sig::signal<void(std::set<int64_t>&)>;

	SigRefresh		sigBeforeRefresh;
	SigRefresh		sigAfterRefresh;
	SigGetSelection	sigGetSelection;
	
	//using SigOperation = sig::signal<void(Operation op, const std::vector<const IIdent64*>&)>;
	//SigOperation	sigOperation;
private:
	std::shared_ptr<ModelActTable>	mActTable;
	FuncActivateCallback			mFuncActivateCallback;

};
//---------------------------------------------------------------------------





} //namespace wh{
#endif // __*_H