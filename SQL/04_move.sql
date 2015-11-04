
SET client_min_messages='debug1';

-------------------------------------------------------------------------------
-- система бизнес блокировок перемещения и действий 
-- записываем в lock_obj текущее местоположение объекта,
-- чтоб уж явно никто не начал двигать этот объект
-- в lock_act записываем возможные варианты действий ??? чтоб блокировать изменение самих действий
-- в lock_dst записываем возможные варианты перемещений объекта
-- при выполнении do_act/do_move проверяем начальное положение, указанное при блокировке,
-- если было изменение в пути - проверяем ещё раз возможность перемещения при изменении путей источника



DROP TABLE IF EXISTS lock_obj CASCADE;
DROP TABLE IF EXISTS lock_dst CASCADE;
DROP TABLE IF EXISTS lock_act CASCADE;

DROP VIEW IF EXISTS moverule_lockup CASCADE;

DROP FUNCTION IF EXISTS    try_lock_obj(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    lock_reset(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    lock_for_act(IN _oid  BIGINT, IN _opid  BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB) CASCADE;
DROP FUNCTION IF EXISTS    lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    do_move( _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT
                                   , IN _qty NUMERIC ) CASCADE;

-------------------------------------------------------------------------------
--таблица блокировки объектов
DROP TABLE IF EXISTS lock_obj CASCADE;
CREATE UNLOGGED TABLE lock_obj(
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
CREATE INDEX idx_lock_obj_lock_user ON lock_obj USING btree (lock_user);

GRANT SELECT        ON TABLE lock_obj  TO "Guest";
GRANT INSERT        ON TABLE lock_obj  TO "User";
GRANT DELETE        ON TABLE lock_obj  TO "User";


-------------------------------------------------------------------------------
--таблица блокировки разрешённых путей назначения
DROP TABLE IF EXISTS lock_dst CASCADE;
CREATE UNLOGGED TABLE lock_dst
(
   oid BIGINT  NOT NULL
  ,pid BIGINT  NOT NULL

  ,dst_path BIGINT[]

  ,CONSTRAINT lock_dst_oid_pid_fkey FOREIGN KEY (oid,pid)
      REFERENCES lock_obj (oid,pid) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);

GRANT SELECT        ON TABLE lock_dst  TO "Guest";
GRANT INSERT        ON TABLE lock_dst  TO "User";
GRANT DELETE        ON TABLE lock_dst  TO "User";

-------------------------------------------------------------------------------
--таблица блокировки разрешённых действий
DROP TABLE IF EXISTS lock_act CASCADE;
CREATE UNLOGGED TABLE lock_act
(
  oid  BIGINT  NOT NULL,
  pid BIGINT  NOT NULL,

  act_id INTEGER,

  CONSTRAINT lock_act_oid_pid_fkey FOREIGN KEY (oid,pid)
      REFERENCES lock_obj (oid,pid) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);

GRANT SELECT        ON TABLE lock_act  TO "Guest";
GRANT INSERT        ON TABLE lock_act  TO "User";
GRANT DELETE        ON TABLE lock_act  TO "User";

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
-- Ищем блокируемый объект в табличке объектов
  SELECT id,pid,get_path_obj_arr_2id(pid) AS path INTO _obj_rec 
    FROM obj 
    WHERE id=_oid AND pid=_pid;
  --RAISE DEBUG 'try_lock_obj: finded object %',_obj_rec;
-- пытаемся вставить в табличку блокировок, если уже блокирован, то исключение откатит транзакцию
  _obj_rec.path := COALESCE(_obj_rec.path, '{}');

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

GRANT EXECUTE ON FUNCTION try_lock_obj(IN _oid  BIGINT, IN _pid BIGINT) TO "User";
-------------------------------------------------------------------------------
-- разблокировка объекта
-- в случае номерного объекта pid не учитывается
DROP FUNCTION IF EXISTS    lock_reset(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_reset(IN _oid  BIGINT, IN _pid BIGINT) 
    RETURNS  VOID AS 
 $BODY$
    DELETE FROM lock_obj 
    WHERE 
    lock_session=pg_backend_pid() AND oid=_oid AND pid=_pid ;
    --AND (
    --        ((SELECT cls_kind FROM obj WHERE id=_oid) > 1 AND pid=_pid )
    --      OR (SELECT cls_kind FROM obj WHERE id=_oid) = 1
    --    );
$BODY$
  LANGUAGE sql VOLATILE  COST 500;

SELECT lock_reset(100,100);
SELECT lock_reset(666,664);
SELECT lock_reset(103,NULL);

GRANT EXECUTE ON FUNCTION lock_reset(IN _oid  BIGINT, IN _pid BIGINT) TO "User";
-----------------------------------------------------------------------------------------------------------------------------
-- блокировать объекты, если правило при проверке изменится или перестанет существовать, то пофиг
-- разрешение действия кэшировано и разрешено в таблице блокировок
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    lock_for_act(IN _oid  BIGINT, IN _opid  BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_for_act(IN _oid  BIGINT, IN _opid  BIGINT)
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
        --AND (src_path IS NULL OR _curr_path LIKE src_path::TEXT)
        AND _curr_path LIKE src_path
      GROUP BY cls_id,obj_id,act_id, src_path
      ) t 
      LEFT JOIN act ON t.act_id= act.id 
      WHERE t.sum=0;


    _cls_id   BIGINT;
    _txt_curr_path_2id TEXT;

    _locked_rec   RECORD;
BEGIN
-- пытаемся найти объект и его местоположение
  SELECT cls_id INTO _cls_id FROM obj WHERE id=_oid AND pid = _opid;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _oid;
  END IF;
  -- пытаемся заблокировать объект, если блокировка не удастся транзакция откатится исключением
  _locked_rec := try_lock_obj(_oid, _opid);
  -- объект блокирован
  -- определяем разрешенные действия и спихиваем их в выходную табличку и в табличку блокировки
  _txt_curr_path_2id := COALESCE(get_path_obj_arr_2id(_opid), '{}');
  -- RAISE DEBUG ' _curr_path=%    _cls_id=%', _txt_curr_path_2id,_cls_id ;
  FOR rec IN _acts( _txt_curr_path_2id, _cls_id) LOOP
    return next rec;                        -- заполняем табличку разрешённых действий
    -- заполняем табличку идентификаторов разрешённых действий 
    INSERT INTO lock_act(oid, pid, act_id)VALUES (_oid, _opid, rec.id);
  END LOOP;
  -- если не нашлись действия, то отпускаем блокировку
  IF NOT FOUND THEN 
    PERFORM lock_reset(_oid,_opid);
    RAISE DEBUG ' There are no action, lock aborted';
  ELSE
    RAISE DEBUG ' Object LOCKED cls_id=% obj_id=% ',_cls_id, _oid;
  END IF;
    
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION lock_for_act(IN _oid  BIGINT, IN _opid  BIGINT) TO "User";

SELECT id, title, note, color, script  FROM lock_for_act(103, 1);
SELECT lock_reset(103,1);
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция выполнения действия
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB) CASCADE;

CREATE OR REPLACE FUNCTION do_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB)
                  RETURNS  VOID AS 
$BODY$
DECLARE
    _lock_info   RECORD;
    _last_log_id BIGINT;
    _prop_str    TEXT;
    _prop_item   TEXT;
    _curr_pathid BIGINT[];
    _cls_id      BIGINT;
    _perm_sum    INTEGER;
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
  -- находим текущее положение и состояние
  SELECT cls_id,prop,get_path_obj_arr_2id(pid) INTO _cls_id,_old_prop,_curr_pathid 
    FROM obj WHERE id=_obj_id;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _obj_id;
  END IF;
  _curr_pathid:=COALESCE(_curr_pathid::TEXT,'{}');
  RAISE DEBUG '_curr_pathid: %',_curr_pathid;
  -- проверяем возможность выполнения выбранного действия в текущем местоположении
    SELECT sum INTO _perm_sum FROM (
    SELECT perm.act_id , sum(access_disabled) 
        FROM perm_act perm
        LEFT  JOIN wh_role _group ON perm.access_group=_group.rolname-- определяем ИМЕНА разрешённых групп
        RIGHT JOIN wh_auth_members membership ON _group.id=membership.roleid -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
        RIGHT JOIN wh_role _user ON _user.id=membership.member -- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
          AND _user.rolname=CURRENT_USER -- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО
      WHERE perm.cls_id = _cls_id
        AND perm.act_id = _act_id
        AND  (perm.obj_id IS NULL OR perm.obj_id = _obj_id)
        --AND  (perm.src_path IS NULL OR _curr_pathid::TEXT LIKE src_path::TEXT)
        AND  _curr_pathid::TEXT LIKE perm.src_path
      GROUP BY act_id
      ) t 
      WHERE t.sum=0;

   RAISE DEBUG '_perm_sum: %',COALESCE(_perm_sum,-1);

  IF _perm_sum IS NULL OR _perm_sum>0 THEN
    RAISE EXCEPTION ' Path was changed for obj_id=% , try again ', _obj_id;
  END IF;

  FOR rec IN _chk_props LOOP
    _prop_item:= concat_ws(':',quote_ident(rec.id::TEXT), rec.result );
    _prop_str := concat_ws(',',_prop_str, _prop_item );
  END LOOP;
  _prop_str:='{'||_prop_str||'}';
  RAISE DEBUG '_prop_str: %',_prop_str;

  _last_log_id := nextval('seq_log_id');

  

  UPDATE obj_name SET act_logid=_last_log_id, prop=_prop_str::JSONB WHERE id=_obj_id;

  INSERT INTO log_act(id,  act_id, prop,   obj_id,  src_path)
    VALUES (_last_log_id, _act_id, _prop_str::JSONB, _obj_id,_curr_pathid);


END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;

GRANT EXECUTE ON FUNCTION do_act(IN _obj_id BIGINT, _act_id INTEGER, IN _prop JSONB) TO "User";


SELECT id, title, note, color, script  FROM lock_for_act(103, 1);

SELECT do_act(103, 100, '{"100":66,"102":"45452ergsdfgd"}');
SELECT lock_reset(103,1);





-------------------------------------------------------------------------------
-- поиск всех возможных вариантов перемещения
-------------------------------------------------------------------------------
DROP VIEW IF EXISTS moverule_lockup CASCADE;
CREATE OR REPLACE VIEW moverule_lockup AS 
SELECT 
    obj.id          AS oid
    ,obj.title      AS otitle
    ,obj.cls_id     AS cid
    ,obj.cls_kind   AS ctype
    ,obj.qty        AS oqty 

    ,obj.pid         AS src_oid
    ,perm.src_cls_id AS src_cid
    ,perm.src_path   AS src_path

    ,dst.title       AS dst_otitle
    ,dst.id          AS dst_oid
    ,perm.dst_cls_id AS dst_cid
    ,perm.dst_path   AS dst_path
    ,dstn.pid        AS dst_opid
    
    ,perm.id              AS perm_id
    ,perm.access_disabled AS perm_access_disabled
    ,perm.script_restrict AS perm_script

FROM obj -- откуда + что

RIGHT JOIN perm_move perm -- находим все объекты классы которых удовлетворяют правилу (которые можно перемещать)
    ON   obj.cls_id IN (SELECT _id FROM get_childs_cls(perm.cls_id))
    AND perm.obj_id IS NULL OR obj.id = perm.obj_id -- отсеиваем по имени
    AND get_path_obj_arr_2id(obj.pid)::TEXT LIKE perm.src_path  -- отсеиваем по местоположению

LEFT JOIN obj_name dst -- куда
    ON perm.dst_cls_id = dst.cls_id
    AND perm.dst_obj_id IS NULL OR  dst.id = perm.dst_obj_id -- отсеиваем по имени
LEFT JOIN obj_num  dstn -- куда
    ON dstn.id = dst.id
    AND get_path_obj_arr_2id(dstn.pid)::TEXT LIKE perm.dst_path  -- отсеиваем по местоположению

-- group permission
LEFT JOIN wh_role _group 
    ON perm.access_group=_group.rolname-- определяем ИМЕНА разрешённых групп
RIGHT JOIN    wh_auth_members membership
    ON _group.id=membership.roleid -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
RIGHT JOIN wh_role _user  
    ON  _user.id=membership.member -- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
    AND _user.rolname=CURRENT_USER -- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО

WHERE obj.pid <> dst.id AND dst.id>0  ;

GRANT SELECT        ON "moverule_lockup" TO "Guest";
-----------------------------------------------------------------------------------------------------------------------------
-- блокировать исходные и конечные объекты, если правило при проверке перестанет существовать, то пофиг
-- перемещение исполнится, т.к. разрешение уже получено и хранится в t_lock_obj
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) 
                  --RETURNS SETOF moverule_lockup AS $BODY$
    RETURNS TABLE(_dst_obj_id BIGINT, _dst_cls_id INTEGER, _dst_obj_label NAME,_dst_obj_pid BIGINT,_oiddstpath BIGINT[])
    AS $BODY$
DECLARE
   _dst_path_2id BIGINT[];
   _locked_rec RECORD;
   _cls_id      BIGINT;
   
    _dst_obj CURSOR IS
        SELECT dst_oid, dst_cid, dst_otitle,dst_opid
        FROM (
              SELECT 
                dst_oid, dst_cid, dst_otitle ,dst_opid, sum(perm_access_disabled)
                FROM moverule_lockup WHERE
                    cid     = _cls_id 
                AND src_oid = _old_pid
                AND oid     = _obj_id
                GROUP BY dst_oid, dst_cid, dst_otitle ,dst_opid
             )t
            WHERE sum=0;

BEGIN
  -- пытаемся найти объект и его местоположение
  SELECT cls_id INTO _cls_id FROM obj WHERE id=_obj_id AND pid=_old_pid;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _obj_id;
  END IF;
  -- пытаемся заблокировать объект, если блокировка не удастся транзакция откатится исключением
  _locked_rec := try_lock_obj(_obj_id, _old_pid);
-- объект блокирован
-- возвращаем пользвателю объекты назначения, попутно складываем их местоположение  в табличку
  FOR rec IN _dst_obj LOOP
    _dst_path_2id := (SELECT get_path_obj_arr_2id(rec.dst_oid));
    _dst_path_2id:=COALESCE(_dst_path_2id::TEXT,'{}');

    INSERT INTO lock_dst(oid,pid, dst_path)VALUES(_obj_id,_old_pid,_dst_path_2id); 

    _dst_obj_id:=rec.dst_oid;
    _dst_cls_id:=rec.dst_cid;
    _dst_obj_label:=rec.dst_otitle;
    _dst_obj_pid:=rec.dst_opid;
    
    RETURN NEXT;
  END LOOP;

  RETURN;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) TO "User";

SELECT * FROM lock_for_move(104,1);

--SELECT do_obj_act(1639, 101, '{"100":"qwe","106":55,"105":"asd"}');
--SELECT do_obj_act(1639, 100, '{"105":"dddddddddd"}');


SELECT lock_reset(104,NULL);

-------------------------------------------------------------------------------
-- перемещение
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_move( IN _oid  BIGINT, 
                                    IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;

CREATE OR REPLACE FUNCTION do_move( IN _oid  BIGINT, 
                                    IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) 
                  RETURNS  VOID AS $BODY$
DECLARE
  _lock_info   RECORD;
  _curr_path BIGINT[];
  _dst_path BIGINT[];
  _ckind SMALLINT;
  _act_logid BIGINT;
  
  _src_qty NUMERIC;
  _dst_qty NUMERIC;

  _move_logid BIGINT;
BEGIN
--  определяем заблокирован ли объект для действия
  SELECT * INTO _lock_info 
    FROM lock_obj
    WHERE
      oid = _oid
      AND pid = _old_opid
      AND lock_time +'00:10:00.00' > now()
      AND lock_session = pg_backend_pid();
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not locked obj_id=% ', _oid;
  END IF;
-- проверяем изменился ли текущий путь объекта
  _curr_path:=(SELECT get_path_obj_arr_2id(_old_opid));
  _curr_path:=COALESCE(_curr_path::TEXT,'{}');
  RAISE DEBUG '_curr_pathid: %',_curr_path;
  if(_curr_path<>_lock_info.path) THEN
    RAISE EXCEPTION ' Object src_path was changed obj_id=%, old_path=% curr_path=% '
                                             , _oid, _lock_info.path,_curr_path;
  END IF;
-- проверяем есть ли среди заблокированных пунктов назначений выбранный 
  _dst_path:=get_path_obj_arr_2id(_new_opid);
  PERFORM FROM lock_dst WHERE oid=_oid AND pid=_old_opid AND dst_path = _dst_path 
                              OR (dst_path IS NULL AND _dst_path IS NULL);
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object dst_path was changed obj_id=%, _dst_path=%'
                                             , _oid, _dst_path;
  END IF;
-- проверяем существование объекта и берём его класс
  SELECT cls_kind,act_logid,qty INTO _ckind, _act_logid,_src_qty FROM obj WHERE id=_oid AND pid=_old_opid;
  IF NOT FOUND OR _ckind IS NULL OR _ckind<1 OR _ckind>3 THEN
    RAISE EXCEPTION ' Object not exist obj_id=%', _oid;
  END IF;
-- находим исходное количество и количество в месте назначения
  SELECT qty INTO _dst_qty FROM obj WHERE pid=_new_opid AND id=_oid;
  RAISE DEBUG ' _dst_qty=% ',_dst_qty;
  CASE
    WHEN _qty < _src_qty  THEN -- div разделяем исходное количество
      UPDATE obj SET qty= (_src_qty - _qty)
                      WHERE pid = _old_opid AND id=_oid;      -- уменьшаем исходное количество
      IF _dst_qty IS NOT NULL THEN                                     -- если в месте назначения есть уже такой объёкт
           UPDATE obj SET qty= (_dst_qty + _qty)
                           WHERE pid = _new_opid AND id=_oid; -- добавляем (обновляем имеющееся количество)
       ELSE                                                            -- если в месте назначения объекта нет
         INSERT INTO obj(id, title, cls_id, move_logid, act_logid, prop,pid, qty)
           SELECT id, title, cls_id, move_logid, act_logid, prop, _new_opid AS pid, _qty AS qty  
             FROM obj_name WHERE id=_oid;
       END IF;
    WHEN _qty = _src_qty THEN -- move перемещение
        IF _dst_qty IS NOT NULL THEN
           UPDATE obj SET qty= (_dst_qty + _qty)
                           WHERE pid = _new_opid AND id=_oid;
           DELETE FROM obj 
                           WHERE pid = _old_opid AND id=_oid;
        ELSE
           UPDATE obj SET pid = _new_opid
                           WHERE pid = _old_opid AND id=_oid;
        END IF;
    ELSE
        RAISE EXCEPTION 'Wrong qty or unknown error'; 
  END CASE;
  _move_logid := nextval('seq_log_id');
-- обновляем ссылку на последнее действие в исходном и конечном объектах
  UPDATE obj_name SET move_logid=_move_logid  WHERE id=_oid;

  INSERT INTO log_move( id,act_logid
                       ,src_objnum_id, src_path
                       ,dst_objnum_id, dst_path
                       ,obj_id, qty )
               VALUES (_move_logid, _act_logid
                       ,_old_opid, _curr_path
                       ,_new_opid, _dst_path
                       ,_oid, _qty );
    

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION do_move( IN _oid  BIGINT, 
                                    IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC )  TO "User";

/*

SELECT _dst_cls_id, cls.title as dst_cls_label , _dst_obj_id, _dst_obj_label , 
        get_path_obj(_dst_obj_pid) AS DST_PATH  
  FROM lock_for_move(104,1)  LEFT JOIN cls cls ON cls.id = _dst_cls_id  ORDER BY _dst_cls_id ;

SELECT do_move(104::BIGINT,1::BIGINT,100::BIGINT,2::NUMERIC);

SELECT lock_reset(104,1);
*/


--SELECT * FROM lock_dst WHERE oid=104 AND pid=100 AND dst_path = _dst_path ;







------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения количественных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,10); /*div to NULL*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*div to 10*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*mov to 15*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,100); SELECT move_object(103,104,100,1,20); /*mov to NULL*/ SELECT lock_reset(103,104,100);
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения номерных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
--SELECT * FROM lock_for_move(104,102,1);
--SELECT move_object(104,102,1,103,1); -- mov objnum
--SELECT lock_reset(104,102,1);

--SELECT * FROM lock_for_move(104,102,103);
--SELECT move_object(104,102,103,1,1); -- mov objnum
--SELECT lock_reset(104,102,103);






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





















