#ifndef __APPVIEWCONFIG_H
#define __APPVIEWCONFIG_H

#include "TModel.h"

namespace wh{
//-----------------------------------------------------------------------------
namespace rec{

struct PageUser{};
struct PageGroup{};
struct PageProp{};
struct PageAct{};

struct PageObjByPath
{
	unsigned long mParent_Oid = 0;
};
//-----------------------------------------------------------------------------

struct PageObjByType
{
	unsigned long mParent_Cid = 0;
};
//-----------------------------------------------------------------------------

struct PageObjDetail
{
	unsigned long mOid = 0;
	unsigned long mParentOid = 0;
};

struct PageHistory
{
};

}//namespace rec{

template <class DATA>
class TPage
	: public TModelData<DATA>
{
public:
	TPage(const char option = ModelOption::EnableParentNotify)
		: TModelData<DATA>(option){}
};
//-----------------------------------------------------------------------------
typedef TPage<rec::PageUser>		MPageUser;
typedef TPage<rec::PageGroup>		MPageGroup;
typedef TPage<rec::PageProp>		MPageProp;
typedef TPage<rec::PageAct>			MPageAct;
typedef TPage<rec::PageObjByPath>	MPageObjByPath;
typedef TPage<rec::PageObjByType>	MPageObjByType;
typedef TPage<rec::PageObjDetail>	MPageObjDetail;
typedef TPage<rec::PageHistory>		MPageHistory;
//-----------------------------------------------------------------------------

class MNotepadCfg
	: public IModel
{
public:
	MNotepadCfg(const char option = ModelOption::EnableNotifyFromChild
		| ModelOption::CommitLoad
		| ModelOption::CommitSave);

protected:
	virtual void LoadChilds()override;
	virtual void SaveChilds()override;
};
//-----------------------------------------------------------------------------







}//namespace wh{
#endif // __****_H