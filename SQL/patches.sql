-------------------------------------------------------------------------------
-- Перемещение всего проверенного из Пункта проката в Пункт выдачи
------------------------------------------------------------------------------
/**
DROP FUNCTION IF EXISTS move_from_pp_to_pv() CASCADE;
CREATE OR REPLACE FUNCTION move_from_pp_to_pv()
 RETURNS VOID  AS
$BODY$
DECLARE
  pp_obj CURSOR IS 
    SELECT obj.id, obj.pid, obj.title, cls_id, prop, qty, move_logid, act_logid, cls_kind
    FROM obj
    WHERE obj.pid=(SELECT id FROM obj_name WHERE title='Пункт проката')
    AND move_logid IS NULL;

    _oid_pv BIGINT;

BEGIN
  SELECT id INTO _oid_pv FROM obj_name WHERE title='Пункт выдачи';

  FOR mobj IN pp_obj LOOP
    PERFORM lock_for_move(mobj.id, mobj.pid);
    PERFORM do_move(mobj.id, mobj.pid,_oid_pv,1);
    PERFORM lock_reset(mobj.id, mobj.pid);
  END LOOP;

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;


SELECT move_from_pp_to_pv();
DROP FUNCTION IF EXISTS move_from_pp_to_pv() CASCADE;

-- удостоверяемся что всё попало в пункт выдачи 
SELECT obj.id, obj.pid, obj.title, cls_id, prop, qty, move_logid, act_logid, cls_kind
  FROM obj
  WHERE obj.pid=(SELECT id FROM obj_name WHERE title='Пункт выдачи');
*/

-------------------------------------------------------------------------------
-- Перенос значения свойства "примечание"
-- из "Изменить основные свойства"->"примечание" в "Изменить особенность"->"Особенность прибора"
------------------------------------------------------------------------------
/**
DROP FUNCTION IF EXISTS move_note_to_spec() CASCADE;
CREATE OR REPLACE FUNCTION move_note_to_spec()
 RETURNS VOID  AS
$BODY$
DECLARE
  source CURSOR IS 
    SELECT id as log_id
          ,log_main.timemark
          ,src_path
          ,obj_id
           ,act_id
           , prop->>(SELECT id::TEXT FROM prop WHERE title='Примечание') , prop
      FROM log_detail_act 
      LEFT JOIN log_main USING (id )
      WHERE 
      act_id = (SELECT id FROM act WHERE title='Изменить основные свойства')
      AND trim(both prop->>(SELECT id::TEXT FROM prop WHERE title='Примечание')) <> ''
      --LIMIT 10
      ;

  _date     TIMESTAMP;
  _prop     JSONB;
  _src_path BIGINT[];
  _obj_id   BIGINT;
  _lid      BIGINT;
  _act_lid_previos BIGINT;
  _aid_ch_spec BIGINT;
  _pid_spec BIGINT;

  _pid_note BIGINT;

  _txt_note TEXT;
BEGIN
  RAISE NOTICE 'Start moving NOTE to SPEC';
  select id INTO _pid_spec FROM prop WHERE title = 'Особенность прибора';
  SELECT id INTO _pid_note FROM prop WHERE title='Примечание';

  SELECT id INTO _aid_ch_spec FROM act WHERE title='Изменить особенность';

  FOR source IN source LOOP
    --RAISE NOTICE 'Prop = %', source.prop;
    --_date    := source.timemark - '00:10:00';
    --_src_path:= source._src_path;
    --_obj_id  := source.obj_id;
    _prop    := source.prop - _pid_note::TEXT;
    --RAISE NOTICE '-prop = %', _prop;
    _txt_note:=to_json(source.prop->>(_pid_note::TEXT));
    --RAISE NOTICE 'note = %', _txt_note;
    _prop    := _prop || format('{"%s":%s}', _pid_spec, _txt_note)::JSONB;
    --RAISE NOTICE '+prop = %', _prop;

    UPDATE log_detail_act SET 
      prop = _prop 
      ,act_id = _aid_ch_spec
      WHERE id=source.log_id;

  END LOOP;

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;


SELECT move_note_to_spec();
DROP FUNCTION IF EXISTS move_note_to_spec() CASCADE;
*/
-------------------------------------------------------------------------------
-- Добавление особенности в текущее состояние прибора
------------------------------------------------------------------------------
SET client_min_messages = 'debug';
DROP FUNCTION IF EXISTS move_spec_to_obj() CASCADE;
CREATE OR REPLACE FUNCTION move_spec_to_obj()
 RETURNS VOID  AS
$BODY$
DECLARE
  source CURSOR IS 
    SELECT id as log_id
          ,log_main.timemark
          ,src_path
          ,obj_id
           ,act_id
           , prop->>(SELECT id::TEXT FROM prop WHERE title='Особенность прибора') , prop
      FROM log_detail_act 
      LEFT JOIN log_main USING (id )
      WHERE 
      act_id = (SELECT id FROM act WHERE title='Изменить особенность')
      AND log_main.username='postgres'
      ;
  _pid_spec BIGINT;
  _txt_spec TEXT;
BEGIN
  RAISE NOTICE 'Start moving NOTE to SPEC';
  select id INTO _pid_spec FROM prop WHERE title = 'Особенность прибора';

  FOR source IN source LOOP
    _txt_spec := to_json(source.prop->>(_pid_spec::TEXT));

    UPDATE obj_name
     SET prop= prop || format('{"%s":%s}', _pid_spec, _txt_spec)::JSONB
     WHERE obj_name.id=source.obj_id;

    UPDATE log_detail_act SET 
      prop = prop || format('{"%s":%s}', _pid_spec, _txt_spec)::JSONB
      WHERE id IN
      (SELECT id FROM log_main lm WHERE lm.timemark>source.timemark AND lm.obj_id=source.obj_id )
      ;

  END LOOP;
--RAISE EXCEPTION 'END moving NOTE to SPEC';
RETURN;
END; 
$BODY$ LANGUAGE plpgsql;


SELECT move_spec_to_obj();
DROP FUNCTION IF EXISTS move_spec_to_obj() CASCADE;























