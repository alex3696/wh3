--SET enable_seqscan = ON;
--SET enable_seqscan = OFF;

---SELECT  json_build_object( 'days_left'::TEXT, ('44')::TEXT)

/*
SELECT * -- tree.id,tree.pid,tree.kind,tree.title,fav_act.*, last_log.* , ref_cls_act.* 
FROM get_path_cls_info(358, 1) tree  


    INNER JOIN fav_act ON fav_act.cid = tree.id
                      AND fav_act.usr = CURRENT_USER
    LEFT JOIN (SELECT MAX(timemark) AS previos, act_id AS aid , obj_id AS oid 
                FROM log_main 
                GROUP BY obj_id,act_id) 
        last_log ON last_log.oid= 5098
                AND last_log.aid=fav_act.aid
                AND last_log.aid<>0 
    LEFT JOIN ref_cls_act  ON ref_cls_act.act_id = fav_act.aid 
                           AND  ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(358, 1))
                           AND ref_cls_act.period IS NOT NULL 
                           AND fav_act.visible>1



SELECT * --tree.id,tree.pid,tree.kind,tree.title,fav_act.*, last_log.* , ref_cls_act.* 
FROM 
    (SELECT MAX(timemark) AS previos, act_id AS aid , obj_id AS oid 
                FROM log_main
                GROUP BY obj_id,act_id) 
    last_log 

    RIGHT JOIN fav_act ON  fav_act.aid = last_log.aid
                      AND fav_act.cid = 358
                      AND fav_act.usr = CURRENT_USER

    LEFT JOIN ref_cls_act  ON  ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(358, 1) )
                           AND ref_cls_act.period IS NOT NULL 
                           AND last_log.aid=ref_cls_act.act_id
                           AND fav_act.visible>1
  WHERE last_log.oid= 5098
  AND last_log.aid<>0 




SELECT * --tree.id,tree.pid,tree.kind,tree.title,fav_act.*, last_log.* , ref_cls_act.* 
FROM fav_act 

    LEFT JOIN LATERAL(SELECT MAX(timemark) AS previos, act_id AS aid , obj_id AS oid 
                FROM log_main
                --WHERE log_main.obj_id= 5098
                --AND log_main.act_id=fav_act.aid 
                --AND log_main.act_id<>0 
                GROUP BY obj_id,act_id) 
    last_log 
    --ON TRUE
    ON last_log.oid=  5098
    AND fav_act.aid = last_log.aid
    AND last_log.aid<>0 

    LEFT JOIN ref_cls_act  ON  ref_cls_act.act_id = fav_act.aid
                           AND ref_cls_act.period IS NOT NULL 
                           AND fav_act.visible>1
                           AND ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(358, 1) )

WHERE fav_act.usr = CURRENT_USER
      AND fav_act.cid        IN (SELECT id FROM get_path_cls_info(358, 1) )
      
      --fav_act.cid = 358
      




*/



SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,acls.title AS ctitle
  ,oo.title AS otitle


,( WITH  cls_tree(id) AS (
    SELECT id FROM get_path_cls_info(oo.cls_id, 1)
   )
    SELECT jsonb_object_agg( fav_act.aid  --ref_cls_act.act_id
--                 ,  jsonb_build_object('visible', visible) 
--			    ||  jsonb_build_object('previos',previos ) 
--			    || jsonb_build_object('period', period ) 
--			    || jsonb_build_object('next',previos+period) 
--			    || jsonb_build_object('left',ROUND( (EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 ) ) 

                , CASE WHEN((visible & 1)>0) THEN jsonb_build_object('previos',previos ) ELSE '{}'::jsonb END 
                || CASE WHEN((visible & 2)>0) THEN jsonb_build_object('period',period ) ELSE '{}' END 
                || CASE WHEN((visible & 4)>0) THEN jsonb_build_object('next',previos+period ) ELSE '{}' END 
                || CASE WHEN((visible & 8)>0) THEN jsonb_build_object('left',ROUND( (EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 ) )  ELSE '{}' END 
                ) as last_act

FROM fav_act 

    LEFT JOIN LATERAL(SELECT MAX(timemark) AS previos, act_id AS aid , obj_id AS oid 
                FROM log_main
            --WHERE log_main.obj_id= oo.id
            --AND log_main.act_id=fav_act.aid 
            --AND log_main.act_id<>0 
                GROUP BY obj_id,act_id) 
    last_log 
    --ON TRUE
    ON  last_log.oid = oo.id
    AND last_log.aid = fav_act.aid
    AND last_log.aid<>0 

    LEFT JOIN ref_cls_act  ON  ref_cls_act.act_id = fav_act.aid
                           AND ref_cls_act.period IS NOT NULL 
                           AND fav_act.visible>1
                           AND ref_cls_act.cls_id IN (SELECT id FROM cls_tree ) --(SELECT id FROM get_path_cls_info(oo.cls_id, 1) )
  
WHERE fav_act.usr = CURRENT_USER
      --fav_act.cid = oo.cls_id
      AND fav_act.cid IN  (SELECT id FROM cls_tree ) --(SELECT id FROM get_path_cls_info(oo.cls_id, 1) ) 
/*
                FROM get_path_cls_info(oo.cls_id, 1) tree

                INNER JOIN fav_act ON fav_act.cid = tree.id
                                  AND fav_act.usr = CURRENT_USER
                                      
                LEFT JOIN (SELECT MAX(timemark) AS previos , act_id AS aid , obj_id AS oid 
                            FROM log_main 
                            GROUP BY obj_id,act_id)
                last_log ON last_log.oid= oo.id
                        AND last_log.aid=fav_act.aid 
                        AND last_log.aid<>0 

                LEFT JOIN ref_cls_act  ON  ref_cls_act.act_id = fav_act.aid
                                       AND ref_cls_act.period IS NOT NULL 
                                       AND  ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(oo.cls_id, 1))
                                       AND fav_act.visible>1
*/
/*
                LEFT JOIN LATERAL (SELECT MAX(timemark) AS previos --, act_id AS aid , obj_id AS oid 
                                    FROM log_main 
                                    WHERE
                                    log_main.obj_id= oo.id
                                    AND log_main.act_id=fav_act.aid 
                                    AND log_main.act_id<>0 
                                    GROUP BY obj_id,act_id)
                last_log ON TRUE 
*/
/*
                          LEFT JOIN LATERAL (SELECT DISTINCT ON (obj_id,act_id) obj_id AS oid, act_id AS aid,timemark AS previos  
                                                     FROM log_main 
                                                     WHERE
					             log_main.obj_id= oo.id
						     AND log_main.act_id=ref_cls_act.act_id 
					             AND log_main.act_id<>0 
					             ORDER BY obj_id, act_id, timemark DESC LIMIT 1)
					     last_log ON TRUE 


*/
  
  

   
   ) AS pp
   --,get_path_objnum(oo.pid,1)  AS path
   --,oo.prop

     FROM obj oo
  LEFT JOIN acls ON acls.id=oo.cls_id
  
WHERE oo.pid=108 
--WHERE oo.cls_id=105
--WHERE oo.cls_id=358;
--WHERE oo.cls_id=350;

--VACUUM FULL ANALYZE




/**
DROP TABLE IF EXISTS fav_act;
DROP SEQUENCE IF EXISTS fav_act_id_seq CASCADE;
CREATE SEQUENCE fav_act_id_seq  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE TABLE fav_act
(
  id  bigint NOT NULL DEFAULT nextval('fav_act_id_seq'::regclass)
  ,usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,aid bigint NOT NULL
  ,visible SMALLINT NOT NULL DEFAULT 1 CHECK (visible BETWEEN 1 AND 15) -- 0x01=previos, 0x02=period, 0x04=next,0x08=last

  ,CONSTRAINT pk_fav_act__id PRIMARY KEY (id)
  ,CONSTRAINT uk_fav_act__usr_cid_aid UNIQUE (usr, cid, aid)
  ,CONSTRAINT fk_fav_act__aid FOREIGN KEY (aid)
      REFERENCES act (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_act_usr FOREIGN KEY (usr)
      REFERENCES wh_role (rolname) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_act__cid FOREIGN KEY (cid)
      REFERENCES acls (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE

)
WITH (
  OIDS=FALSE
);
GRANT SELECT ON TABLE fav_act TO "Guest";
GRANT INSERT, DELETE ON TABLE fav_act TO "User";
GRANT UPDATE(usr,aid) ON fav_act TO "User";



DROP VIEW IF EXISTS log_last_act;
CREATE OR REPLACE VIEW log_last_act AS 
  SELECT MAX(timemark) AS log_dt, obj_id AS oid, act_id AS aid
  FROM log_main 
  --WHERE log_main.act_id<>0
  GROUP BY obj_id,act_id;

*/


/**
DROP FUNCTION IF EXISTS get_last_act(IN _oid BIGINT, IN _cid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_last_act(IN _oid BIGINT, IN _cid BIGINT)
 RETURNS TABLE(
  aid    BIGINT
  ,period    INTERVAL
  ,previos   TIMESTAMPTZ
) AS $BODY$ 
BEGIN
RETURN QUERY 
  SELECT --
     ref_cls_act.act_id AS aid
    ,ref_cls_act.period
    ,(SELECT MAX(timemark) AS log_dt --, obj_id AS oid, act_id AS aid 
                                     FROM log_main 
                                     WHERE
					    log_main.obj_id= _oid 
					AND log_main.act_id=ref_cls_act.act_id 
					AND log_main.act_id<>0 
					GROUP BY obj_id,act_id) AS log_dt
  FROM get_path_cls_info( _cid, 1) tree -- (SELECT cls_id FROM obj_num WHERE id=_oid)
    INNER JOIN ref_cls_act ON ref_cls_act.cls_id = tree.id  AND ref_cls_act.period IS NOT NULL
    INNER JOIN fav_act     ON ref_cls_act.act_id = fav_act.aid AND usr=CURRENT_USER
    
    --INNER JOIN         (SELECT MAX(timemark) AS log_dt, obj_id AS mobj_id, act_id 
     --                   FROM log_main 
     --                   WHERE log_main.act_id<>0
     --                   GROUP BY obj_id,act_id
     --                 ) lastlog 
     --                  ON lastlog.mobj_id= _oid
     --                 AND lastlog.act_id = ref_cls_act.act_id 
    
                        ;
END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1;
GRANT EXECUTE ON FUNCTION get_last_act(IN BIGINT, IN BIGINT) TO "Guest";



SELECT * FROM get_last_act(4052,2777717)




SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,acls.title AS ctitle
  ,oo.title AS otitle
	,(SELECT jsonb_object_agg( aid
			    ,  jsonb_build_object('interval',period ) 
			    || jsonb_build_object('previos',previos ) 
			    || jsonb_build_object('next',previos+period) 
			    || jsonb_build_object('left_days',ROUND( (EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 ) ) 
			    )
			    AS jperiod
	FROM get_last_act(oo.id, oo.cls_id)
	) AS jperiod
  ,get_path_objnum(oo.pid,1)  AS path
  ,oo.prop
     FROM obj oo
  LEFT JOIN acls ON acls.id=oo.cls_id
  
   WHERE oo.pid=108 
   WHERE oo.cls_id=105
   --WHERE oo.cls_id=257;
*/





SELECT *  FROM get_path_cls_info(359, 1) cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id --AND ref_cls_act.period IS NOT NULL
INNER JOIN fav_act ON fav_act.cid IN (SELECT id FROM get_path_cls_info(359, 1))
                 AND fav_act.aid = ref_cls_act.act_id 
                 AND fav_act.usr = CURRENT_USER

SELECT * FROM 
(SELECT * FROM fav_act WHERE 
 fav_act.cid IN (SELECT id FROM get_path_cls_info(359, 1))
                 AND fav_act.usr = CURRENT_USER
)fav_all
LEFT JOIN --LATERAL
(SELECT *  FROM get_path_cls_info(359, 1) cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
 --AND ref_cls_act.act_id = aid
)pp 
--ON TRUE
ON pp.act_id = aid




  drop table IF EXISTS cls_path_359;
  create temporary TABLE cls_path_359  
    AS(
      SELECT id FROM get_path_cls_info(359, 1)
        --ORDER BY path 
      );
CREATE UNIQUE INDEX idx_cid_cls_path_359 ON cls_path_359 (id);


SELECT *  FROM cls_path_359 cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id
INNER JOIN fav_act ON fav_act.aid = ref_cls_act.act_id 
                 AND fav_act.cid IN (SELECT id FROM cls_path_359)



SELECT * FROM 
(SELECT * FROM fav_act WHERE 
 fav_act.cid IN (SELECT id FROM cls_path_359)
                 AND fav_act.usr = CURRENT_USER
)fav_all
LEFT JOIN LATERAL
(SELECT *  FROM cls_path_359 cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
AND ref_cls_act.act_id = aid
)pp ON TRUE





-- вывод всех действий для всех классов
SELECT 
  cls_list.*,
  fav_all.aid 
 ,CASE WHEN((visible & 1)>0) THEN true ELSE FALSE END  AS previos
 ,CASE WHEN((visible & 2)>0) THEN true ELSE FALSE END  AS period
 ,CASE WHEN((visible & 4)>0) THEN true ELSE FALSE END  AS next
 ,CASE WHEN((visible & 8)>0) THEN true ELSE FALSE END  AS left
--,fav_all.visible
--, pp.period
 FROM
(SELECT distinct ON(cls_id)cls_id FROM obj WHERE pid=108)  cls_list
LEFT JOIN LATERAL
(SELECT aid,visible FROM fav_act WHERE 
 fav_act.cid IN (SELECT id FROM get_path_cls_info(cls_list.cls_id, 1))
                 AND fav_act.usr = CURRENT_USER
)fav_all ON TRUE
/**
LEFT JOIN LATERAL
(SELECT ref_cls_act.*  FROM get_path_cls_info(cls_list.cls_id, 1) cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
 AND ref_cls_act.act_id = aid
)pp ON TRUE
*/




-- вывод всех видимых столбцов для ветки классов 1
SELECT fav_all.aid --,cls_list.*
 ,bool_or(CASE WHEN((visible & 1)>0) THEN true ELSE FALSE END)  AS previos
 ,bool_or(CASE WHEN((visible & 2)>0) THEN true ELSE FALSE END)  AS period
 ,bool_or(CASE WHEN((visible & 4)>0) THEN true ELSE FALSE END)  AS next
 ,bool_or(CASE WHEN((visible & 8)>0) THEN true ELSE FALSE END)  AS left
--,fav_all.visible
--, pp.period
 FROM
(SELECT distinct ON(cls_id)cls_id FROM obj WHERE pid=108)  cls_list
LEFT JOIN LATERAL
(SELECT aid,visible FROM fav_act WHERE 
 fav_act.cid IN (SELECT id FROM get_path_cls_info(cls_list.cls_id, 1))
                 AND fav_act.usr = CURRENT_USER
)fav_all ON TRUE
/**
LEFT JOIN LATERAL
(SELECT ref_cls_act.*  FROM get_path_cls_info(cls_list.cls_id, 1) cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
 AND ref_cls_act.act_id = aid
)pp ON TRUE
*/
GROUP BY fav_all.aid



-- вывод всех видимых столбцов для ветки классов 2
SELECT fav_all.aid --,cls_list.*
 ,bool_or(CASE WHEN((visible & 1)>0) THEN true ELSE FALSE END)  AS previos
 ,bool_or(CASE WHEN((visible & 2)>0) THEN true ELSE FALSE END)  AS period
 ,bool_or(CASE WHEN((visible & 4)>0) THEN true ELSE FALSE END)  AS next
 ,bool_or(CASE WHEN((visible & 8)>0) THEN true ELSE FALSE END)  AS left
--,fav_all.visible
--, pp.period
 FROM
(SELECT distinct ON(cls_id)cls_id FROM obj WHERE pid=108)  cls_list
LEFT JOIN LATERAL
(SELECT aid,visible  FROM get_path_cls_info(cls_list.cls_id, 1) cls_tree
    INNER JOIN fav_act ON fav_act.usr = CURRENT_USER AND fav_act.cid=cls_tree.id
)fav_all ON TRUE
/**
LEFT JOIN LATERAL
(SELECT ref_cls_act.*  FROM get_path_cls_info(cls_list.cls_id, 1) cls_tree
INNER JOIN ref_cls_act ON cls_tree.id=ref_cls_act.cls_id AND ref_cls_act.period IS NOT NULL
 AND ref_cls_act.act_id = aid
)pp ON TRUE
*/
GROUP BY fav_all.aid




-- вывод всех видимых столбцов для каждого класса ОБЩАЯ по местоположению / типу
SELECT cid,fav_all.aid, fav_all.visible
-- ,CASE WHEN((visible & 1)>0) THEN true ELSE FALSE END  AS previos
--,CASE WHEN((visible & 2)>0) THEN true ELSE FALSE END  AS period
-- ,CASE WHEN((visible & 4)>0) THEN true ELSE FALSE END  AS next
-- ,CASE WHEN((visible & 8)>0) THEN true ELSE FALSE END  AS left
--,fav_all.visible
--, pp.period
 FROM
--(SELECT DISTINCT ON (cls_id) cls_id AS cid FROM obj WHERE pid=108)  cls_list -- по месту
--(SELECT cls_id AS cid FROM obj WHERE pid=108 GROUP BY cls_id)  cls_list -- по месту
(SELECT id AS cid FROM acls WHERE pid=112) cls_list -- по типу
LEFT JOIN LATERAL
(SELECT aid,visible FROM fav_act WHERE 
 fav_act.cid IN (SELECT id FROM get_path_cls_info(cls_list.cid, 1))
                 AND fav_act.usr = CURRENT_USER
)fav_all ON TRUE
ORDER BY cid




-- вывод всех видимых столбцов для ветки классов ОБЩАЯ по pid -- оптимизированный по отдельности

SELECT  cid, fav_all.aid,fav_all.visible
FROM
  (SELECT id AS cid FROM acls WHERE pid=112
  )cls_list
    LEFT JOIN LATERAL
  (SELECT aid,visible  FROM  fav_act WHERE fav_act.usr = CURRENT_USER AND fav_act.cid=cls_list.cid
  )fav_all ON TRUE


SELECT cid2, fav_all.aid 
,fav_all.visible
--, pp.period
 FROM
(SELECT cls_tree.id AS cid2, aid,visible  FROM get_path_cls_info(112, 1) cls_tree
    INNER JOIN fav_act ON fav_act.usr = CURRENT_USER AND fav_act.cid=cls_tree.id
)fav_all 


-- вывод всех видимых столбцов для ветки классов ОБЩАЯ по pid -- оптимизированный с рекурсией
WITH  cls_tree(cid,aid,visible) AS (
    SELECT cls.id , aid, visible FROM get_path_cls_info( 112, 1) cls
      INNER JOIN fav_act ON fav_act.usr = CURRENT_USER AND fav_act.cid=cls.id

            )
SELECT * FROM
            (
WITH RECURSIVE top(cid,aid,visible,n) AS (

    SELECT  cls_list.id, fav_act.aid, fav_act.visible,row_number() over ()AS n
    FROM acls  cls_list
    LEFT JOIN  fav_act ON fav_act.cid=cls_list.id AND fav_act.usr = CURRENT_USER 
    WHERE cls_list.pid=112

  UNION  (
     SELECT p.cid, fav_act.aid, fav_act.visible , NULL::BIGINT AS n
      FROM top p 
      LEFT JOIN  cls_tree ON TRUE
      INNER JOIN fav_act ON fav_act.usr = CURRENT_USER AND fav_act.cid=cls_tree.cid

      WHERE n=p.n 
  )


   

)
SELECT * FROM top WHERE aid IS NOT NULL 
) mm
ORDER BY cid
--LIMIT 100


-- вывод всех видимых столбцов для ветки классов ОБЩАЯ по pid -- оптимизированный c вложенными запросами
WITH curr AS (SELECT 112::BIGINT AS pid)  
SELECT * FROM
(
  WITH  cls_current(cid,aid,visible) AS (
    SELECT  acls.id AS cid, fav_act.aid, fav_act.visible
    FROM acls   
    LEFT JOIN  fav_act ON fav_act.cid=acls.id AND fav_act.usr = CURRENT_USER 
    WHERE acls.pid=(SELECT pid FROM curr)
  )
  SELECT * FROM
  (
    WITH  cls_tree(cid,aid,visible) AS (
    SELECT cls.id , aid, visible FROM get_path_cls_info( (SELECT pid FROM curr), 1) cls
      INNER JOIN fav_act ON fav_act.usr = CURRENT_USER AND fav_act.cid=cls.id
    )
    SELECT  cls_current.cid, cls_tree.aid, cls_tree.visible, cls_tree.cid AS tree_cid
      FROM cls_current
      LEFT JOIN fav_act ON fav_act.cid= cls_current.cid AND fav_act.usr = CURRENT_USER 
      LEFT JOIN cls_tree ON TRUE
    UNION ALL
    SELECT cls_current.*, cls_current.cid FROM cls_current WHERE cls_current.aid IS NOT NULL
  )w1
)w2


ORDER BY w2.cid,w2.aid







