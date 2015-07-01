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
int	whDB::Exec(const wxString& query)
{
	whTable* table=new whTable(this);
	int result = table->Exec(query,false); 
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
		//if(with_result)
		//	m_Result=m_DB->GetConn()->RunQueryWithResults(query); 
		//else
		//	result=!m_DB->GetConn()->RunQuery(query,false);
		m_Result = m_DB->GetConn()->RunQueryWithResults(query);
		if (!with_result)
			Close();

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


/*
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// запись в таблицце классов 
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void whClassRec::clear()
{
	m_LoadVal.clear();

	delete m_NewVal;
	m_NewVal=NULL;
}
//------------------------------------------------------------------------------
int whClassRec::LoadPerms()
{
	whSlotRefTableMI idxtable;
	
	for(whSlotRefTable::iterator it=m_Slot.begin();it!=m_Slot.end();++it)
		idxtable.push_back( *it );
	
	
	whDataMgr* mgr=whDataMgr::GetInstance();
	wxString query=wxString::Format("SELECT slotclass,permid,rolname,permtype,movobj,slotownerobj,srcobj_path,dstobj_path \
									FROM wh_perm \
									WHERE slotownerclass='%s' FOR UPDATE NOWAIT ",GetName() );
	whTable* table=mgr->m_DB.ExecWithResults(query);
	if(table)
	{
		for(unsigned int i=0;i<table->GetRowCount();++i)
		{
			whSlotRefTableMI::idx_slottype::iterator it = idxtable.get<1>().find( table->GetAsString(0,i)) ;

			std::tr1::shared_ptr<whSlotRec> slot = *it;
			
			int			id=table->GetAsInt(1,i);
			wxString	group=table->GetAsString(2,i);
			int			perm_type=table->GetAsInt(3,i);
	
			wxString	MovObjName=table->GetAsString(4,i);
			wxString	DstObjName=table->GetAsString(5,i);

			wxString	src_path=table->GetAsString(6,i);
			wxString	dst_path=table->GetAsString(7,i);
			
			std::tr1::shared_ptr<whPermRec>  rec=slot->AppendPerm(id,group,perm_type,MovObjName,DstObjName,src_path,dst_path);



		}//for(unsigned int i=0;i<table->GetRowCount();++i)
		delete table;
	}//if(table)
	mgr->FreeInst();


	return 0;
}
//------------------------------------------------------------------------------
int whClassRec::LoadSlots()
{
	int ret_code=0;

	m_Slot.clear();// очищаем всё что было

	whDataMgr* mgr=whDataMgr::GetInstance();
	wxString query=wxString::Format("SELECT slotclass,slotminqty,slotmaxqty FROM wh_slot \
									WHERE slotownerclass='%s' FOR UPDATE NOWAIT ",GetName() );
	whTable* table=mgr->m_DB.ExecWithResults(query);
		
	if(table) // загружаем табличку слотов
	{
		for(unsigned int i=0;i<table->GetRowCount();++i)
		{
			//wxString owner = m_Class->GetName();
			wxString type = table->GetAsString(0,i);
			wxString min=table->GetAsString(1,i);

			wxString max="NULL";
			if(!table->IsFieldNull(2,i) )
				max=table->GetAsString(2,i);

			std::tr1::shared_ptr<whSlotRec> newslot=AppendSlot(type,max,min);
			if (!newslot )
				ret_code++;
		}//for(unsigned int i=0;i<table->GetRowCount();++i)
		delete table;
	}//if(table)
	mgr->FreeInst();
	return ret_code;	
}
//------------------------------------------------------------------------------
int whClassRec::LoadByClassId(int class_id)	
{ 
	int ret_code=0;
	//m_Class->clear();// очищаем всё что было
	whDataMgr* mgr=whDataMgr::GetInstance();
	wxString query=wxString::Format("SELECT classid, classname, classparent, classdesc, measurename \
										FROM wh_class \
										WHERE classid=%d AND classdeleted=0 FOR UPDATE NOWAIT ",class_id );

	mgr->m_DB.BeginTransaction();
	whTable* table=mgr->m_DB.ExecWithResults(query);
	if(table && table->GetRowCount() ) // загружаем табличку слотов
	{
		m_LoadVal.m_Id =		table->GetAsInt(0,0) ;
		m_LoadVal.m_Name =		table->GetAsString(1,0);
		m_LoadVal.m_ParentName =table->GetAsString(2,0);
		m_LoadVal.m_Desc =		table->GetAsString(3,0);
		m_LoadVal.m_Measure=	table->GetAsString(4,0);
		ret_code=0;
	}
	else
		ret_code=1;
	delete table;

	if(!ret_code)
		ret_code = LoadSlots();

	if(!ret_code)
		ret_code = LoadPerms();

	mgr->m_DB.Commit();
	mgr->FreeInst();

	m_ClassPropTable.Init(class_id);

	return ret_code; 
}
//------------------------------------------------------------------------------
int whClassRec::UpdateClassById()
{
	int ret_code=0;

	if(m_NewVal)
	{
		wxString query;

		if(m_NewVal->m_Name.size() && m_LoadVal.m_Name!=m_NewVal->m_Name )
			query += wxString::Format("classname='%s'", m_NewVal->m_Name );

		if(m_NewVal->m_ParentName.size() && m_LoadVal.m_ParentName!=m_NewVal->m_ParentName )
		{
			if(query.size() )
				query+=",";
			query += wxString::Format("classparent='%s'", m_NewVal->m_ParentName );
		}

		if(m_LoadVal.m_Measure!=m_NewVal->m_Measure )
		{
			if(query.size() )
				query+=",";

			if( m_NewVal->m_Measure.size() )
			{
				if("ед."==m_NewVal->m_Measure)
					query += "measurename='ед.',classtype=1";
				else
					query += wxString::Format("measurename='%s',classtype=2", m_NewVal->m_Measure );
			}
			else
			{
				query += "measurename=NULL,classtype=0";
			}
		}


		if(m_LoadVal.m_Desc!=m_NewVal->m_Desc )
		{
			if(query.size() )
				query+=",";

			if(m_NewVal->m_Desc.size() )
				query += wxString::Format("classdesc='%s'", m_NewVal->m_Desc );
			else
				query += "classdesc=NULL";
		}
	
		if(query.size() )
		{
			whDataMgr* mgr=whDataMgr::GetInstance();
			query= wxString::Format("UPDATE wh_class SET %s WHERE classid=%d",query,m_LoadVal.m_Id);
			ret_code=mgr->m_DB.Exec(query,false);
			mgr->FreeInst();		
		}//if(query.size() )
	
	}//if(m_NewVal)
	
	return ret_code; 
}
//------------------------------------------------------------------------------
int whClassRec::InsertClass()
{
	wxString mes;
	int classtype;

	if( m_NewVal->m_Measure.size()==0)
	{
		mes = "NULL";
		classtype=0;
	}
	else if(m_NewVal->m_Measure=="ед.")
	{
		mes = "'ед.'";
		classtype=1;
	}
	else
	{
		mes = "'"+m_NewVal->m_Measure+"'";
		classtype=2;
	}

	wxString classdesc;
	if(!m_NewVal->m_Desc.size())
		classdesc="NULL";
	else
		classdesc = wxString::Format("'%s'",m_NewVal->m_Desc);

	
	int ret_code=0;
	wxString query = "INSERT INTO wh_class(classname,classparent,measurename,classdesc,classtype)\
						VALUES('%s','%s',%s,%s,%d)";
	query = wxString::Format(query,m_NewVal->m_Name,m_NewVal->m_ParentName,mes,classdesc,classtype );
	whDataMgr* mgr=whDataMgr::GetInstance();
	ret_code=mgr->m_DB.Exec(query,false);
	mgr->FreeInst();


	return ret_code; 
}

//------------------------------------------------------------------------------
int whClassRec::Save()
{
	whDataMgr* mgr=whDataMgr::GetInstance();
	int ret_code=0;
	mgr->m_DB.BeginTransaction();

	if(m_LoadVal.m_Id==0)
		ret_code = InsertClass();
	else
	{
		if(GetName().size())
			ret_code = UpdateClassById() ;
		else
			ret_code = 1;
	}

	if(!ret_code)
		ret_code=SaveSlots();


	if(!ret_code)
		mgr->m_DB.Commit();
	else
		mgr->m_DB.RollBack();

	m_ClassPropTable.Save();



	mgr->FreeInst();
	return ret_code;
}
//------------------------------------------------------------------------------
int whClassRec::SaveSlots()
{
	whDataMgr* mgr=whDataMgr::GetInstance();
	int ret_code=0;
	
	wxString slotlist;
	// удаляем ненужные разрешения сравнивая
	for(whSlotRefTable::iterator it=m_DeletedSlot.begin();it!=m_DeletedSlot.end();++it)
	{
		std::tr1::shared_ptr<whSlotRec> slot = *it;
		slotlist+= wxString::Format("slotclass='%s' OR ",slot->GetSlotClassName() );
	}
	
	if(slotlist.size())
	{
		slotlist.RemoveLast(3);
		wxString query = wxString::Format("DELETE FROM wh_slot WHERE slotownerclass='%s' AND (%s)",GetName(),slotlist);
		ret_code=mgr->m_DB.Exec(query,false);

	}


	// создаём новые слоты
	for(whSlotRefTable::iterator it=m_Slot.begin();it!=m_Slot.end();++it)
	{
		std::tr1::shared_ptr<whSlotRec> slot = *it;
		ret_code += slot->Save();
		if(ret_code)
			break;
	}// for

	mgr->FreeInst();
	return ret_code;
}











//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// запись в таблицце слотов 
// class whSlotRec
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
wxString whSlotRec::GetOwnerClassName()const
{ 
	return m_OwnerClass.GetName();
}
//------------------------------------------------------------------------------
int whSlotRec::Save()
{
	whDataMgr* mgr=whDataMgr::GetInstance();
	int ret_code=0;
	
	if( (m_LoadVal.m_SlotClassName=="<>" || m_LoadVal.m_SlotClassName==wxEmptyString) && m_NewVal )
	{
		wxString query = wxString::Format("INSERT INTO wh_slot(slotownerclass, slotclass, slotmaxqty, slotminqty, classtype)\
											VALUES ('%s','%s', %s, %s, %d);",
											GetOwnerClassName(),
											GetSlotClassName(),
											GetMaxQty(),
											GetMinQty(),
											this->GetOwnerClass().GetClassType() );
		ret_code+=mgr->m_DB.Exec(query,false);
	}//IsNewVal()

	if( (m_LoadVal.m_SlotClassName!="<>" && m_LoadVal.m_SlotClassName!=wxEmptyString) && m_NewVal )
	{
		wxString update_query;
		update_query+= wxString::Format(" slotclass='%s'",GetSlotClassName() );
			
		if(update_query.size() )
			update_query+=",";
		update_query+= wxString::Format(" slotmaxqty=%s" ,GetMaxQty() );

		if(update_query.size() )
			update_query+=",";
		update_query+= wxString::Format(" slotminqty=%s" ,GetMinQty() );
			
		if(update_query.size() )
		{
			wxString query = wxString::Format("UPDATE wh_slot SET %s WHERE slotownerclass='%s' AND slotclass='%s' ",
												update_query,
												GetOwnerClassName(),
												m_LoadVal.m_SlotClassName );
			ret_code+=mgr->m_DB.Exec(query);
		}//if(update_query.size() )
	}

	if(!ret_code)
		ret_code = SavePerms();

	
	mgr->FreeInst();
	return ret_code;
}
//------------------------------------------------------------------------------
int whSlotRec::SavePerms()
{
	int ret_code=0;
	whDataMgr* mgr=whDataMgr::GetInstance();

	wxString permlist;
	// удаляем ненужные разрешения сравнивая
	for(whPermRefTable::iterator it=m_DeletedPerm.begin();it!=m_DeletedPerm.end();++it)
	{
		std::tr1::shared_ptr<whPermRec> perm = *it;
		permlist+= wxString::Format("permid=%d OR ",perm->GetId() );
	}
	
	if(permlist.size())
	{
		permlist.RemoveLast(3);
		wxString query = wxString::Format("DELETE FROM wh_perm WHERE %s",permlist);
		ret_code=mgr->m_DB.Exec(query,false);

	}

	// создаём новые разрешения
	for(whPermRefTable::iterator it=m_Perm.begin();it!=m_Perm.end();++it)
	{
		std::tr1::shared_ptr<whPermRec> perm = *it;
		ret_code += perm->Save();
		if(ret_code)
			break;
	}// for
	



	mgr->FreeInst();
	return ret_code;
}




//------------------------------------------------------------------------------
wxString whPermRec::GetSlotClassName()			
{	
	return m_OwnerSlot.GetSlotClassName();	
}

wxString whPermRec::GetSlotOwnerClassName()
{
	return m_OwnerSlot.GetOwnerClassName();
}

//------------------------------------------------------------------------------
int whPermRec::Save()
{
	int ret_code=0;
	whDataMgr* mgr=whDataMgr::GetInstance();

	if(m_NewVal && m_NewVal->m_Id==0)//insert new
	{
		wxString slotownerobj="NULL",movobj="NULL",src_path="NULL",dst_path="NULL";
		
		if(m_NewVal->m_DstObjName.size() )
			slotownerobj=m_NewVal->m_DstObjName;

		if(m_NewVal->m_MovObjName.size() )
			movobj=m_NewVal->m_MovObjName;

		if( GetSrcSize() )
		{
			GenerateSrcArray(src_path);
			src_path="'"+src_path+"'";
		}
		
		if( GetDstSize() )
		{
			GenerateDstArray(dst_path);
			dst_path="'"+dst_path+"'";
		}
		
		wxString query=wxString::Format(" INSERT INTO wh_perm \
										(rolname,permtype,slotownerclass,slotclass,slotownerobj,movobj,srcobj_path,dstobj_path )\
										VALUES\
										('%s'   ,%d      ,'%s'          ,'%s'     , %s         ,%s    ,%s         ,%s        )",
										m_NewVal->m_Group,
										m_NewVal->m_PermType,
										GetSlotOwnerClassName(),
										GetSlotClassName(),
										slotownerobj,
										movobj,
										src_path,
										dst_path );

		ret_code+=mgr->m_DB.Exec(query);
	}
	
	if(m_NewVal && m_LoadVal.m_Id!=0)//update
	{
		wxString slotownerobj="NULL",movobj="NULL",src_path="NULL",dst_path="NULL";
		
		if(m_NewVal->m_DstObjName.size() )
			slotownerobj="'"+m_NewVal->m_DstObjName+"'";

		if(m_NewVal->m_MovObjName.size() )
			movobj="'"+m_NewVal->m_MovObjName+"'";

		if( GetSrcSize() )
		{
			GenerateSrcArray(src_path);
			src_path="'"+src_path+"'";
		}
		
		if( GetDstSize() )
		{
			GenerateDstArray(dst_path);
			dst_path="'"+dst_path+"'";
		}
		
		
		wxString query=wxString::Format("UPDATE wh_perm SET \
										rolname='%s',\
										permtype=%d ,\
										slotownerclass='%s',\
										slotclass='%s',\
										slotownerobj= %s ,\
										movobj=%s,\
										srcobj_path=%s,\
										dstobj_path=%s \
										WHERE permid=%d",
										m_NewVal->m_Group,
										m_NewVal->m_PermType,
										GetSlotOwnerClassName(),
										GetSlotClassName(),
										slotownerobj,
										movobj,
										src_path,
										dst_path,
										m_NewVal->m_Id );	
		ret_code+=mgr->m_DB.Exec(query);
	
	}



	mgr->FreeInst();
	return ret_code;
}
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//whClassPropTable
//------------------------------------------------------------------------------
whClassPropTable::whClassPropTable()
	:mClassID(0)
	,mNewPrefix(wxString::Format("_new_%s_",wh::Cfg::Instance()->Db.mUser))
{
}
//------------------------------------------------------------------------------

void whClassPropTable::Load()
{
	if(!mClassID)
		return;
	this->clear();
	whDataMgr* mgr= whDataMgr::GetInstance();
	wxString query=wxString::Format("SELECT classid,prop_id, label, type, val, \"time\", editor \
									FROM t_whclass_prop WHERE classid=%d",
									mClassID);
		
	auto table=mgr->m_DB.ExecWithResultsSPtr(query);
	if(table)
	for(size_t i=0;i<table->GetRowCount();i++)
	{
		t_whclass_prop prop;
		prop.mClassID = table->GetAsString(0,i);
		prop.mPropID = table->GetAsString(1,i);

		prop.mLabel = table->GetAsString(2,i);
		prop.mType  = table->GetAsInt(3,i);
		prop.mVal   = table->GetAsString(4,i);
		prop.mTime  = table->GetAsString(5,i);
		prop.mEditor= table->GetAsString(6,i);

		PropModel* new_model=new PropModel(prop);
		new_model->SetState( msExist);
		insert(new_model);
	}
	mgr->FreeInst();
}
//------------------------------------------------------------------------------

void whClassPropTable::Save()
{
	DoRemoveTmpFiles();
		
	PropModelSet	to_upload;	// файлы для загрузки на FTP сервер -идентификатор+имя локального файла для выгрузки на FTP
	PropModelSet	to_del;	// перечень моделей для удаления
	FilesToDel		del_files;
	bool is_commited(false);

	whDataMgr::GetDB().BeginTransaction();
	try
	{
		for(auto it=begin();it!=end();++it)
		{
			 TModel<t_whclass_prop>*	model=*it;
			t_whclass_prop					prop = model->GetProperty();
			wxString query;

			switch( model->GetState() )
			{
			default:BOOST_THROW_EXCEPTION( prop_table_error()<<wxstr("unknown type") );break;
			case  msExist:		break;
			case  msCreated:	DoInsert(model,to_upload);			break;
			case  msDeleted:	DoRemove(model,to_del,del_files);	break;
			case  msUpdated:	DoUpdate(model,to_del,to_upload,del_files);	break;
			}//switch
		}//for(auto it=begin();it!=end();++it)
		whDataMgr::GetDB().Commit();
		is_commited=true;
	}
	catch(boost::exception & e)
	{
		whDataMgr::GetDB().RollBack();
		DoRollback(to_del,to_upload);
		wxLogWarning(wxString(diagnostic_information(e)) );
	}

	// если коммит нормально прошёл, то удаляем старые файлы
	if(is_commited)
		DoCommit(to_del,del_files);

}
//------------------------------------------------------------------------------

void whClassPropTable::DoRemoveTmpFiles()
{
	if(!mClassID)
		BOOST_THROW_EXCEPTION( prop_table_error()<<wxstr("ClassId not exists ") );

	// чистим файлы для удаления от прошлых сессий
	wxFileName ftp_file(wxString::Format("/%d/%s",mClassID,mNewPrefix));

	
	wh::Ftp::Instance()->DoChDir(ftp_file);
	wxArrayString files;
	if(wh::Ftp::Instance()->GetFilesList(files))
		for(auto it=files.begin();it!=files.end();++it)
		{
			const wxString& filename=*it;
			if(mNewPrefix.size()<filename.size() )
			{
				const wxString sub = filename.substr(0,mNewPrefix.size() );
				if(sub==mNewPrefix)
				{
					ftp_file.SetFullName(filename);
					wh::Ftp::Instance()->Remove(ftp_file);
				}//if(sub==del_prefix)
			}//if(del_prefix.size()<filename.size() )
		}//for(auto it=files.begin();it!=files.end();++it)

}

//------------------------------------------------------------------------------

void whClassPropTable::DoFtpPrepareDel(const PropModel* model,FilesToDel& del_files)
{
	const t_whclass_prop	prop = model->GetProperty();
	wxString		query=wxString::Format("SELECT val,type FROM t_whclass_prop WHERE prop_id=%s", prop.mPropID );
	auto table=whDataMgr::GetDB().ExecWithResultsSPtr(query);
	if(table)
	{
		wxString del_val = table->GetAsString(0,0);
		del_val.Trim(true);
		del_val.Trim(false);

		if(table->GetAsInt(1,0)==t_whclass_prop::propFile && del_val!=wxEmptyString )
		{
			del_files.push_back(del_val);
		}//if(table->GetAsString(0,2)==t_whclass_prop::propFile && del_val!=wxEmptyString )
	}//if(table)
	else
		BOOST_THROW_EXCEPTION( prop_table_error()<<wxstr("can`t get previos value from "+query) );
}
//------------------------------------------------------------------------------

void whClassPropTable::DoFtpPrepareUpload(const PropModel* model,PropModelSet& to_upload)
{
	t_whclass_prop	prop =	model->GetProperty();

	prop.mVal.Trim(true);
	prop.mVal.Trim(false);

	if(t_whclass_prop::propFile==prop.mType && wxEmptyString != prop.mVal)	// если это стало свойство-файл
	{
		// в значении модели должно быть имя НОВОГО файла
		wxFileName ftp_file(prop.mVal);
		ftp_file.SetFullName( mNewPrefix+ftp_file.GetFullName() );
		ftp_file.SetPath(wxString::Format("/%d",mClassID),wxPATH_UNIX);

		wxFileName local_file(prop.mVal);
		wh::Ftp::Instance()->Upload(local_file,ftp_file);
		prop.mVal = ftp_file.GetFullName();
		to_upload.insert(model);
	}//if(t_whclass_prop::propFile==prop.mType)	// если это стало свойство-файл
}
//------------------------------------------------------------------------------

void whClassPropTable::DoRemove(const PropModel* model,PropModelSet& models_to_del,FilesToDel& del_files)
{
	DoFtpPrepareDel(model,del_files);
	
	models_to_del.insert(model);
	
	t_whclass_prop	prop = model->GetProperty();
	wxString		query;
	query=wxString::Format("DELETE FROM t_whclass_prop WHERE prop_id=%s", prop.mPropID );
	if( 0 != whDataMgr::GetDB().Exec(query) )	// удалить свойство-файл из БД
		BOOST_THROW_EXCEPTION( prop_table_error_commitremove()<<wxstr("can`t execute: "+query) );
}
//------------------------------------------------------------------------------

void whClassPropTable::DoInsert(const PropModel* model,PropModelSet& to_upload)
{
	DoFtpPrepareUpload(model,to_upload);
	
	t_whclass_prop	prop =	model->GetProperty();
	wxFileName ftp_file(prop.mVal);
	prop.mVal= ftp_file.GetFullName();

	wxString query=wxString::Format("INSERT INTO t_whclass_prop(classid, label, type, val)VALUES \
										(%d, '%s', %d, '%s')",
										mClassID, prop.mLabel, prop.mType, prop.mVal);
	if(0 !=whDataMgr::GetDB().Exec(query) )
		BOOST_THROW_EXCEPTION( prop_table_error_commitinsert()<<wxstr("can`t execute: "+query) );

	
}
//------------------------------------------------------------------------------

void whClassPropTable::DoUpdate(const PropModel* model,PropModelSet& to_del,PropModelSet& to_upload,FilesToDel& del_files)
{
	DoFtpPrepareDel(model,del_files);
	DoFtpPrepareUpload(model,to_upload);

	// обновляем запись в базе
	t_whclass_prop	prop =	model->GetProperty();
	wxFileName ftp_file(prop.mVal);
	prop.mVal= ftp_file.GetFullName();

	wxString query=wxString::Format("UPDATE t_whclass_prop SET label='%s', type=%d, val='%s' \
								WHERE prop_id=%s"
								,prop.mLabel, prop.mType, prop.mVal
								,prop.mPropID );
	if(0 != whDataMgr::GetDB().Exec(query) )
		BOOST_THROW_EXCEPTION( prop_table_error_commitupdate()<<wxstr("can`t execute: "+query) );

}


//------------------------------------------------------------------------------

void whClassPropTable::DoCommit(PropModelSet& model_to_del,FilesToDel& del_files)
{
	// удаляем файлы подготовленные для удаления
	for(auto it=del_files.begin();it!=del_files.end();++it)
	{
		wxFileName ftp_file(wxString::Format("/%d/%s",mClassID,*it));
		try
		{
			wh::Ftp::Instance()->Remove(ftp_file);
		}
		catch(wh::Ftp::ftp_not_exists )	{}			// ничего не делаем нету так нету файла
	}

	// удаляем модели подготовленные для удаления
	for(auto it=model_to_del.begin();it!=model_to_del.end();++it)
	{
		const PropModel* model=*it;
		delete *it;
	}
	model_to_del.clear();
	
	 // обновляем состояния моделей на "СУЩЕСТВУЮЩИЕ"
	for(auto it=begin();it!=end();++it)
	{
		PropModel* model=*it;
		auto prop=model->GetProperty();
		// обновляем имена файлов в изменённых моделях
		if(prop.mType==t_whclass_prop::propFile && (model->GetState()== msUpdated || model->GetState()== msCreated) )
		{
			wxFileName ftp_file(prop.mVal);
			prop.mVal= ftp_file.GetFullName();
			model->SetProperty(prop);
		}
		(*it)->SetState( msExist);
	}

	// переименовываем новые файлы с временными названиями
	wxFileName ftp_file(wxString::Format("/%d/%s",mClassID,mNewPrefix));

	wh::Ftp::Instance()->DoChDir(ftp_file);
	wxArrayString files;
	if(wh::Ftp::Instance()->GetFilesList(files))
		for(auto it=files.begin();it!=files.end();++it)
		{
			const wxString& filename=*it;
			if(mNewPrefix.size()<filename.size() )
			{
				const wxString sub = filename.substr(0,mNewPrefix.size() );
				if(sub==mNewPrefix)
				{
					ftp_file.SetFullName(filename);
					const wxString oldname = filename.substr(mNewPrefix.size(),filename.size()-mNewPrefix.size() );
					wh::Ftp::Instance()->Rename(ftp_file, oldname );			
				}//if(sub==del_prefix)
			}//if(del_prefix.size()<filename.size() )
		}//for(auto it=files.begin();it!=files.end();++it)

}
//------------------------------------------------------------------------------

void whClassPropTable::DoRollback(PropModelSet& to_del, PropModelSet& to_upload)
{
	// если ролбэкнулись то удаляем новые файлы
	DoRemoveTmpFiles();
}


*/