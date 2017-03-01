DROP FUNCTION IF EXISTS pg_temp.get_childs_cls_info(IN _obj_id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.get_childs_cls_info(IN _obj_id BIGINT)
 RETURNS TABLE(
  _id    BIGINT
  ,_title WHNAME
  ,_kind  SMALLINT
  ,_pid   BIGINT
  ,_note  TEXT
  ,_measure WHNAME
  ,_dobj  BIGINT
  ,_path  TEXT
  ,_arr_id  BIGINT[]
  ,_arr_title NAME[]
) AS $BODY$ 
BEGIN
RETURN QUERY(
    WITH RECURSIVE children AS (
    SELECT id,  title, kind, pid,  note, measure,dobj
           ,ARRAY[id]                            AS exist
           ,FALSE                                AS cycle
           , '' AS path
           , ARRAY[id] AS arr_id
           , ARRAY[title::NAME] AS arr_title

    FROM acls
    WHERE id = _obj_id
    UNION ALL
        SELECT t.id, t.title, t.kind, t.pid, t.note, t.measure, t.dobj
               ,exist || t.id 
               ,t.id = ANY(exist)
               , c.path||'/'||t.title
               , c.arr_id     || ARRAY[t.id]::BIGINT[]
               , c.arr_title  || ARRAY[t.title::NAME]::NAME[]
        FROM children AS c, 
             acls  AS t
        WHERE t.pid = c.id AND 
              NOT cycle 
              --AND array_length(exist, 1) < 1000 -- глубина дерева
)
SELECT id,  title, kind, pid,  note, measure,dobj,path,arr_id,arr_title
    FROM children WHERE NOT cycle --ORDER BY ord LIMIT 100;
    );
END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1000;

/**
SELECT obj_name.title,cls.* 
FROM pg_temp.get_childs_cls_info(101) cls
LEFT  JOIN obj_name ON obj_name.cls_id=cls._id
ORDER BY _arr_id,(substring(obj_name.title, '^[0-9]+')::INT, obj_name.title) ASC 
*/
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------



DROP FUNCTION IF EXISTS pg_temp.place_time(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.place_time(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) 
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
  ELSE
    cid          := rec.mcls_id;
    prev_src_oid := rec.src_oid;
    prev_place   := rec.dst_oid;
  END IF;
  RAISE NOTICE  'rec=%', rec;
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
SELECT * FROM pg_temp.place_time (3626,'2016.09.10 12:00+5','2017.02.27 20:00+5');

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

DROP FUNCTION IF EXISTS pg_temp.place_stat(_cid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.place_stat(_cid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) 
 RETURNS TABLE( cid      BIGINT 
               ,oid      BIGINT
               ,ctitle   NAME
               ,otitle   NAME
               ,ckind    SMALLINT
               ,ipath    BIGINT[]
               ,path     TEXT
               ,dst_time INTERVAL
               ,dst_percent DOUBLE PRECISION
               ,dst_oid  BIGINT
              ) AS $BODY$ 
DECLARE
  _obj_cursor CURSOR IS
    SELECT cls._id        AS cid
          ,obj_name.id    as oid
          ,cls._title     AS ctitle
          ,obj_name.title AS otitle
          ,cls._kind      AS ckind
          ,cls._arr_id    AS arr_id
          ,cls._path      AS path
    FROM pg_temp.get_childs_cls_info(_cid) cls
    LEFT  JOIN obj_name ON obj_name.cls_id=cls._id
    --WHERE _kind>0
    --ORDER BY _arr_id,(substring(obj_name.title, '^[0-9]+')::INT, obj_name.title) ASC 
  ;
  _stat_cursor CURSOR( _oid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP) IS
    select SUM(ostat.dst_time) AS dst_time,ostat.dst_oid,ostat.cid,ostat.oid 
    from pg_temp.place_time (_oid,_start,_stop) ostat
    GROUP BY ostat.dst_oid,ostat.cid,ostat.oid 
  ;

  distance INTERVAL;
BEGIN 
  distance := _end - _begin;
  RAISE NOTICE  'distance = %',distance;
  FOR obj IN _obj_cursor LOOP
    otitle:=  obj.otitle;
    ctitle:=  obj.ctitle;
    ipath:=   obj.arr_id;
    path:=    obj.path;
    ckind:=   obj.ckind;
    cid :=    obj.cid;
    IF ckind=0 THEN 
      --cid:=NULL;
      oid:=NULL;
      dst_oid:=NULL;
      dst_time:=NULL;
      dst_percent:=NULL;
      RETURN NEXT;
    ELSE
      FOR stat IN _stat_cursor(obj.oid,_begin,_end) LOOP
        --cid:=stat.cid;
        oid:=stat.oid;
        dst_oid:=stat.dst_oid;
        dst_time:=stat.dst_time;
        dst_percent:= EXTRACT(EPOCH FROM stat.dst_time)/ EXTRACT(EPOCH FROM distance) * 100;
        RETURN NEXT;
      END LOOP; --FOR stat IN _stat_cursor(obj.oid,_begin,_end) LOOP
    END IF;
  END LOOP; --FOR obj IN _obj_cursor LOOP

RETURN;

END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1000;
/**
    SELECT * FROM pg_temp.place_stat(112,'2016.09.10 12:00+5','2017.02.27 20:00+5')
    order by ipath,oid




    SELECT obj_name.title,obj_name.id as oid,cls.* 
    FROM pg_temp.get_childs_cls_info(112) cls
    LEFT  JOIN obj_name ON obj_name.cls_id=cls._id
    WHERE _kind>0
    ORDER BY _arr_id,(substring(obj_name.title, '^[0-9]+')::INT, obj_name.title) ASC 

    select SUM(ostat.dst_time) AS dst_time,ostat.dst_oid,ostat.cid,ostat.oid 
    from pg_temp.place_time (3626,'2016.09.10 12:00+5','2017.02.27 20:00+5') ostat
    GROUP BY ostat.dst_oid,ostat.cid,ostat.oid;

    SELECT ss.* , obj.title AS dst_title
    FROM pg_temp.place_stat(101,'2016.09.10 12:00+5','2017.02.27 20:00+5') ss
    LEFT JOIN obj ON obj.id=ss.dst_oid
    ORDER BY path,(substring(otitle, '^[0-9]+')::INT, otitle) ASC, dst_oid




    SELECT distinct(ss.dst_oid) AS dst_oid, obj.title
    FROM pg_temp.place_stat(101,'2016.09.10 12:00+5','2017.02.27 20:00+5') ss
    LEFT JOIN obj ON obj.id=ss.dst_oid
    ORDER BY dst_oid


    SELECT sum(ss.dst_time) AS dst_summ, obj.title
    FROM pg_temp.place_stat(101,'2016.09.10 12:00+5','2017.02.27 20:00+5') ss
    LEFT JOIN obj ON obj.id=ss.dst_oid
    GROUP BY ss.dst_oid,obj.title
    ORDER BY ss.dst_oid


*/
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

DO LANGUAGE plpgsql $$ 
DECLARE
  _begin TIMESTAMP;
  _end TIMESTAMP;
  _cid BIGINT;

    _stat_cursor2 CURSOR( _cid  BIGINT, _start TIMESTAMP, _stop TIMESTAMP) IS
    SELECT ss.* , obj.title AS dst_title
    FROM pg_temp.place_stat(_cid,_start,_stop) ss
    LEFT JOIN obj ON obj.id=ss.dst_oid
    ORDER BY dst_oid
    --path,(substring(otitle, '^[0-9]+')::INT, otitle) ASC , dst_oid
    ;

  curr_otitle NAME;
  curr_rec    RECORD;

  update_str TEXT;
BEGIN
  _begin := '2016.12.31 00:00'::TIMESTAMP;
  _end := '2017.02.27 00:00'::TIMESTAMP;
  _cid := 433 ;

  drop table IF EXISTS place_time;
  
  create temporary TABLE place_time( 
                cid      BIGINT 
               ,oid      BIGINT
               ,ctitle   NAME
               ,otitle   NAME
               ,ckind    SMALLINT
               ,ipath    BIGINT[]
               ,path     TEXT
   );
  FOR stat IN _stat_cursor2(_cid,_begin,_end) LOOP
    IF stat.ckind=0 THEN
      INSERT INTO place_time(cid,ctitle,ckind,ipath,path) 
                    VALUES (stat.cid,stat.ctitle,stat.ckind,stat.ipath,stat.path) ;
    ELSE
      RAISE NOTICE  'stat=%   ',stat;
      curr_otitle:=stat.otitle;
      WHILE curr_otitle=stat.otitle AND curr_otitle IS NOT NULL LOOP
        PERFORM FROM information_schema.columns 
          WHERE table_name   = 'place_time'  AND table_schema ~~* 'pg_temp%'
          AND column_name ILIKE stat.dst_title||'(perc)';
        IF NOT FOUND THEN 
          --EXECUTE 'ALTER TABLE pg_temp.place_time ADD COLUMN "'||stat.dst_title||'(sec)" DOUBLE PRECISION';
          EXECUTE 'ALTER TABLE pg_temp.place_time ADD COLUMN "'||stat.dst_title||'(perc)" NUMERIC';
        END IF;
        PERFORM FROM pg_temp.place_time WHERE cid=stat.cid AND oid=stat.oid;
        IF NOT FOUND THEN 
          INSERT INTO place_time(cid,ctitle,ckind,ipath,path,oid,otitle) 
                      VALUES (stat.cid,stat.ctitle,stat.ckind,stat.ipath,stat.path,stat.oid,stat.otitle) ;
        END IF;
        --RAISE NOTICE  'stat.dst_percent=%   ',stat.dst_percent;
        --update_str :=round(stat.dst_percent,2);
        EXECUTE 'UPDATE pg_temp.place_time SET "'||stat.dst_title||'(perc)"='||round(stat.dst_percent::NUMERIC,3)
         ||' WHERE pg_temp.place_time.cid='||stat.cid||' AND pg_temp.place_time.oid='||stat.oid;
      --INSERT  INTO place_time VALUES stat;
        FETCH _stat_cursor2 INTO stat;
        curr_otitle:=stat.otitle;
      END LOOP;
    END IF;
   END LOOP; --FOR stat IN _stat_cursor(obj.oid,_begin,_end) LOOP

END $$;


select 

* 
from pg_temp.place_time 
 ORDER BY path,(substring(otitle, '^[0-9]+')::INT, otitle) ASC 



-- SELECT ROUND(02.19::NUMERIC(10,4),3)





