#include "_pch.h"
#include "globaldata.h"
#include "MainFrame.h"
#include "whPnlCtrl_MovHistory.h"

using namespace wh;
//using namespace std;


//---------------------------------------------------------------------------

void Cfg::DbConnect::Load()
{
	wxFileName		local_cfg_file = wxFileConfig::GetLocalFile ("wh3.ini");;
	


	if(!local_cfg_file.FileExists() )//если пользовательского конфига нет 
	{
		wxFileName		vendor_cfg_file("wh3.ini");
		if(vendor_cfg_file.FileExists())//ищем глобальный который рядом с экзешкой		
		{
			if(!wxCopyFile("wh3.ini", local_cfg_file.GetFullPath() ) ) // копируем как пользовательский
				return;
		}
		else
			return;
	}

	wxFileConfig cfg("wh3","alex3696@ya.ru",wxEmptyString,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
		
	cfg.Read("DbConnect/Server",&mServer);
	cfg.Read("DbConnect/Db",	&mDB);
	cfg.Read("DbConnect/Port",	&mPort);
	cfg.Read("DbConnect/User",	&mUser);
	cfg.Read("DbConnect/Pass",	&mPass);
	cfg.Read("DbConnect/Role",	&mRole);

}//void Load()
//---------------------------------------------------------------------------

void Cfg::DbConnect::Save()
{
	wxFileConfig cfg("wh3","alex3696@ya.ru",wxEmptyString,wxEmptyString,wxCONFIG_USE_LOCAL_FILE);
		
	cfg.Write("DbConnect/Server",mServer);
	cfg.Write("DbConnect/Db",	mDB);
	cfg.Write("DbConnect/Port", mPort);
	cfg.Write("DbConnect/User", mUser);
	cfg.Write("DbConnect/Pass", mPass);
	cfg.Write("DbConnect/Role", mRole);
	cfg.Flush();

}//void Save()


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Other Config Data
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void Cfg::DbProp::Load()
{
	wxString query =
		"SELECT  prop.id    AS prop_id"
		" , prop.title AS prop_title"
		" , prop.kind  AS prop_kind"
		" , val"
		" , prop_cls.id AS id"
		" FROM prop_cls"
		" LEFT JOIN prop ON prop.id = prop_cls.prop_id"
		" WHERE prop_cls.cls_id = 2";
		
	auto table= whDataMgr::GetDB().ExecWithResultsSPtr (query);
	if(table)
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

	
}//void Load()
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// wh::Ftp
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Ftp::Ftp()
	: mCfg(&whDataMgr::GetInstance()->mCfg)
{}



void Ftp::DoConnect()
{
	if(mFtp.IsDisconnected() )
	{
		const wxString& server=mCfg->Prop["FTP server"];
		const wxString& user=mCfg->Prop["FTP username"];
		const wxString& pass=mCfg->Prop["FTP password"];
	
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
	if(!src || !dst && src_size<=MAXINT )
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