
SET client_min_messages='debug1';

-------------------------------------------------------------------------------
-- система бизнес блокировок перемещения и действий b(block) - префикс
-- для номерных bn
-- для количественных bq
-- noid - Named object identificator 
-- qoid - Quantitative object identificator 


DROP TABLE IF EXISTS lock_obj CASCADE;
DROP TABLE IF EXISTS lock_dst CASCADE;
DROP TABLE IF EXISTS lock_act CASCADE;


-------------------------------------------------------------------------------
--таблица блокировки объектов
DROP TABLE IF EXISTS lock_obj CASCADE;
CREATE TABLE lock_obj(
   lock_session INTEGER     NOT NULL DEFAULT pg_backend_pid()
  ,lock_user    NAME        NOT NULL DEFAULT CURRENT_USER
     REFERENCES wh_role( rolname ) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
  ,lock_time    TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
     

  ,oid      BIGINT      NOT NULL
    REFERENCES obj_name( id ) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
  ,pid      BIGINT      NOT NULL

  ,path     BIGINT[]


  ,CONSTRAINT lock_obj__oid_pid_pkey PRIMARY KEY (oid,pid)

);

-------------------------------------------------------------------------------
--таблица блокировки разрешённых путей назначения
DROP TABLE IF EXISTS lock_dst CASCADE;
CREATE TABLE lock_dst
(
   oid BIGINT  NOT NULL
  ,pid BIGINT  NOT NULL

  ,dst_path BIGINT[]

  ,CONSTRAINT lock_dst_oid_pid_fkey FOREIGN KEY (oid,pid)
      REFERENCES lock_obj (oid,pid) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);

-------------------------------------------------------------------------------
--таблица блокировки разрешённых действий
DROP TABLE IF EXISTS lock_act CASCADE;
CREATE TABLE lock_act
(
  oid  BIGINT  NOT NULL,
  pid BIGINT  NOT NULL,

  act_id INTEGER,

  CONSTRAINT lock_act_oid_pid_fkey FOREIGN KEY (oid,pid)
      REFERENCES lock_obj (oid,pid) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);



-------------------------------------------------------------------------------
-- проверка блокировки + очистка своих просроченых блокировок
-- разрешение действия кэшировано и разрешено в таблице блокировок

DROP FUNCTION IF EXISTS    try_lock_obj(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION try_lock_obj(IN _oid  BIGINT, IN _pid BIGINT)
    RETURNS RECORD
    AS $BODY$
DECLARE
    _locked_rec RECORD;
    _obj_rec RECORD;
BEGIN
  -- удаляем просроченые и свои же блокировки на объект, если таковые имеются
  DELETE 
    FROM lock_obj
    WHERE oid=_oid AND pid=_pid
    AND ( lock_time +'00:10:00.00' < now() OR lock_session = pg_backend_pid());
-- Ищем блокируемый объект
  SELECT id,pid,(SELECT pathid FROM fget_objnum_pathinfo_table(pid) WHERE _obj_pid=1) AS path 
    INTO _obj_rec 
    FROM obj 
    WHERE id=_oid AND pid=_pid;
  --RAISE DEBUG 'try_lock_obj: finded object %',_obj_rec;
-- пытаемся вставить в табличку блокировок, если уже блокирован, то исключение откатит транзакцию
  IF FOUND THEN
    INSERT INTO lock_obj(oid,pid,path) 
      VALUES (_obj_rec.id,_obj_rec.pid,_obj_rec.path) RETURNING * INTO _locked_rec;
    --RAISE DEBUG 'try_lock_obj: inserted object %',_locked_rec;
    RETURN _locked_rec;
  END IF;
  RETURN NULL;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

SELECT try_lock_obj(103,100);
SELECT try_lock_obj(100,1);
SELECT try_lock_obj(666,664);


-------------------------------------------------------------------------------
-- разблокировка объекта
-- в случае номерного объекта pid не учитывается
DROP FUNCTION IF EXISTS    lock_reset(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_reset(IN _oid  BIGINT, IN _pid BIGINT) 
    RETURNS  VOID AS 
 $BODY$
    DELETE FROM lock_obj 
    WHERE 
    lock_session=pg_backend_pid() AND oid=_oid 
    AND (
            ((SELECT cls_kind FROM obj WHERE id=_oid) > 1 AND pid=_pid )
          OR (SELECT cls_kind FROM obj WHERE id=_oid) = 1
        );
$BODY$
  LANGUAGE sql VOLATILE  COST 500;

SELECT lock_reset(100,100);
SELECT lock_reset(666,664);
SELECT lock_reset(103,NULL);


-----------------------------------------------------------------------------------------------------------------------------
-- блокировать объекты, если правило при проверке изменится или перестанет существовать, то пофиг
-- разрешение действия кэшировано и разрешено в таблице блокировок
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    lock_for_act(IN _oid  BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_for_act(IN _oid  BIGINT)
    --RETURNS TABLE(_act_id INTEGER, _act_label NAME)
    RETURNS SETOF act
    AS $BODY$
DECLARE
    _acts CURSOR(_curr_path TEXT, _cls_id BIGINT) IS
    SELECT act.* from (
    SELECT perm.cls_id,perm.obj_id,perm.act_id,perm.src_path, sum(access_disabled) 
        FROM perm_act perm
        LEFT JOIN wh_role _group ON perm.access_group=_group.rolname-- определяем ИМЕНА разрешённых групп
        RIGHT JOIN    wh_auth_members membership ON _group.id=membership.roleid -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
        RIGHT JOIN wh_role _user ON _user.id=membership.member -- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
          AND _user.rolname=CURRENT_USER -- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО
      WHERE
        perm.cls_id = _cls_id -- 100
        AND (obj_id IS NULL OR obj_id = _oid) -- 101)
        AND (src_path IS NULL OR _curr_path LIKE src_path::TEXT)
      GROUP BY cls_id,obj_id,act_id, src_path
      ) t 
      LEFT JOIN act ON t.act_id= act.id 
      WHERE t.sum=0;


    _cls_id   BIGINT;
    _pid      BIGINT;
    _locked_rec   RECORD;
BEGIN
-- пытаемся найти объект и его местоположение
  SELECT pid,cls_id INTO _pid,_cls_id FROM obj WHERE id=_oid;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _oid;
  END IF;
  -- пытаемся заблокировать объект, если блокировка не удастся транзакция откатится исключением
  _locked_rec := try_lock_obj(_oid, _pid);
  -- объект блокирован
  -- определяем разрешенные действия и спихиваем их в выходную табличку и в табличку блокировки
  FOR rec IN _acts( get_path(_pid), _cls_id) LOOP
    return next rec;                        -- заполняем табличку разрешённых действий
    -- заполняем табличку идентификаторов разрешённых действий 
    INSERT INTO lock_act(oid, pid, act_id)VALUES (_oid, _pid, rec.id);
  END LOOP;

  IF NOT FOUND THEN
    PERFORM lock_reset(_oid,_pid);
    RAISE DEBUG ' There are no action, lock aborted';
  ELSE
    RAISE DEBUG ' Object LOCKED cls_id=% obj_id=% ',_cls_id, _oid;
  END IF;
    
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;



-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция выполнения действия
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_obj_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB) CASCADE;

CREATE OR REPLACE FUNCTION do_obj_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB)
                  RETURNS  VOID AS 
$BODY$
DECLARE
    _lock_info   RECORD;
    _last_log_id BIGINT;
    _prop_str    TEXT;
    _prop_item   TEXT;
    _pathid      BIGINT[];
    _cls_id      BIGINT;
    _old_prop    JSONB;

  _chk_props CURSOR IS

SELECT distinct( ref_act_prop.prop_id) as id, pold.value
       ,all_new.id as new_id,all_new.value
       ,CASE WHEN all_new.id IS NOT NULL THEN all_new.value ELSE pold.value END AS result
       --, COALESCE( all_new.value, pold.value) as result
  FROM  ref_cls_act
  RIGHT JOIN ref_act_prop  ON ref_act_prop.act_id  = ref_cls_act.act_id
  LEFT JOIN (select key::BIGINT, value::TEXT from jsonb_each(_old_prop)) pold ON pold.key=ref_act_prop.prop_id
  LEFT JOIN(
             SELECT ref_act_prop.prop_id as id, pnew.value
             FROM  ref_cls_act
             RIGHT JOIN ref_act_prop  ON ref_act_prop.act_id  = ref_cls_act.act_id
             LEFT JOIN (select key::BIGINT, value::TEXT from jsonb_each(_prop)) pnew ON pnew.key=ref_act_prop.prop_id
             WHERE ref_cls_act.cls_id=_cls_id AND ref_cls_act.act_id=_act_id
             AND ref_act_prop.prop_id IS NOT NULL 
            ) all_new ON all_new.id=ref_act_prop.prop_id
  WHERE ref_cls_act.cls_id=_cls_id 
  AND ( (all_new.id IS NOT NULL AND all_new.value IS NOT NULL) OR (all_new.id IS NULL  AND pold.value IS NOT NULL)) ;

BEGIN
  -- проверяем - заблокирован ли объект для действия
  SELECT * INTO _lock_info 
    FROM lock_obj
    LEFT JOIN lock_act USING (oid)
    WHERE
      oid = _obj_id
      AND lock_time +'00:10:00.00' > now()
      AND lock_session = pg_backend_pid()
      --AND ((src_path IS NULL AND _src_path IS NULL) OR src_path=_src_path)
      AND act_id = _act_id;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not locked obj_id=% ', _obj_id;
  END IF;

  -- пытаемся найти объект и его местоположение
  SELECT cls_id,prop INTO _cls_id,_old_prop FROM obj WHERE id=_obj_id;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _obj_id;
  END IF;

  FOR rec IN _chk_props LOOP
    _prop_item:= concat_ws(':',quote_ident(rec.id::TEXT), rec.result );
    _prop_str := concat_ws(',',_prop_str, _prop_item );
  END LOOP;
  _prop_str:='{'||_prop_str||'}';
  RAISE DEBUG '_prop_str: %',_prop_str;

  _last_log_id := nextval('seq_log_id');

  SELECT pathid INTO _pathid FROM fget_objnum_pathinfo_table(_lock_info.pid) WHERE _obj_pid=1;

  UPDATE obj_num SET act_logid=_last_log_id, prop=_prop_str::JSONB WHERE id=_obj_id;

  INSERT INTO log_act(id,  act_id, prop,   obj_id,  src_path)
    VALUES (_last_log_id, _act_id, _prop_str::JSONB, _obj_id,_pathid);


END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;

SELECT * FROM lock_for_act(1639);

SELECT do_obj_act(1639, 101, '{"100":"qwe","106":55,"105":"asd"}');
SELECT do_obj_act(1639, 100, '{"105":"dddddddddd"}');


SELECT lock_reset(1639,NULL);


-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция  
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_log_state( IN _cls_id   INTEGER,  IN _obj_id   BIGINT,   IN _act_id  INTEGER, IN _last_log_id BIGINT,
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[], IN _old_pid BIGINT,  IN _new_pid     BIGINT) CASCADE;

CREATE OR REPLACE FUNCTION do_log_state( IN _cls_id   INTEGER,  IN _obj_id   BIGINT,   IN _act_id  INTEGER, IN _last_log_id BIGINT,
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[], IN _old_pid BIGINT,  IN _new_pid     BIGINT) 
                  RETURNS  VOID AS 
$BODY$
DECLARE
    _hdr_str TEXT;
    _val_str TEXT;
    _all_prop_names CURSOR IS
        SELECT DISTINCT ON(t_prop.id) t_prop.*
          FROM  t_ref_class_act
          LEFT JOIN t_ref_act_prop  ON t_ref_act_prop.act_id = t_ref_class_act.act_id
          LEFT JOIN t_prop          ON t_ref_act_prop.prop_id = t_prop.id 
          WHERE t_ref_class_act.cls_id=_cls_id;

    _insert_log_query TEXT;
    _select_state     TEXT;
    _prop_val_array     TEXT[];
    
    _hdr_qty INTEGER;
BEGIN
    _hdr_qty :=0;
    _hdr_str :='';
    _val_str :='';
    FOR rec IN _all_prop_names LOOP
        _hdr_str := concat_ws(',',_hdr_str,quote_ident(rec.label) );
        _hdr_qty := _hdr_qty+1;
    END LOOP;


    IF _hdr_str<>'' THEN
        _hdr_str:=TRIM(leading ',' from _hdr_str);
        _select_state := format( 'SELECT ARRAY[%s] FROM t_state_%s WHERE obj_id=%s'
                                     ,_hdr_str, _cls_id, _obj_id);
        RAISE DEBUG '_select_state= %',_select_state;
        EXECUTE _select_state INTO _prop_val_array;
        
        FOR i IN 1.._hdr_qty LOOP
            _val_str := concat_ws(',',_val_str,COALESCE(quote_literal(_prop_val_array[i]),'NULL'));
        END LOOP;
        _val_str:=TRIM(leading ',' from _val_str);

        _hdr_str:=_hdr_str||',';
        _val_str:=_val_str||',';
    END IF;

    RAISE DEBUG '_hdr_str: %',_hdr_str;
    RAISE DEBUG '_val_str: %',_val_str;

    _insert_log_query:= format(
        'INSERT INTO t_log_%s( %s log_id, cls_id, obj_id, act_id, src_path, dst_path, old_obj_pid, new_obj_pid)'
        'VALUES(%s %s, %s, %s, %s, %L, %L, %L, %L)'
        ,_cls_id
        ,_hdr_str
        ,_val_str, _last_log_id, _cls_id, _obj_id, _act_id, _src_path, _dst_path, _old_pid, _new_pid );
    RAISE DEBUG 'insert_log_str: %',_insert_log_query;

    EXECUTE _insert_log_query;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;


/*

 insert_log_str: INSERT INTO t_log_105
 ( "рем2","тест1_2","рем1", 
 log_id, cls_id, obj_id, act_id, src_path, dst_path, old_obj_pid, new_obj_pid)
 VALUES
 ('bla-bla2','asd',NULL, 118, 105, 103, 1, '{101}', '{101}', '101', '101')

SELECT TRIM(both '{}' from
(
SELECT ARRAY["рем1","рем2","тест1_2"]::TEXT
FROM t_state_105 WHERE obj_id=103
)
)


SELECT ARRAY["рем1","рем2","тест1_2"]
FROM t_state_105 WHERE obj_id=103

SELECT TRIM(trailing ',' from 'sdfsdf,fsdf,sfds,fds,f,g');

SELECT COALESCE(NULL, 'alt')


    SELECT * FROM t_objnum 
    RIGHT JOIN t_state_105 ON id= obj_id
    WHERE t_objnum.cls_id=105 AND id=103


SELECT * --t_prop.*-- , user_data.col2 AS val
    FROM  t_ref_class_act
    RIGHT JOIN t_ref_act_prop  USING(act_id)
    LEFT  JOIN t_prop          ON t_prop.id = t_ref_act_prop.prop_id

    LEFT JOIN (SELECT * FROM fn_array2_to_table('{{рем1,bla-bla},{тест1_2,asd},{qwe,qwe}}'::TEXT[]))user_data ON user_data.col1=t_prop.label
    WHERE t_ref_class_act.cls_id=105 AND t_ref_class_act.act_id=2




        SELECT DISTINCT ON(t_prop.id) t_prop.*
          FROM  t_ref_class_act
          LEFT JOIN t_ref_act_prop  ON t_ref_act_prop.act_id = t_ref_class_act.act_id
          LEFT JOIN t_prop          ON t_ref_act_prop.prop_id = t_prop.id 
          WHERE t_ref_class_act.cls_id=105;





    SELECT column_name FROM information_schema.columns 
        WHERE table_name='t_state_105'
        EXCEPT ALL SELECT column_name FROM information_schema.columns WHERE table_name='t_state';
*/



















-----------------------------------------------------------------------------------------------------------------------------
-- блокировать исходные и конечные объекты, если правило при проверке перестанет существовать, то пофиг
-- перемещение исполнится, т.к. разрешение уже получено и хранится в t_lock_obj
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    lock_for_move( IN _cls_id  INTEGER, IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_for_move( IN _cls_id  INTEGER, IN _obj_id  BIGINT, IN _old_pid BIGINT) 
                  --RETURNS SETOF moverule_lockup AS $BODY$
    RETURNS TABLE(_dst_obj_id BIGINT, _dst_cls_id INTEGER, _dst_obj_label NAME,_dst_obj_pid BIGINT)
    AS $BODY$
DECLARE

    _dst_obj CURSOR IS
        SELECT dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid
        FROM (
              SELECT 
                dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid,sum(perm_access_disabled)
                FROM moverule_lockup WHERE
                    mov_cls_id = _cls_id -- NOT NULL
                AND (mov_obj_pid IS NULL OR mov_obj_pid = _old_pid)
                AND (mov_obj_id  IS NULL OR mov_obj_id = _obj_id)
                GROUP BY dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid
             )t
            WHERE sum=0;

   _dst_path BIGINT[];
   _locked_rec RECORD;
BEGIN
-- пытаемся заблокировать объект, если блокировка не удастся транзакция откатится исключением
  _locked_rec := try_lock_obj(_cls_id, _obj_id, _old_pid);
-- объект блокирован
-- возвращаем пользвателю объекты назначения, попутно складываем их местоположение  в табличку
  FOR rec IN _dst_obj LOOP
    _dst_path := (SELECT fget_get_oid_path(rec.dst_obj_id));

    INSERT INTO t_lock_dst(cls_id,obj_id,obj_pid, dst_path)VALUES(_cls_id,_obj_id,_old_pid,_dst_path); 

    _dst_obj_id:=rec.dst_obj_id;
    _dst_cls_id:=rec.dst_cls_id;
    _dst_obj_label:=rec.dst_obj_label;
    _dst_obj_pid:=rec.dst_obj_pid;
    RETURN NEXT;
  END LOOP;

  RETURN;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

--------------------------------------------------------------------------------------------------------------
-- проверяем - есть ли разрешения на перенос в табличке блокировок  
--------------------------------------------------------------------------------------------------------------
/*
DROP FUNCTION IF EXISTS    lock_move_check( IN _cls_id  INTEGER, IN _obj_id  BIGINT, IN _old_pid BIGINT,
                                       IN _src_path BIGINT[], IN _dst_path BIGINT[]) CASCADE;
CREATE OR REPLACE FUNCTION lock_move_check( IN _cls_id  INTEGER, IN _obj_id  BIGINT, IN _old_pid BIGINT,
                                       IN _src_path BIGINT[], IN _dst_path BIGINT[]) 
    RETURNS  BIGINT AS 
$BODY$
DECLARE
   _result BIGINT;
   --_old_pid BIGINT;
BEGIN
    --_old_pid:= ( CASE WHEN _src_path[1] IS NULL THEN 1 ELSE _src_path[1] END );
    
    
    SELECT count(*) INTO _result 
        FROM t_lock_obj
        INNER JOIN t_lock_dst USING (cls_id,obj_id,obj_pid)
        WHERE cls_id=_cls_id AND obj_id=_obj_id AND obj_pid=_old_pid
        AND lock_time +'00:10:00.00' > now()
        AND session_pid = pg_backend_pid()
        AND ((src_path IS NULL AND _src_path IS NULL) OR src_path=_src_path)
        AND ((dst_path IS NULL AND _dst_path IS NULL) OR dst_path=_dst_path);

    RETURN _result;
END;
$BODY$
  LANGUAGE plpgsql STABLE COST 500;


--------------------------------------------------------------------------------------------------------------


SELECT * FROM lock_for_move(104,102,1);
SELECT * FROM lock_move_check(104,102,1,NULL,'{103}');
SELECT lock_reset(104,102,1);

*/

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция перемещения объекта
-----------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    LOG_QTY_MOVE( IN _cls_id INTEGER, IN _obj_id BIGINT, 
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[], 
                                         IN _qty NUMERIC ) CASCADE;

CREATE OR REPLACE FUNCTION LOG_QTY_MOVE( IN _cls_id INTEGER, IN _obj_id BIGINT, 
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[], 
                                         IN _qty NUMERIC )
                  RETURNS  VOID AS $BODY$
DECLARE
    _old_pid BIGINT;
    _new_pid BIGINT;

    _src_qty NUMERIC;
    _dst_qty NUMERIC;

    _move_logid BIGINT;
BEGIN
-- если пути пустые, то считаем что объекты в корне
  _old_pid:= ( CASE WHEN _src_path[1] IS NULL THEN 1 ELSE _src_path[1] END );
  _new_pid:= ( CASE WHEN _dst_path[1] IS NULL THEN 1 ELSE _dst_path[1] END );
  RAISE DEBUG '_old_pid = %', _old_pid;
  RAISE DEBUG '_new_pid = %', _new_pid;
-- находим исходное количество и количество в месте назначения
  SELECT qty INTO _src_qty FROM t_objqty WHERE pid = _old_pid AND objqty_id=_obj_id;
  SELECT qty INTO _dst_qty FROM t_objqty WHERE pid = _new_pid AND objqty_id=_obj_id;

  CASE
    WHEN _qty < _src_qty  THEN -- div разделяем исходное количество
      UPDATE t_objqty SET qty= (_src_qty - _qty)
                      WHERE pid = _old_pid AND objqty_id=_obj_id;      -- уменьшаем исходное количество
      IF _dst_qty IS NOT NULL THEN                                     -- если в месте назначения есть уже такой объёкт
           UPDATE t_objqty SET qty= (_dst_qty + _qty)
                           WHERE pid = _new_pid AND objqty_id=_obj_id; -- добавляем (обновляем имеющееся количество)
       ELSE                                                            -- если в месте назначения объекта нет
           INSERT INTO t_objqty(objqty_id, pid, qty)                   -- вставляем 
                        VALUES (_obj_id, _new_pid, _qty);
       END IF;
    WHEN _qty = _src_qty THEN -- move перемещение
        IF _dst_qty IS NOT NULL THEN
           UPDATE t_objqty SET qty= (_dst_qty + _qty)
                           WHERE pid = _new_pid AND objqty_id=_obj_id;
           DELETE FROM t_objqty 
                           WHERE pid = _old_pid AND objqty_id=_obj_id;
        ELSE
           UPDATE t_objqty SET pid = _new_pid
                           WHERE pid = _old_pid AND objqty_id=_obj_id;
        END IF;
    ELSE
        RAISE EXCEPTION 'Wrong qty or unknown error'; 
  END CASE;

  _move_logid := nextval('seq_log_id');
-- обновляем ссылку на последнее действие в исходном и конечном объектах
  UPDATE t_objqty SET last_log_id=_last_log_id
    WHERE objqty_id=_obj_id AND ( pid=_old_pid OR pid=_new_pid);

  INSERT INTO log_move_objqty( id
                              ,src_objnum_id
                              ,src_path
                              ,dst_objnum_id
                              ,dst_path
                              ,objqty_id
                              , qty)
                              VALUES
                             (_move_logid
                              ,_old_pid
                              ,_src_path
                              ,_new_pid
                              ,_dst_path
                              ,_obj_id
                              ,_qty);

RETURN;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;




-----------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    LOG_NUM_MOVE( IN _cls_id INTEGER, IN _obj_id BIGINT, 
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[]) CASCADE;

CREATE OR REPLACE FUNCTION LOG_NUM_MOVE( IN _cls_id INTEGER, IN _obj_id BIGINT, 
                                         IN _src_path BIGINT[], IN _dst_path BIGINT[])
                  RETURNS  VOID AS $BODY$
DECLARE
    _old_pid BIGINT;
    _new_pid BIGINT;
    _move_logid BIGINT;
BEGIN
    _old_pid:= ( CASE WHEN _src_path[1] IS NULL THEN 1 ELSE _src_path[1] END );
    _new_pid:= ( CASE WHEN _dst_path[1] IS NULL THEN 1 ELSE _dst_path[1] END );

    IF _old_pid=_new_pid THEN
        RAISE EXCEPTION 'Object already here'; 
    END IF;

    _move_logid := nextval('seq_log_id');

    UPDATE t_objnum SET pid = _new_pid, move_logid=_move_logid WHERE id=_obj_id;


    INSERT INTO log_move_objnum( id
                                ,src_objnum_id
                                ,src_path
                                ,dst_objnum_id
                                ,dst_path
                                ,objnum_id)
                                VALUES
                               (_move_logid
                                ,_old_pid
                                ,_src_path
                                ,_new_pid
                                ,_dst_path
                                ,_obj_id);
    
    --PERFORM do_log_state(_cls_id, _obj_id, 0, _last_log_id, _src_path, _dst_path, _old_pid, _new_pid);

RETURN;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;



-----------------------------------------------------------------------------------------------
-- функция перемещения номерных объектов
-----------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    move_object( IN _obj_id  BIGINT, IN _old_pid BIGINT,  IN _new_pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION move_object( IN _obj_id  BIGINT, IN _old_pid BIGINT,  IN _new_pid BIGINT) 
  RETURNS  VOID AS 
$BODY$ 
DECLARE
  _cls_id INTEGER;

BEGIN
-- проверяем что объект блокирован
  SELECT count(*) INTO _access_granted 
   FROM t_lock_obj
   INNER JOIN t_lock_dst USING (cls_id,obj_id,obj_pid)
     WHERE lock_time +'00:10:00.00' > now()
       AND session_pid = pg_backend_pid()
       AND ((src_path IS NULL AND _src_path IS NULL) OR src_path=_src_path)
       AND ((dst_path IS NULL AND _dst_path IS NULL) OR dst_path=_dst_path)
       AND 

cls_id=_cls_id AND obj_id=_obj_id AND obj_pid=_old_pid
     
  RAISE DEBUG '_access_granted = %', _access_granted;










  SELECT FROM t_objnum
    INNER JOIN t_clsnum USING (cls_id)
    WHERE t_objnum.id = 105;
  
      IF NOT FOUND THEN
        RAISE EXCEPTION ' Object not exists cls_id=% obj_id=% ',_cls_id, _obj_id;
    END IF;


END;
$BODY$ LANGUAGE plpgsql VOLATILE  COST 500;
















DROP FUNCTION IF EXISTS    move_object( IN _cls_id  INTEGER, IN _obj_id  BIGINT, 
                                        IN _old_pid BIGINT,  IN _new_pid BIGINT, IN _qty NUMERIC ) CASCADE;

CREATE OR REPLACE FUNCTION move_object( IN _cls_id  INTEGER, IN _obj_id  BIGINT, 
                                        IN _old_pid BIGINT,  IN _new_pid BIGINT, IN _qty NUMERIC ) 
                  RETURNS  VOID AS $BODY$
DECLARE
    _src_cls_type SMALLINT;
    _src_path BIGINT[];
    _dst_path BIGINT[];
    _access_granted INTEGER; -- запрещение перемещения по результатам суммы правил
BEGIN
--  определяем 
    SELECT type INTO _src_cls_type FROM w_obj 
        WHERE cls_id=_cls_id AND obj_id=_obj_id AND obj_pid=_old_pid ;

    _src_path := (SELECT fget_get_oid_path(_old_pid));
    _dst_path := (SELECT fget_get_oid_path(_new_pid));
    RAISE DEBUG '_src_path = %', _src_path;
    RAISE DEBUG '_dst_path = %', _dst_path;

    SELECT count(*) INTO _access_granted 
        FROM t_lock_obj
        INNER JOIN t_lock_dst USING (cls_id,obj_id,obj_pid)
        WHERE cls_id=_cls_id AND obj_id=_obj_id AND obj_pid=_old_pid
        AND lock_time +'00:10:00.00' > now()
        AND session_pid = pg_backend_pid()
        AND ((src_path IS NULL AND _src_path IS NULL) OR src_path=_src_path)
        AND ((dst_path IS NULL AND _dst_path IS NULL) OR dst_path=_dst_path);
    RAISE DEBUG '_access_granted = %', _access_granted;
    
    IF _access_granted IS NULL OR _access_granted<1  THEN
            RAISE EXCEPTION 'No permission or permission denied for move';
    END IF;

    CASE
    WHEN _src_cls_type=1 THEN
        PERFORM LOG_NUM_MOVE(_cls_id, _obj_id, _src_path, _dst_path);
    WHEN _src_cls_type=2 OR _src_cls_type=3 THEN
        PERFORM LOG_QTY_MOVE(_cls_id, _obj_id, _src_path, _dst_path, _qty);
    ELSE
        RAISE EXCEPTION 'Wrong _src_cls_type %', _src_cls_type; 
    END CASE;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

  SELECT fget_get_oid_path(103)

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения количественных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,10); /*div to NULL*/ SELECT lock_reset(103,104,1);
SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*div to 10*/ SELECT lock_reset(103,104,1);
SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*mov to 15*/ SELECT lock_reset(103,104,1);
SELECT * FROM lock_for_move(103,104,100); SELECT move_object(103,104,100,1,20); /*mov to NULL*/ SELECT lock_reset(103,104,100);
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения номерных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
SELECT * FROM lock_for_move(104,102,1);
SELECT move_object(104,102,1,103,1); -- mov objnum
SELECT lock_reset(104,102,1);

SELECT * FROM lock_for_move(104,102,103);
SELECT move_object(104,102,103,1,1); -- mov objnum
SELECT lock_reset(104,102,103);






/**
    SELECT *
        FROM moverule_lockup 
        WHERE
                mov_cls_id = 103 -- NOT NULL
            AND (mov_obj_pid IS NULL OR mov_obj_pid = 1)
            AND (mov_obj_id  IS NULL OR mov_obj_id = 104)

            AND dst_cls_id = 101 -- NOT NULL
            AND (dst_obj_id IS NULL OR dst_obj_id = 100 )




    SELECT *
        FROM moverule_lockup 
        WHERE
                mov_cls_id = 104 -- NOT NULL
            AND (mov_obj_pid IS NULL OR mov_obj_pid = 1)
            AND (mov_obj_id  IS NULL OR mov_obj_id = 102)

            AND dst_cls_id = 105 -- NOT NULL
            AND (dst_obj_id IS NULL OR dst_obj_id = 103 );

SELECT * FROM w_obj WHERE cls_id=103 AND obj_pid=1 AND obj_id=104;
*/



/*
SELECT dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid FROM 
(
        SELECT 
        dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid,sum(perm_access_disabled)
            FROM moverule_lockup WHERE
                mov_cls_id = 104 -- NOT NULL
            AND mov_obj_pid = 1
            AND mov_obj_id = 102
            GROUP BY dst_obj_id, dst_cls_id, dst_obj_label ,dst_obj_pid
            )t
            WHERE sum=0



    SELECT (SELECT fn_oidarray_to_path(fget_get_oid_path(dst_obj_id))) AS dst_path, *
        FROM moverule_lockup 
        WHERE
                mov_cls_id = 104 -- NOT NULL
            AND mov_obj_pid = 1
            AND mov_obj_id = 102
           
            AND mov_obj_pid <> dst_obj_id
            

            AND dst_cls_id = 105 -- NOT NULL
            AND (dst_obj_id IS NULL OR dst_obj_id = 103 );


*/





















