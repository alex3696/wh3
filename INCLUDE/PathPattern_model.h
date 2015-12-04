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

	void SetTmpPath(const wxString& arrId, const wxString& arrTitle);
	wxString GetTmpPathArr2IdSql(bool includeLast=false)const;
	wxString GetTmpPath()const;

};

//-------------------------------------------------------------------------


} //namespace wh{
} //namespace temppath{
} //namespace model {

#endif // __****_H