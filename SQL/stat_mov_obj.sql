

DROP FUNCTION IF EXISTS pg_temp.stat_mov_obj() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.stat_mov_obj() 
 RETURNS VOID
AS $BODY$ 
DECLARE
  _cls_cursor CURSOR (in_cid BIGINT) IS
    SELECT cls._id        AS cid
          ,cls._title     AS ctitle
          ,cls._kind      AS ckind
          ,cls._path      AS path
    FROM get_childs_cls(in_cid) cls --ORDER BY cls._path
  ;
  _obj_cursor CURSOR(in_cid  BIGINT)  IS
    SELECT * FROM obj WHERE cls_id =in_cid
  ;
  _stat_cursor CURSOR( _oid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP) IS
    select SUM(ostat.dst_time) AS dst_time,ostat.dst_oid
    from stat_mov_obj_all (_oid,_start,_stop) ostat
    GROUP BY ostat.dst_oid
  ;
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

  distance  INTERVAL;
  curr_oqty BIGINT;
  mov_row   RECORD;
  curr_perc NUMERIC;
  col_name  NAME;

  obj_exists BOOLEAN;
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
               ,oid          BIGINT 
               ,constraint pk_sum_dst_time_cls PRIMARY KEY(cid,oid,dst_oid)
   );

  drop table IF EXISTS stat_mov;
  create temporary TABLE stat_mov( 
                cid      BIGINT 
               ,ctitle   NAME
               ,ckind    SMALLINT
               ,path     TEXT
               ,oid      BIGINT
               ,otitle   TEXT
               ,oqty     BIGINT
               --,CONSTRAINT uk_stat_mov UNIQUE (path)
   );
   
   -- iterate CLS
  FOR cls_row IN _cls_cursor(_cid) LOOP
    IF cls_row.ckind=0 THEN
      INSERT INTO stat_mov (cid,ctitle,ckind,path)
        VALUES(cls_row.cid, cls_row.ctitle, cls_row.ckind,cls_row.path);
    ELSE
      --collect stat by all objects
      --TRUNCATE _sum_dst_time_cls CASCADE;
      -- iterate OBJECTS in CLS
      FOR obj_row IN _obj_cursor( cls_row.cid) LOOP
        -- iterate STATISTIC for OBJECTS in CLS
        obj_exists:=FALSE;
        curr_oqty:=0;
        FOR stat_row IN _stat_cursor( obj_row.id,_begin,_end) LOOP
          obj_exists:=TRUE;
          INSERT INTO _sum_dst_time_cls (cid, oid, dst_oid,sum_dst_time)
            VALUES (cls_row.cid, obj_row.id, stat_row.dst_oid, stat_row.dst_time)
            ON CONFLICT ON CONSTRAINT pk_sum_dst_time_cls
            DO UPDATE SET sum_dst_time = _sum_dst_time_cls.sum_dst_time+EXCLUDED.sum_dst_time
            ;
        END LOOP;--FOR stat_row
        curr_oqty:= CASE WHEN obj_exists THEN curr_oqty+obj_row.qty ELSE curr_oqty END;
        --insert obj
        INSERT INTO stat_mov (oid,otitle,cid,ctitle,ckind,path,oqty)
          VALUES(obj_row.id, obj_row.title, cls_row.cid, cls_row.ctitle, cls_row.ckind,cls_row.path,curr_oqty);
      END LOOP;--FOR obj_row
    END IF;--IF cls_row.ckind=0 THEN
  END LOOP;--FOR cls_row


  -- adding missing columns
  FOR mov_row IN (SELECT ss.*,obj_name.title AS dst_otitle FROM _sum_dst_time_cls ss
                    LEFT JOIN obj_name ON obj_name.id=ss.dst_oid 
                    --WHERE ss.cid = cls_row.cid
                    ORDER BY dst_oid,dst_otitle
                 )LOOP
    col_name:='(%'||mov_row.dst_oid||')'||mov_row.dst_otitle;
    col_name:= NULLIF(REGEXP_REPLACE(col_name, '"', '','g') ,'');
    PERFORM FROM information_schema.columns 
      WHERE table_name   = 'stat_mov'  AND table_schema ~~* 'pg_temp%'
      AND column_name = col_name;
    IF NOT FOUND THEN 
      EXECUTE 'ALTER TABLE pg_temp.stat_mov ADD COLUMN "'||col_name||'" NUMERIC';
    END IF;
    
    curr_perc:= EXTRACT(EPOCH FROM mov_row.sum_dst_time)/ EXTRACT(EPOCH FROM distance) * 100;
    EXECUTE 'UPDATE pg_temp.stat_mov SET "'
      ||col_name||'"='||round(curr_perc,3)
      ||' WHERE pg_temp.stat_mov.cid='||mov_row.cid
      ||' AND pg_temp.stat_mov.oid='||mov_row.oid
      ;
  END LOOP;--FOR mov_row


  drop table IF EXISTS _sum_dst_time_cls CASCADE;
--RETURN;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000
-- ROWS 3000
;

select * from pg_temp.stat_mov_obj();
select * from pg_temp.stat_mov ORDER BY path,(substring(otitle, '^[0-9]+')::INT, otitle) ASC 
