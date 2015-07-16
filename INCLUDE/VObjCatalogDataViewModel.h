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
	
	// настройки каталога
	const rec::CatalogCfg* mCfg = nullptr;



	scoped_connection		mConnClsAppend;
	scoped_connection		mConnClsRemove;
	scoped_connection		mConnClsChange;

	void OnClsAppend(const IModel&, const std::vector<unsigned int>&);
	void OnClsRemove(const IModel&, const std::vector<unsigned int>&);
	void OnClsChange(const IModel&, const std::vector<unsigned int>&);

	void OnObjAppend(const IModel&, const std::vector<unsigned int>&);
	void OnObjRemove(const IModel&, const std::vector<unsigned int>&);
	//void OnObjChange(const IModel&, const std::vector<unsigned int>&);
	std::map<wxDataViewItem, scoped_connection> mConnAddObj;
	std::map<wxDataViewItem, scoped_connection> mConnDelObj;
	std::map<wxDataViewItem, scoped_connection> mConnEditObj;



	//void GetTypeGroupValue( wxVariant &variant,const model::GroupInfo* groupInfo, 
	//	unsigned int col ) const;
	//void GetObjRowValue( wxVariant &variant,const model::ObjInfo* groupInfo, 
	//unsigned int col ) const;

};//class DataModel: public wxDataViewModel

}//namespace view
}//namespace wh
#endif //__*_H