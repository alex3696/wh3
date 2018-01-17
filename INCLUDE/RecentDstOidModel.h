#ifndef __RECENTDSROIDMODEL_H
#define __RECENTDSROIDMODEL_H

#include "globaldata.h"
#include "config.h"

namespace wh{


namespace rec{
	typedef multi_index_container<
		wxString,
		indexed_by
		<
		 sequenced<>
		 , ordered_unique<identity<wxString> >
		>
	> MruList;
}//namespace wh{

//-----------------------------------------------------------------------------
class RecentDstOidModel
{
public:
	using SigMoveMru = sig::signal<void(const rec::MruList& model)>;

	RecentDstOidModel(std::size_t max_items = 10);

	

	void Clear(std::size_t max_num_items_ = 10);
	bool Check(const wxString& item)const;
	void Insert(const wxString& item);

	void Load(const boost::property_tree::wptree& app_cfg);
	void Save(boost::property_tree::wptree& app_cfg)const;

	inline void DoSigUpdate(){ sigAfterUpdate(mMruList); }

	const rec::MruList& GetMruList()const { return mMruList; }

	sig::connection ConnAU(SigMoveMru::slot_type& fn) { return sigAfterUpdate.connect(fn); }

	char GetRecentEnable()const	{ return mRecentEnable; }
	char GetFilterEnable()const	{ return mFilterEnable; }

	void SetRecentEnable(char enable){ mRecentEnable = enable; }
	void SetFilterEnable(char enable){ mFilterEnable = enable; }

private:
	SigMoveMru		sigAfterUpdate;

	rec::MruList	mMruList;
	std::size_t		mMaxItems=10;
	char			mRecentEnable=1;
	char			mFilterEnable=1;

	void InsertWithoutSig(const wxString& item);
};//MoveMruModel


}//namespace wh{
#endif // __****_H