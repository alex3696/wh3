#include "_pch.h"
#include "globaldata.h"


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// whDB
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
whDB::whDB()
{

}
//------------------------------------------------------------------------------
whDB::~whDB()
{
	Close();
}
//------------------------------------------------------------------------------
void whDB::BeginTransaction()	
{ 
	wxLogMessage("whDB::BeginTransaction");
	try
	{
		m_Connection.BeginTransaction();
	}
	catch (DatabaseLayerException & e)
	{
		wxString str = wxString::Format("whDB::BeginTransaction\t %d %s"
			, e.GetErrorCode(), e.GetErrorMessage().GetData());
		wxLogError(str);
		throw;
	}
	catch (...)
	{
		wxString str = wxString::Format("whDB::BeginTransaction\t unknown error");
		wxLogError(str);
		throw;
	}

}
//------------------------------------------------------------------------------
void whDB::Commit()
{ 
	wxLogMessage("whDB::Commit");
	try
	{
		m_Connection.Commit();
		
	}
	catch (DatabaseLayerException & e)
	{
		wxString str = wxString::Format("whDB::Commit\t %d %s"
			, e.GetErrorCode(), e.GetErrorMessage().GetData());
		wxLogError(str);
		throw;
	}
	catch (...)
	{
		wxString str = wxString::Format("whDB::Commit\t unknown error");
		wxLogError(str);
		throw;
	}
}
//------------------------------------------------------------------------------
void whDB::RollBack()
{ 
	wxLogMessage("whDB::RollBack");
	try
	{
		m_Connection.RollBack();
	}
	catch (DatabaseLayerException & e)
	{
		wxString str = wxString::Format("whDB::RollBack\t %d %s"
			, e.GetErrorCode(), e.GetErrorMessage().GetData());
		wxLogError(str);
		throw;
	}
	catch (...)
	{
		wxString str = wxString::Format("whDB::RollBack\t unknown error");
		wxLogError(str);
		throw;
	}
}

//------------------------------------------------------------------------------
bool whDB::Open(const wxString& strServer, int nPort, const wxString& strDatabase, 
				const wxString& strUser, const wxString& strPassword)
{
	wxLogMessage("whDB::Open");
	bool result = false;
	try
	{
		Close();
		result = m_Connection.Open(strServer,nPort,strDatabase,strUser,strPassword);
		SigAfterConnect(*this);
	}
   	catch(DatabaseLayerException & e)
	{
		wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
		wxLogError(str);
	}
	catch (...)
	{
		wxString str = wxString::Format("whDB::Open\t unknown error");
		wxLogError(str);
	}
	return result;
}
//------------------------------------------------------------------------------
bool whDB::Close()
{ 
	wxLogMessage("whDB::Close");
	bool successfull = false;
	try
	{
		if (IsOpen())
			SigBeforeDisconnect(*this);
		successfull = m_Connection.Close();

	}
	catch (DatabaseLayerException & e)
	{
		wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData());
		wxLogError(str);
	}
	catch (...)
	{
		wxString str = wxString::Format("whDB::Open\t unknown error");
		wxLogError(str);
	}
	return successfull;
}

//------------------------------------------------------------------------------
whTable*	whDB::ExecWithResults(const wxString& query)
{
	whTable* table = nullptr;
	try
	{
		table = new whTable(this);
		if (table->Exec(query))
		{
			delete table;
			table = NULL;
		}
	}
	catch (...)
	{
		delete table;
		table = nullptr;
		wxString str = wxString::Format("whDB::ExecWithResults\t unknown error");
		wxLogError(str);
		throw;
	}

	return table;
}
//------------------------------------------------------------------------------
whTable_shared_ptr	whDB::ExecWithResultsSPtr(const wxString& query)
{
	return whTable::shared_ptr(ExecWithResults(query));
}

//------------------------------------------------------------------------------
int	whDB::Exec(const wxString& query, bool with_result)
{
	whTable* table=new whTable(this);
	int result = table->Exec(query, with_result);
	delete table;
	return result;

}
//------------------------------------------------------------------------------
int	whDB::Exec(const wxString& query,whTable* table)
{
	if(table)
	{
		table->Close();
		table->SetDB(this);
		return table->Exec(query,true);
	}
	table=new whTable(this);
	int result = table->Exec(query,false); 
	delete table;
	return result;
}
//------------------------------------------------------------------------------
whTable* whDB::Exec(const wxString& query,std::deque<wxString>& pathes)
{
	whTable* table=new whTable(this);
	if(table->Exec(query,pathes))
	{
		delete table;
		table = NULL;
	}
	return table;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
whTable::whTable(whDB* db)
	:m_DB(db),m_Result(NULL)//,m_Connection(NULL)
{
	SetDB(db);
}
//------------------------------------------------------------------------------
whTable::~whTable()
{
	Close();
}
//------------------------------------------------------------------------------
void whTable::Close()
{
	if(m_DB && m_Result)
	{
		m_DB->GetConn()->CloseResultSet(m_Result);
		m_Result=NULL;
	}
}
//------------------------------------------------------------------------------
int whTable::Exec(const wxString& query,bool with_result)
{
	
	auto p1 = GetTickCount();

	int result=0;
	try
	{
		Close();
		if(with_result)
			m_Result=m_DB->GetConn()->RunQueryWithResults(query); 
		else
			result=!m_DB->GetConn()->RunQuery(query, false);
	}
	catch(DatabaseLayerException & e)
	{
		result = e.GetErrorCode();
		auto estr = e.GetErrorMessage().GetData().AsString();

		if ((result != 2 && !with_result) || !estr.IsEmpty() )
		{
			wxString str = query;
			str.Replace("%", "?");
			wxLogError(str);
			str = wxString::Format(("%d %s"), result, estr);
			wxLogError(str);
			m_DB->RollBack();
			throw;
		}
		else
			result = 0;

	}
	catch (...)
	{
		wxString str = wxString::Format("whTable::Exec\t unknown error");
		wxLogError(str);
		throw;
	}
	
	auto p2 = GetTickCount();

	wxString logStr;
	logStr << "whTable::Exec\t" << p2 - p1 << "\t" << query;
	logStr.Replace("%", "?");
	wxLogMessage(logStr);

	return result;	
}
//------------------------------------------------------------------------------
int whTable::Exec(const wxString& query,std::deque<wxString>& pathes)
{
	auto p1 = GetTickCount();

	int result=0;
	try
	{
		Close();
		auto conn = m_DB->GetConn();

		
		PreparedStatement* pStatement = conn->PrepareStatement(query);

		int err = conn->GetErrorCode();

		typedef std::deque<wxString>::size_type size_type;

		if ( 0==err 
			 && pStatement 
			 && (size_type)pStatement->GetParameterCount() == pathes.size() )
		{
			for(unsigned int i=0;i<pathes.size();i++)
			{
				//wxFile* file=new wxFile;
				//bool opened = file->Open(wxString("c:\\SAV_DOC\\VS\\Projects\\redist\\BUILD\\Release\\setings.conf"));
				
				wxFile FileIn;//=new wxFile;
				if( FileIn.Open(pathes[i]) )
				{
					int nDataLength = FileIn.Length();

					wxMemoryBuffer BufferIn;
					int nBytesRead = FileIn.Read(BufferIn.GetWriteBuf(nDataLength), nDataLength);
					pStatement->SetParamBlob(i+1, BufferIn.GetData(), nDataLength);

					err = conn->GetErrorCode();

					if (nDataLength == nBytesRead)
						err = 0x22000;
				
					if( 0 == err )
						pStatement->RunQuery();
				}

				err = conn->GetErrorCode();
				conn->CloseStatement(pStatement);
			
			}
			
			
			
		}

	}
	catch(DatabaseLayerException & e)
	{
		wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
		wxLogError(query);
		wxLogError(str);
		m_DB->RollBack();
		throw;
	}

	auto p2 = GetTickCount();
	wxLogMessage("whTable::ExecPathes\t%d\t%s", p2 - p1, query);

	return result;	
}
//------------------------------------------------------------------------------
unsigned int whTable::GetColumnCount()const
{
	unsigned int count=0;
	ResultSetMetaData* md(NULL);
	if(m_Result)
	{
		try
		{
			md = m_Result->GetMetaData();
			count = md->GetColumnCount();
			m_Result->CloseMetaData(md);
		}
		catch(DatabaseLayerException & e)
		{
			wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
			wxMessageBox(str );
			count=0;
		}
		
	}
	return count;
}
//------------------------------------------------------------------------------
wxString whTable::GetColumnName(unsigned int col)
{
	wxString colname;
	ResultSetMetaData* md(NULL);
	if(m_Result)
	{
		try
		{
			md = m_Result->GetMetaData();
			colname = md->GetColumnName(col+1);
			
			m_Result->CloseMetaData(md);
		}
		catch(DatabaseLayerException & e)
		{
			wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
			wxMessageBox(str );
			colname=wxEmptyString;
		}
		
	}
	return colname;
}
//------------------------------------------------------------------------------
int whTable::GetColumnType(int col)
{
	int coltype=-1;
	ResultSetMetaData* md(NULL);
	if (m_Result)
	{
		try
		{
			md = m_Result->GetMetaData();
			coltype = md->GetColumnType(col + 1);

			m_Result->CloseMetaData(md);
		}
		catch (DatabaseLayerException & e)
		{
			wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData());
			wxMessageBox(str);
			coltype = -1;
		}

	}
	return coltype;
}

//------------------------------------------------------------------------------
unsigned int whTable::GetRowCount()const
{
	unsigned int  row_count=0;
	if(m_Result)
	{
		row_count = m_Result->GetRecQty();
	}

	return row_count;
}
//------------------------------------------------------------------------------	
wxString whTable::GetAsString(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultString(col+1,row);
	return wxEmptyString;
}
//------------------------------------------------------------------------------	
void whTable::GetAsString(unsigned int col,unsigned int row,wxString& result)
{
	if(m_Result)
		m_Result->GetResultString(++col,row,result);
	else
		result.Clear();
}
//------------------------------------------------------------------------------	
int whTable::GetAsInt(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultInt(col+1,row);
	return -1;
}


//------------------------------------------------------------------------------	
wxDateTime whTable::GetAsDate(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultDate(col+1,row);
	return wxDateTime();
}
//------------------------------------------------------------------------------	
double whTable::GetAsDouble(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultDouble(col+1,row);
	return 0.0;
}

//------------------------------------------------------------------------------	
long whTable::GetAsLong(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultLong(col+1,row);
	return 0;
}
//------------------------------------------------------------------------------	
bool whTable::GetAsBool(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->GetResultBool(col+1,row);
	return false;
}
//------------------------------------------------------------------------------	
wxMemoryBuffer whTable::GetAsBlob(unsigned int col,unsigned int row)
{
	if(m_Result)
	{
		wxMemoryBuffer BufferOut;
		//int sz= BufferOut.GetDataLen();
		//void* pBlob = m_Result->GetResultBlob(col+1, row,BufferOut);
		m_Result->GetResultBlob(col+1, row,BufferOut);
		return BufferOut;
	}
	return wxMemoryBuffer();
}
//------------------------------------------------------------------------------	
bool whTable::IsFieldNull(unsigned int col,unsigned int row)
{
	if(m_Result)
		return m_Result->IsFieldNull(col+1,row);
	return true;
}

