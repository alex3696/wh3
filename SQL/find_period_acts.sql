SELECT tree.*
  ,ref_cls_act.act_id
  ,ref_cls_act.period
FROM (
SELECT 
  --DISTINCT(obj.cls_id),
  pt.arr_title[1] AS ctitle
  ,obj.cls_id  AS cid
  ,obj.title   AS otitle
  ,obj.id      AS oid
  ,pt.*
  FROM obj 
  --RIGHT JOIN (SELECT id FROM get_path_cls_info(obj.cls_id, 0))pcls ON pcls.id=obj.cls_id
  --RIGHT JOIN ref_cls_act cact ON obj.cls_id=cact.cls_id
  LEFT JOIN LATERAL get_path_cls_info(obj.cls_id, 1)pt on true
  WHERE obj.pid=108
  ) tree
  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
  WHERE period IS NOT NULL


SELECT tree.*
  --,act.title
  ,ref_cls_act.act_id
  ,ref_cls_act.period
FROM (
SELECT 
  pt.arr_title[1] AS ctitle
  ,oo.cid
  ,oo.oid
  ,oo.otitle
  ,pt.id AS id
  FROM  
  (SELECT /*DISTINCT(obj.cls_id)*/obj.cls_id  AS cid,obj.title AS otitle, obj.id AS oid FROM obj WHERE obj.pid=108)oo
  LEFT JOIN LATERAL get_path_cls_info(oo.cid, 1)pt on true
  ) tree
  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
  --LEFT JOIN act ON act.id = ref_cls_act.act_id
  WHERE period IS NOT NULL



/*
SELECT DISTINCT ON (mobj_id,act_id) mobj_id,act_id, log_dt
FROM LOG 
WHERE 
--mobj_id=1894 AND 
act_id IS NOT NULL
ORDER BY mobj_id,act_id,log_dt DESC

SELECT *
FROM LOG 
WHERE 
    mobj_id=2071
AND act_id IS NOT NULL
ORDER BY log_dt DESC
*/



SELECT tree.*
  ,act.title
  ,ref_cls_act.act_id
  ,ref_cls_act.period
  ,lastlog.log_dt
  --, (now() - lastlog.log_dt) AS dt_after
  , ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/3600/24)::NUMERIC,2 )
  --,to_char(justify_hours(ref_cls_act.period - (now() - lastlog.log_dt)),'DD HH24:MI:SS') AS to_next2
  --,justify_hours(ref_cls_act.period - (now() - lastlog.log_dt)) AS to_next1
  --,ref_cls_act.period + ( lastlog.log_dt - now()) AS to_next0
  --,(lastlog.log_dt + ref_cls_act.period -now()) AS to_next00
  
FROM (
SELECT 
  pt.arr_title[1] AS ctitle
  ,oo.cid
  ,oo.oid
  ,oo.otitle
  ,pt.id AS id
  FROM  
  (SELECT /*DISTINCT(obj.cls_id),*/ obj.cls_id  AS cid,obj.title AS otitle, obj.id AS oid 
    FROM obj WHERE obj.pid=108)oo
  LEFT JOIN LATERAL get_path_cls_info(oo.cid, 1)pt on true
  ) tree
  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
  LEFT JOIN act ON act.id = ref_cls_act.act_id
  LEFT JOIN LATERAL (SELECT DISTINCT ON (mobj_id,act_id) act_id, log_dt,mobj_id
             FROM LOG WHERE act_id IS NOT NULL ORDER BY mobj_id,act_id,log_dt DESC)lastlog 
               ON  lastlog.mobj_id=tree.oid
               AND lastlog.act_id=act.id
  WHERE period IS NOT NULL
  AND act.id=105





/**
Вычисление остатка периодического действия с временной таблицей
*/
-- Таблица-Кэш для последних выполненных действий
DROP TABLE IF EXISTS log_last_act;
CREATE TEMPORARY TABLE log_last_act AS(
    SELECT DISTINCT ON (mobj_id,act_id) act_id, log_dt, mobj_id AS obj_id
    FROM LOG WHERE act_id IS NOT NULL ORDER BY mobj_id,act_id,log_dt DESC);
ALTER TABLE log_last_act
  ADD CONSTRAINT pk_log_last_act PRIMARY KEY(obj_id,act_id);



DROP INDEX idx_log_last_act__oid_aid_tid;

CREATE INDEX idx_log_last_act__oid
  ON log_last_act (obj_id );
CREATE INDEX idx_log_last_act__aid
  ON log_last_act (act_id );
CREATE INDEX idx_log_last_act__oid_aid
  ON log_last_act (obj_id, act_id );


SELECT * FROM log_last_act;

  ANALYZE;


SELECT tree.*
  ,act.title
  ,ref_cls_act.act_id
  ,ref_cls_act.period
  ,lastlog.log_dt
  --, (now() - lastlog.log_dt) AS dt_after
  , ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/3600/24)::NUMERIC,2 )
  --,to_char(justify_hours(ref_cls_act.period - (now() - lastlog.log_dt)),'DD HH24:MI:SS') AS to_next2
  --,justify_hours(ref_cls_act.period - (now() - lastlog.log_dt)) AS to_next1
  --,ref_cls_act.period + ( lastlog.log_dt - now()) AS to_next0
  --,(lastlog.log_dt + ref_cls_act.period -now()) AS to_next00
  
FROM (
SELECT 
  pt.arr_title[1] AS ctitle
  ,oo.cid
  ,oo.oid
  ,oo.otitle
  ,pt.id AS id
  FROM  
  (SELECT /*DISTINCT(obj.cls_id)*/obj.cls_id  AS cid,obj.title AS otitle, obj.id AS oid 
    FROM obj WHERE obj.pid=108)oo
  LEFT JOIN LATERAL get_path_cls_info(oo.cid, 1)pt on true
  ) tree
  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
  LEFT JOIN act ON act.id = ref_cls_act.act_id
  LEFT JOIN log_last_act lastlog
               ON  lastlog.obj_id=tree.oid
               AND lastlog.act_id=act.id
  WHERE 
  period IS NOT NULL
  AND act.id=105



SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,(ARRAY(SELECT
                  ref_cls_act.act_id||':'||
                  ROUND( (EXTRACT(EPOCH FROM (log_last_act.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 )
                  FROM get_path_cls_info(oo.cls_id, 0) tree 
                       --(SELECT * FROM acls WHERE id=oo.cls_id) tree
                  LEFT  JOIN ref_cls_act  ON tree.id = ref_cls_act.cls_id
                  LEFT  JOIN act  ON act.id = ref_cls_act.act_id
                  RIGHT JOIN log_last_act ON log_last_act.obj_id=oo.id AND log_last_act.act_id=ref_cls_act.act_id
                  WHERE ref_cls_act.period IS NOT NULL
                  )
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108


SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,(SELECT 
     json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (log_last_act.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) )
                  FROM get_path_cls_info(oo.cls_id, 0) tree 
                       --(SELECT * FROM acls WHERE id=oo.cls_id) tree
                  LEFT  JOIN ref_cls_act  ON tree.id = ref_cls_act.cls_id
                  LEFT  JOIN act  ON act.id = ref_cls_act.act_id
                  RIGHT JOIN log_last_act ON log_last_act.obj_id=oo.id AND log_last_act.act_id=ref_cls_act.act_id
                  WHERE ref_cls_act.period IS NOT NULL
                  --AND ( ref_cls_act.act_id =105 /*OR ref_cls_act.act_id =104 */)
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108
   --WHERE oo.cls_id=252

SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,(SELECT 
     json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (log_last_act.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) )
                  FROM get_path_cls_info(oo.cls_id, 0) tree 
                       --(SELECT * FROM acls WHERE id=oo.cls_id) tree
                  INNER  JOIN ref_cls_act  ON tree.id = ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
                  LEFT JOIN log_last_act ON log_last_act.obj_id=oo.id AND log_last_act.act_id=ref_cls_act.act_id
                  --WHERE log_last_act.act_id=ref_cls_act.act_id
                  --AND ( ref_cls_act.act_id =105 /*OR ref_cls_act.act_id =104 */)
                  LEFT  JOIN act  ON act.id = ref_cls_act.act_id
                  --WHERE( act.id =105 OR act.id =104 )
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108


SELECT count(*),cls_id FROM obj GROUP BY cls_id ORDER BY count DESC

   
--SELECT jsonb_pretty('{"104":56.78, "105":{"aname":"Калибровка","days left":54.89 }}'::JSONB)

SELECT jsonb_pretty('{"104":35.01,"105":34.01}')


SELECT string_agg(quote_ident(id::TEXT), ':55, ') AS actor_list FROM (SELECT id FROM acls LIMIT 5)xx
SELECT json_object_agg(id,title)AS result FROM (SELECT id,title FROM acls LIMIT 5)xx
-------------------------------



/** JSON массив остатков дней*/
SELECT ARRAY(
SELECT
  ARRAY[ref_cls_act.act_id,ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/3600/24)::NUMERIC,2 ) ]
FROM get_path_cls_info(199, 1) tree
  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
  LEFT JOIN act ON act.id = ref_cls_act.act_id
  LEFT JOIN LATERAL (SELECT DISTINCT ON (mobj_id,act_id) act_id, log_dt,mobj_id
             FROM LOG WHERE act_id IS NOT NULL ORDER BY mobj_id,act_id,log_dt DESC)lastlog 
               ON  lastlog.mobj_id=2071
               AND lastlog.act_id=act.id
  WHERE period IS NOT NULL
)



/** список объектов с массивом остатков периодов с вычислением последних действий*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,( ARRAY(SELECT
                  ref_cls_act.act_id ||':'||ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/3600/24)::NUMERIC,2 ) 
                  FROM get_path_cls_info(oo.cls_id, 1) tree
                  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
                  LEFT JOIN act ON act.id = ref_cls_act.act_id
                  LEFT JOIN LATERAL (SELECT DISTINCT ON (mobj_id,act_id) act_id, log_dt,mobj_id
                                     FROM LOG 
                                     WHERE act_id IS NOT NULL 
                                     ORDER BY mobj_id,act_id,log_dt DESC
                                     ) lastlog 
                                     ON  lastlog.mobj_id=oo.id
                                     AND lastlog.act_id=act.id
                  WHERE period IS NOT NULL
                  AND lastlog.log_dt IS NOT NULL
                  )
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108


/** список объектов с массивом остатков периодов с укороченым вычислением последних действий*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) )
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  LEFT JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id
                  RIGHT JOIN (SELECT DISTINCT ON (obj_id,act_id) log_detail_act.act_id,obj_id AS mobj_id,timemark AS log_dt
                                     FROM log_detail_act
                                     LEFT JOIN log_main USING (id)
                                     ORDER BY obj_id,act_id,timemark DESC
                                     ) lastlog 
                                     ON  lastlog.mobj_id=oo.id
                                     AND lastlog.act_id=ref_cls_act.act_id
                  WHERE ref_cls_act.period IS NOT NULL
                  
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108
   --WHERE oo.cls_id=252
   


/** список объектов с массивом остатков периодов с дополнительным стодбцом в log_main*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) )
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  INNER JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id --AND ref_cls_act.period IS NOT NULL
                  RIGHT JOIN (SELECT DISTINCT ON (obj_id,act_id)
                                     obj_id AS mobj_id, act_id,timemark AS log_dt
                                     FROM log_main 
                                     WHERE act_id<>0
                                     ORDER BY obj_id, act_id, timemark DESC
                                     ) lastlog 
                                     ON  lastlog.mobj_id=oo.id
                                     AND lastlog.act_id=ref_cls_act.act_id
                  WHERE ref_cls_act.period IS NOT NULL
                  --AND ( ref_cls_act.act_id =105 /*OR ref_cls_act.act_id =104 */)
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108
   --WHERE oo.cls_id=252

/** список объектов с массивом остатков периодов с дополнительным стодбцом в log_main + MAX*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) ) AS ss
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  INNER JOIN ref_cls_act ON tree.id = ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
                  LEFT JOIN (SELECT MAX(timemark) AS log_dt,
                                     obj_id AS mobj_id, act_id
                                     FROM log_main 
                                     WHERE act_id<>0
                                     GROUP BY obj_id,act_id
                                     ) lastlog 
                                     ON  lastlog.mobj_id=oo.id
                                     AND lastlog.act_id=ref_cls_act.act_id
                  --WHERE ref_cls_act.period IS NOT NULL
                  --AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108


/** список объектов с массивом остатков периодов с дополнительным стодбцом в log_main + MAX ++++++*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) )
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  INNER JOIN ref_cls_act ON ref_cls_act.cls_id = tree.id  AND ref_cls_act.period IS NOT NULL
                        --AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
                  LEFT JOIN LATERAL (SELECT MAX(timemark) AS log_dt,
                                     obj_id AS mobj_id, act_id
                                     FROM log_main 
                                     WHERE act_id<>0
                                     AND log_main.act_id=ref_cls_act.act_id
                                     AND log_main.obj_id=oo.id
                                     GROUP BY obj_id,act_id
                                     ) lastlog 
                                     ON TRUE
                                     --ON lastlog.mobj_id=oo.id
                                     --AND lastlog.act_id=ref_cls_act.act_id
                  --WHERE ref_cls_act.period IS NOT NULL
                  --AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
   ) AS pp
  FROM obj oo
   WHERE oo.pid=108
--ORDER BY cid


/** список объектов с массивом остатков периодов с дополнительным стодбцом в log_main + MAX +2*/
SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,oo.title AS otitle
  ,jperiod
  FROM obj oo
  LEFT JOIN LATERAL
( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) ) AS jperiod
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  INNER JOIN ref_cls_act ON ref_cls_act.cls_id = tree.id  AND ref_cls_act.period IS NOT NULL
                     AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
                  LEFT JOIN LATERAL(SELECT MAX(timemark) AS log_dt, obj_id AS mobj_id, act_id
                                    --SELECT DISTINCT ON (obj_id,act_id) obj_id AS mobj_id, act_id,timemark AS log_dt  
                                    --SELECT  obj_id AS mobj_id, act_id,timemark AS log_dt  
                                     FROM log_main 
                                     WHERE act_id<>0
                                     --AND log_main.act_id=ref_cls_act.act_id
                                     --AND log_main.obj_id=oo.id
                                     GROUP BY obj_id,act_id
                                     --ORDER BY obj_id, act_id, timemark DESC LIMIT 1
                                     ) lastlog 
                                     --ON TRUE
                                     --
                                     ON lastlog.act_id=ref_cls_act.act_id
                                     AND lastlog.mobj_id=oo.id
                  --WHERE ref_cls_act.period IS NOT NULL
                  --AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
   ) AS pp
   ON TRUE

  
   WHERE oo.pid=108

--ORDER BY cid



SET enable_seqscan TO oN;
SET enable_seqscan TO off;
DROP INDEX idx_ref_cls_act__ll;
CREATE INDEX idx_ref_cls_act__ll
  ON ref_cls_act (cls_id,act_id)
  WHERE ( period IS NOT NULL );


DROP INDEX IF EXISTS idx_log_main__oid_dt ; 
CREATE INDEX idx_log_main__oid_dt 
  ON log_main (obj_id, timemark DESC);
  


SELECT log_id, log_dt, log_user ,mcls_id, mcls_title, mobj_id, mobj_title, qty 
      ,act_id, act_title, act_color, prop 
      ,src_cid, src_oid, src_ipath, src_path ,dst_cid, dst_oid
      , dst_ipath, dst_path ,prop_lid, mcls_kind, mcls_measure  
 FROM log  
 WHERE    --( mobj_id='2032') AND 
 act_id=105
 ORDER BY   log_dt DESC  LIMIT 10

DROP INDEX IF EXISTS idx_log_main__last_act ;

/**
CREATE INDEX idx_log_main__last_act 
  ON log_main (obj_id, act_id, timemark DESC)
  WHERE ( act_id IS NOT NULL);
*/

CREATE INDEX idx_log_main__last_act 
  ON log_main (obj_id, act_id, timemark DESC)
  WHERE ( act_id <> 0);
  
CREATE UNIQUE INDEX idx_log_main__last_act 
  ON log_main (obj_id, act_id, timemark DESC)
  WHERE ( act_id<>0 );

CREATE INDEX idx_log_main__last_act 
  ON log_main (obj_id, act_id)
  WHERE ( act_id<>0 );

DROP FUNCTION maxtsz( _oid BIGINT, _aid BIGINT ) ;
CREATE OR REPLACE FUNCTION maxtsz( _oid BIGINT, _aid BIGINT ) 
--RETURNS TABLE(obj_id BIGINT, act_id BIGINT, timemark TIMESTAMPTZ)
RETURNS TIMESTAMPTZ
AS $$
    SELECT MAX(timemark) AS timemark --,obj_id, act_id
    FROM log_main
    WHERE act_id <>0
    AND obj_id=_oid
    AND act_id=_aid
    GROUP BY obj_id,act_id
$$ LANGUAGE SQL IMMUTABLE;

SELECT * FROM maxtsz(2071,105)

DROP EXTENSION hstore;
CREATE EXTENSION hstore;
SELECT 'a=>1,a=>2'::hstore;

DROP EXTENSION btree_gist 
CREATE EXTENSION btree_gist 

DROP EXTENSION btree_gin
CREATE EXTENSION btree_gin 


CREATE INDEX idx_log_main__last_act 
  ON log_main USING GIN
  (obj_id, act_id,timemark )
  WHERE ( act_id<>0 );


ANALYZE log_main;






ANALYZE log_main;
VACUUM FULL ANALYZE log_main;























