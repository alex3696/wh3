#ifndef __VTABLE_H
#define __VTABLE_H

#include "MTable.h"
#include "BaseTable.h"

namespace wh{
//-----------------------------------------------------------------------------
class VTable
	: public view::BaseTable
{
public:
	VTable(wxWindow*		parent,
		wxWindowID		id = wxID_ANY,
		const wxPoint&	pos = wxDefaultPosition,
		const wxSize &	size = wxDefaultSize,
		long			style = 0 | wxDV_MULTIPLE);

	void SetModel(std::shared_ptr<ITable> model);
protected:
	void OnColumnHeaderlClick(wxDataViewEvent &event);
	void OnChangeVecState(ModelState state);

	void SetGuiColumnSort(const wh::Field& field, wxDataViewColumn* col );

	virtual bool GetAttrByRow(unsigned int row, unsigned int WXUNUSED(col)
		, wxDataViewItemAttr &attr) const override;
	virtual void GetValueByRow(wxVariant& val, unsigned int row
		, unsigned int col) override final;

	std::shared_ptr<ITable>		mModel;
		
	sig::scoped_connection		mConnRowAI;
	sig::scoped_connection		mConnRowBR;
	sig::scoped_connection		mConnRowAR;
	sig::scoped_connection		mConnRowAC;
	
	sig::scoped_connection		mConnFieldAI;
	sig::scoped_connection		mConnFieldBR;
	sig::scoped_connection		mConnFieldAC;


	void OnRowAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnRowBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnRowAfterChange(const IModel& newVec, const std::vector<unsigned int>& itemVec);

	void OnFieldAfterInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnFieldBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnFieldInfoChange(const IModel& newVec, const std::vector<unsigned int>& itemVec);
	
};
//-----------------------------------------------------------------------------




























}//namespace wh{
#endif // __****_H