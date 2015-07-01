#ifndef __MFAVPROP_H
#define __MFAVPROP_H


#include "db_rec.h"
#include "TModelArray.h"


//-------------------------------------------------------------------------
namespace wh{
//-------------------------------------------------------------------------
namespace rec
{

	struct FavProp
	{
		FavProp()
			:mCls(new std::set<wxString>)
		{}

		wxString	mID;
		wxString	mLabel;
		wxString	mType;

		std::shared_ptr<std::set<wxString> > mCls;

		FavProp(const FavProp& r)
		{
			mID = r.mID;
			mLabel = r.mLabel;
			mType = r.mType;
			mCls.reset(new std::set<wxString>(*r.mCls));
			//(*mCls) = *r.mCls;
		}
	};

	using FavProps = boost::multi_index_container
		<
			FavProp,
			indexed_by
			<
				random_access<>
				, ordered_unique< BOOST_MULTI_INDEX_MEMBER(FavProp, wxString, mLabel) >
			>
		>;

};
//-------------------------------------------------------------------------	
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