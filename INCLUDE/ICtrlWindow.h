#ifndef __ICTRLWINDOW_H
#define __ICTRLWINDOW_H

#include "IViewWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class ICtrlWindow
{
public:
	virtual ~ICtrlWindow(){}
	//virtual std::shared_ptr<IModelWindow > GetModel()const = 0;
	virtual std::shared_ptr<IViewWindow> GetView()const = 0;

	virtual void UpdateTitle() = 0;
	virtual void Show() = 0;
	virtual void RmView() = 0;
	virtual void MkView() {};
	virtual void Init() {};

	virtual void Load(const boost::property_tree::wptree&) = 0;
	virtual void Save(boost::property_tree::wptree&) = 0;

	sig::signal<void(ICtrlWindow*, const wxString&, const wxIcon&)>	sigUpdateTitle;
	sig::signal<void(ICtrlWindow*)>	sigShow;
	sig::signal<void(ICtrlWindow*)>	sigCloseView;
	sig::signal<void(ICtrlWindow*)>	sigCloseModel;


};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H