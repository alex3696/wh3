#ifndef __DLG_MOVE_VIEW_DVMODEL_H
#define __DLG_MOVE_VIEW_DVMODEL_H

#include "dlg_move_model_MovableObj.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_move {
namespace view {
//-------------------------------------------------------------------------



class DvModel
	: public wxDataViewModel 
	, public ctrlWithResMgr
{
	
	
public:
	DvModel();
	~DvModel();

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

	void SetModel(std::shared_ptr<IModel> model);
	
	void ClearModel();
protected:
	std::shared_ptr<model::MovableObj> mMovable;

	sig::scoped_connection		mConnClsAppend;
	sig::scoped_connection		mConnClsRemove;
	sig::scoped_connection		mConnClsChange;

	void OnAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnClsBeforeRemove(const IModel&, const std::vector<SptrIModel>&);


	void OnClsChange(const IModel&, const std::vector<unsigned int>&);


};//class DwModel




//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H