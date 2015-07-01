#ifndef __RDBS_H
#define __RDBS_H

#include "rdbsIndex.h"
#include "rdbsField.h"

namespace wh{
namespace rdbs{


//-------------------------------------------------------------------------
class Table
{
private:
	struct error : virtual exception_base {};

	field::Array			mFields;
	index::Array			mIndexes;
	//record::TArray<RECORD>	mRecords;
	record::Array			mRecords;
	TriggerArray			mTriggers;

	bool				mUpdateOnInsert = true;

protected:

public:


	Table();

	void SetUpdateOnInsert(bool updateonInsert = true);
	bool GetUpdateOnInsert()const;

	void Clear();

	void SetFieldArray(std::initializer_list<Field> il);
	void SetFieldArray(field::Array& fieldArray);
	const Field& GetField(unsigned int pos)const;
	const Field& GetField(const wxString& name)const;
	int GetFieldIndex(const wxString& name)const;
	const field::Array& GetFieldArray()const;

	const Index& CreateIndex(
		std::initializer_list<unsigned int> il, const wxString& type = "unique");
	const Index& CreateIndex(
		const std::vector<unsigned int>& idxFields, const wxString& type = "unique");
	const Index& GetIndex(unsigned int pos)const;
	void DeleteIndex(unsigned int pos);

	void ClearRecords();
	RecordSp CreateRecord()const;
	RecordSp CreateRecord(const RecordData* current, const RecordData* stored = nullptr)const;
	
	const record::Array& GetRecordArray()const;
	void Insert(RecordSp& newRec);
	void Update(std::shared_ptr<Record> newRec, record::IdxPtr::iterator& oldIt);
	void Update(std::shared_ptr<Record> newRec, std::shared_ptr<Record> oldRec);
	void Delete(record::IdxPtr::iterator& oldIt);
	void Delete(RecordSp& oldRec);

	sig::connection ConnectTrigger(TgOp tg_op, TriggerFunc& tg_func);
	void DisconnectTrigger(TgOp tg_op, TriggerFunc& tg_func);

	void SaveRecords();
	void LoadRecords();
	void StructLoad(const wxString& path = wxEmptyString);


	virtual bool GetSelectAllQuery(wxString& query)const;
	virtual bool GetSelectQuery(wxString& query, RecordSp& rec)const;
	virtual bool GetInsertQuery(wxString& query, RecordSp& rec)const;
	virtual bool GetUpdateQuery(wxString& query, RecordSp& rec)const;
	virtual bool GetDeleteQuery(wxString& query, RecordSp& rec)const;
};

//-------------------------------------------------------------------------
static void RunTest()
{



	//Table<Record> table;
	Table table;

	table.StructLoad("c:\\Users\\snake\\Desktop\\prop_table.xml");


	table.SetFieldArray(	{ { "id", ftInt }
							, { "label", ftName }
							, { "type", ftInt } }
							);



	auto record_1 = table.CreateRecord();
		

	table.Insert(record_1);


	RecordData rec_data = {  "1" , "тест" ,  "0"  };

	record_1->SetData(&rec_data);


	table.ClearRecords();


	DWORD pos0(0), pos1(0), pos2(0), pos3(0);
	pos0 = GetTickCount();
	table.LoadRecords();
	pos1 = GetTickCount();
	table.SaveRecords();
	pos2 = GetTickCount();
	table.ClearRecords();
	pos3 = GetTickCount();
	
	
	
	//pos3 = GetTickCount();
	wxMessageBox(wxString::Format("Load  %d \t Clear %d", pos1 - pos0, pos3 - pos2));
}


//-------------------------------------------------------------------------
}//namespace wh{
}//namespace wh{
#endif // __****_H