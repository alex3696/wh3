#ifndef __VIEWFACTORY_H
#define __VIEWFACTORY_H

#include "IPageView.h"
#include "INotebookView.h"
namespace mvp{

class ViewFactory
{
public:
	static IPageView* MakePage(IPresenter* presenter);
	
	static IPageView* MakePageGroup();
	static IPageView* MakePageUser();
	static IPageView* MakePageAct();
	static IPageView* MakePageProp();
	static IPageView* MakePageLog();
	static IPageView* MakePageCatalog();
	static IPageView* MakePageDetail();
};






} // namespace mvp{
#endif // __INOTEBOOKVIEW_H