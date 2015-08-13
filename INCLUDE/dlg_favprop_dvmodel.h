#ifndef __DLG_FAVPROP_DVMODEL_H
#define __DLG_FAVPROP_DVMODEL_H

#include "dlg_favprop_model.h"

//-------------------------------------------------------------------------
namespace wh{
namespace dlg{
namespace favprop {
namespace view {
//-------------------------------------------------------------------------



class DvModel
	: public wxDataViewIndexListModel
	, public ctrlWithResMgr
{
	
	
public:
	DvModel();
	~DvModel();

	// implementation of base class virtuals to define model
	virtual unsigned int	GetColumnCount() const override;
	virtual wxString		GetColumnType(unsigned int col) const override;
	virtual bool			GetAttrByRow(unsigned int row, unsigned int col, 
		wxDataViewItemAttr &attr) const override;
	virtual bool			SetValueByRow(const wxVariant &variant, 
		unsigned int row, unsigned int col) override;
	virtual void			GetValueByRow(wxVariant &variant, 
		unsigned int row, unsigned int col) const override;
	//static void GetStringValue(const wxVariant& var, wxString& retStr );
	//virtual int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2, 
	//	unsigned int column, bool ascending) const override final;

	void SetModel(std::shared_ptr<IModel> model);
	
	void ClearModel();
protected:
	std::shared_ptr<model::FavPropArray> mPropArray;

	sig::scoped_connection		mConnPropAppend;
	sig::scoped_connection		mConnPropRemove;
	sig::scoped_connection		mConnPropChange;

	void OnPropAppend(const IModel&, const std::vector<unsigned int>&);
	void OnPropRemove(const IModel&, const std::vector<unsigned int>&);
	void OnPropChange(const IModel&, const std::vector<unsigned int>&);

	//virtual bool IsEnabledByRow(unsigned int row, unsigned int col) const override;

};//class DwModel




} //namespace wh{
} //namespace dlg{
} //namespace favprop {
} //namespace model {

#endif // __****_H