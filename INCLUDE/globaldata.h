#ifndef __GLOBALDATA_H
#define __GLOBALDATA_H

#include "_pch.h"
#include "whDB.h"
#include "ResManager.h"
#include "wxComboBtn.h"
#include "favorites.h"


class MainFrame;

namespace wh
{
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Config
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class Cfg
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

	
	}	mConnect;

	struct DbProp: public std::map<wxString,wxString>
	{
		BaseGroup	mBaseGroup = bgNull;
		void Load();
		//wxString FtpServer()	{	return (*this)[wxString("FTP server")];	}

	}	Prop;
};//class Cfg: public TSingletonSptr<Cfg>
//---------------------------------------------------------------------------

class Ftp
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

protected:

	wxFTP						mFtp;
	//std::shared_ptr<wh::Cfg>	mCfg;
	wh::Cfg*	mCfg;

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
class whDataMgr
{
private:
	whDataMgr() {};                   // Constructor? (the {} brackets) are needed here.
	whDataMgr(whDataMgr const&) = delete;
	void operator=(whDataMgr const&) = delete;
public:
	static whDataMgr* GetInstance()
	{
		static whDataMgr instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return &instance;
	}

	struct data_is_null: virtual exception_base { };

	whDB		mDb;
	wh::Cfg		mCfg;
	wh::Ftp		mFtp;

	MainFrame*					m_MainFrame = nullptr;
	wh::favorites::DataModel	mFavoritesModel;

	static whDB& GetDB()
	{	
		return GetInstance()->mDb;
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