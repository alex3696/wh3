#ifndef __GLOBALDATA_H
#define __GLOBALDATA_H

#include "_pch.h"
#include "whDB.h"
#include "ResManager.h"
#include "ioc_strcontainer.h"

//----------------------------------------------------------------------------
#define whID_MOVE				6300
#define whID_MOVE_HERE			6301
#define whID_ACTION				6302
//----------------------------------------------------------------------------
#define wxID_NEW_FOLDER			6000
#define wxID_NEW_TYPE			6001
#define wxID_NEW_OBJECT			6002
#define wxID_ADD_FAVORITES		6003
// ----------------------------------------------------------------------------
#define wxID_MKOBJ				6004
#define wxID_MKCLS				6005
// ----------------------------------------------------------------------------
#define whID_CATALOG_SELECT		6100
#define whID_CATALOG_PATH		6101
#define whID_CATALOG_TYPE		6102
#define whID_CATALOG_FAVORITE	6103


//----------------------------------------------------------------------------




class MainFrame;


namespace wh
{

class MConnectCfg;
class MDbCfg;
class RecentDstOidPresenter;

};//namespace wh
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Config
//---------------------------------------------------------------------------
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
	static void	DoCopyFile(wxInputStream* src,wxOutputStream* dst,const size_t src_size,const wxString& msg);
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

};
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// whDataMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** �������� ������ ���������� 
��� �������� �������� � �������� ��������� ������ ��������� ����� ����
*/
class whDataMgr
{
private:
	whDataMgr();
	~whDataMgr();
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

	wxLocale mLocale;

	whDB	mDb;
	Ftp		mFtp;
	
	std::shared_ptr<wh::MConnectCfg>	mConnectCfg;
	
	std::shared_ptr<wh::MDbCfg>			mDbCfg;

	std::unique_ptr<wh::RecentDstOidPresenter>	mRecentDstOidPresenter;

	static whDB& GetDB()
	{	
		return GetInstance()->mDb;
	}

	void InitContainer();

	std::shared_ptr<IOCStrContainer<wxString>>	mContainer
		= std::make_shared<IOCStrContainer<wxString>>();
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

//-----------------------------------------------------------------------------
struct SafeCallCommandEvent
{
	void operator()(const std::function<void(wxCommandEvent&)>& method, wxCommandEvent& evt)const
	{
		try
		{
			if (method)
				method(evt);
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
	}
	
	void operator()(std::function<void(wxCommandEvent&)>& method, wxCommandEvent& evt)const
	{
		try
		{
			if (method)
				method(evt);
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
//-----------------------------------------------------------------------------





#endif // __GLOBALDATA_H