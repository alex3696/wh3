#ifndef __MCONFIG_H
#define __MCONFIG_H

#include "TModelArray.h"

namespace wh{
//-----------------------------------------------------------------------------
namespace rec{
	
using GuiCfg = boost::property_tree::wptree;
//struct GuiCfg
//{
//	wxString						mPersp;
//	int								mActivePage=0;
//};

struct ConnectCfg
{
	wxString	mServer;
	wxString	mDB;
	long		mPort;
	wxString	mUser;
	wxString	mPass;
	bool		mStorePass;
	wxString	mRole;
};

struct FtpCfg
{
	wxString	mServer;
	long		mPort;
	wxString	mUser;
	wxString	mPass;
};

}//namespace rec{
//-----------------------------------------------------------------------------

class MGuiCfg
	: public TModelData<rec::GuiCfg>
{
public:
	MGuiCfg(const char option = ModelOption::EnableParentNotify);

	void LoadDefaults(const boost::property_tree::wptree& app_cfg);
	void SaveDefaults(boost::property_tree::wptree& app_cfg);
	void LoadClientInfo(const boost::property_tree::wptree& app_cfg);
	void SaveClientInfo(boost::property_tree::wptree& app_cfg);
protected:
	virtual void LoadData() override;
	virtual void SaveData() override;
	/*
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
	*/
};
//-----------------------------------------------------------------------------

class MConnectCfg
	: public TModelData<rec::ConnectCfg>
{
public:
	MConnectCfg(const char option = ModelOption::EnableParentNotify);

protected:
	virtual void LoadData() override;
	virtual void SaveData() override;
};
//-----------------------------------------------------------------------------

class MFtpCfg
	: public TModelData<rec::FtpCfg>
{
public:
	MFtpCfg(const char option = ModelOption::EnableParentNotify);

protected:
	virtual void LoadData() override;
	virtual void SaveData() override;
	/*
	virtual bool LoadThisDataFromDb(std::shared_ptr<whTable>&, const size_t)override;
	virtual bool GetSelectQuery(wxString&)const override;
	virtual bool GetInsertQuery(wxString&)const override;
	virtual bool GetUpdateQuery(wxString&)const override;
	virtual bool GetDeleteQuery(wxString&)const override;
	*/
};
//-----------------------------------------------------------------------------

class MBaseGroup
	: public TModelData<BaseGroup>
{
public:
	MBaseGroup(const char option = ModelOption::EnableParentNotify);

protected:
	virtual void LoadData() override;
};
//-----------------------------------------------------------------------------

class MDbCfg
	: public IModel
{
public:
	MDbCfg(const char option = ModelOption::EnableNotifyFromChild
		| ModelOption::CommitLoad
		| ModelOption::CommitSave);

	std::shared_ptr<MBaseGroup>		mBaseGroup;
	std::shared_ptr<MGuiCfg>		mGuiCfg;
	std::shared_ptr<MFtpCfg>		mFtpCfg;

protected:
	virtual void LoadChilds()override;
	virtual void SaveChilds()override;
};
//-----------------------------------------------------------------------------





}//namespace wh{
#endif // __****_H