#include "_pch.h"
#include "config.h"

using namespace wh;

//-----------------------------------------------------------------------------
MGuiCfg::MGuiCfg(const char option)
	:TModelData<rec::GuiCfg>(option)
{
}
//-----------------------------------------------------------------------------

void MGuiCfg::LoadData()
{
	try{
		using ptree = boost::property_tree::ptree;
		ptree	app_cfg;
		//boost::property_tree::read_json(std::string("notepad_cfg.txt"), notepad_cfg);
		whDataMgr::GetDB().BeginTransaction();
		wxString query = "SELECT cfg FROM app_config WHERE usr = CURRENT_USER";
		auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
		wxString str_app_config;
		table->GetAsString(0, 0, str_app_config);
		whDataMgr::GetDB().Commit();
		std::stringstream ss(str_app_config.ToStdString());
		boost::property_tree::read_json(ss, app_cfg);
		this->SetData(app_cfg, true);

	}//try
	catch (boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)));
	}///catch(boost::exception & e)
	catch (...)
	{
		wxLogWarning(wxString("Ошибка загрузки конфигурации"));
	}//catch(...)
}
//-----------------------------------------------------------------------------

void MGuiCfg::SaveData()
{
	try{
		std::ostringstream  ss;
		boost::property_tree::write_json(ss, this->GetData() );

		wxString s;
		s = ss.str();

		whDataMgr::GetDB().BeginTransaction();
		wxString query = wxString::Format(
			"INSERT INTO app_config(cfg)VALUES('%s')"
			" ON CONFLICT(usr) DO UPDATE SET cfg = EXCLUDED.cfg "
			, s);
		whDataMgr::GetDB().Exec(query);
		whDataMgr::GetDB().Commit();

	}//try
	catch (boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		wxLogWarning(wxString(diagnostic_information(e)));
	}///catch(boost::exception & e)
	catch (...)
	{
		wxLogWarning(wxString("Ошибка сохранения конфигурации"));
	}//catch(...)	
}
//-----------------------------------------------------------------------------











//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MConnectCfg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MConnectCfg::MConnectCfg(const char option)
	:TModelData<rec::ConnectCfg>(option)
{
}
//-----------------------------------------------------------------------------

void MConnectCfg::LoadData()
{
	T_Data data = this->GetData();

	wxFileName		local_cfg_file = wxFileConfig::GetLocalFile("wh3.ini");;


	if (!local_cfg_file.FileExists())//если пользовательского конфига нет 
	{
		wxFileName		vendor_cfg_file("wh3.ini");
		if (vendor_cfg_file.FileExists())//ищем глобальный который рядом с экзешкой		
		{
			if (!wxCopyFile("wh3.ini", local_cfg_file.GetFullPath())) // копируем как пользовательский
				return;
		}
		else
			return;
	}

	wxFileConfig cfg("wh3", "alex3696@ya.ru", wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	cfg.Read("DbConnect/Server", &data.mServer);
	cfg.Read("DbConnect/Db", &data.mDB);
	cfg.Read("DbConnect/Port", &data.mPort);
	cfg.Read("DbConnect/User", &data.mUser);
	cfg.Read("DbConnect/Pass", &data.mPass);
	cfg.Read("DbConnect/Role", &data.mRole);
	cfg.Read("DbConnect/StorePass", &data.mStorePass);

	this->SetData(data);
}
//-----------------------------------------------------------------------------

void MConnectCfg::SaveData()
{
	const T_Data& data = this->GetData();

	wxFileConfig cfg("wh3", "alex3696@ya.ru", wxEmptyString, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

	cfg.Write("DbConnect/Server", data.mServer);
	cfg.Write("DbConnect/Db", data.mDB);
	cfg.Write("DbConnect/Port", data.mPort);
	cfg.Write("DbConnect/User", data.mUser);
	if (data.mStorePass)
		cfg.Write("DbConnect/Pass", data.mPass);
	else
		cfg.Write("DbConnect/Pass", wxEmptyString);
	cfg.Write("DbConnect/Role", data.mRole);
	cfg.Write("DbConnect/StorePass", data.mStorePass);
	cfg.Flush();

}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MFtpCfg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MFtpCfg::MFtpCfg(const char option)
	:TModelData<rec::FtpCfg>(option)
{
}
//-----------------------------------------------------------------------------

void MFtpCfg::LoadData()
{
	T_Data data = this->GetData();
	/*
	wxString query =
		"SELECT  prop.id    AS prop_id"
		" , prop.title AS prop_title"
		" , prop.kind  AS prop_kind"
		" , val"
		" , prop_cls.id AS id"
		" FROM prop_cls"
		" LEFT JOIN prop ON prop.id = prop_cls.prop_id"
		" WHERE prop_cls.cls_id = 2";

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table)
	{
		for (size_t i = 0; i < table->GetRowCount(); i++)
		{
			DbProp&	propTable = *this;
			const  wxString propLabel = table->GetAsString(1, i);
			propTable[propLabel] = table->GetAsString(3, i);
		}
	}

	query = "SELECT idx, groupname "
		" FROM fn_array1_to_table( "
		"  '{Guest,User,ObjDesigner,TypeDesigner,Admin}'::NAME[]) stdGroup "
		" INNER JOIN wh_membership ON wh_membership.groupname = stdGroup.id "
		" WHERE username = CURRENT_USER "
		" ORDER BY idx DESC LIMIT 1 ";
	table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
		mBaseGroup = (BaseGroup)table->GetAsInt(0, 0);
	*/

}
//-----------------------------------------------------------------------------

void MFtpCfg::SaveData()
{
	//const T_Data& data = this->GetData();



}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MFtpCfg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MBaseGroup::MBaseGroup(const char option)
	:TModelData<BaseGroup>(option)
{
}
//-----------------------------------------------------------------------------

void MBaseGroup::LoadData()
{
	const wxString query = 
		"SELECT idx, groupname "
		" FROM fn_array1_to_table( "
		"  '{Guest,User,ObjDesigner,TypeDesigner,Admin}'::NAME[]) stdGroup "
		" INNER JOIN wh_membership ON wh_membership.groupname = stdGroup.id "
		" WHERE username = CURRENT_USER "
		" ORDER BY idx DESC LIMIT 1 ";

	auto table = whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if (table && table->GetRowCount())
	{
		BaseGroup bg = (BaseGroup)table->GetAsInt(0, 0);
		this->SetData(bg);
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// MDbCfg
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MDbCfg::MDbCfg(const char option)
	:IModel(option)
	, mBaseGroup(new MBaseGroup)
	, mGuiCfg(new MGuiCfg)
	, mFtpCfg(new MFtpCfg)
{

	BaseGroup default_base_group = BaseGroup::bgNull;
	mBaseGroup->SetData(default_base_group);

	rec::GuiCfg default_gui_cfg;
	mGuiCfg->SetData(default_gui_cfg);

	rec::FtpCfg default_ftp_cfg;
	mFtpCfg->SetData(default_ftp_cfg);

	this->Insert(mBaseGroup);
	this->Insert(mGuiCfg);
	this->Insert(mFtpCfg);

	
}
//-----------------------------------------------------------------------------

void MDbCfg::LoadChilds()
{
	mBaseGroup->Load();
	mGuiCfg->Load();
	mFtpCfg->Load();
}
//-----------------------------------------------------------------------------

void MDbCfg::SaveChilds()
{
	mBaseGroup->Save();
	mGuiCfg->Save();
	mFtpCfg->Save();
}
