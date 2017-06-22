-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_temp.add_obj_dt_insert() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.add_obj_dt_insert() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _cid BIGINT;
  _oid BIGINT;
  _dt_insert TIMESTAMP;
  _dt_update TIMESTAMP;

  cursor_obj CURSOR IS
    SELECT id FROM obj_name ;

  cursor_cls CURSOR IS
    SELECT id FROM acls ;

BEGIN 
  -- Обновляем таблицу классов 
  ALTER TABLE acls     DROP COLUMN IF EXISTS dt_insert;
  ALTER TABLE acls     DROP COLUMN IF EXISTS usr_insert;
  ALTER TABLE acls     DROP COLUMN IF EXISTS dt_update;
  ALTER TABLE acls     DROP COLUMN IF EXISTS usr_update;
  
  ALTER TABLE acls     ADD  COLUMN dt_insert timestamp with time zone DEFAULT now();
  ALTER TABLE acls     ADD  COLUMN usr_insert NAME DEFAULT CURRENT_USER;
  ALTER TABLE acls     ADD  COLUMN dt_update timestamp with time zone DEFAULT now();
  ALTER TABLE acls     ADD  COLUMN usr_update NAME DEFAULT CURRENT_USER;

  GRANT UPDATE(dt_insert) ON acls TO "Admin";
  GRANT UPDATE(usr_insert)ON acls TO "Admin";
  GRANT UPDATE(dt_update) ON acls TO "TypeDesigner";
  GRANT UPDATE(usr_update)ON acls TO "TypeDesigner";

  -- Обновляем таблицу объектов
  ALTER TABLE obj_name DROP COLUMN IF EXISTS dt_insert;
  ALTER TABLE obj_name DROP COLUMN IF EXISTS usr_insert;
  ALTER TABLE obj_name DROP COLUMN IF EXISTS dt_update;
  ALTER TABLE obj_name DROP COLUMN IF EXISTS usr_update;

  ALTER TABLE obj_name ADD COLUMN dt_insert timestamp with time zone DEFAULT now();
  ALTER TABLE obj_name ADD COLUMN usr_insert NAME DEFAULT CURRENT_USER;
  ALTER TABLE obj_name ADD COLUMN dt_update timestamp with time zone DEFAULT now();
  ALTER TABLE obj_name ADD COLUMN usr_update NAME DEFAULT CURRENT_USER;
  
  GRANT UPDATE(dt_insert) ON obj_name TO "Admin";
  GRANT UPDATE(usr_insert)ON obj_name TO "Admin";
  GRANT UPDATE(dt_update) ON obj_name TO "ObjDesigner";
  GRANT UPDATE(usr_update)ON obj_name TO "ObjDesigner";

  -- adding missing information dt_insert and dt_update
  FOR obj IN cursor_obj LOOP
    SELECT log_dt INTO _dt_insert FROM log WHERE mobj_id=obj.id ORDER BY log_dt ASC  LIMIT 1;
    _dt_insert:=COALESCE( _dt_insert, '2016.10.05 00:00:00'::TIMESTAMP);
    SELECT log_dt INTO _dt_update FROM log WHERE mobj_id=obj.id ORDER BY log_dt DESC LIMIT 1;
    _dt_update:=COALESCE( _dt_update, _dt_insert);

    UPDATE obj_name SET dt_insert=_dt_insert, dt_update=_dt_update WHERE id=obj.id;

  END LOOP;--FOR cursor_obj

  ALTER TABLE obj_name ALTER COLUMN dt_insert SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN dt_update SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN usr_insert SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN usr_update SET NOT NULL;

  ALTER TABLE acls ALTER COLUMN dt_insert SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN dt_update SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN usr_insert SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN usr_update SET NOT NULL;

  -- период действий завязян через свойство класса
  DROP TABLE IF EXISTS ref_cls_act_period;
  CREATE TABLE ref_cls_act_period
  (
    cls_id bigint NOT NULL
    ,act_id bigint NOT NULL
    ,prop_id bigint NOT NULL

    ,CONSTRAINT fk_ref_cls_act_period__clsact FOREIGN KEY (cls_id,act_id)
      REFERENCES ref_cls_act (cls_id,act_id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
    ,CONSTRAINT fk_ref_cls_act_period__clsprop FOREIGN KEY (cls_id,prop_id)
      REFERENCES prop_cls (prop_id, cls_id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  );



  
--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_bu_obj_name() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bu_obj_name()  RETURNS trigger AS
$body$
DECLARE
BEGIN
  NEW.usr_update:=CURRENT_USER;
  NEW.dt_update:=CURRENT_TIMESTAMP;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bu_obj_name BEFORE UPDATE ON obj_name FOR EACH ROW EXECUTE PROCEDURE ftr_bu_obj_name();
GRANT EXECUTE ON FUNCTION ftr_bu_obj_name() TO "ObjDesigner";


-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

select * from pg_temp.add_obj_dt_insert();



