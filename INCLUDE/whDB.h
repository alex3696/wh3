#ifndef __WHDB_H
#define __WHDB_H

#include "_pch.h"

//----------------------------------------------------------------------------
/// Базовые группы
enum BaseGroup
{
	bgNull = 0,
	bgGuest,
	bgUser,
	bgObjDesigner,
	bgTypeDesigner,
	bgAdmin
};
//----------------------------------------------------------------------------
class	whTable;
typedef std::shared_ptr<whTable> whTable_shared_ptr;

//------------------------------------------------------------------------------
/** Класс обеспечивает соединение с БД и получение данных в таблицах типа whTable */
class whDB//: public Base
{
protected:
	mutable PostgresDatabaseLayer	m_Connection;

	//void ShowError(const DatabaseLayerException& err)const;
public:
	using Signal = sig::signal<void(const whDB&)>;
	using Slot = Signal::slot_type;

	Signal SigAfterConnect;
	Signal SigBeforeDisconnect;


	whDB();
	~whDB();
	//virtual int GetClassID()const	{	return whID_TABLE;	}

	void BeginTransaction();
	void Commit();
	void RollBack();

	whTable*			ExecWithResults(const wxString& query);
	whTable_shared_ptr	ExecWithResultsSPtr(const wxString& query);

	int			Exec(const wxString& query, bool with_result = true);
	int			Exec(const wxString& query,whTable* table);
	whTable*	Exec(const wxString& query,std::deque<wxString>& pathes);

	inline PostgresDatabaseLayer* GetConn()		{ return &m_Connection; }

	bool Open(const wxString& strServer, int nPort, const wxString& strDatabase, const wxString& strUser, const wxString& strPassword);
	bool Close();

	inline bool IsOpen()const	{	return m_Connection.IsOpen();	} 
	
	

};
//------------------------------------------------------------------------------
/** Таблица данных получает результаты запроса результата запроса через метод
int whDB::Exec(const wxString& query,whTable* table=NULL);
или выполняет его internal
*/
class whTable//: public Base
{
protected:
	whDB*					m_DB;
	DatabaseResultSet*		m_Result;
	//PostgresDatabaseLayer*	m_Connection;


	//TODO void ThrowNoResultSet()const;
public:
	typedef std::tr1::shared_ptr<whTable>	shared_ptr;

	whTable(whDB* db=NULL);
	~whTable();

	//virtual int GetClassID()const	{	return whID_DB;	}
	
	inline whDB* GetDB()const		{ return m_DB;		}
	inline void  SetDB(whDB* db)
	{ 
		this->Close(); 
		m_DB=db;	
	}
	
	int Exec(const wxString& query,bool with_result=true);
	int Exec(const wxString& query,std::deque<wxString>& pathes);
	void Close();

	

	/*virtual*/ unsigned int GetColumnCount()const;
	/*virtual*/ wxString GetColumnName(unsigned int col);
	            int      GetColumnType(int col);

	/*virtual*/ unsigned int	GetRowCount()const;
	/*virtual*/ wxString		GetAsString(unsigned int col,unsigned int row);
	/*virtual*/ void			GetAsString(unsigned int col,unsigned int row,wxString& result);
	/*virtual*/ int				GetAsInt(unsigned int col,unsigned int row);
	
	/*virtual*/ wxDateTime	GetAsDate(unsigned int col,unsigned int row);
	/*virtual*/ double		GetAsDouble(unsigned int col,unsigned int row);
	/*virtual*/ long		GetAsLong(unsigned int col,unsigned int row);
	/*virtual*/ bool		GetAsBool(unsigned int col,unsigned int row);
	/*virtual*/ wxMemoryBuffer	GetAsBlob(unsigned int col,unsigned int row);
	/*virtual*/ bool		IsFieldNull(unsigned int col,unsigned int row);

	inline wxString	GetAsString	(const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultString(colname,row):"";	}
	inline int		GetAsInt	(const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultInt(colname,row):0;		}
	inline wxDateTime GetAsDate (const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultDate(colname,row):wxDateTime(); }
	inline double	GetAsDouble (const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultDouble(colname,row):0.0;}
	inline long		GetAsLong	(const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultLong(colname,row):0;	}
	inline bool		GetAsBool	(const wxString& colname,unsigned int row)		{return m_Result?m_Result->GetResultBool(colname,row):0;	}
	inline bool		IsFieldNull	(const wxString& colname,unsigned int row)		{return m_Result?m_Result->IsFieldNull(colname,row):true;	}
	
};
//----------------------------------------------------------------------------


namespace wh
{


//----------------------------------------------------------------------------
/// Ключ таблицы объектов
struct ObjKey
{
	wxString m_Name;
	wxString m_Type;

	ObjKey(){}

	ObjKey(const wxString& _type,const wxString& _name)
		: m_Name(_name), m_Type(_type)
	{}

	inline bool IsEmpty()const	{	return m_Type.IsEmpty() || m_Name.IsEmpty();  }

	ObjKey(const ObjKey& data)
	{
		m_Name=data.m_Name;
		m_Type=data.m_Type;
	}

	ObjKey& operator=(const ObjKey& data)
	{
		m_Name = data.m_Name;
		m_Type = data.m_Type;
		return *this;
	}
};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/** Шаблон генератора путей и массивов из массива ObjKey == std::deque<ObjKey>.
Если элемент содержит пробелы то он обрамляется кавычками */
template <class Iterator>
struct GetStr
{
	static wxString do_quotes(const wxString& str)
	{
		for(unsigned int i=0;i<str.size()   ;++i)
			if( str[i]==L' ' || str[i]==L',')
				return wxString::Format("\"%s\"",str);
		return str;
	}//do_quotes
	
	/** Генератор массивов типа  /NAME[TYPE]/NAME[TYPE]...  из ObjKeyPath*/
	static void Path(wxString& str, const Iterator& begin, const Iterator& end)
	{
		
		if(begin!=end)
		{
			str.clear();
			for( Iterator it=begin;it!=end;++it)
				str+= wxString::Format("/[%s]%s",it->m_Type,it->m_Name);
		}
		else
			str="/";
	}

	static wxString Path(const Iterator& begin, const Iterator& end)
	{
		wxString str;
		Path(str,begin,end);
		return str;
	}//wxString Path(Iterator& begin,Iterator& end)

	/** Генератор массивов типа {{NAME,TYPE},{NAME,TYPE},{NAME,TYPE}... } из ObjKeyPath*/
	static void Array(wxString& str, const Iterator& begin, const Iterator& end)
	{
		str="{";
		for( Iterator it=begin;it!=end;++it)
			str+="{" + do_quotes(it->m_Type) + "," + do_quotes(it->m_Name) + "}," ;
		
		if( str.size() > 1 )
			str[str.size()-1]='}';
		else
			str+="}";
	}//Array(wxString& str,const Iterator& begin,const Iterator& end)const

	static wxString Array(const Iterator& begin, const Iterator& end)
	{
		wxString str;
		Array(str,begin,end);
		return str;
	}//wxString Array(Iterator& begin,Iterator& end)


};



//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/// наиболее простая реализация пути - массив объектов для заполнения из парсера
class ObjKeyPath: public std::deque<ObjKey>
{
protected:


public:
	inline void push_back(const wxString& _type,const wxString& _name)	{	std::deque<ObjKey>::push_back( ObjKey(_type,_name) );	}
	inline void push_back(const ObjKey& _data)							{	std::deque<ObjKey>::push_back( _data );					}

	inline void push_front(const wxString& _type,const wxString& _name)	{	std::deque<ObjKey>::push_front( ObjKey(_type,_name) );	}
	inline void push_front(const ObjKey& _data)							{	std::deque<ObjKey>::push_front( _data );				}
	
	wxString GetPathArray(bool reverse=true)const
	{
		return reverse ? GetStr<const_reverse_iterator>::Array(rbegin(),rend())
		/*else*/		: GetStr<const_iterator>::Array(begin(),end());
	}
	wxString GetPathString(bool reverse=true)const
	{
		return reverse ? GetStr<const_reverse_iterator>::Path(rbegin(),rend())
		/*else*/		: GetStr<const_iterator>::Path(begin(),end());
	}
	
	inline void GenerateArray(wxString& str,bool reverse=false)  
	{ 
		reverse ? GetStr<const_reverse_iterator>::Array(str,rbegin(),rend())
		/*else*/		: GetStr<const_iterator>::Array(str,begin(),end());
	}
	inline void GeneratePath (wxString& str,bool reverse=true)	
	{
		return reverse ? GetStr<const_reverse_iterator>::Path(str,rbegin(),rend())
		/*else*/			   : GetStr<const_iterator>::Path(str,begin(),end());
	}
	
	bool ParseArray(const std::wstring& str,bool reverse=false);
	bool ParsePath(const  std::wstring& str,bool reverse=false);
	
	bool ParseArray(const wxString& str,bool reverse=false)	{ return ParseArray(std::wstring(str.wc_str()),reverse);   }
	bool ParsePath( const wxString& str,bool reverse=false)  { return ParsePath (std::wstring(str.wc_str()),reverse);   }


};

static wxString ObjArrayToPath(const wxString& arr, 
	bool reverseArray = false, bool reversePath = false)
{
	wxString	path;
	ObjKeyPath	tmp;
	tmp.ParseArray(arr, reverseArray);
	tmp.GeneratePath(path, reversePath);
	return path;
}


//-----------------------------------------------------------------------------
wxArrayString Sql2ArrayString(const wxString& sql_value);
wxString ArrayString2Sql(const wxArrayString& array_string_value);

bool Sql2Bool(const wxString& sql_string);
wxString Bool2Sql(bool bool_value);



} //namespace wh





#endif // __WHDB_H



