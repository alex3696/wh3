#ifndef __TABLEMGR_H
#define __TABLEMGR_H

#include "_pch.h"

//---------------------------------------------------------------------------
/** Виды сортировок */
enum whSortDir
{
	whSORT_NO=2,
	whSORT_ASC=1,
	whSORT_DESC=0
};
//---------------------------------------------------------------------------
/// Перевод значения типа сортировки в строковое значения
wxString SortDirToStr(const whSortDir dir);
/// Перевод строкового название сортировки в значение типа 
whSortDir StrToSortDir(const wxString& str);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** Виды операций сравнения */
enum whCompOp
{
	whCO_UNKNOWN= 0x00000000,
	whCO_EQ		= 0x00000010,
	whCO_NOTEQ	= 0x10000000,
	whCO_LESS	= 0x00000100,
	whCO_MORE	= 0x10000100,
	whCO_LESSEQ = 0x00000110,
	whCO_MOREEQ = 0x10000110,
	whCO_LIKE	= 0x00001000,
	whCO_NOTLIKE= 0x10001000

};
//---------------------------------------------------------------------------
/// Перевод значения типа операции сравнения в строковое значения
wxString CompOpToStr(const whCompOp dir);
/// Перевод строкового название операции сравнения в значение типа 
whCompOp StrToCompOp(const wxString& str);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** Виды логических операций */
enum whLogicOp
{
	whLO_UNKNOWN= 0,
	whLO_OR		= 0x00010000,
	whLO_AND	= 0x00100000 
};
//---------------------------------------------------------------------------
/// Перевод значения ЛОГИЧЕСКОЙ операции в строковое значения
wxString LogicOpToStr(const whLogicOp dir);
/// Перевод строкового значения в значение ЛОГИЧЕСКОГО типа 
whLogicOp StrToLogicOp(const wxString& str);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** Простой фильтр по полю m_SysLabel*/
class whFieldFilter
{
public:
	wxString		m_SysLabel;
	whCompOp		m_Operator;
	wxString		m_RVal;
	whLogicOp		m_Connector;
	
	whFieldFilter(	const wxString& syslabel=wxEmptyString,const wxString& rval=wxEmptyString,
					const whCompOp  oper=whCO_EQ,		const whLogicOp conn=whLO_AND)
		:m_SysLabel(syslabel) ,m_Operator(oper), m_RVal(rval), m_Connector(conn)
	{}
};
//---------------------------------------------------------------------------
/** Список фильтров
последний коннектор (whLogicOp OR или AND) принадлежит группе, а не фильтру
*/
struct whGroupFilter: public std::deque<whFieldFilter>	
{
public:

};

//---------------------------------------------------------------------------

struct group_index_error: virtual exception_base { };

//---------------------------------------------------------------------------
class whGroupFilterList: public std::deque<whGroupFilter>	
{
public:
	whGroupFilter& operator[](size_type _Pos)			
	{	
		if(!(_Pos<size() ) )
			throw group_index_error();
		return *(this->begin()+_Pos);
	}

	const whGroupFilter& operator[](size_type _Pos)const
	{	
		if(!(_Pos<size() ) )
			throw group_index_error();
		return *(this->begin()+_Pos);
	}

	std::deque<whGroupFilter>::iterator erase(size_type _Pos)
	{
		if(!(_Pos<size() ) )
			throw group_index_error();		
		std::deque<whGroupFilter>::iterator it = begin()+_Pos;
		return std::deque<whGroupFilter>::erase( it );
	}


};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** Поле таблицы  для управления */
class whDbField
{
public:
	wxString		m_SysLabel;
	wxString		m_UserLabel;
	whSortDir		m_Sort;

	whDbField(const wxString& sname=wxEmptyString,const wxString& uname=wxEmptyString,whSortDir sort=whSORT_NO)
		:m_SysLabel(sname),m_UserLabel(uname),m_Sort(sort)
	{}


};
//---------------------------------------------------------------------------

typedef multi_index_container
		<	whDbField,
			indexed_by	
			<
				random_access<>,
				ordered_unique< BOOST_MULTI_INDEX_MEMBER(whDbField,wxString,m_SysLabel)    >,
				ordered_unique< BOOST_MULTI_INDEX_MEMBER(whDbField,wxString,m_UserLabel)   >
			>
		> _field_table;

/** таблица полей*/
class field_table: public _field_table
{
public:
	DEFINE_MI_ITERATORS(idx_random,0);
	DEFINE_MI_ITERATORS(idx_sysname,1);
	DEFINE_MI_ITERATORS(idx_username,2);

	int Relocate(unsigned int new_pos,unsigned int item_pos)
	{
		if(new_pos<size() && item_pos<size())
		{
			get<0>().relocate( get<0>().begin()+new_pos,get<0>().begin()+item_pos);
			return 0;
		}
		return 1;
	}
};
//---------------------------------------------------------------------------

/** данные для визуализации запроса к истории перемещения
все данные(LogQuery) о запросе к таблице истории находятся в основной панели m_LogQueryData, 
в дочерние контролы передаются указателями.

При изменении данных необходимо перестраивать GUI(события)
*/
class whLogQueryData
{
public:
	field_table			m_field_table;
	whGroupFilterList	m_Filter;
	
	whLogQueryData()
	{
	}

	~whLogQueryData()
	{}


	//---------------------------------------------------------------------------
	/// сформировать строку SQL для сортировки
	wxString GetSQL_Order()const
	{
		wxString sorter;
		for(field_table::idx_random::const_iterator 
				it=m_field_table.begin_idx_random();
				it!=m_field_table.end_idx_random();	++it)
		{
			switch(it->m_Sort)
			{
				default:			break;
				case whSORT_ASC:	sorter+= it->m_SysLabel+" ASC ,";	break;
				case whSORT_DESC:	sorter+= it->m_SysLabel+" DESC ,";	break;
			}//switch
		}//for
			

		if(sorter.size())
		{
			sorter=sorter.BeforeLast(' ');	// отрезаем последнюю запятую
			sorter=" ORDER BY "+sorter;		// добавляем опцию сортировки
		}
		return sorter;	
	}//wxString GetSQLOrder()const

	//---------------------------------------------------------------------------	
	/// сформировать строку со списком полей запроса
	wxString GetSQL_Fields()const
	{
		wxString fields;
		for(field_table::idx_random::const_iterator 
							it=m_field_table.begin_idx_random();
							it!=m_field_table.end_idx_random();	++it)
		{
			fields+= wxString::Format("\"%s\" ,",it->m_SysLabel);
		}
		fields=fields.BeforeLast(' ');// отрезаем последнюю запятую
		return fields;
	}
	//---------------------------------------------------------------------------
	wxString GetSQL_Filters()const
	{
		wxString filter;
		for(unsigned int gid=0;gid<m_Filter.size();++gid)
		{
			wxString gfilter;
			for(unsigned int fid=0;fid<m_Filter[gid].size();++fid)
			{
				const whFieldFilter& fdata=m_Filter[gid][fid];
				// формируем для каждого фильтра что-то такое "m_SysLabel" = 'fdata.m_RVal' AND
				if(fdata.m_SysLabel.size() && fdata.m_RVal.size() && fdata.m_Operator!=whCO_UNKNOWN)
				{
					wxString fieldname=fdata.m_SysLabel;
					field_table::idx_username::iterator it= m_field_table.get<2>().find(fieldname);
					
					if(it!= m_field_table.get<2>().end())
						fieldname=it->m_SysLabel;
								
					gfilter+=wxString::Format(" \"%s\" %s '%s' %s",fieldname,CompOpToStr(fdata.m_Operator),fdata.m_RVal,
										LogicOpToStr(fdata.m_Connector));
				}

			}//for(unsigned int fid=0;fid<m_Filter[gid].size();++fid)
			
			// ставим скобки для группы
			int last = gfilter.Last(' ');
			if(last>0)
			{
				gfilter[0]='(';
				gfilter.insert(last,L")" );
			}
			filter+=gfilter;
		}
	
		// если есть фильтр, то добавляем условие фильтрации
		if(filter.size())
			filter="WHERE " + filter.BeforeLast(' ');
		
		return filter;
	}

};





#endif // __TABLEMGR_H