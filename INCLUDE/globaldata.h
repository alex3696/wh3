#ifndef __GLOBALDATA_H
#define __GLOBALDATA_H

#include "_pch.h"
#include "whDB.h"
#include "ResManager.h"
#include "wxComboBtn.h"

//----------------------------------------------------------------------------
#define whID_MOVE				5300
#define whID_MOVE_HERE			5301
#define whID_ACTION				5302
//----------------------------------------------------------------------------
#define wxID_NEW_FOLDER			1000
#define wxID_NEW_TYPE			1001
#define wxID_NEW_OBJECT			1002
#define wxID_ADD_FAVORITES		1003
// ----------------------------------------------------------------------------
#define wxID_MKOBJ				1004
#define wxID_MKCLS				1005
// ----------------------------------------------------------------------------
#define whID_CATALOG_SELECT		1100
#define whID_CATALOG_PATH		1101
#define whID_CATALOG_TYPE		1102


//----------------------------------------------------------------------------




class MainFrame;


namespace wh
{

class MConnectCfg;
class MDbCfg;


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
	whDataMgr(whDataMgr const&) = delete;
	void operator=(whDataMgr const&) = delete;

	boost::signals2::scoped_connection mSSC_AfterDbConnected;
	boost::signals2::scoped_connection mSSC_BeforeDbDisconnected;
public:
	static whDataMgr* GetInstance()
	{
		static whDataMgr instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return &instance;
	}

	struct data_is_null: virtual exception_base { };

	whDB	mDb;
	Ftp		mFtp;
	
	std::shared_ptr<wh::MConnectCfg>	mConnectCfg;
	
	std::shared_ptr<wh::MDbCfg>			mDbCfg;
	

	MainFrame*					m_MainFrame = nullptr;

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