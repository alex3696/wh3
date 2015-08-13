#ifndef __MFAVPROP_H
#define __MFAVPROP_H


#include "db_rec.h"
#include "TModelArray.h"


//-------------------------------------------------------------------------
namespace wh{
namespace object_catalog {


//-------------------------------------------------------------------------
class MFavProp
	: public TModelData<rec::FavProps>
{
public:
	MFavProp(const char option = ModelOption::EnableParentNotify);

protected:
	virtual bool GetSelectQuery(wxString&)const override;
	virtual void LoadData()override;
	virtual void SaveData()override {};

};




//-------------------------------------------------------------------------
}//object_catalog {
}//namespace wh{
//-------------------------------------------------------------------------
#endif // __****_H