#ifndef __ICTRLWINDOW_H
#define __ICTRLWINDOW_H

#include "_pch.h"

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
	virtual void Close() = 0;
	virtual void Load(const boost::property_tree::ptree&) = 0;
	virtual void Save(boost::property_tree::ptree&) = 0;

	sig::signal<void(ICtrlWindow*, const wxString&, const wxIcon&)>	sigUpdateTitle;
	sig::signal<void(ICtrlWindow*)>	sigClose;
	sig::signal<void(ICtrlWindow*)>	sigShow;

};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H