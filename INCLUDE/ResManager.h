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

class ResMgr: public  TSingleton<ResMgr>
{
protected:

	
	

public:


	DEFINE_ICO(rule,24);


	wxIcon	m_icoFolder24;
	wxIcon	m_icoDirObj24;
	wxIcon	m_icoDirAct24;
	wxIcon	m_icoDirRul24;
	wxIcon	m_icoDirPrp24;


	wxIcon	m_icoBack24;
	wxIcon	m_icoType24;
	wxIcon	m_icoTrash24;

	wxIcon	m_icoViews24;
	wxIcon	m_icoHistory24;
	wxIcon	m_icoMove24;
	wxIcon	m_icoObj24;

	wxIcon	m_AddObjTab24;
	wxIcon	m_AddTypeTab24;

	//wxIcon	m_ico_UserGroup24;
	//wxIcon	m_ico_User24;

	
	DEFINE_ICO_ALL(refresh);
	DEFINE_ICO(folderup,24);
	
	
	DEFINE_ICO_ALL(newfolder);
	DEFINE_ICO(newsheet,24);

	DEFINE_ICO_ALL(edit);

	DEFINE_ICO(cut,24);
	DEFINE_ICO(paste,24);
	DEFINE_ICO(move,24);

	DEFINE_ICO_ALL(sort_asc);
	DEFINE_ICO_ALL(sort_desc);

	DEFINE_ICO_ALL(plus);
	DEFINE_ICO_ALL(delete);

	DEFINE_ICO_ALL(addfilter);

	DEFINE_ICO_ALL(group_key); 
	DEFINE_ICO_ALL(slot); 
	DEFINE_ICO_ALL(slot_list); 

	DEFINE_ICO_ALL(key); 

	DEFINE_ICO_ALL(add_type); 
	DEFINE_ICO_ALL(add_obj); 

	DEFINE_ICO_ALL(favorites); 
	DEFINE_ICO_ALL(add_favorites); 
	DEFINE_ICO_ALL(objtype); 

	DEFINE_ICO_ALL(connect); 
	DEFINE_ICO_ALL(disconnect); 

	DEFINE_ICO_ALL(user); 
	DEFINE_ICO_ALL(usergroup); 

	DEFINE_ICO_ALL(classprop); 
	DEFINE_ICO_ALL(save); 

	DEFINE_ICO_ALL(db); 

	DEFINE_ICO_ALL(folder_acts);
	DEFINE_ICO_ALL(type);

	DEFINE_ICO_ALL(folder_type);
	DEFINE_ICO_ALL(act);

	DEFINE_ICO_ALL(folder_props);

	DEFINE_ICO_ALL(create);
	DEFINE_ICO_ALL(minus);

	DEFINE_ICO_ALL(accept);
	DEFINE_ICO_ALL(reject);

	DEFINE_ICO_ALL(type_abstract);
	DEFINE_ICO_ALL(type_num);
	DEFINE_ICO_ALL(type_qty);
	DEFINE_ICO_ALL(obj_num);
	DEFINE_ICO_ALL(obj_qty);
	DEFINE_ICO_ALL(obj_num_group);
	DEFINE_ICO_ALL(obj_qty_group);
	
	

/*
	void AddIcon(const wxString& path,int index)
	{
		wxImage		ico;
		ico.LoadFile(path,wxBITMAP_TYPE_ANY,index);
		if(ico.IsOk() )
			m_Img.Add(ico);
	}
*/
	//wxImageList m_Img;

protected:
	friend class TSingleton<ResMgr>;

	ResMgr()
		:TSingleton<ResMgr>()
	{
		LOAD_ICO(rule,24,1);

		LOAD_ICO(cut,24,1);
		LOAD_ICO(paste,24,1);
		LOAD_ICO(move,24,1);

		LOAD_ICO_ALL(refresh);
		LOAD_ICO(folderup,24,1);

		LOAD_ICO_ALL(newfolder);
		LOAD_ICO(newsheet,24,1);
		
		LOAD_ICO_ALL(edit);

		LOAD_ICO_ALL(sort_asc);
		LOAD_ICO_ALL(sort_desc);
		LOAD_ICO_ALL(plus);
		LOAD_ICO_ALL(delete);
		LOAD_ICO_ALL(addfilter);
		LOAD_ICO_ALL(group_key);
		LOAD_ICO_ALL(key);
		LOAD_ICO_ALL(slot);
		LOAD_ICO_ALL(slot_list);

		LOAD_ICO_ALL(add_type);
		LOAD_ICO_ALL(add_obj);
		LOAD_ICO_ALL(favorites);
		LOAD_ICO_ALL(add_favorites);
		LOAD_ICO_ALL(objtype);

		LOAD_ICO_ALL(connect); 
		LOAD_ICO_ALL(disconnect); 

		LOAD_ICO_ALL(user); 
		LOAD_ICO_ALL(usergroup); 

		LOAD_ICO_ALL(classprop); 
		LOAD_ICO_ALL(save); 

		LOAD_ICO_ALL(db); 

		LOAD_ICO_ALL(folder_acts);
		LOAD_ICO_ALL(folder_type);
		LOAD_ICO_ALL(folder_props);

		LOAD_ICO_ALL(type);
		LOAD_ICO_ALL(act);
		LOAD_ICO_ALL(create);

		LOAD_ICO_ALL(minus);
		

		LOAD_ICO_ALL(accept);
		LOAD_ICO_ALL(reject);

		LOAD_ICO_ALL(type_abstract);
		LOAD_ICO_ALL(type_num);
		LOAD_ICO_ALL(type_qty);
		LOAD_ICO_ALL(obj_num);
		LOAD_ICO_ALL(obj_qty);
		LOAD_ICO_ALL(obj_num_group);
		LOAD_ICO_ALL(obj_qty_group);


		//bool b = m_Img.Create(24,24);
		//if(b)
		//{
		//	// Upload images
		//}
		
		m_icoFolder24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\folder.ico",wxBITMAP_TYPE_ANY,1));
		m_icoBack24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\back.ico",wxBITMAP_TYPE_ANY,1));
		m_icoType24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\type.ico",wxBITMAP_TYPE_ANY,1));
		m_icoTrash24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\trash.ico",wxBITMAP_TYPE_ANY,1));
		m_icoHistory24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\history.ico",wxBITMAP_TYPE_ANY,1));
		m_icoViews24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\views.ico",wxBITMAP_TYPE_ANY,1));
		m_icoObj24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\obj.ico",wxBITMAP_TYPE_ANY,1));
		m_icoMove24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\move.ico",wxBITMAP_TYPE_ANY,1));

		m_icoDirObj24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\folder_obj.ico",wxBITMAP_TYPE_ANY,1));
		m_icoDirAct24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\folder_acts.ico",wxBITMAP_TYPE_ANY,1));
		m_icoDirRul24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\folder_rules.ico",wxBITMAP_TYPE_ANY,1));
		m_icoDirPrp24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\folder_props.ico",wxBITMAP_TYPE_ANY,1));

		m_AddObjTab24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\add_obj_tab.ico",wxBITMAP_TYPE_ANY,1));
		m_AddTypeTab24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\add_type_tab.ico",wxBITMAP_TYPE_ANY,1));

		//m_ico_UserGroup24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\usergroup.ico",wxBITMAP_TYPE_ANY,1));
		//m_ico_User24.CopyFromBitmap( wxImage("..\\..\\RESOURCES\\user.ico",wxBITMAP_TYPE_ANY,1));



	}




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

	~ctrlWithResMgr()
	{
		m_ResMgr->FreeInst();
	}

	const ResMgr*	GetResMgr()const
	{
		return m_ResMgr;
	}



};



#endif // __RESMANAGER_H