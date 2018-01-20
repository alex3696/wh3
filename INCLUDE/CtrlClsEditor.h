#ifndef __CTRL_CLSEDITOR_H
#define __CTRL_CLSEDITOR_H

#include "CtrlWindowBase.h"
//#include "ModelClsEditor.h"
//#include "IViewBrowser.h"

namespace wh {

class IViewClsEditor : public IViewWindow 
{
public:
	IViewClsEditor(wxWindow* parent)
	{

	}
	IViewClsEditor(const std::shared_ptr<IViewWindow>& parent)
		:IViewClsEditor(parent->GetWnd())
	{}
	virtual wxWindow* GetWnd()const override
	{
		return nullptr;
	}
};
class ModelClsEditor : public IModelWindow {};

//-----------------------------------------------------------------------------
class CtrlClsEditor final : public CtrlWindowBase<IViewClsEditor, ModelClsEditor>
{
	
	sig::scoped_connection connViewCmd_Find;
	
	sig::scoped_connection connModel_AfterRefreshCls;
public:
	CtrlClsEditor(const std::shared_ptr<IViewClsEditor>& view
		, const  std::shared_ptr<ModelClsEditor>& model);


	void Insert(int64_t parent_cid);
	void Delete(int64_t cid);
	void Update(int64_t cid);

};




//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H