#ifndef __RDBSTRIGGER_H
#define __RDBSTRIGGER_H

#include "TModelArray.h"
#include "db_rec.h"

namespace wh{
namespace rdbs{

enum class TgOp
{
	toBeforeInsert = 0,
	toAfterInsert,
	toBeforeUpdate,
	toAfterUpdate,
	toBeforeDelete,
	toAfterDelete,

	toAfterBeforeLoad,
	toAfterAfterLoad,
	toAfterBeforeSave,
	toAfterAfterSave,

};


//-------------------------------------------------------------------------
class Table;
class Record;
//-------------------------------------------------------------------------
using RecordSp = std::shared_ptr<Record>;

using Trigger = sig::signal<void(const Table* table,
	RecordSp& newItem, RecordSp& oldItem, const TgOp tg_op) >;
//using TriggerFunc = Trigger::slot_function_type;
using TriggerFunc = std::function < void(const Table* table,
	RecordSp& newItem, RecordSp& oldItem, const TgOp tg_op)  >;

//-------------------------------------------------------------------------
class TriggerArray
{
public:

	sig::connection Connect(const TgOp tg_op, TriggerFunc& tg_func)
	{
		return mTriggerMap[tg_op].connect(tg_func);
	}

	void Disconnect(const TgOp tg_op, const TriggerFunc& tg_func)
	{
		auto tgIt = mTriggerMap.find(tg_op);
		if (mTriggerMap.end() != tgIt)
			tgIt->second.disconnect(&tg_func);
	}

	void Do(const Table* table,
		RecordSp& newItem, RecordSp& oldItem, const TgOp tg_op)const
	{
		auto tgIt = mTriggerMap.find(tg_op);
		if (mTriggerMap.end() != tgIt)
			tgIt->second(table, newItem, oldItem, tg_op);
	}

protected:
	std::map<TgOp, Trigger> mTriggerMap;
};

}//namespace wh{
}//namespace wh{
#endif // __****_H