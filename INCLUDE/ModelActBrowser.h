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
		= std::function<int(const ModelActTable::RowType& obj)>;

	ModelActBrowserWindow();
	
	void DoSwap(std::shared_ptr<ModelActTable> table);
	void UpdateExist();
	void SetActs(const std::set<int64_t>& act_idents);
	void DoActivate(int64_t aid);

	void SetActivateCallback(const FuncActivateCallback& fn);

	using SigRefresh = sig::signal<void(std::shared_ptr<const ModelActTable>)>;
	SigRefresh		sigBeforeRefresh;
	SigRefresh		sigAfterRefresh;
	
	//using SigOperation = sig::signal<void(Operation op, const std::vector<const IIdent64*>&)>;
	//SigOperation	sigOperation;
private:
	std::shared_ptr<ModelActTable>	mActTable;
	FuncActivateCallback			mFuncActivateCallback;

};
//---------------------------------------------------------------------------





} //namespace wh{
#endif // __*_H