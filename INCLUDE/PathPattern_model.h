#ifndef __PATHPATTERNMODEL_H
#define __PATHPATTERNMODEL_H

#include "db_rec.h"
#include "TModelArray.h"

namespace wh{
namespace temppath{
namespace model {

//-------------------------------------------------------------------------
class Item
	: public TModelData<rec::PathNode>
	, public std::enable_shared_from_this<Item>
{
public:
	Item(const char option = ModelOption::EnableParentNotify);

};
//-------------------------------------------------------------------------	

class Array
	: public TModelArray<Item>
{
public:
	Array(const char option = ModelOption::EnableNotifyFromChild);

	void SetArr2Id2Title(const wxString& arr2Id, const wxString& arr2Title);
	
	wxString GetArr2Id(bool includeLast = false,bool reverse=true)const;
	wxString GetArr2Title(bool includeLast = false, bool reverse = true)const;

	std::shared_ptr<Item>	GetLast()const;


};

//-------------------------------------------------------------------------


} //namespace wh{
} //namespace temppath{
} //namespace model {

#endif // __****_H