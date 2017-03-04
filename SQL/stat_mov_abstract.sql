

DROP FUNCTION IF EXISTS pg_temp.stat_mov_obj_all(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_mov_obj_all(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) 
 RETURNS TABLE( cid      BIGINT 
               ,oid      BIGINT
               ,dst_time INTERVAL
               ,dst_oid  BIGINT
              ) AS $BODY$ 
  DECLARE 
    --curr_dt TIMESTAMP;
    --curr_place BIGINT;
    prev_dt TIMESTAMP;
    prev_place BIGINT;
    prev_src_oid BIGINT;
    rec RECORD;

    _olog CURSOR(_start TIMESTAMP, _stop TIMESTAMP, _oid  BIGINT ) IS
    SELECT log_dt, src_oid, log.dst_oid, mobj_id, mcls_id FROM log WHERE mobj_id= _oid 
    AND act_id IS NULL
    AND log_dt<_stop AND log_dt>_start
    ORDER BY log_dt DESC
    ;
BEGIN
  OPEN _olog(_begin,_end,_oid);
  FETCH _olog INTO rec;
  
  oid := _oid;
  prev_dt:= _end;

  IF rec.log_dt IS NULL THEN
    SELECT log.dst_oid,log.mcls_id INTO prev_src_oid,cid
    FROM log WHERE mobj_id= _oid 
      AND act_id IS NULL
      AND log_dt<=_begin
      ORDER BY log_dt DESC LIMIT 1;
    IF prev_src_oid IS NULL OR cid IS NULL THEN 
      SELECT obj.pid,obj.cls_id INTO prev_src_oid,cid FROM obj WHERE id=_oid;
    END IF;
  ELSE
    cid          := rec.mcls_id;
    prev_src_oid := rec.src_oid;
    prev_place   := rec.dst_oid;
  END IF;
  --RAISE NOTICE  'rec=%', rec;
  --IF rec IS NOT NULL THEN RAISE NOTICE  'NOT NULL'; ELSE RAISE NOTICE  'IS NULL'; END IF;
  WHILE prev_src_oid IS NOT NULL LOOP
    --RAISE NOTICE  'prev_dt=%   prev_place=%',prev_dt,prev_place ;
    
    --RAISE NOTICE  'curr_dt=%   curr_place=%',curr_dt,curr_place ;
    IF rec.log_dt IS NOT NULL THEN 
      dst_time := prev_dt-rec.log_dt;
      dst_oid := rec.dst_oid;
      RETURN NEXT;
    ELSE
      --RAISE NOTICE  'LAST NS curr_dt=%   curr_place=%',curr_dt,curr_place ;
      dst_time := prev_dt-_begin;
      dst_oid := prev_src_oid;
      RETURN NEXT;
      EXIT;
    END IF;
    prev_src_oid:=rec.src_oid;
    prev_dt:=rec.log_dt;
    prev_place:=rec.dst_oid;
    FETCH _olog INTO rec;
  END LOOP;
  CLOSE _olog;

  RETURN;
END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1000;

/**
SELECT * FROM pg_temp.stat_mov_obj_all (3626,'2016.09.10 12:00+5','2017.02.27 20:00+5');

SELECT * FROM pg_temp.place_time (3934,'2017.01.01 00:00+5','2017.02.27 20:00+5');

SELECT *,obj_name.title FROM pg_temp.place_time (3782,'2017.01.31 00:00+5','2017.02.27 20:00+5') ss
LEFT  JOIN obj_name ON obj_name.id=ss.dst_oid

SELECT *,obj_name.title FROM pg_temp.place_time (3782,'2014.10.01 00:00+5','2014.11.30 20:00+5') ss
LEFT  JOIN obj_name ON obj_name.id=ss.dst_oid


    SELECT log_dt, src_oid, log.dst_oid, mobj_id, mcls_id FROM log WHERE mobj_id= 3782 
    AND act_id IS NULL
    AND log_dt<'2017.02.27 20:00+5' AND log_dt>'2017.01.01 00:00+5'
    ORDER BY log_dt DESC


select SUM(dst_time),dst_oid,cid,oid 
  from pg_temp.place_time (3626,'2016.09.10 12:00+5','2017.02.27 20:00+5')
  GROUP BY dst_oid,cid,oid 

*/
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS pg_temp.stat_mov_cls() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_mov_cls() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _cls_cursor CURSOR (in_cid BIGINT) IS
    SELECT cls._id        AS cid
          ,cls._title     AS ctitle
          ,cls._kind      AS ckind
          ,cls._path      AS path
    FROM get_childs_cls(in_cid,0) cls --ORDER BY cls._path
  ;
  _obj_cursor CURSOR(in_cid  BIGINT)  IS
    SELECT * FROM obj WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=in_cid)
  ;
  _stat_cursor CURSOR( _oid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP) IS
    select SUM(ostat.dst_time) AS dst_time,ostat.dst_oid,ostat.cid,ostat.oid 
    from pg_temp.stat_mov_obj_all (_oid,_start,_stop) ostat
    GROUP BY ostat.dst_oid,ostat.cid,ostat.oid 
  ;
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

  distance  INTERVAL;
  curr_oqty BIGINT;
  mov_row   RECORD;
  curr_perc NUMERIC;
BEGIN 
  --_begin := '?Начало периода?DATE?2017.01.01? 00:00:00'::TIMESTAMP ;
  --_end := '?Конец периода?DATE?2017.12.31? 23:59:56'::TIMESTAMP ;
  
  _begin := '2017.01.01 00:00'::TIMESTAMP;
  _end := '2017.02.27 23:59'::TIMESTAMP;
  _cid := 101 ;

  _end := CASE WHEN _end > CURRENT_TIMESTAMP THEN CURRENT_TIMESTAMP ELSE _end END;

  distance := _end - _begin;
  RAISE NOTICE  'distance = %',distance;

  drop table IF EXISTS _sum_dst_time_cls;
  create temporary TABLE _sum_dst_time_cls( 
                cid          BIGINT 
               ,dst_oid      BIGINT
               ,sum_dst_time INTERVAL
               ,constraint pk_sum_dst_time_cls PRIMARY KEY(cid,dst_oid)
   );

  drop table IF EXISTS stat_move_cls;
  create temporary TABLE stat_move_cls( 
                cid      BIGINT 
               ,ctitle   NAME
               ,ckind    SMALLINT
               ,path     TEXT
               ,oqty     BIGINT
               --,CONSTRAINT uk_stat_move_cls UNIQUE (path)
   );
   
   -- iterate CLS
  FOR cls_row IN _cls_cursor(_cid) LOOP
    --collect stat by all objects
    --TRUNCATE _sum_dst_time_cls CASCADE;
    curr_oqty:=0;
    -- iterate OBJECTS in CLS
    FOR obj_row IN _obj_cursor( cls_row.cid) LOOP
      -- iterate STATISTIC for OBJECTS in CLS
      curr_oqty:= curr_oqty+obj_row.qty;
      FOR stat_row IN _stat_cursor( obj_row.id,_begin,_end) LOOP
        INSERT INTO _sum_dst_time_cls (cid, dst_oid,sum_dst_time)
          VALUES (cls_row.cid, stat_row.dst_oid, stat_row.dst_time)
          ON CONFLICT ON CONSTRAINT pk_sum_dst_time_cls
          DO UPDATE SET sum_dst_time = _sum_dst_time_cls.sum_dst_time+EXCLUDED.sum_dst_time
          ;
      END LOOP;--FOR stat_row
    END LOOP;--FOR obj_row
    INSERT INTO stat_move_cls (cid,ctitle,ckind,path,oqty)
      VALUES(cls_row.cid, cls_row.ctitle, cls_row.ckind,cls_row.path,curr_oqty);

  END LOOP;--FOR cls_row

  -- adding missing columns
  FOR mov_row IN (SELECT ss.*,obj_name.title AS dst_otitle FROM _sum_dst_time_cls ss
                    LEFT JOIN obj_name ON obj_name.id=ss.dst_oid 
                    --WHERE ss.cid = cls_row.cid
                    ORDER BY dst_oid,dst_otitle
                 )LOOP
    PERFORM FROM information_schema.columns 
      WHERE table_name   = 'stat_move_cls'  AND table_schema ~~* 'pg_temp%'
      AND column_name ILIKE mov_row.dst_otitle||'(%)';
    IF NOT FOUND THEN 
      EXECUTE 'ALTER TABLE pg_temp.stat_move_cls ADD COLUMN "'||mov_row.dst_otitle||'(%)" NUMERIC';
    END IF;
    SELECT sum(qty) INTO curr_oqty FROM obj WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=mov_row.cid);
    curr_perc:= EXTRACT(EPOCH FROM mov_row.sum_dst_time)/ EXTRACT(EPOCH FROM distance)/curr_oqty * 100;
    EXECUTE 'UPDATE pg_temp.stat_move_cls SET "'
      ||mov_row.dst_otitle||'(%)"='||round(curr_perc,3)
      ||' WHERE pg_temp.stat_move_cls.cid='||mov_row.cid;
  END LOOP;--FOR mov_row

  --drop table IF EXISTS _sum_dst_time_cls CASCADE;

--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;
/**
    SELECT cls._id        AS cid
          ,cls._title     AS ctitle
          ,cls._kind      AS ckind
          ,cls._path      AS path
    FROM get_childs_cls(101,0) cls
    ORDER BY path

    SELECT * FROM obj WHERE cls_id IN (SELECT id FROM acls WHERE kind=1 AND pid=113)
    SELECT * FROM  _sum_dst_time_cls


*/
select * from pg_temp.stat_mov_cls();
select * from pg_temp.stat_move_cls ORDER BY path;


















