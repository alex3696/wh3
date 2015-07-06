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
	m_Connection.Close();
}
//------------------------------------------------------------------------------
//void whDB::ShowError(const DatabaseLayerException& err)const
//{
//	
//}

//------------------------------------------------------------------------------
bool whDB::Open(const wxString& strServer, int nPort, const wxString& strDatabase, 
				const wxString& strUser, const wxString& strPassword)
{
	bool result = false;
	try
	{
		result = m_Connection.Open(strServer,nPort,strDatabase,strUser,strPassword);
	}
   	catch(DatabaseLayerException & e)
	{
		wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
		wxMessageBox(str );
	}
	return result;
}
//------------------------------------------------------------------------------
whTable*	whDB::ExecWithResults(const wxString& query)
{
	whTable* table=new whTable(this);
	if(table->Exec(query))
	{
		delete table;
		table = NULL;
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
		//m_Connection->CloseResultSet(m_Result);
		m_Result=NULL;
	}
}
//------------------------------------------------------------------------------
int whTable::Exec(const wxString& query,bool with_result)
{
	int result=0;
	try
	{
		Close();
		if(with_result)
			m_Result=m_DB->GetConn()->RunQueryWithResults(query); 
		else
			result=!m_DB->GetConn()->RunQuery(query, false);
		//m_Result = m_DB->GetConn()->RunQueryWithResults(query);
		//if (!with_result)
		//	Close();
	}
	catch(DatabaseLayerException & e)
	{
		result = e.GetErrorCode();
		auto estr = e.GetErrorMessage().GetData().AsString();

		if (result != 0 || !estr.IsEmpty() )
		{
			wxString str = wxString::Format(("%d %s"), result, estr);
			wxMessageBox(str);
		}
	}

	return result;	
}
//------------------------------------------------------------------------------
int whTable::Exec(const wxString& query,std::deque<wxString>& pathes)
{
	int result=0;
	try
	{
		Close();
		PreparedStatement* pStatement = m_DB->GetConn()->PrepareStatement(query);

		int err = m_DB->GetConn()->GetErrorCode();

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

					err = m_DB->GetConn()->GetErrorCode();

					if (nDataLength == nBytesRead)
						err = 0x22000;
				
					if( 0 == err )
						pStatement->RunQuery();
				}

				err=m_DB->GetConn()->GetErrorCode();
				m_DB->GetConn()->CloseStatement(pStatement);
			
			}
			
			
			
		}

	}
	catch(DatabaseLayerException & e)
	{
		wxString str = wxString::Format(("%d %s"), e.GetErrorCode(), e.GetErrorMessage().GetData() ); 
		wxMessageBox(str );
		result=1;
	}
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

