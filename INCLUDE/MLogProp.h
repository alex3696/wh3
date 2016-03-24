#ifndef __MLOGPROP_H
#define __MLOGPROP_H

#include "db_rec.h"
#include "MTable.h"


namespace wh{
//-------------------------------------------------------------------------

class MLogPropItem
	:public ITableRow
{
public:
	MLogPropItem(const char option = ModelOption::EnableParentNotify)
		:ITableRow(option)
	{}

	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>& db, const size_t pos)override;
protected:

	virtual bool GetSelectQuery(wxString&)const override	{ return false; }
	virtual bool GetInsertQuery(wxString&)const override	{ return false; }
	virtual bool GetUpdateQuery(wxString&)const override	{ return false; }
	virtual bool GetDeleteQuery(wxString&)const override	{ return false; }


};
//-------------------------------------------------------------------------
class MLogPropDataArr
	:public TTableDataArr<MLogPropItem>
{
public:
	MLogPropDataArr(const char option
		= ModelOption::EnableParentNotify
		| ModelOption::EnableNotifyFromChild) 
		:TTableDataArr<MLogPropItem>(option)
	{}

	virtual bool GetSelectChildsQuery(wxString& query)const override;
	virtual bool LoadChildDataFromDb(std::shared_ptr<IModel>& child,
		std::shared_ptr<whTable>& table, const size_t pos)override;


	struct prop_compare {
		bool operator() (const std::shared_ptr<MLogPropItem>& lhs, const std::shared_ptr<MLogPropItem>& rhs) const{
			long s1, s2;
			if (lhs->GetData().at(3).ToLong(&s1) && rhs->GetData().at(3).ToLong(&s2))
				return s1 < s2;
			return false;
		}
	};



	std::set < std::shared_ptr<MLogPropItem>, prop_compare> mAP;

	typedef std::map <long, std::shared_ptr<MLogPropItem> > PropSet;

	std::map<long, PropSet> mPropAct;
protected:
	

	
	
};

//-------------------------------------------------------------------------
class MLogProp
	: public TTable<MLogPropDataArr>
{
public:
	MLogProp(const char option = ModelOption::EnableNotifyFromChild);

	void SetLogProp(const std::set<unsigned long>& prop_id);
	
	const wxString& GetPropArr()const { return mPropArr; }
protected:
	wxString mPropArr;
};


//-------------------------------------------------------------------------
}//namespace wh{
#endif // __****_H