
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_temp.stat_act_obj() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_act_obj() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

  distance  INTERVAL;
  curr_oqty BIGINT;

  cursor_stat_act_acls CURSOR(_cid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP ) IS
    SELECT count(act_id) AS sum,act_id,acls._id as cid, mobj_id AS oid, act.title AS atitle FROM log
      RIGHT JOIN (SELECT * FROM get_childs_cls(_cid)) acls ON mcls_id=acls._id
      LEFT JOIN act ON act.id=act_id 
      WHERE act_id IS NOT NULL  
      AND log_dt<_stop AND log_dt>_start
      GROUP BY mobj_id,acls._id, act_id,act.title 
      order BY act.title
    ;

BEGIN 
  --_begin := '?Начало периода?DATE?2017.01.01? 00:00:00'::TIMESTAMP ;
  --_end := '?Конец периода?DATE?2017.12.31? 23:59:56'::TIMESTAMP ;
  
  _begin := '2017.01.01 00:00'::TIMESTAMP;
  _end := '2017.02.27 23:59'::TIMESTAMP;
  _cid := 101 ;

  _end := CASE WHEN _end > CURRENT_TIMESTAMP THEN CURRENT_TIMESTAMP ELSE _end END;

  distance := _end - _begin;
  RAISE NOTICE  'distance = %',distance;

  drop table IF EXISTS stat_act;
  create temporary TABLE stat_act  
    AS(
      SELECT cls._id        AS cid
            ,cls._title     AS ctitle
            ,cls._kind      AS ckind
            ,cls._path      AS path
            ,obj.id         AS oid
            ,obj.title      AS otitle
            ,obj.qty        AS oqty   
        FROM get_childs_cls(_cid) cls
        LEFT JOIN obj ON obj.cls_id=cls._id
        --ORDER BY path, (substring(obj.title, '^[0-9]+')::INT, obj.title) ASC 
      );

  -- adding missing columns
  FOR act_row IN cursor_stat_act_acls(_cid, _begin, _end) LOOP
    PERFORM FROM information_schema.columns 
      WHERE table_name   = 'stat_act'  AND table_schema ~~* 'pg_temp%'
      AND column_name ILIKE act_row.atitle||'(#)';
    IF NOT FOUND THEN 
      EXECUTE 'ALTER TABLE pg_temp.stat_act ADD COLUMN "'||act_row.atitle||'(#)" NUMERIC';
    END IF;
    --SELECT sum(qty) INTO curr_oqty FROM obj WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=mov_row.cid);
    --curr_perc:= EXTRACT(EPOCH FROM mov_row.sum_dst_time)/ EXTRACT(EPOCH FROM distance)/curr_oqty * 100;
    EXECUTE 'UPDATE pg_temp.stat_act SET "'
      ||act_row.atitle||'(#)"='||act_row.sum
      ||' WHERE pg_temp.stat_act.cid='||act_row.cid
      ||' AND   pg_temp.stat_act.oid='||act_row.oid
      ;
  END LOOP;--FOR mov_row
  
  
--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;

select * from pg_temp.stat_act_obj();
select * from pg_temp.stat_act ORDER BY path, (substring(otitle, '^[0-9]+')::INT, otitle) ASC 
