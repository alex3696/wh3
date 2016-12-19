#ifndef __PAGEMVP_H
#define __PAGEMVP_H

#include "PageModel.h"
#include "IPageView.h"
namespace mvp{
//---------------------------------------------------------------------------

class PagePresenter
	: public IPresenter
{
public:
	PagePresenter(IPresenter* presenter);
	~PagePresenter();

	virtual void SetView(IView* view) override;
	virtual void SetModel(const std::shared_ptr<IModel>& model)override;
	virtual std::shared_ptr<IModel> GetModel() override;
	virtual IView* GetView() override;

private:
	std::shared_ptr<PageModel>	mModel;
	std::unique_ptr<IPageView>	mView;

};



//---------------------------------------------------------------------------
} //namespace mvp{
#endif // __IMVP_H