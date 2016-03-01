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

	int GetRowHeight()const {return mRowHeight;}
	virtual bool SetRowHeight(int height) override;
protected:
	int mRowHeight;

	std::shared_ptr<ITable>		mModel;
	sig::scoped_connection		mConnAppend;
	sig::scoped_connection		mConnRemove;
	sig::scoped_connection		mConnChange;

	sig::scoped_connection		mConnAR;

	void OnRowAfterInsert(const IModel& vec
		, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnRowBeforeRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);
	void OnChange(const IModel& newVec, const std::vector<unsigned int>& itemVec);

	void OnChangeVecState(ModelState state);

	virtual bool GetAttrByRow(unsigned int row, unsigned int WXUNUSED(col)
		,	wxDataViewItemAttr &attr) const override;
	virtual void GetValueByRow(wxVariant& val, unsigned int row
		, unsigned int col) override final;

};
//-----------------------------------------------------------------------------




























}//namespace wh{
#endif // __****_H