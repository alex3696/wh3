#include "_pch.h"

#include "whDB.h"
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_symbols.hpp>

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>


#include <boost/fusion/include/io.hpp>

#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/fusion/adapted/adt/adapt_adt.hpp>
//#include <boost/fusion/include/adapt_adt.hpp>

#include <boost/fusion/adapted/boost_tuple.hpp>
#include <boost/fusion/include/boost_tuple.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;
namespace unicode =  boost::spirit::standard_wide ;
	
using qi::phrase_parse;
using unicode::space;

BOOST_FUSION_ADAPT_STRUCT(
    wh::ObjKey,
    (wxString, m_Type)
	(wxString, m_Name)
)


	/*
	BOOST_FUSION_ADAPT_TPL_STRUCT(
	(STRTYPE),
	(wxArrayParser::wh::ObjKeyTPL) (STRTYPE),
	(STRTYPE, m_Name)
	(STRTYPE, m_Type)
	)
	*/

///////////////////////////////////////////////////////////////////////////////////////////
/** ѕарсер дл€ массивов типа {{TYPE,NAME},{TYPE,NAME},{TYPE,NAME}... }
в вектор структур wh::ObjKeyTPL
*/
template <typename Iterator>
struct obj_array_parser : qi::grammar<Iterator,  std::deque<wh::ObjKey>(), unicode::space_type>
{
	obj_array_parser() 
		: obj_array_parser::base_type(obj_vec)
	{
		using qi::lit;
		using qi::lexeme;
		using qi::omit;
		using unicode::char_;

		any_type %= lexeme['%'];					// any_type

		n_type	%= lexeme[ +(char_ -'{' -'}' -',') ];					// some_type
		q_type	%=  omit['"'] >>lexeme[ +(char_ -'"' ) ] >> omit['"'];	// "some_type"
		oq_type %= omit['\''] >> lexeme[+(char_ - '\'')] >> omit['\''];	//'some_type'
		type %= any_type | oq_type | q_type | n_type;  							// TYPE
		
		n_name %= lexeme[ *(char_ -'{' -'}' ) ];						// some_name	- может быть и пустым
		q_name %=  omit['"'] >>lexeme[ *(char_ -'"' ) ] >> omit['"'];	// "some_name"	- может быть и пустым ""
		oq_name %= omit['\''] >> lexeme[*(char_ - '\'')] >> omit['\''];	// 'some_name'	- может быть и пустым ""
		name %= any_type | oq_name | q_name | n_name;  							// NAME


		nobj = lexeme['%'][qi::_val = phoenix::construct<wh::ObjKey>()];
		dobj %= '{' >> type >> ',' >> name >> '}';	
		obj %= dobj | nobj;
		//obj %= '{' >> type >> ',' >> name >> '}';
		
		obj_vec %= '{'>> (obj % ',') >> '}';						//{ *item }

	}

	qi::rule<Iterator, std::wstring(), unicode::space_type>		
		type, q_type, oq_type, n_type, any_type
		, name, q_name, oq_name, n_name;
	qi::rule<Iterator, wh::ObjKey(), unicode::space_type>		obj, nobj, dobj;
	qi::rule<Iterator, std::deque<wh::ObjKey>(), unicode::space_type> obj_vec;

};//struct obj_array_parser





///////////////////////////////////////////////////////////////////////////////////////////
bool _ParseArray(const std::wstring& str, std::deque<wh::ObjKey>& ret )
{
	std::wstring::const_iterator	begin = str.begin(), end   = str.end();	

	obj_array_parser	< std::wstring::const_iterator> gr;
		
	bool r = qi::phrase_parse(begin, end, gr, unicode::space, ret);

	if (begin != end) // fail if we did not get a full match
		return false;
	return r;		
}


///////////////////////////////////////////////////////////////////////////////////////////
/** ѕарсер дл€ массивов типа /[TYPE]NAME/[TYPE]NAME... 
*/
template <typename Iterator>
struct obj_path_parser : qi::grammar<Iterator,  std::deque<wh::ObjKey>(), unicode::space_type>
{
	obj_path_parser() 
		: obj_path_parser::base_type(obj_vec)
	{
		using qi::lit;
		using qi::lexeme;
		using qi::omit;
		using unicode::char_;

		q_type %= omit['"'] >>lexeme[ +(char_ -'"' ) ] >> omit['"'];
		n_type %= lexeme[  +(char_ -'[' -']' ) ];
		type   %= q_type | n_type;

		q_name %= omit['"'] >>lexeme[ *(char_ -'"' ) ] >> omit['"'];
		n_name %= lexeme[  *(char_ -'/') ];
		name   %= q_name | n_name;

		obj		%= '[' >> type >> ']' >> name ;

		obj_vec %= *('/'>> obj) ;

	}
	
	qi::rule<Iterator, std::wstring(), unicode::space_type>		type,q_type,n_type, name,q_name,n_name;
	qi::rule<Iterator, wh::ObjKey(), unicode::space_type>		obj;
	qi::rule<Iterator, std::deque<wh::ObjKey>(), unicode::space_type> obj_vec;
};//struct obj_path_parser
//-----------------------------------------------------------------------------
bool _ParsePath(const std::wstring& str, std::deque<wh::ObjKey>& ret, bool reverse  )
{
	std::wstring::const_iterator	begin = str.begin(), end   = str.end();	
	obj_path_parser	< std::wstring::const_iterator> gr;

	// в случае если результат надо реверсировать создаЄм временный массив
	std::deque<wh::ObjKey>*  ret_data=NULL;
	std::deque<wh::ObjKey>   tmp_data;
	ret_data = reverse ? &tmp_data : &ret;

	bool r = qi::phrase_parse(begin, end, gr, unicode::space, *ret_data);

	if (begin == end) // parse is OK
	{
		if(reverse)
			for(std::deque<wh::ObjKey>::const_reverse_iterator it=tmp_data.rbegin();it!=tmp_data.rend();++it)
				ret.push_back( *it);
	}
	else// fail if we did not get a full match
	{
		ret_data->clear();
		r=false;
	}

	return r;		
}


using namespace wh;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// ObjKeyPath
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
bool ObjKeyPath::ParseArray(const std::wstring& str,bool reverse)
{
	this->clear();
	
	bool res = _ParseArray(str, *(std::deque<wh::ObjKey>*)this);
	if (res && reverse)
		std::reverse(this->begin(), this->end());
	return res;
}
//------------------------------------------------------------------------------
bool ObjKeyPath::ParsePath(const  std::wstring& str,bool reverse)
{
	this->clear();
	bool res = _ParsePath(str, *(std::deque<wh::ObjKey>*)this, reverse);
	if (res && reverse)
		std::reverse(this->begin(), this->end());
	return res;

}