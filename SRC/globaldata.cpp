#include "_pch.h"
#include "globaldata.h"
#include "MainFrame.h"
#include "config.h"

using namespace wh;
//using namespace std;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// wh::Ftp
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Ftp::Ftp()
{

}



void Ftp::DoConnect()
{
	const rec::FtpCfg& ftp_cfg_data = whDataMgr::GetInstance()->mDbCfg->mFtpCfg->GetData();

	if(mFtp.IsDisconnected() )
	{
		const wxString& server = ftp_cfg_data.mServer;
		const wxString& user = ftp_cfg_data.mUser;
		const wxString& pass = ftp_cfg_data.mPass;
	
		mFtp.SetUser	(user);
		mFtp.SetPassword(pass);

		wxBusyCursor bc;
		wxProgressDialog progress("connecting",wxEmptyString);
		progress.Pulse("Waiting connection FTP \n"
						"server "+server+"\n"
						"user   "+user+"\n"
						"pass   ***");
		if ( !mFtp.Connect(server) )
		{
			mFtp.Close();
			wxString errmsg = wxString::Format("Can`t connect to FTP server '%s' user '%s' pass '%s'",server,user,pass);
			BOOST_THROW_EXCEPTION( ftp_error_connect()<<wxstr(errmsg) );
		}
	}
}
//---------------------------------------------------------------------------

void Ftp::DoChDir(const wxFileName& ftpfile, bool make_dirs)
{
	DoConnect();

	try
	{
		const wxString curr_path=mFtp.Pwd();
		if(curr_path == ftpfile.GetPath(false,wxPATH_UNIX))
			return;

		if(!mFtp.ChDir("/") )
			BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Can`t change directory to '/'") );

		const wxArrayString& dirs=ftpfile.GetDirs();
		for(auto it=dirs.begin();it!=dirs.end();++it)
		{
			const wxString& dirname=*it;
			if(!mFtp.ChDir(dirname) )
			{
				if(make_dirs)
				{
					if(! mFtp.MkDir(*it) )
						BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Can`t find make directory '"+ftpfile.GetPath(0,wxPATH_UNIX)+"'") );
					else
						if(!mFtp.ChDir(*it))
							BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Can`t find make ftp directory '"+ftpfile.GetPath(0,wxPATH_UNIX)+"'") );
				}
				else// if(make_dirs)
					BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Can`t find make ftp directory '"+ftpfile.GetPath(0,wxPATH_UNIX)+"'") );
			}//if(!mFtp.ChDir(*it) )
		}//for
	}
	catch(...)// boost::exception & e 
    {
		mFtp.Close();
		throw;
	}

}

//---------------------------------------------------------------------------

void Ftp::Remove(const wxFileName& ftpfile)
{
	DoConnect();
	DoChDir(ftpfile);
	
	if( !mFtp.FileExists( ftpfile.GetFullName() ))
		BOOST_THROW_EXCEPTION( ftp_not_exists()<<wxstr("File not exist: "+ftpfile.GetFullPath(wxPATH_UNIX)) );


	if (!mFtp.RmFile(ftpfile.GetFullName() ))
	{
		mFtp.Close();
		BOOST_THROW_EXCEPTION( ftp_error_remove()<<wxstr("Can`t remove file "+ftpfile.GetFullPath(wxPATH_UNIX)) );
	}

}//void Ftp::Remove(const wxFileName& file)
//---------------------------------------------------------------------------

void Ftp::Upload(const wxFileName& localfile,const wxFileName& ftpfile)
{
	wxOutputStream* ftp_stream=NULL;

	DoConnect();
	try
	{
		wxFileInputStream	local_stream(localfile.GetFullPath() );
		if(!local_stream.IsOk() )
			BOOST_THROW_EXCEPTION( ftp_error_upload()<<wxstr("Can`t read local file "+localfile.GetFullPath() ) );

		DoChDir(ftpfile);
		if( mFtp.FileExists( ftpfile.GetFullName() ))
		{
			if(wxNO ==wxMessageBox(ftpfile.GetFullName()+"\n file already existsis,owerwrite ?\n No - abort operation","Warning",wxYES_NO))
				BOOST_THROW_EXCEPTION( ftp_error_upload()<<wxstr("File already exists "+ftpfile.GetFullName() ) );
		}
			

		ftp_stream = mFtp.GetOutputStream(ftpfile.GetFullPath(wxPATH_UNIX) );
		if(!ftp_stream )
			BOOST_THROW_EXCEPTION( ftp_error_upload()<<wxstr("Can`t open upload stream '"+ftpfile.GetFullPath(wxPATH_UNIX)+"'") );

		const unsigned int local_size=local_stream.GetSize();
		wxString msg =	wxString::Format(	"Uploading to FTP \n\n"
											"%s (size %d)\n"
											"to\n"
											"%s ",
											localfile.GetFullPath(),local_size,
											ftpfile.GetFullPath(wxPATH_UNIX));
		DoCopyFile(&local_stream,ftp_stream,local_size,msg);
		
	}
	catch(...)// boost::exception & e 
    {
		mFtp.Abort();
		delete ftp_stream;
		mFtp.Close();
		throw;
	}
	delete ftp_stream;
	


	
}

//---------------------------------------------------------------------------
void Ftp::Download(const wxFileName& localfile,const wxFileName& ftpfile)
{
	wxInputStream* ftp_stream(NULL);

	DoConnect();
	DoChDir(ftpfile,false);

	try
	{
		int	ftp_size=-1;
		if(ftpfile.GetFullName()!=wxEmptyString)
		{
			ftp_size = mFtp.GetFileSize( ftpfile.GetFullName() );
			if(-1 == ftp_size )
				BOOST_THROW_EXCEPTION( ftp_error_download()<<wxstr("File size error? may be not exists on FTP '"+ftpfile.GetFullPath(wxPATH_UNIX)+"'") );
		}
		else
			BOOST_THROW_EXCEPTION( ftp_error_download()<<wxstr("File name is empty '"+ftpfile.GetFullPath(wxPATH_UNIX)+"'") );
		

		ftp_stream = mFtp.GetInputStream(ftpfile.GetFullName());
		if(!ftp_stream || !ftp_stream->IsOk() || !ftp_stream->CanRead() )
			BOOST_THROW_EXCEPTION( ftp_error_download()<<wxstr("File not found on FTP '"+ftpfile.GetFullPath(wxPATH_UNIX)+"'") );

		wxFileOutputStream	local_stream( localfile.GetFullPath()  );
		if(!local_stream.IsOk() )
			BOOST_THROW_EXCEPTION( ftp_error_download()<<wxstr("can`t create local temp file '"+localfile.GetFullPath()+"'") );
		
		wxString msg =	wxString::Format(	"Downloading from FTP \n\n"
											"%s (size %d)\n"
											"to\n"
											"%s ",
											ftpfile.GetFullPath(wxPATH_UNIX),ftp_size,
											localfile.GetFullPath());
		DoCopyFile(ftp_stream,&local_stream,ftp_size,msg);
		delete ftp_stream;	
	}
	catch(...)//boost::exception & e)  
    {
		mFtp.Abort();
		delete ftp_stream;
		mFtp.Close();
		throw;
    }
	
	



}
//---------------------------------------------------------------------------

void Ftp::DoCopyFile(wxInputStream* src,wxOutputStream* dst,const size_t src_size,const wxString& msg)
{
	if (MAXINT == src_size )
		BOOST_THROW_EXCEPTION(ftp_error_io() << wxstr(wxString::Format("Error src_size=%d", MAXINT)));
	if (!src || !dst )
		BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr(wxString::Format("Error while copying stream src=%d dst=%d",src,dst)) );

	#define CHUNK_SIZE	4096
	size_t	dst_size=0;
	char	buf[CHUNK_SIZE];

	wxProgressDialog progress("File copying",msg+"\n", 100 ,NULL,wxPD_AUTO_HIDE|wxPD_APP_MODAL|wxPD_CAN_ABORT );		  
	
	while( dst_size < src_size )
	{
		const wxInputStream& r=src->Read(buf, CHUNK_SIZE);

		if(r.GetLastError()!=wxSTREAM_NO_ERROR )
			if(r.GetLastError()!=wxSTREAM_EOF )
				BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Error src stream") );

		size_t cur_chunk = r.LastRead();

		const wxOutputStream& w = dst->Write(buf,cur_chunk );
			
		if(w.GetLastError()!=wxSTREAM_NO_ERROR )
			BOOST_THROW_EXCEPTION( ftp_error_io()<<wxstr("Error dst stream") );
			
		cur_chunk = (w.LastWrite()==cur_chunk ) ? cur_chunk : 0 ;
		dst_size+=cur_chunk;
			
		//mgr->m_MainFrame->SetTitle( wxString::Format("%s ( %d )",msg,dst_size) );

		size_t p = ((double)dst_size/src_size)*100;
			
		if( p > (size_t)progress.GetValue() )
			progress.Update(p, wxString::Format("%s ( %d )",msg,dst_size)); 
		if(progress.WasCancelled() )
			BOOST_THROW_EXCEPTION( ftp_cancel_io()<<wxstr("Warning cancel copying rollback transaction") );
			
	}//while



}
//---------------------------------------------------------------------------

void Ftp::Rename(const wxFileName& ftpfile,const wxString& new_name)
{
	DoConnect();

	DoChDir(ftpfile);

	if( !mFtp.Rename( ftpfile.GetFullName(),new_name) )
	{
		mFtp.Close();
		BOOST_THROW_EXCEPTION( ftp_error_rename()<<wxstr("Can`t rename ftp file '"+ftpfile.GetFullPath(wxPATH_UNIX)+"'") );
	}
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// whDataMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

whDataMgr::whDataMgr()
	:mConnectCfg(new wh::MConnectCfg())
	, mDbCfg(new wh::MDbCfg())
{
	rec::ConnectCfg  default_connect_cfg;
	mConnectCfg->SetData(default_connect_cfg);

	whDB::Slot on_connect = [this](const whDB* const)
	{
		this->mDbCfg->Load();
	};
	
	mSSC_AfterDbConnected = mDb.SigAfterConnect.connect(on_connect);


	whDB::Slot on_disconnect = [this](const whDB* const db)
	{
		if (db && db->IsOpen())
			this->mDbCfg->Save();
	};

	mSSC_BeforeDbDisconnected = mDb.SigBeforeDisconnect.connect(on_disconnect);

}