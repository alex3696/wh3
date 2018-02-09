#ifndef __ICTRLWINDOW_H
#define __ICTRLWINDOW_H

#include "IViewWindow.h"
#include "IModelWindow.h"

namespace wh{
//---------------------------------------------------------------------------
class ICtrlWindow
{
public:
	virtual ~ICtrlWindow(){}
	virtual std::shared_ptr<IModelWindow> GetModel()const { return nullptr; };
	virtual std::shared_ptr<IViewWindow> GetView()const = 0;

	virtual void UpdateTitle() = 0;
	virtual void Show() = 0;
	virtual void RmView() = 0;
	virtual void MkView() {};
	virtual void Init() {};

	virtual void Load(const boost::property_tree::wptree&) = 0;
	virtual void Save(boost::property_tree::wptree&) = 0;

};
//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H