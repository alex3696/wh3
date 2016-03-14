#ifndef __MHISTORY_H
#define __MHISTORY_H

#include "db_rec.h"
#include "MTable.h"
#include "MLogProp.h"

namespace wh{

//-------------------------------------------------------------------------
class MLogItem
	:public ITableRow

{
public:
	MLogItem(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave)
		:ITableRow(option)
	{}
};


//-------------------------------------------------------------------------
class MLogTableDataArr
	:public TTableDataArr<MLogItem>
{
public:
	MLogTableDataArr(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	std::set<unsigned long>	mClsId;
	MLogProp				mLogProp;
	

protected:
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& db, const size_t pos)override;
	

	sig::scoped_connection		mConnRowBI;
	sig::scoped_connection		mConnRowAR;
	void OnRowBeforeInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
	void OnRowAfterRemove(const IModel& vec, const std::vector<SptrIModel>& remVec);

};
//-------------------------------------------------------------------------
class MLogTable
	: public TTable<MLogTableDataArr>
{
public:
	MLogTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	unsigned int			mStaticColumnQty;

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override;
	virtual bool GetAttrByRow(unsigned int row
		, unsigned int col, wxDataViewItemAttr &attr) const override;



};





}//namespace wh{
#endif // __****_H