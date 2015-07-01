#ifndef __RDBSFIELD_H
#define __RDBSFIELD_H

#include "rdbsRecord.h"

namespace wh{
namespace rdbs{



//-------------------------------------------------------------------------
struct Field
{
	wxString	mName;
	FieldType	mType;
	wxString	mDefault;


	Field(){}

	Field(const char* name, FieldType ft, const wxString& def=wxEmptyString)
		:mName(name), mType(ft), mDefault(def)
	{}
	Field(const wxString& name, FieldType ft, const wxString& def = wxEmptyString)
		:mName(name), mType(ft), mDefault(def)
	{}


	Field(const boost::property_tree::ptree& pt)
	{
		mName = pt.get<std::string>("label");
		mType = StringToFt(pt.get<std::string>("type"));
		mDefault = pt.get<std::string>("default");
	}
};
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
namespace field
{
using Store =
	boost::multi_index_container
	<
		Field,
		indexed_by
		<
			random_access<>
			, ordered_unique< BOOST_MULTI_INDEX_MEMBER(Field, wxString, mName) >
		>
	>;

//-------------------------------------------------------------------------
class Array :
	public Store
{
protected:
public:
	Array(){}

	Array(std::initializer_list<Field> il)
		:Store(il)
	{}
	
	void StructLoad(const boost::property_tree::ptree& pt)
	{
		for (const auto& field_pair : pt.get_child("fields"))
			emplace_back(field_pair.second);////Field field(pt_field.second);

	};

};


}//namespace field{
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------






}//namespace rdbs{
}//namespace wh{
#endif // __****_H