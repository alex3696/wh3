
SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;



--SET enable_seqscan = ON;
--SET enable_seqscan = OFF;
/*
DROP VIEW IF EXISTS log;

CREATE OR REPLACE VIEW log AS 
SELECT log.id::BIGINT       AS log_id
      ,date_trunc('second' ,log.timemark)::timestamp  AS log_dt
      ,log.username AS log_user
      
      ,log.act_id
      ,log.act_title
      ,log.act_color
      ,log.obj_id AS mobj_id
      ,mcls.id    AS mcls_id
      ,mobj.title AS mobj_title
      ,mcls.title AS mcls_title
      ,log.qty 
--      ,xsrc.path  AS src_path
--      ,xdst.path  AS dst_path
,(SELECT path FROM tmppath_to_2id_info(log.src_path::TEXT,1)) AS src_path
,(SELECT path FROM tmppath_to_2id_info(log.dst_path::TEXT,1)) AS dst_path
,log.curr_prop AS prop
       ,log.timemark::timestamptz::date  AS log_date
       ,date_trunc('second' ,log.timemark)::timestamptz::time AS log_time

FROM
(
SELECT log_move.id, log_move.timemark, log_move.username, 0::BIGINT AS act_id, 'Перемещение'::WHNAME AS act_title,'white'::VARCHAR(64) AS act_color
     , log_move.obj_id --, cls.title, obj_name.title 
     , log_move.qty
     , log_move.src_path 
     , log_move.dst_path
     , la.prop AS curr_prop
  FROM log_move
  LEFT JOIN log_act la ON la.id=act_logid
UNION ALL
SELECT log_act.id, timemark, username, act_id, act.title, act.color
     , log_act.obj_id
     , 1::NUMERIC AS qty
     , log_act.src_path 
     , NULL::BIGINT[]
     , log_act.prop
  FROM log_act
  LEFT JOIN act ON act.id=act_id
) log
--LEFT JOIN LATERAL tmppath_to_2id_info(log.src_path::TEXT) as xsrc ON true 
--LEFT JOIN LATERAL tmppath_to_2id_info(log.dst_path::TEXT) as xdst ON true 

  LEFT JOIN obj_name mobj ON mobj.id=obj_id
  LEFT JOIN cls mcls ON mcls.id=mobj.cls_id
--WHERE act_title ~~* '%рем%'
--WHERE username ~~* '%iva%'
--WHERE xsrc.path ~~* '%сц%рем%' AND act_title !~~* '%перем%'
;

*/


DROP VIEW log;
CREATE OR REPLACE VIEW log (
 log_id
 ,log_dt
 ,log_user
 ,act_id
 ,act_title
 ,act_color
 ,mobj_id
 ,mcls_id
 ,mobj_title
 ,mcls_title
 ,qty
 ,src_path
 ,dst_path
 ,prop
 ,log_date
 ,log_time

 ,src_cid
 ,src_oid

 ,dst_cid
 ,dst_oid
)AS 

SELECT log_move.id, log_move.timemark, log_move.username, 0::BIGINT AS act_id, 'Перемещение'::WHNAME AS act_title,'white'::VARCHAR(64) AS act_color 
     ,log_move.obj_id 
     ,mcls.id 
     ,mobj.title
     ,mcls.title
     , qty
     ,(SELECT path FROM tmppath_to_2id_info(log_move.src_path::TEXT,1)) 
     ,(SELECT path FROM tmppath_to_2id_info(dst_path::TEXT,1)) 
     , la.prop AS curr_prop
     ,log_move.timemark::timestamptz::date  AS log_date
     ,date_trunc('second' ,log_move.timemark)::timestamptz::time AS log_time
     ,log_move.src_path[1][1]
     ,log_move.src_path[1][2]
     ,log_move.dst_path[1][1]
     ,log_move.dst_path[1][2]
  FROM log_move
  LEFT JOIN obj_name mobj ON mobj.id=obj_id
  LEFT JOIN cls mcls ON  mcls.id=mobj.cls_id

  LEFT JOIN log_act la ON la.id=log_move.act_logid
UNION ALL
SELECT log_act.id, timemark, username, act_id, act.title , act.color
     , obj_id
     , mcls.id 
     ,mobj.title
     ,mcls.title
     , 1::NUMERIC AS qty
     , (SELECT path FROM tmppath_to_2id_info(src_path::TEXT,1)) 
     , NULL::TEXT
     , log_act.prop
     ,log_act.timemark::timestamptz::date  AS log_date
     ,date_trunc('second' ,log_act.timemark)::timestamptz::time AS log_time
     ,log_act.src_path[1][1]
     ,log_act.src_path[1][2]
     ,NULL::BIGINT
     ,NULL::BIGINT
  FROM log_act
  LEFT JOIN act ON act.id=act_id
  LEFT JOIN obj_name mobj ON mobj.id=obj_id
  LEFT JOIN cls mcls ON mcls.id=mobj.cls_id

;

GRANT SELECT        ON log  TO "Guest";
GRANT DELETE        ON log  TO "User";

DROP FUNCTION IF EXISTS ftg_del_log() CASCADE;
CREATE FUNCTION ftg_del_log() RETURNS TRIGGER AS $$
BEGIN
  DELETE FROM log_move WHERE id = OLD.log_id;
  DELETE FROM log_act WHERE id = OLD.log_id;
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_bd_log INSTEAD OF DELETE ON log FOR EACH ROW EXECUTE PROCEDURE ftg_del_log();
GRANT EXECUTE ON FUNCTION ftg_del_log() TO "User";



DROP FUNCTION IF EXISTS ftr_bd_log_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bd_log_act()
  RETURNS trigger AS
$BODY$
DECLARE
  prev_act_log_id BIGINT;
  prev_prop     JSONB;
BEGIN
  IF OLD.timemark < CURRENT_TIMESTAMP - '3 00:00:00'::interval THEN
    RAISE EXCEPTION ' %: can`t delete time is up log_rec=% ',TG_NAME, OLD;
  END IF;
  IF OLD.username <> CURRENT_USER THEN
    RAISE EXCEPTION ' %: can`t delete wrong user log_rec=% ',TG_NAME, OLD;
  END IF;
  PERFORM FROM log WHERE mobj_id=OLD.obj_id AND log_dt > OLD.timemark;
  IF FOUND THEN
    RAISE EXCEPTION ' %: can`t delete log was updated log_rec=%',TG_NAME, OLD;
  END IF;

  prev_act_log_id:=NULL;
  prev_prop := NULL;

  SELECT id,prop INTO prev_act_log_id, prev_prop FROM log_act 
    WHERE obj_id=OLD.obj_id AND timemark < OLD.timemark
    ORDER BY timemark DESC LIMIT 1;

  UPDATE obj_name SET act_logid = prev_act_log_id, prop = prev_prop
    WHERE id=OLD.obj_id;
  
RETURN OLD;
END;
$BODY$
LANGUAGE plpgsql VOLATILE  COST 100;
CREATE TRIGGER tr_bd_log_act BEFORE DELETE ON log_act FOR EACH ROW EXECUTE PROCEDURE ftr_bd_log_act();
GRANT EXECUTE ON FUNCTION ftr_bu_acls() TO "User";



DROP FUNCTION IF EXISTS ftr_bd_log_move() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bd_log_move()
  RETURNS trigger AS
$BODY$
DECLARE
  _qty NUMERIC;
  _src_qty NUMERIC;
  _dst_qty NUMERIC;
  _old_opid BIGINT;
  _new_opid BIGINT;
  _oid BIGINT;
  _prev_mov_log_id BIGINT;
BEGIN
  IF OLD.timemark < CURRENT_TIMESTAMP - '3 00:00:00'::interval THEN
    RAISE EXCEPTION ' %: can`t delete time is up log_rec=% ',TG_NAME, OLD;
  END IF;
  IF OLD.username <> CURRENT_USER THEN
    RAISE EXCEPTION ' %: can`t delete wrong user log_rec=% ',TG_NAME, OLD;
  END IF;
  PERFORM FROM log WHERE mobj_id=OLD.obj_id AND log_dt > OLD.timemark;
  IF FOUND THEN
    RAISE EXCEPTION ' %: can`t delete log was updated log_rec=%',TG_NAME, OLD;
  END IF;

  _oid:=OLD.obj_id;
  _old_opid:=OLD.dst_path[1][2];
  _new_opid:=OLD.src_path[1][2];
  _qty := OLD.qty;
  
  SELECT qty INTO _dst_qty FROM obj WHERE pid=_new_opid AND id=_oid;
  SELECT qty INTO _src_qty FROM obj WHERE pid=_old_opid AND id=_oid;

  CASE
    WHEN _qty < _src_qty  THEN -- div разделяем исходное количество
      RAISE DEBUG 'DIV src.qty= (% - %) WHERE id=% AND pid=%',_src_qty, _qty,_oid,_old_opid;
      UPDATE obj SET qty= (_src_qty - _qty)
                      WHERE pid = _old_opid AND id=_oid;      -- уменьшаем исходное количество
      IF _dst_qty IS NOT NULL THEN                                     -- если в месте назначения есть уже такой объёкт
           RAISE DEBUG 'DIV dst.qty= (% + %) WHERE id=% AND pid=%',_dst_qty, _qty,_oid,_new_opid;
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

  SELECT id INTO _prev_mov_log_id FROM log_move 
    WHERE obj_id=_oid AND timemark < OLD.timemark
    ORDER BY timemark DESC LIMIT 1;

  UPDATE obj_name SET move_logid=_prev_mov_log_id WHERE id=_oid;

RETURN OLD;
END;
$BODY$
LANGUAGE plpgsql VOLATILE  COST 100;
CREATE TRIGGER tr_bd_log_move BEFORE DELETE ON log_move FOR EACH ROW EXECUTE PROCEDURE ftr_bd_log_move();
GRANT EXECUTE ON FUNCTION ftr_bd_log_move() TO "User";

--DELETE FROM log WHERE log_id='101';



/**
SELECT *  FROM log;

--SELECT src_path||'->'||dst_path  FROM log;
SELECT log_time::timestamptz::date
      ,date_trunc('second' ,log_time)::timestamptz::time
      ,* 
FROM log --WHERE log_time::timestamptz::date='2016.01-23'
ORDER BY log_time::timestamptz::date ASC

SELECT log_time::timestamptz::date AS ldate
      ,date_trunc('second' ,log_time)::timestamptz::time AS ltime
      ,* 
FROM log --WHERE log_time::timestamptz::date='2016.01-23'
ORDER BY log_time;

*/




















