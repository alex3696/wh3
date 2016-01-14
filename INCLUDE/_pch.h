#ifndef __PCH_H
#define __PCH_H

/* STD headers */
#include <map>
#include <set>
#include <list>
#include <vector>
#include <memory>
#include <deque>
#include <algorithm> 
#include <stack>
#include <fstream>
#include <typeinfo>       // operator typeid
// STD C++12
#include <typeindex>		// std::type_index
#include <functional>		// std::bind
#include <unordered_map>

//using namespace std::tr1;

#ifdef __WX__

/* wxWidgets headers */
#include <wx/wxprec.h>
#include <wx/treebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/combo.h> 
#include <wx/choice.h>
#include <wx/valgen.h> 
#include <wx/dataview.h> 
#include <wx/splitter.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>
#include "wx/artprov.h"
#include <wx/srchctrl.h>
#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h> 
#include <wx/wfstream.h>
#include <wx/listbook.h>
#include <wx/protocol/ftp.h>
#include <wx/fileconf.h>
#include <wx/progdlg.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>
#include <wx/wupdlock.h>
#include <wx/regex.h>

static const wxString wxEmptyString2;

//#if wxUSE_DATEPICKCTRL
    #include <wx/datectrl.h>
//#endif

/* DatabaseLayer headers */
#include <DatabaseLayer.h>
#include <PostgresDatabaseLayer.h>
#include <PostgresResultSet.h>
#include <DatabaseLayerException.h>

/* SAV headers */
#include "wxComboBtn.h"


wxMenuItem* AppendBitmapMenu(wxMenu* menu,int id,const wxString& label,const wxBitmap &bmp);

#endif // #ifdef __WXMSW__ || __WX__

//---------------------------------------------------------------------------
/* BOOST headers */
//---------------------------------------------------------------------------
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

//---------------------------------------------------------------------------
// ћультииндексы буста
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/composite_key.hpp>

using namespace boost;
using namespace multi_index;
using boost::multi_index_container;


#define DEFINE_TMI_ITERATORS(TYPENAME,VAR,IDX_NAME,IDX_NUM)	\
	typedef nth_index<typename TYPENAME,IDX_NUM>   IDX_NAME; \
	nth_index_iterator<typename TYPENAME,IDX_NUM> begin_##IDX_NAME()			{	return VAR.get<IDX_NUM>().begin();	}\
	nth_index_iterator<typename TYPENAME,IDX_NUM> end_##IDX_NAME()				{	return VAR.get<IDX_NUM>().end();	}\
	nth_index_const_iterator<typename TYPENAME,IDX_NUM> begin_##IDX_NAME()const{	return VAR.get<IDX_NUM>().begin();	}\
	nth_index_const_iterator<typename TYPENAME,IDX_NUM> end_##IDX_NAME()const	{	return VAR.get<IDX_NUM>().end();	}


#define DEFINE_MI_ITERATORS(NAME,IDX_NUM)	\
	typedef nth_index<IDX_NUM>::type NAME;								 \
	virtual NAME::iterator begin_##NAME()			{	return get<IDX_NUM>().begin();	}\
	virtual NAME::iterator end_##NAME()				{	return get<IDX_NUM>().end();	}\
	virtual	NAME::const_iterator begin_##NAME()const{	return get<IDX_NUM>().begin();	}\
	virtual	NAME::const_iterator end_##NAME()const	{	return get<IDX_NUM>().end();	}

//---------------------------------------------------------------------------
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/ptr_container/ptr_deque.hpp>
#include <boost/ptr_container/ptr_map.hpp>

//---------------------------------------------------------------------------
#include <boost/assert.hpp>

#include <boost/exception/all.hpp>
struct exception_base: virtual std::exception,virtual boost::exception { };
typedef boost::error_info<struct tag_strmsg,std::string>	strmsg;
#ifdef __WX__
typedef boost::error_info<struct tag_wxstr,wxString>		wxstr;
#endif // #ifdef __WXMSW__ || __WX__

#define DEFINE_GETACESSOR(TYPE,NAME,VARNAME )	\
	inline TYPE Get##NAME()const			{	return VARNAME; } 	

#define DEFINE_SETACESSOR(TYPE,NAME,VARNAME )	\
	inline void Set##NAME(const TYPE& data)	{	VARNAME=data;	 }


/* объ€вл€лка функций доступа к переменной */
#define DEFINE_ACESSOR(TYPE,NAME,VARNAME )	\
	inline TYPE Get##NAME()const			{	return VARNAME; } 	\
	inline void Set##NAME(const TYPE& data)	{	VARNAME=data;	 }

/* объ€вл€лка функций доступа к переменной */
#define DEFINE_ACESSOR_RW(TYPE,NAME,VARNAME )	\
public: \
	inline TYPE Get##NAME()const			{	return VARNAME; } 	\
	inline void Set##NAME(const TYPE& data)	{	VARNAME=data;	 }

/* объ€вл€лка функций доступа к переменной */
#define DEFINE_PROTECTED_PROPERTY( TYPE,NAME )	\
protected: \
	TYPE m_##NAME; \
public: \
	DEFINE_ACESSOR_RW( TYPE,NAME,m_##NAME)



#define DEFINE_CLASS_ID(TYPE)	public: virtual int GetClassID()const	{ return TYPE; }

/** Ѕазовый класс с виртуальным деструктором и идентификацией класса*/
class Base
{
public:
	
	DEFINE_CLASS_ID(0)	////virtual int GetClassID()const	{	return whID_UNKNOWN;	}

	#ifdef __WX__
	wxString GetClassName()const
	{
		wxString str = typeid(*this).name();
		str.erase(0,6);
		return str;
	}
	#else
	std::wstring GetClassName()const
	{
		std::string strPicPath = typeid(*this).name();
		strPicPath.erase(0,6);
		std::wstring temp(strPicPath.length(), L' ');
		std::copy(strPicPath.begin(), strPicPath.end(), temp.begin());
		return temp;
	}

	#endif // __WX__


	virtual size_t	GetClassHashCode()const		{	return typeid(*this).hash_code();	}

	virtual ~Base(){}
};





#endif // __PCH_H