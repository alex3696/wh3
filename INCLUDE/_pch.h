#ifndef __PCH_H
#define __PCH_H

/* STD headers */
#include <map>
#include <set>
//#include <list>
#include <vector>
#include <memory>
#include <deque>
#include <algorithm> 
//#include <stack>
#include <fstream>
//#include <typeinfo>       // operator typeid
// STD C++12
//#include <typeindex>		// std::type_index
#include <functional>		// std::bind
//#include <unordered_map>

/* wxWidgets headers */
#include <wx/wxprec.h>
//#include <wx/treebook.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/notebook.h>
//#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/collpane.h>
#include <wx/combo.h> 
#include <wx/choice.h>
#include <wx/valgen.h> 
#include <wx/dataview.h> 
#include "wxDataViewRenderer.h"
#include <wx/splitter.h>
#include <wx/aui/auibar.h>
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>
#include <wx/artprov.h>
#include <wx/ffile.h>
#include <wx/stdpaths.h>
#include <wx/filefn.h> 
#include <wx/wfstream.h>
#include <wx/datstrm.h>
#include <wx/txtstrm.h>
#include <wx/mstream.h>
#include <wx/srchctrl.h>

#include <wx/protocol/ftp.h>
#include <wx/fileconf.h>
#include <wx/progdlg.h>
#include <wx/msgdlg.h>
#include <wx/utils.h>
#include <wx/busyinfo.h>
#include <wx/wupdlock.h>
#include <wx/regex.h>

#include <wx/datectrl.h>
#include <wx/timectrl.h>

//---------------------------------------------------------------------------
/* BOOST headers */
//---------------------------------------------------------------------------
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/tuple/tuple.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>
#include <boost/signals2.hpp>
namespace sig = boost::signals2;
//---------------------------------------------------------------------------
// Мультииндексы буста
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
//---------------------------------------------------------------------------
//#include <boost/ptr_container/ptr_vector.hpp>
//#include <boost/ptr_container/ptr_deque.hpp>
//#include <boost/ptr_container/ptr_map.hpp>
//---------------------------------------------------------------------------
#include <boost/assert.hpp>
#include <boost/exception/all.hpp>
struct exception_base: virtual std::exception,virtual boost::exception { };
typedef boost::error_info<struct tag_strmsg,std::string>	strmsg;
#ifdef __WX__
typedef boost::error_info<struct tag_wxstr,wxString>		wxstr;
#endif // #ifdef __WXMSW__ || __WX__
//---------------------------------------------------------------------------
/* DatabaseLayer headers */
#include <DatabaseLayer.h>
#include <PostgresDatabaseLayer.h>
#include <PostgresResultSet.h>
#include <DatabaseLayerException.h>
//---------------------------------------------------------------------------
/* SAV headers && defines */
#include "wxComboBtn.h"
static const wxString wxEmptyString2;
wxMenuItem* AppendBitmapMenu(wxMenu* menu, int id, const wxString& label, const wxBitmap &bmp);

//---------------------------------------------------------------------------
class wxFuncTester
{
	ULONG		mStart;
	wxString	mFname;
	wxString	mInfo;
public:
	wxFuncTester(const char* fname)
		:mFname(fname), mStart(GetTickCount())
	{}

	~wxFuncTester()
	{
		wxLogMessage("%d\t %s \t%s"
			, GetTickCount() - mStart
			, mFname
			, mInfo);
	}
	ULONG GetStartTickCount()const
	{
		return mStart;
	}
	const wxString& GetFuncName()const
	{
		return mFname;
	}
	template<class... Args>
	void SetInfo(const wxString& format, Args&&... args) {
		mInfo = wxString::Format(format, std::forward<Args>(args)...);
	};

};

#define TEST_FUNC_TIME wxFuncTester ftester(__FUNCTION__);
//---------------------------------------------------------------------------



#endif // __PCH_H