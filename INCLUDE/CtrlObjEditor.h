#ifndef __CTRL_OBJEDITOR_H
#define __CTRL_OBJEDITOR_H

#include "CtrlWindowBase.h"
//#include "ModelObjEditor.h"
//#include "IViewBrowser.h"

namespace wh {
//-----------------------------------------------------------------------------
class IViewObjEditor : public IViewWindow
{
public:
	IViewObjEditor(wxWindow* parent)
	{

	}
	IViewObjEditor(const std::shared_ptr<IViewWindow>& parent)
		:IViewObjEditor(parent->GetWnd())
	{}
	virtual wxWindow* GetWnd()const override
	{
		return nullptr;
	}
};
class ModelObjEditor : public IModelWindow {};

//-----------------------------------------------------------------------------
class CtrlObjEditor final : public CtrlWindowBase<IViewObjEditor, ModelObjEditor>
{
	
	sig::scoped_connection connViewCmd_Find;
	
	sig::scoped_connection connModel_AfterRefreshCls;
public:
	CtrlObjEditor(const std::shared_ptr<IViewObjEditor>& view
		, const  std::shared_ptr<ModelObjEditor>& model);


	void Insert(int64_t cid);
	void Delete(int64_t oid, int64_t parent_oid);
	void Update(int64_t oid, int64_t parent_oid);

};




//-----------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H