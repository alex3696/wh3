BEGIN TRANSACTION;

--SET client_min_messages = 'error';
SET client_min_messages = 'warning';
--SET client_min_messages = 'notice';
--SET client_min_messages = 'debug';
--SHOW client_min_messages;
SHOW client_min_messages;

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
DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) CASCADE;
DROP FUNCTION IF EXISTS    lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    do_move( _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT
                                   , IN _qty NUMERIC ) CASCADE;
DROP FUNCTION IF EXISTS    do_move_objnum( _oid  BIGINT, _new_opid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS    do_move_objqtyi( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;
DROP FUNCTION IF EXISTS    do_move_objqtyf( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;

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

  ,logid_act  BIGINT DEFAULT NULL
  ,logid_move BIGINT DEFAULT NULL

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

  act_id BIGINT,

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

--SELECT try_lock_obj(103,100);
--SELECT try_lock_obj(100,1);
--SELECT try_lock_obj(666,664);

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

--SELECT lock_reset(100,100);
--SELECT lock_reset(666,664);
--SELECT lock_reset(103,NULL);

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
    SELECT perm.act_id , sum(access_disabled) 
        FROM perm_act perm
        --RIGHT JOIN wh_membership ON groupname=perm.access_group AND wh_membership.username=CURRENT_USER
      WHERE
        perm.cls_id IN(SELECT id FROM get_path_cls_info(_cls_id,0))
        AND (obj_id IS NULL OR obj_id = _oid)
        AND _curr_path LIKE src_path
        AND perm.access_group IN (SELECT groupname FROM wh_membership WHERE username=CURRENT_USER)
      GROUP BY act_id
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

--SELECT id, title, note, color, script  FROM lock_for_act(103, 1);
--SELECT lock_reset(103,1);

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция выполнения действия
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) CASCADE;

CREATE OR REPLACE FUNCTION do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB)
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
             WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(_cls_id)) AND ref_cls_act.act_id=_act_id
             AND ref_act_prop.prop_id IS NOT NULL 
            ) all_new ON all_new.id=ref_act_prop.prop_id
  WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(_cls_id)) 
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
        RIGHT JOIN wh_membership ON groupname=perm.access_group AND wh_membership.username=CURRENT_USER
      WHERE 
        perm.cls_id IN(SELECT id FROM get_path_cls_info(_cls_id,0))
        AND perm.act_id = _act_id
        AND  (perm.obj_id IS NULL OR perm.obj_id = _obj_id)
        AND  _curr_pathid::TEXT LIKE perm.src_path
        --AND perm.access_group IN (SELECT groupname FROM wh_membership WHERE username=CURRENT_USER)
      GROUP BY act_id
      ) t 
      WHERE t.sum=0;

   RAISE DEBUG '_perm_sum: %',COALESCE(_perm_sum,-1);

  IF _perm_sum IS NULL OR _perm_sum>0 THEN
    RAISE EXCEPTION ' Path was changed for obj_id=% _cls_id=% _act_id=% try again ', _obj_id,_cls_id,_act_id;
  END IF;

  FOR rec IN _chk_props LOOP
    _prop_item:= concat_ws(':',quote_ident(rec.id::TEXT), rec.result );
    _prop_str := concat_ws(',',_prop_str, _prop_item );
  END LOOP;
  _prop_str:='{'||_prop_str||'}';
  RAISE DEBUG '_prop_str: %',_prop_str;

  INSERT INTO log_main(src_path, obj_id)
    VALUES ( _curr_pathid, _obj_id) RETURNING id INTO _last_log_id;
  INSERT INTO log_detail_act(id, act_id, prop) 
    VALUES (_last_log_id,_act_id, _prop_str::JSONB);
  UPDATE obj_name SET act_logid=_last_log_id, prop=_prop_str::JSONB WHERE id=_obj_id;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;

GRANT EXECUTE ON FUNCTION do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) TO "User";


--SELECT id, title, note, color, script  FROM lock_for_act(103, 1);

--SELECT do_act(103, 100, '{"100":66,"102":"45452ergsdfgd"}');
--SELECT lock_reset(103,1);


-------------------------------------------------------------------------------
-- поиск всех возможных вариантов перемещения
-------------------------------------------------------------------------------
DROP VIEW IF EXISTS moverule_lockup CASCADE;
CREATE OR REPLACE VIEW moverule_lockup AS 
  SELECT  
    mov.id         AS oid
   ,mov.pid        AS opid
   ,mov.cls_id     AS ocid
   ,dst.id         AS dst_oid
   ,dst.cls_id     AS dst_cid
   ,dst_name.title AS dst_otitle
  ,dst.pid         AS dst_opid
  ,perm.access_disabled AS perm_access_disabled

FROM 
  (SELECT * FROM perm_move perm
     WHERE perm.access_group IN (SELECT groupname FROM wh_membership WHERE username=CURRENT_USER)
  )perm

RIGHT JOIN obj mov ON -- находим все объекты+классы которые можно перемещать)
                (  mov.cls_id IN (SELECT _id FROM get_childs_cls(perm.cls_id))) 
                AND (perm.obj_id = mov.id OR perm.obj_id IS NULL)
RIGHT JOIN obj_num src ON src.id = mov.pid -- отсеиваем по текущему положению
                    AND  perm.src_cls_id = src.cls_id
                    AND (perm.src_obj_id = src.id OR perm.src_obj_id IS NULL)
                   AND ( (src.pid=0 AND perm.src_path='{}')OR(get_path_objnum_arr_2id(src.pid)::TEXT LIKE perm.src_path) )
RIGHT JOIN obj_num dst ON perm.dst_cls_id = dst.cls_id -- находим все места куда можно перемещать
                  AND (perm.dst_obj_id = dst.id OR perm.dst_obj_id IS NULL)
                  AND ((dst.pid=0 AND perm.dst_path='{}')OR(get_path_objnum_arr_2id(dst.pid)::TEXT LIKE perm.dst_path))
LEFT JOIN obj_name dst_name 
                ON dst_name.id = dst.id
/*
-- group permission
LEFT JOIN wh_role _group 
    ON perm.access_group=_group.rolname-- определяем ИМЕНА разрешённых групп
RIGHT JOIN    wh_auth_members membership
    ON _group.id=membership.roleid -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
RIGHT JOIN wh_role _user  
    ON  _user.id=membership.member -- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
    AND _user.rolname=CURRENT_USER -- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО
*/    
WHERE 
mov.pid <> dst.id AND dst.id>0  
;

GRANT SELECT        ON "moverule_lockup" TO "Guest";
-----------------------------------------------------------------------------------------------------------------------------
-- блокировать исходные и конечные объекты, если правило при проверке перестанет существовать, то пофиг
-- перемещение исполнится, т.к. разрешение уже получено и хранится в t_lock_obj
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) 
                  --RETURNS SETOF moverule_lockup AS $BODY$
    RETURNS TABLE(_dst_obj_id BIGINT, _dst_cls_id BIGINT, _dst_obj_label NAME,_dst_obj_pid BIGINT,_oiddstpath BIGINT[])
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
                    ocid = _cls_id 
                AND opid = _old_pid
                AND oid  = _obj_id
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
    _dst_path_2id := (SELECT get_path_objnum_arr_2id(rec.dst_oid));
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

--SELECT * FROM lock_for_move(104,1);
--SELECT do_obj_act(1639, 101, '{"100":"qwe","106":55,"105":"asd"}');
--SELECT do_obj_act(1639, 100, '{"105":"dddddddddd"}');


SELECT lock_reset(104,NULL);

--------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_move_objnum( _oid  BIGINT, _new_opid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION do_move_objnum( _oid  BIGINT, _new_opid BIGINT) 
  RETURNS  VOID AS $BODY$
DECLARE
BEGIN
  RAISE DEBUG 'MOV obj_num  id=%  new_opid=%',_oid,_new_opid;
  UPDATE obj_num SET pid = _new_opid WHERE id=_oid;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;
--------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_move_objqtyi( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;
CREATE OR REPLACE FUNCTION do_move_objqtyi( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC )
  RETURNS  VOID AS $BODY$
DECLARE
  _src_qty NUMERIC;
  _dst_qty NUMERIC;
BEGIN
  SELECT qty INTO _dst_qty FROM obj_qtyi WHERE pid=_new_opid AND id=_oid;
  SELECT qty INTO _src_qty FROM obj_qtyi WHERE pid=_old_opid AND id=_oid;
  RAISE DEBUG 'MOV obj_qtyi id=%  new_opid=%  _old_opid=%',_oid,_new_opid,_old_opid;
  CASE
    WHEN _qty < _src_qty  THEN -- div разделяем исходное количество
      RAISE DEBUG 'DIV src.qty= (% - %) WHERE id=% AND pid=%',_src_qty, _qty,_oid,_old_opid;
      UPDATE obj_qtyi SET qty= (_src_qty - _qty) WHERE pid = _old_opid AND id=_oid;      -- уменьшаем исходное количество
      IF _dst_qty IS NOT NULL THEN -- если в месте назначения есть уже такой объект
           RAISE DEBUG 'DIV dst.qty= (% + %) WHERE id=% AND pid=%',_dst_qty, _qty,_oid,_new_opid;
           UPDATE obj_qtyi SET qty= (_dst_qty + _qty) WHERE pid = _new_opid AND id=_oid; -- добавляем (обновляем имеющееся количество)
       ELSE -- если в месте назначения объекта нет
         INSERT INTO obj_qtyi(id, cls_id, cls_kind, pid, qty)
           SELECT id,cls_id,cls_kind,_new_opid,_qty FROM obj_name WHERE id=_oid;
       END IF;
    WHEN _qty = _src_qty THEN -- move перемещение
        IF _dst_qty IS NOT NULL THEN -- если в месте назначения есть уже такой объект
           UPDATE obj_qtyi SET qty= (_dst_qty + _qty) WHERE pid = _new_opid AND id=_oid;-- добавляем (обновляем имеющееся количество)
           DELETE FROM obj_qtyi WHERE pid = _old_opid AND id=_oid;-- удаляем всё с старого местоположения
        ELSE
           UPDATE obj_qtyi SET pid = _new_opid WHERE pid = _old_opid AND id=_oid;
        END IF;
    ELSE
        RAISE EXCEPTION 'Wrong qty or unknown error'; 
  END CASE;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;
--------------------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS    do_move_objqtyf( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;
CREATE OR REPLACE FUNCTION do_move_objqtyf( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC )
  RETURNS  VOID AS $BODY$
DECLARE
  _src_qty NUMERIC;
  _dst_qty NUMERIC;
BEGIN
  SELECT qty INTO _dst_qty FROM obj_qtyf WHERE pid=_new_opid AND id=_oid;
  SELECT qty INTO _src_qty FROM obj_qtyf WHERE pid=_old_opid AND id=_oid;
  
  CASE
    WHEN _qty < _src_qty  THEN -- div разделяем исходное количество
      RAISE DEBUG 'DIV src.qty= (% - %) WHERE id=% AND pid=%',_src_qty, _qty,_oid,_old_opid;
      UPDATE obj_qtyf SET qty= (_src_qty - _qty) WHERE pid = _old_opid AND id=_oid;      -- уменьшаем исходное количество
      IF _dst_qty IS NOT NULL THEN -- если в месте назначения есть уже такой объект
           RAISE DEBUG 'DIV dst.qty= (% + %) WHERE id=% AND pid=%',_dst_qty, _qty,_oid,_new_opid;
           UPDATE obj_qtyf SET qty= (_dst_qty + _qty) WHERE pid = _new_opid AND id=_oid; -- добавляем (обновляем имеющееся количество)
       ELSE -- если в месте назначения объекта нет
         INSERT INTO obj_qtyf(id, cls_id, cls_kind, pid, qty)
           SELECT id,cls_id,cls_kind,_new_opid,_qty FROM obj_name WHERE id=_oid;
       END IF;
    WHEN _qty = _src_qty THEN -- move перемещение
        IF _dst_qty IS NOT NULL THEN -- если в месте назначения есть уже такой объект
           UPDATE obj_qtyf SET qty= (_dst_qty + _qty) WHERE pid = _new_opid AND id=_oid;-- добавляем (обновляем имеющееся количество)
           DELETE FROM obj_qtyf WHERE pid = _old_opid AND id=_oid; -- удаляем всё с старого местоположения
        ELSE
           UPDATE obj_qtyf SET pid = _new_opid WHERE pid = _old_opid AND id=_oid;
        END IF;
    ELSE
        RAISE EXCEPTION 'Wrong qty or unknown error'; 
  END CASE;
END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;
--------------------------------------------------------------------------------------------------
GRANT EXECUTE ON FUNCTION do_move_objnum(BIGINT,BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION do_move_objqtyi( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) TO "User";
GRANT EXECUTE ON FUNCTION do_move_objqtyf( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) TO "User";
--------------------------------------------------------------------------------------------------


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
  _dst_path:=COALESCE(_dst_path::TEXT,'{}');
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

  CASE _ckind
   WHEN 1 THEN PERFORM do_move_objnum(_oid, _new_opid);
   WHEN 2 THEN PERFORM do_move_objqtyi(_oid, _old_opid, _new_opid, _qty);
   WHEN 3 THEN PERFORM do_move_objqtyf(_oid, _old_opid, _new_opid, _qty);
   ELSE RAISE EXCEPTION 'Wrong kind or unknown error'; 
  END CASE;

  

  

  INSERT INTO log_main(src_path, obj_id)
      VALUES (_curr_path, _oid) RETURNING id INTO _move_logid;
  INSERT INTO log_detail_move(id, dst_path, qty, prop_lid)
      VALUES (_move_logid,  _dst_path, _qty, _act_logid);
-- обновляем ссылку на последнее действие в исходном и конечном объектах
  UPDATE obj_name SET move_logid=_move_logid  WHERE id=_oid;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION do_move( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC )  TO "User";

/*

SELECT _dst_cls_id, cls.title as dst_cls_label , _dst_obj_id, _dst_obj_label , 
        get_path_obj(_dst_obj_pid) AS DST_PATH  
  FROM lock_for_move(104,1)  LEFT JOIN cls cls ON cls.id = _dst_cls_id  ORDER BY _dst_cls_id ;

SELECT do_move(104::BIGINT,1::BIGINT,100::BIGINT,2::NUMERIC);

SELECT lock_reset(104,1);
*/


--SELECT * FROM lock_dst WHERE oid=104 AND pid=100 AND dst_path = _dst_path ;

------------------------------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS log;
CREATE OR REPLACE VIEW log AS 
 SELECT 
  lm.id       AS log_id
 ,lm.timemark AS log_dt
 ,lm.username AS log_user
 ,CASE WHEN lad.act_id IS NOT NULL THEN lad.act_id ELSE NULL::BIGINT END AS act_id --,det.act_id  AS act_id
 ,act.title   AS act_title
 ,act.color   AS act_color
 ,lm.obj_id   AS mobj_id
 ,mobj.cls_id AS mcls_id
 ,mobj.title  AS mobj_title
 ,mcls.title  AS mcls_title
 ,CASE WHEN mcls.kind=1 THEN 1::NUMERIC ELSE lmd.qty END AS qty
 ,CASE WHEN lad.act_id IS NOT NULL THEN lad.prop ELSE NULL::JSONB END AS prop --,det.prop    AS prop
 --,CASE WHEN lad.act_id IS NOT NULL THEN lad.prop ELSE move_prop.prop END AS prop 
 ,lm.timemark::timestamptz::date  AS log_date
 ,date_trunc('second' ,lm.timemark)::timestamptz::time AS log_time
 ,lm.src_path[1][1]  AS src_cid
 ,lm.src_path[1][2]  AS src_oid
 ,CASE WHEN lad.act_id IS NOT NULL THEN NULL ELSE lmd.dst_path[1][1] END AS dst_cid -- ,det.dst_path[1][1] AS dst_cid
 ,CASE WHEN lad.act_id IS NOT NULL THEN NULL ELSE lmd.dst_path[1][2] END AS dst_oid -- ,det.dst_path[1][2] AS dst_oid
 ,lm.src_path        AS src_ipath
 ,CASE WHEN lad.act_id IS NOT NULL THEN NULL ELSE lmd.dst_path END AS dst_ipath --,det.dst_path       AS dst_ipath
 ,(SELECT path FROM tmppath_to_2id_info(lm.src_path::TEXT,1)) AS src_path
 ,(SELECT path FROM tmppath_to_2id_info(lmd.dst_path::TEXT,1)) AS dst_path

FROM log_main lm
  LEFT JOIN log_detail_act  lad ON lad.id=lm.id
  LEFT JOIN log_detail_move lmd ON lmd.id=lm.id
  --LEFT JOIN log_detail_act move_prop ON lmd.prop_lid=move_prop.id

  LEFT JOIN act ON act.id=lad.act_id
  LEFT JOIN obj_name mobj ON mobj.id=lm.obj_id
  LEFT JOIN acls     mcls ON mcls.id=mobj.cls_id
  ;
  
------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS log2;
CREATE OR REPLACE VIEW log2 AS 
 SELECT 
  lm.id       AS log_id
 ,lm.timemark AS log_dt
 ,lm.username AS log_user
 ,det.act_id  AS act_id
 ,act.title   AS act_title
 ,act.color   AS act_color
 ,lm.obj_id   AS mobj_id
 ,mobj.cls_id AS mcls_id
 ,mobj.title  AS mobj_title
 ,mcls.title  AS mcls_title
 ,det.qty     AS qty
 --,(SELECT path FROM tmppath_to_2id_info(lm.src_path::TEXT,1)) AS src_path
 --,(SELECT path FROM tmppath_to_2id_info(det.dst_path::TEXT,1)) AS dst_path
 ,det.prop    AS prop
 ,lm.timemark::timestamptz::date  AS log_date
 ,date_trunc('second' ,lm.timemark)::timestamptz::time AS log_time
 ,lm.src_path[1][1]  AS src_cid
 ,lm.src_path[1][2]  AS src_oid
 ,det.dst_path[1][1] AS dst_cid
 ,det.dst_path[1][2] AS dst_oid
 ,lm.src_path        AS src_ipath
 ,det.dst_path       AS dst_ipath
 --,src.path AS src_path
 --,dst.path AS dst_path
 ,path.src AS src_path
 ,path.dst AS dst_path
  FROM log_main lm
  LEFT JOIN
  ( SELECT lad.id
                ,lad.act_id
                ,lad.prop
                ,NULL::BIGINT[] AS dst_path
                ,1::NUMERIC     AS qty
                ,lad.id         AS prop_lid
           FROM log_detail_act lad
        UNION ALL
         SELECT lmd.id
               ,NULL::BIGINT AS act_id
               ,lad.prop     AS prop
               ,lmd.dst_path AS dst_path
               ,lmd.qty      AS qty
               ,lmd.prop_lid AS prop_lid
           FROM log_detail_move lmd
           LEFT JOIN log_detail_act lad ON lad.id=lmd.prop_lid
  ) det ON lm.id=det.id
  LEFT JOIN act ON act.id=det.act_id
  LEFT JOIN obj_name mobj ON mobj.id=lm.obj_id
  LEFT JOIN acls     mcls ON mcls.id=mobj.cls_id
  --LEFT JOIN LATERAL tmppath_to_2id_info(lm.src_path::TEXT,1) src ON true 
  --LEFT JOIN LATERAL tmppath_to_2id_info(det.dst_path::TEXT,1) dst ON true 
  LEFT JOIN LATERAL (SELECT 
                       (SELECT path FROM tmppath_to_2id_info(lm.src_path::TEXT,1)) as src,
                       (SELECT path FROM tmppath_to_2id_info(det.dst_path::TEXT,1)) as dst )path 
       ON TRUE

  ;
------------------------------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------------------------------
GRANT SELECT        ON log  TO "Guest";
GRANT DELETE        ON log  TO "User";
------------------------------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_del_log() CASCADE;
CREATE FUNCTION ftg_del_log() RETURNS TRIGGER AS $$
BEGIN
  DELETE FROM log_main WHERE id = OLD.log_id;
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_bd_log INSTEAD OF DELETE ON log FOR EACH ROW EXECUTE PROCEDURE ftg_del_log();
GRANT EXECUTE ON FUNCTION ftg_del_log() TO "User";
------------------------------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_bd_log_main() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bd_log_main()
  RETURNS trigger AS
$BODY$
DECLARE
  _lock_info       RECORD;
  _rec_detail_act  RECORD;
  prev_act_log_id  BIGINT;
  prev_prop        JSONB;

  _old_detail_move RECORD;
  _ckind           SMALLINT;
  _oid             BIGINT;
  _old_opid        BIGINT;
  _new_opid        BIGINT;
  _qty             NUMERIC;
  _prev_mov_log_id BIGINT;
  
  
BEGIN
  IF OLD.timemark < CURRENT_TIMESTAMP - '3 00:00:00'::interval THEN
    RAISE EXCEPTION ' %: can`t delete time is up log_rec=% ',TG_NAME, OLD;
  END IF;
  IF OLD.username <> CURRENT_USER THEN
    RAISE EXCEPTION ' %: can`t delete wrong user log_rec=% ',TG_NAME, OLD;
  END IF;
  PERFORM FROM log_main WHERE obj_id=OLD.obj_id AND timemark > OLD.timemark;
  IF FOUND THEN
    RAISE EXCEPTION ' %: can`t delete log was updated log_rec=%',TG_NAME, OLD;
  END IF;

  SELECT * INTO _rec_detail_act FROM log_detail_act WHERE id=OLD.id;
  IF FOUND THEN
    SELECT log_main.id, log_detail_act.prop INTO prev_act_log_id, prev_prop 
      FROM log_main
      INNER JOIN log_detail_act USING (id)
      WHERE obj_id=OLD.obj_id AND timemark < OLD.timemark
      ORDER BY timemark DESC LIMIT 1;

    UPDATE obj_name SET act_logid = prev_act_log_id, prop = prev_prop
      WHERE id=OLD.obj_id;
  END IF;

  SELECT * INTO _old_detail_move FROM log_detail_move WHERE id=OLD.id;
  IF FOUND THEN
    _oid:=OLD.obj_id;
    _old_opid:=_old_detail_move.dst_path[1][2];
    _new_opid:=OLD.src_path[1][2];
    _qty := _old_detail_move.qty;
    SELECT cls_kind INTO _ckind FROM obj_name WHERE id=_oid;
    
    -- Ищем непросроченые чужие блокировки
    SELECT * INTO _lock_info 
      FROM lock_obj
      WHERE
      oid = _oid
      AND now() < lock_time +'00:10:00.00' AND lock_session <> pg_backend_pid()
      AND ( 
            _ckind=1
            OR --Если обьект относится к количественным, то надо проверить в таблице блокировок текущее место и предыдущее
            ( _ckind<>1 
              AND 
              (pid = _old_opid OR pid=_new_opid) 
            )
          )
    ;
    IF FOUND THEN -- отменяем откат действия если стоит блокировка
      RAISE EXCEPTION ' %: can`t delete object(id=%) is locked by user % in %.'
        ,TG_NAME, _oid, _lock_info.lock_user, _lock_info.lock_time;
    END IF;

    CASE _ckind
      WHEN 1 THEN PERFORM do_move_objnum(_oid, _new_opid);
      WHEN 2 THEN PERFORM do_move_objqtyi(_oid, _old_opid, _new_opid, _qty);
      WHEN 3 THEN PERFORM do_move_objqtyf(_oid, _old_opid, _new_opid, _qty);
      ELSE RAISE EXCEPTION 'Wrong kind or unknown error'; 
    END CASE;
    SELECT id INTO _prev_mov_log_id 
      FROM log_main
      INNER JOIN log_detail_move USING (id)
      WHERE obj_id=_oid AND timemark < OLD.timemark
      ORDER BY timemark DESC LIMIT 1;
    UPDATE obj_name SET move_logid=_prev_mov_log_id WHERE id=_oid;
  END IF;

RETURN OLD;
END;
$BODY$
LANGUAGE plpgsql VOLATILE  COST 100;
CREATE TRIGGER tr_bd_log_main BEFORE DELETE ON log_main FOR EACH ROW EXECUTE PROCEDURE ftr_bd_log_main();
GRANT EXECUTE ON FUNCTION ftr_bd_log_main() TO "User";


COMMIT TRANSACTION;












