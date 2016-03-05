#ifndef __MHISTORY_H
#define __MHISTORY_H

#include "db_rec.h"
#include "MTable.h"

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>

namespace wh{

//-------------------------------------------------------------------------
class MLogItem
	:public ITableRow

{
public:
	MLogItem(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild
		| ModelOption::CommitSave);
	

	
	
};

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
class MLogTable
	: public ITable
{
public:
	MLogTable(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild);

	virtual std::shared_ptr<IModel> CreateChild()override
	{
		return std::make_shared<MLogItem>();
	};

	virtual void GetValueByRow(wxVariant& val, unsigned int row, unsigned int col)override;
	virtual bool GetAttrByRow(unsigned int row
		, unsigned int col, wxDataViewItemAttr &attr) const override;
protected:
	//std::vector<boost::property_tree::ptree> mProp;

	virtual wxString GetTableName()const override;

	unsigned int mStaticColumnQty;

	sig::scoped_connection		mConnRowBI;
	void OnRowBeforeInsert(const IModel& vec, const std::vector<SptrIModel>& newItems, const SptrIModel& itemBefore);
};





}//namespace wh{
#endif // __****_H