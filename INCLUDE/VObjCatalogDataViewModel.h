#ifndef __VOBJCATALOGDATAVIEWMODEL_H
#define __VOBJCATALOGDATAVIEWMODEL_H
	

#include "MObjCatalog.h"
#include "TView.h"



namespace wh{
namespace view{

//----------------------------------------------------------------------------

	

class VObjCatalogDataViewModel
	: public wxDataViewModel 
{
	
	
public:
	VObjCatalogDataViewModel();
	~VObjCatalogDataViewModel();

	// implementation of base class virtuals to define model
	virtual unsigned int	GetColumnCount() const;
	virtual wxString		GetColumnType( unsigned int col ) const;
	
	virtual bool IsContainer( const wxDataViewItem &item )const;
	
	virtual void GetValue( wxVariant &variant,const wxDataViewItem &item, 
		unsigned int col ) const;
	virtual bool GetAttr (const wxDataViewItem &item, unsigned int col, 
		wxDataViewItemAttr &attr) const ; 
	virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item,
		unsigned int col);

	virtual wxDataViewItem GetParent( const wxDataViewItem &item ) const;
    virtual unsigned int GetChildren( const wxDataViewItem &parent,wxDataViewItemArray &array ) const;
	//int Compare( const wxDataViewItem &item1, const wxDataViewItem &item2,
	//	unsigned int column, bool ascending ) const;

	void SetModel(std::shared_ptr<IModel> model);
	
	void ClearModel();
protected:
	std::shared_ptr<wh::object_catalog::MObjCatalog> mCatalogModel;

	sig::scoped_connection		mConnClsAppend;
	sig::scoped_connection		mConnClsRemove;
	sig::scoped_connection		mConnClsChange;

	void OnClsAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnClsBeforeRemove(const IModel&, const std::vector<SptrIModel>&);

	void OnClsChange(const IModel&, const std::vector<unsigned int>&);

	void OnObjAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems
		, const SptrIModel& itemBefore);
	void OnObjBeforeRemove(const IModel&, const std::vector<SptrIModel>&);


	std::map<wxDataViewItem, sig::scoped_connection> mConnAddObj;
	std::map<wxDataViewItem, sig::scoped_connection> mConnDelObj;
	std::map<wxDataViewItem, sig::scoped_connection> mConnEditObj;



	//void GetTypeGroupValue( wxVariant &variant,const model::GroupInfo* groupInfo, 
	//	unsigned int col ) const;
	//void GetObjRowValue( wxVariant &variant,const model::ObjInfo* groupInfo, 
	//unsigned int col ) const;

};//class DataModel: public wxDataViewModel

}//namespace view
}//namespace wh
#endif //__*_H