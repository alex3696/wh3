
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_temp.stat_act_cls() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_act_cls() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

  distance  INTERVAL;
  curr_oqty BIGINT;

  cursor_stat_act_acls CURSOR(_cid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP ) IS
    SELECT count(act_id) AS sum,act_id,acls._id as cid,act.title AS atitle FROM log
      RIGHT JOIN (SELECT * FROM get_childs_cls(_cid)) acls ON mcls_id=acls._id
      LEFT JOIN act ON act.id=act_id 
      WHERE act_id IS NOT NULL  
      AND log_dt<_stop AND log_dt>_start
      GROUP BY acls._id, act_id,act.title 
      order BY act.title
    ;

    col_name  NAME;
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
            ,(SELECT sum(qty) FROM obj WHERE cls_id=cls._id
                                         AND stat_get_obj_location(obj.id,_end) IS NOT NULL
            ) as oqty
        FROM get_childs_cls(_cid) cls
        --ORDER BY path 
      );

  -- adding missing columns
  FOR act_row IN cursor_stat_act_acls(_cid, _begin, _end) LOOP
    col_name:='(#'||act_row.act_id||')'||act_row.atitle;
    PERFORM FROM information_schema.columns 
      WHERE table_name   = 'stat_act'  AND table_schema ~~* 'pg_temp%'
      AND column_name = col_name;
    IF NOT FOUND THEN 
      EXECUTE 'ALTER TABLE pg_temp.stat_act ADD COLUMN "'||col_name||'" NUMERIC';
    END IF;
    --SELECT sum(qty) INTO curr_oqty FROM obj WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=mov_row.cid);
    --curr_perc:= EXTRACT(EPOCH FROM mov_row.sum_dst_time)/ EXTRACT(EPOCH FROM distance)/curr_oqty * 100;
    EXECUTE 'UPDATE pg_temp.stat_act SET "'
      ||col_name||'"='||act_row.sum
      ||' WHERE pg_temp.stat_act.cid='||act_row.cid
      ;
  END LOOP;--FOR mov_row
  
  
--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;

select * from pg_temp.stat_act_cls();
select * from pg_temp.stat_act ORDER BY path;






