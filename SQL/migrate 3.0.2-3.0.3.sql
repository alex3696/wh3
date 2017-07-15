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
  _usr_insert TEXT;
  _usr_update TEXT;

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
  --GRANT INSERT,DELETE ON TABLE public.acls TO "TypeDesigner";

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
  --GRANT INSERT, DELETE ON TABLE public.obj_name TO "ObjDesigner";

  -- adding missing information dt_insert and dt_update
  FOR obj IN cursor_obj LOOP
    SELECT log_dt,log_user INTO _dt_insert, _usr_insert FROM log WHERE mobj_id=obj.id ORDER BY log_dt ASC  LIMIT 1;
    --SELECT * FROM obj 
    --LEFT JOIN LATERAL (SELECT log_dt,log_user,mobj_id FROM log WHERE mobj_id=obj.id ORDER BY log_dt ASC  LIMIT 1)hist ON hist.mobj_id=obj.id
    --WHERE obj.id>4000 
    _dt_insert :=COALESCE( _dt_insert, '2016.10.05 00:00:00'::TIMESTAMP);
    _usr_insert:=COALESCE( _usr_insert, 'postgres');

    SELECT log_dt,log_user INTO _dt_update,_usr_update FROM log WHERE mobj_id=obj.id ORDER BY log_dt DESC LIMIT 1;
    _dt_update :=COALESCE( _dt_update,  _dt_insert);
    _usr_update:=COALESCE( _usr_update, _usr_insert);


    UPDATE obj_name SET dt_insert=_dt_insert
                      , dt_update=_dt_update 
                      , usr_insert=_usr_insert
                      , usr_update=_usr_update 
      WHERE id=obj.id;

  END LOOP;--FOR cursor_obj

  ALTER TABLE obj_name ALTER COLUMN dt_insert SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN dt_update SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN usr_insert SET NOT NULL;
  ALTER TABLE obj_name ALTER COLUMN usr_update SET NOT NULL;

  ALTER TABLE acls ALTER COLUMN dt_insert SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN dt_update SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN usr_insert SET NOT NULL;
  ALTER TABLE acls ALTER COLUMN usr_update SET NOT NULL;

  ALTER TABLE ref_cls_act ADD COLUMN period interval;

--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
/**
SELECT * FROM LOG 
WHERE 
prop->>'115' ~~* 'Я балбес%'
AND act_id=102
ORDER BY mobj_id, log_dt

*/

-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

select * from pg_temp.add_obj_dt_insert();



