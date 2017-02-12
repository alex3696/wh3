#ifndef __NOTEBOOKMODEL_H
#define __NOTEBOOKMODEL_H

#include "globaldata.h"
#include "config.h"

#include "PageModel.h"


#include "MUser2.h"
#include "MGroup.h"
#include "MAct.h"
#include "MProp2.h"
#include "MObjCatalog.h"
#include "MHistory.h"
#include "detail_model.h"
//---------------------------------------------------------------------------
namespace mvp{

//---------------------------------------------------------------------------
class NotebookModel
	:public IModel
{
public:
	~NotebookModel();
	void MakePage(const wh::rec::PageUser& cfg);
	void MakePage(const wh::rec::PageGroup& cfg);
	void MakePage(const wh::rec::PageProp& cfg);
	void MakePage(const wh::rec::PageAct& cfg);
	void MakePage(const wh::rec::PageObjByType& cfg);
	void MakePage(const wh::rec::PageObjByPath& cfg);
	void MakePage(const wh::rec::ObjInfo& cfg);
	void MakePage(const wh::rec::PageHistory& cfg);
	void MakePage(const wh::rec::PageReport& cfg);

	void DelPage(unsigned int page_index);
	void SelPage(unsigned int page_index);

	unsigned int GetPageQty()const;

	std::shared_ptr<PageModel> GetPageModel(unsigned int idx)const;

	void Load();
	void Save();

	using SigNotebook = sig::signal<void(const NotebookModel& nb
		, const std::shared_ptr<PageModel>& pg)>;

	SigNotebook sigAfterAddPage;
	SigNotebook sigBeforeDelPage;
private:
	int mSelectedPage = -1;

	enum ModelType
	{
		mdlUser,
		mdlGroup,
		mdlProp,
		mdlAct,
		mdlObjByType,
		mdlObjByPath,
		mdlObjDetail,
		mdlHistory,
		mdlReport
	};

	struct PageInfo
	{
		PageInfo() 
			:mPageModel(std::make_shared<PageModel>())
			, mType(mdlGroup)
		{
		}
		ModelType					mType;
		std::shared_ptr<PageModel>	mPageModel;
	};

	std::vector<PageInfo> mPages;

};








} // namespace mvp{
#endif // __INOTEBOOKVIEW_H