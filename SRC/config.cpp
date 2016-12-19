#include "_pch.h"
#include "config.h"

#include "wx/wfstream.h"
#include "wx/datstrm.h"
#include "wx/txtstrm.h"
#include "wx/mstream.h"

using namespace wh;


//-----------------------------------------------------------------------------
MGuiCfg::MGuiCfg(const char option)
	:TModelData<rec::GuiCfg>(option)
{
}
//-----------------------------------------------------------------------------

void MGuiCfg::LoadData()
{
	T_Data data = this->GetData();
	
	
	//data.mCfg.clear();
	//std::wstringstream ss;
	//boost::property_tree::read_json(ss, data.mCfg);

	/*
	data.mPersp.clear();
	wxFileInputStream file_input("presp.cfg");
	if (!file_input.IsOk())
		return;
	wxBufferedInputStream buf_input(file_input);
	if (!buf_input.IsOk())
		return;

	wxTextInputStream text_input(buf_input);

	wxString str;
	text_input >> data.mPersp;

	this->SetData(data);
	*/
}
//-----------------------------------------------------------------------------

void MGuiCfg::SaveData()
{
	using ptree = boost::property_tree::ptree;
	
	ptree	notepad_cfg;
	ptree pages;
	/*
	
	ptree child1, child2, child3;
	
	child1.put("Type", "User");
	
	child2.put("Type", "ObjByPath");
	child2.put("Parent_Oid", 4);

	child3.put("Type", "ObjDetail");
	child3.put("Oid", 6);
	child3.put("ParentOid", 6);
	

	pages.push_back(std::make_pair("", child1));
	pages.push_back(std::make_pair("", child2));
	pages.push_back(std::make_pair("", child3));

	*/

	/*
	for (unsigned int i = 0; i < mNotepadCfg->GetChildQty(); ++i)
	{
		auto pageUser = std::dynamic_pointer_cast<MPageUser>(mNotepadCfg->GetChild(i));
		if (pageUser)
		{
			ptree page;
			page.put("Type", "Users");
			pages.push_back(std::make_pair("", page));
			continue;
		}
		auto pageGroup = std::dynamic_pointer_cast<MPageGroup>(mNotepadCfg->GetChild(i));
		if (pageGroup)
		{
			ptree page;
			page.put("Type", "Groups");
			pages.push_back(std::make_pair("", page));
			continue;
		}
		auto pageProp = std::dynamic_pointer_cast<MPageProp>(mNotepadCfg->GetChild(i));
		if (pageProp)
		{
			ptree page;
			page.put("Type", "Propetries");
			pages.push_back(std::make_pair("", page));
			continue;
		}
		auto pageAct = std::dynamic_pointer_cast<MPageAct>(mNotepadCfg->GetChild(i));
		if (pageAct)
		{
			ptree page;
			page.put("Type", "Acts");
			pages.push_back(std::make_pair("", page));
			continue;
		}
		auto pageObjByPath = std::dynamic_pointer_cast<MPageObjByPath>(mNotepadCfg->GetChild(i));
		if (pageObjByPath)
		{
			ptree page;
			page.put("Type", "ObjByPath");
			page.put("ParentOid", pageObjByPath->GetData().mParent_Oid);
			pages.push_back(std::make_pair("", page));
			continue;
		}
	}

	notepad_cfg.put("ActivePage", "0");
	notepad_cfg.add_child("Pages", pages);

	boost::property_tree::write_json(std::string("notepad_cfg.txt"), notepad_cfg);
	*/

	/*
	const T_Data& data = this->GetData();

	wxFileOutputStream file_output("presp.cfg");
	if (!file_output.IsOk())
		return;
	wxBufferedOutputStream buf_output(file_output);
	if (!buf_output.IsOk())
		return;
	wxTextOutputStream text_output(buf_output);

	text_output << data.mPersp;
	*/
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


	if (!local_cfg_file.FileExists())//���� ����������������� ������� ��� 
	{
		wxFileName		vendor_cfg_file("wh3.ini");
		if (vendor_cfg_file.FileExists())//���� ���������� ������� ����� � ��������		
		{
			if (!wxCopyFile("wh3.ini", local_cfg_file.GetFullPath())) // �������� ��� ����������������
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
