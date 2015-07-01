#ifndef __TABLEMGR_H
#define __TABLEMGR_H

#include "_pch.h"

//---------------------------------------------------------------------------
/** ���� ���������� */
enum whSortDir
{
	whSORT_NO=2,
	whSORT_ASC=1,
	whSORT_DESC=0
};
//---------------------------------------------------------------------------
/// ������� �������� ���� ���������� � ��������� ��������
wxString SortDirToStr(const whSortDir dir);
/// ������� ���������� �������� ���������� � �������� ���� 
whSortDir StrToSortDir(const wxString& str);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** ���� �������� ��������� */
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
/// ������� �������� ���� �������� ��������� � ��������� ��������
wxString CompOpToStr(const whCompOp dir);
/// ������� ���������� �������� �������� ��������� � �������� ���� 
whCompOp StrToCompOp(const wxString& str);
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** ���� ���������� �������� */
enum whLogicOp
{
	whLO_UNKNOWN= 0,
	whLO_OR		= 0x00010000,
	whLO_AND	= 0x00100000 
};
//---------------------------------------------------------------------------
/// ������� �������� ���������� �������� � ��������� ��������
wxString LogicOpToStr(const whLogicOp dir);
/// ������� ���������� �������� � �������� ����������� ���� 
whLogicOp StrToLogicOp(const wxString& str);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** ������� ������ �� ���� m_SysLabel*/
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
/** ������ ��������
��������� ��������� (whLogicOp OR ��� AND) ����������� ������, � �� �������
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
/** ���� �������  ��� ���������� */
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

/** ������� �����*/
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

/** ������ ��� ������������ ������� � ������� �����������
��� ������(LogQuery) � ������� � ������� ������� ��������� � �������� ������ m_LogQueryData, 
� �������� �������� ���������� �����������.

��� ��������� ������ ���������� ������������� GUI(�������)
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
	/// ������������ ������ SQL ��� ����������
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
			sorter=sorter.BeforeLast(' ');	// �������� ��������� �������
			sorter=" ORDER BY "+sorter;		// ��������� ����� ����������
		}
		return sorter;	
	}//wxString GetSQLOrder()const

	//---------------------------------------------------------------------------	
	/// ������������ ������ �� ������� ����� �������
	wxString GetSQL_Fields()const
	{
		wxString fields;
		for(field_table::idx_random::const_iterator 
							it=m_field_table.begin_idx_random();
							it!=m_field_table.end_idx_random();	++it)
		{
			fields+= wxString::Format("\"%s\" ,",it->m_SysLabel);
		}
		fields=fields.BeforeLast(' ');// �������� ��������� �������
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
				// ��������� ��� ������� ������� ���-�� ����� "m_SysLabel" = 'fdata.m_RVal' AND
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
			
			// ������ ������ ��� ������
			int last = gfilter.Last(' ');
			if(last>0)
			{
				gfilter[0]='(';
				gfilter.insert(last,L")" );
			}
			filter+=gfilter;
		}
	
		// ���� ���� ������, �� ��������� ������� ����������
		if(filter.size())
			filter="WHERE " + filter.BeforeLast(' ');
		
		return filter;
	}

};





#endif // __TABLEMGR_H