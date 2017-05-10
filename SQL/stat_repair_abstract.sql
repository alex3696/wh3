-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS pg_temp.stat_act_absrtact() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_act_absrtact() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

  distance  INTERVAL;
  curr_oqty BIGINT;

  cursor_stat_act_acls CURSOR(_cid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP, _aid_ext BIGINT,_pid_ext TEXT) IS
    SELECT sum(count),act_cls._pid AS cid, prop_title
    FROM (
      SELECT count(act_id),log.prop->_pid_ext AS prop_title,acls._id,acls._pid FROM log
      RIGHT JOIN (SELECT * FROM get_childs_cls(_cid)) acls ON mcls_id=acls._id --RIGHT JOIN (SELECT * FROM get_childs_cls(101)) acls ON mcls_id=acls._id
      WhERE act_id = _aid_ext
      AND log_dt<_stop AND log_dt>_start --AND log_dt<'2017.02.27 23:59' AND log_dt>'2017.01.01 00:00' 
      GROUP BY acls._pid, log.prop->_pid_ext,acls._id
      --order BY acls._title,act_id 
         )act_cls 
    GROUP BY act_cls._pid , act_cls.prop_title
    order BY act_cls.prop_title
    ;

  col_name  NAME;
  aid_ext BIGINT;
  pid_ext BIGINT;
BEGIN 
  --_begin := '?Начало периода?DATE?2017.01.01? 00:00:00'::TIMESTAMP ;
  --_end := '?Конец периода?DATE?2017.12.31? 23:59:56'::TIMESTAMP ;
  
  _begin := '2016.01.01 00:00'::TIMESTAMP;
  _end := '2016.12.27 23:59'::TIMESTAMP;
  _cid := 101 ;

  _end := CASE WHEN _end > CURRENT_TIMESTAMP THEN CURRENT_TIMESTAMP ELSE _end END;

  SELECT * INTO pid_ext FROM prop WHERE title = 'Причина ремонта';
  SELECT * INTO aid_ext FROM act WHERE title = 'Ремонт';

  distance := _end - _begin;
  RAISE NOTICE  'distance = %',distance;

  drop table IF EXISTS stat_act;
  create temporary TABLE stat_act  
    AS(
      SELECT cls._id        AS cid
            ,cls._title     AS ctitle
            ,cls._kind      AS ckind
            ,cls._path      AS path
            ,(SELECT sum(qty) FROM obj 
                WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=cls._id)
                  AND stat_get_obj_location(obj.id,_end) IS NOT NULL
              ) as oqty
        FROM get_childs_cls(_cid,0) cls
        --ORDER BY path 
      );

  -- adding missing columns
  FOR act_row IN cursor_stat_act_acls(_cid, _begin, _end, aid_ext, pid_ext) LOOP
    col_name:= trim(both '"' from act_row.prop_title::TEXT);
    IF col_name IS NULL OR col_name='' THEN 
      col_name:='Без категории';
    END IF;
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

select * from pg_temp.stat_act_absrtact();
select * from pg_temp.stat_act ORDER BY path;






