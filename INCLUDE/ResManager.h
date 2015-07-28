#ifndef __RESMANAGER_H
#define __RESMANAGER_H

#include "_pch.h"

#define DEFINE_ICO(name,size)	wxIcon  m_ico_##name##size	
#define LOAD_ICO(name,size,pos)	m_ico_##name##size.CopyFromBitmap( wxImage( "..\\..\\RESOURCES\\"#name".ico",wxBITMAP_TYPE_ANY,##pos))

#define DEFINE_ICO_ALL(name) \
	DEFINE_ICO(name,32);	\
	DEFINE_ICO(name,24);	\
	DEFINE_ICO(name,16);	

#define LOAD_ICO_ALL(name)	\
	LOAD_ICO(name,32,0);	\
	LOAD_ICO(name,24,1);	\
	LOAD_ICO(name,16,2);	

#define DEF_INIT_ICO(name,size)	wxIcon  m_ico_##name##size = wxIcon("..\\..\\RESOURCES\\"#size"\\"#name".ico", wxBITMAP_TYPE_ANY, ##size, ##size);	
#define DEF_INIT_ICO16(name)	DEF_INIT_ICO(name,16)
#define DEF_INIT_ICO24(name)	DEF_INIT_ICO(name,24)
#define DEF_INIT_ICO32(name)	DEF_INIT_ICO(name,32)


#define DEF_INIT_ICO_ALL(name)	\
	DEF_INIT_ICO(name,32);	\
	DEF_INIT_ICO(name,24);	\
	DEF_INIT_ICO(name,16);	


class ResMgr
{
private:
	ResMgr(ResMgr const&) = delete;
	void operator=(ResMgr const&) = delete;
public:
	static ResMgr* GetInstance()
	{
		static ResMgr instance; // Guaranteed to be destroyed.
		// Instantiated on first use.
		return &instance;
	}

	DEF_INIT_ICO24(rule);
	DEF_INIT_ICO24(folder);
	
	DEF_INIT_ICO24(folder_obj);
	
	DEF_INIT_ICO24(back);
	DEF_INIT_ICO24(type);
	DEF_INIT_ICO24(views);

	DEF_INIT_ICO24(obj);
	DEF_INIT_ICO24(move);

	DEF_INIT_ICO24(add_obj_tab);
	DEF_INIT_ICO24(add_type_tab);

	
	DEF_INIT_ICO24(refresh);
	DEF_INIT_ICO24(folderup);

	
	
	
	DEF_INIT_ICO24(newfolder);

	DEF_INIT_ICO24(edit);
	
	DEF_INIT_ICO24(plus);
	DEF_INIT_ICO24(delete);
	DEF_INIT_ICO24(addfilter);
	DEF_INIT_ICO24(add_type);
	DEF_INIT_ICO24(add_obj);
	DEF_INIT_ICO24(favorites);
	DEF_INIT_ICO24(objtype);
	DEF_INIT_ICO24(connect);
	DEF_INIT_ICO24(disconnect);
	DEF_INIT_ICO24(user);
	DEF_INIT_ICO24(usergroup);
	DEF_INIT_ICO24(classprop);
	DEF_INIT_ICO24(save);
	DEF_INIT_ICO24(db);
	DEF_INIT_ICO24(folder_acts);
	DEF_INIT_ICO24(folder_type);
	DEF_INIT_ICO24(act);
	DEF_INIT_ICO24(folder_props);
	DEF_INIT_ICO24(create);
	DEF_INIT_ICO24(minus);
	DEF_INIT_ICO24(accept);
	DEF_INIT_ICO24(reject);


	DEF_INIT_ICO24(type_abstract);
	DEF_INIT_ICO24(type_num);
	DEF_INIT_ICO24(type_qty);
	DEF_INIT_ICO24(obj_num);
	DEF_INIT_ICO24(obj_qty);
	DEF_INIT_ICO24(obj_num_group);
	DEF_INIT_ICO24(obj_qty_group);
	

	DEF_INIT_ICO16(connect);
	DEF_INIT_ICO16(disconnect);

	DEF_INIT_ICO16(newfolder);
	DEF_INIT_ICO16(sort_asc);
	DEF_INIT_ICO16(sort_desc);
	DEF_INIT_ICO16(plus);
	DEF_INIT_ICO16(delete);
	DEF_INIT_ICO16(slot_list);
	DEF_INIT_ICO16(save);
	DEF_INIT_ICO16(db);
	DEF_INIT_ICO16(create);
	DEF_INIT_ICO16(minus);
	DEF_INIT_ICO16(edit);
	DEF_INIT_ICO16(refresh);
protected:

	ResMgr(){	}




};

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/** ћенеджер ресурсов - наследу€ этот класс, потомки получают доступ к менеджеру ресурсов
посредством вызова GetResMgr() или напр€мую указател€ m_ResMgr */
class ctrlWithResMgr
{
public:
	ResMgr*				m_ResMgr;

	ctrlWithResMgr()
	{
		m_ResMgr = ResMgr::GetInstance();
	}

	const ResMgr*	GetResMgr()const
	{
		return m_ResMgr;
	}



};



#endif // __RESMANAGER_H