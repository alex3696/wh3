#ifndef __GLOBALDATA_H
#define __GLOBALDATA_H

#include "_pch.h"
#include "whDB.h"
#include "ResManager.h"
#include "wxComboBtn.h"
#include "whTableMgr.h"
#include "favorites.h"


class MainFrame;

namespace wh
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Config
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class Cfg: public TSingletonSptr<Cfg>
{
public:
	class DbConnect
	{
	public:
		wxString	mServer;
		wxString	mDB;
		long		mPort;
		wxString	mUser;
		wxString	mPass;

		wxString	mRole;
	
		DbConnect()
			:mServer(L"localhost"),mDB(L"wh3"),mPort(5432),mRole("User")
		{}
	
		void Load();
		void Save();

	
	}	Db;

	struct DbProp: public std::map<wxString,wxString>
	{
		void Load();
		//wxString FtpServer()	{	return (*this)[wxString("FTP server")];	}

	}	Prop;
};//class Cfg: public TSingletonSptr<Cfg>
//---------------------------------------------------------------------------

class Ftp: public TSingletonSptr<Ftp>
{
public:
	struct ftp_error:				virtual exception_base { };
		struct ftp_error_connect:	virtual ftp_error { };
		struct ftp_cancel_io:		virtual ftp_error { };
		
		struct ftp_error_io:		virtual ftp_error { };
			struct ftp_error_upload:	virtual ftp_error_io { };
			struct ftp_error_download:	virtual ftp_error_io { };
			struct ftp_error_remove:	virtual ftp_error_io { };
			struct ftp_error_rename:	virtual ftp_error_io { };
			struct ftp_not_exists:		virtual ftp_error_io { };

protected:
	friend class TSingletonSptr<Ftp>;

	wxFTP						mFtp;
	std::shared_ptr<wh::Cfg>	mCfg;

	Ftp();

	void	DoConnect();
	void	DoCopyFile(wxInputStream* src,wxOutputStream* dst,const size_t src_size,const wxString& msg);
public:
	void	Remove(const wxFileName& file);
	void	Upload(const wxFileName& localfile,const wxFileName& ftpfile);
	void	Download(const wxFileName& localfile,const wxFileName& ftpfile);
	void	Rename(const wxFileName& ftpfile,const wxString& new_name);

	void	DoChDir(const wxFileName& ftpfile, bool make_dirs=true);
	bool	GetFilesList (wxArrayString &files, const wxString &wildcard=wxEmptyString)
	{
		DoConnect();
		return mFtp.GetFilesList(files,wildcard);
	}
};
//---------------------------------------------------------------------------

};//namespace wh











//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// whDataMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** менеджер данных приложения 
Все основные действия и комманды изменения данных выполняем через него
*/
class whDataMgr: public  TSingleton<whDataMgr>
{
protected:
	friend class TSingleton<whDataMgr>;
	whDataMgr();
	~whDataMgr();
public:
	struct data_is_null: virtual exception_base { };

	whDB						m_DB;
	MainFrame*					m_MainFrame;
	wh::favorites::DataModel*	mFavoritesModel;
	
	std::shared_ptr<wh::Cfg>	mCfg;
	std::shared_ptr<wh::Ftp>	mFtp;



	static whDB& GetDB()//throw(data_is_null)
	{	
		whDataMgr* ptr = GetInstance();
		whDB* retDB=nullptr;
		if (!ptr)
			throw data_is_null();//?????

		retDB = &ptr->m_DB;
		ptr->FreeInst();
		return *retDB;
	}



};



//-----------------------------------------------------------------------------
struct SafeCallEvent
{
	void operator()(std::function<void()>& method, wxCommandEvent& evt)const
	{
		try
		{
			if (method)
				method();
		}//try
		catch (boost::exception & e)
		{
			whDataMgr::GetDB().RollBack();
			wxLogWarning(wxString(diagnostic_information(e)));
		}///catch(boost::exception & e)
		catch (...)
		{
			wxLogWarning(wxString("Unhandled exception"));
		}//catch(...)
	};

};








#endif // __GLOBALDATA_H