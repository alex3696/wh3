#ifndef __CTRL_BROWSERRO_H
#define __CTRL_BROWSERRO_H

#include "CtrlBrowser.h"

namespace wh {
//---------------------------------------------------------------------------
class CtrlTableObjBrowser_RO final : public CtrlWindowBase<IViewTableBrowser, ModelBrowser>
{
	sig::scoped_connection connModel_BeforeRefreshCls;
	sig::scoped_connection connModel_AfterRefreshCls;
	sig::scoped_connection connModel_ObjOperation;

	sig::scoped_connection connViewCmd_UpdatedQty;
public:
	CtrlTableObjBrowser_RO(const std::shared_ptr<IViewTableBrowser>& view
		, const  std::shared_ptr<ModelBrowser>& model)
		: CtrlWindowBase(view, model)
	{
		namespace ph = std::placeholders;
		connModel_BeforeRefreshCls = mModel->sigBeforeRefreshCls
			.connect(std::bind(&T_View::SetBeforeRefreshCls
				, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4, ph::_5));
		connModel_AfterRefreshCls = mModel->sigAfterRefreshCls
			.connect(std::bind(&T_View::SetAfterRefreshCls
				, mView.get(), ph::_1, ph::_2, ph::_3, ph::_4, ph::_5));
		connModel_ObjOperation = mModel->sigObjOperation
			.connect(std::bind(&T_View::SetObjOperation
				, mView.get(), ph::_1, ph::_2));
		
		connViewCmd_UpdatedQty = mView->sigSetQty
			.connect(std::bind(&CtrlTableObjBrowser_RO::SetQty, this, ph::_1, ph::_2));
	}

	void SetObjects(const std::set<ObjectKey>& obj)
	{
		mModel->DoSetObjects(obj);
	}

	bool SetQty(const ObjectKey& key, const wxString& str_val)
	{
		return mModel->DoSetQty(key, str_val);
	}



};

//---------------------------------------------------------------------------


}//namespace wh{
#endif // __****_H