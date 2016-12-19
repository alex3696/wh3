#ifndef __VIEWFACTORY_H
#define __VIEWFACTORY_H

#include "IPageView.h"
#include "INotebookView.h"
namespace mvp{

class ViewFactory
{
public:
	static IPageView* MakePage(const wh::SptrIModel& wh_model, wxWindow* parent_wnd);
};






} // namespace mvp{
#endif // __INOTEBOOKVIEW_H