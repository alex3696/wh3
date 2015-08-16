#ifndef __OBJCATALOGTABLE_H
#define __OBJCATALOGTABLE_H

#include "TModelArray.h"
#include "MObjCatalog.h"
#include "VObjCatalogDataViewModel.h"

namespace wh{
namespace view{


//---------------------------------------------------------------------------
/// 
class VObjCatalogTable
	: public wxDataViewCtrl
	//, public T_VecView
{

public:

	VObjCatalogTable(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 );//| wxDV_MULTIPLE

	//virtual void SetModel(std::shared_ptr<IModel> model)override;
	//virtual void GetSelected(std::vector<unsigned int>& vec)override;
	void SetModel(std::shared_ptr<IModel> model);
	void GetSelected(std::vector<unsigned int>& vec);

	void ExpandAll();
	

	void EnableAutosizeColumn(unsigned int columnNo);
	void DisableAutosizeColumn(unsigned int columnNo);
	void SetMultiselect(bool enabled = true);

protected:
	std::shared_ptr<wh::object_catalog::MObjCatalog> mCatalogModel;
	VObjCatalogDataViewModel* mDataViewModel = nullptr;


	sig::scoped_connection		mConnAppend;
	sig::scoped_connection		mConnRemove;
	sig::scoped_connection		mConnChange;

	void OnAppend(const IModel&, const std::vector<unsigned int>&);
	void OnRemove(const IModel&, const std::vector<unsigned int>&);
	void OnChange(const IModel&, const std::vector<unsigned int>&);
	//virtual void OnAppend(const IModel&, const std::vector<unsigned int>&)override;
	//virtual void OnRemove(const IModel&, const std::vector<unsigned int>&)override;
	//virtual void OnChange(const IModel&, const std::vector<unsigned int>&)override;


	sig::scoped_connection mCatalogUpdate;
	void OnAfterUpdateFavProp(const IModel* model, 
		const object_catalog::MFavProp::T_Data* data);
	void RebuildColumns();
	void ResetColumns();
	void BuildColumns();

	std::set<unsigned int>	mAutosizeColumn;
	void OnResize(wxSizeEvent& evt);
		
};



}//namespace view
}//namespace wh
#endif