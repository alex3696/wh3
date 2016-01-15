#ifndef __DLG_ACT_VIEW_ACTLISTDVMODEL_H
#define __DLG_ACT_VIEW_ACTLISTDVMODEL_H

#include "dlg_act_model_Obj.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg_act {
namespace view {
//-------------------------------------------------------------------------



class ActListDvModel
	: public wxDataViewModel 
	, public ctrlWithResMgr
{
	
	
public:
	ActListDvModel();
	~ActListDvModel();

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

	void SetModel(std::shared_ptr<model::ActArray> model);
	
	void ClearModel();
protected:
	std::shared_ptr<model::ActArray> mActArray;

	sig::scoped_connection		mConnAppend;
	sig::scoped_connection		mConnRemove;
	sig::scoped_connection		mConnChange;

	void OnAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnBeforeRemove(const IModel&, const std::vector<SptrIModel>&);
	void OnChange(const IModel&, const std::vector<unsigned int>&);


};//class DwModel




//-------------------------------------------------------------------------
} // namespace view {
} // namespace dlg_move {
} // namespace wh{
#endif // __****_H