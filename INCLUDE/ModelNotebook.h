#ifndef __MODELNOTEBOOK_H
#define __MODELNOTEBOOK_H

#include "CtrlWindowBase.h"

namespace wh{

//-----------------------------------------------------------------------------

class ModelNotebook : public IModelWindow
{
	class WindowItem
	{
	public:
		WindowItem(std::shared_ptr<ICtrlWindow> ctrl)
			:mCtrlWindow(ctrl)
		{

		}
		sig::scoped_connection			connModelChTitle;
		sig::scoped_connection			connModelShow;
		sig::scoped_connection			connModelClose;
		std::shared_ptr<ICtrlWindow>	mCtrlWindow;
	};

	struct extr_wnd
	{
		typedef const wxWindow* result_type;
		inline result_type operator()(const std::shared_ptr<WindowItem>& r)const
		{
			return r->mCtrlWindow->GetView()->GetWnd();
		}
	};
	struct extr_ptr
	{
		typedef const void* result_type;
		inline result_type operator()(const std::shared_ptr<WindowItem>& r)const
		{
			return r->mCtrlWindow.get();
		}
	};


	using CtrlStore = boost::multi_index_container
	<
		std::shared_ptr<WindowItem>,
		indexed_by
		<
		   random_access<> 
		   , ordered_unique< extr_wnd >
		   , ordered_unique< extr_ptr >
		>
	>;

	CtrlStore mWindowList;

	void ShowWindowByPos(size_t pos);

	std::shared_ptr<ICtrlWindow> MkCtrl(const wxString& name);

	void RmCtrl(ICtrlWindow* wnd);
public:
	void MkWindow(const wxString& wi);
	void RmWindow(wxWindow* wnd);
	void ShowWindow(wxWindow* wnd);
	
	virtual void Load(const boost::property_tree::ptree& val)override;
	virtual void Save(boost::property_tree::ptree& val)override;

	sig::signal<void(ICtrlWindow*)>									sigAfterMkWindow;
	sig::signal<void(ICtrlWindow*)>									sigBeforeRmWindow;
	sig::signal<void(ICtrlWindow*)>									sigShowWindow;
	sig::signal<void(ICtrlWindow*, const wxString&, const wxIcon&)>	sigAfterChWindow;
	

};


//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H