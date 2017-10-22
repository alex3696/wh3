--SET enable_seqscan = ON;
--SET enable_seqscan = OFF;

---SELECT  json_build_object( 'days_left'::TEXT, ('44')::TEXT)

SELECT 
  --pt.arr_title[1] AS ctitle,pt.id AS id
  oo.cls_id  AS cid
  ,oo.id AS oid
  ,acls.title AS ctitle
  ,oo.title AS otitle

  ,get_path_objnum(oo.pid,1)  AS path
  ,oo.prop

,( SELECT jsonb_object_agg( ref_cls_act.act_id
			    ,  jsonb_build_object('interval', period ) 
			    || jsonb_build_object('previos',previos ) 
			    || jsonb_build_object('next',previos+period) 
			    || jsonb_build_object('left_days',ROUND( (EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 ) ) 
			    ) as last_act
				
                            
			    FROM get_path_cls_info(oo.cls_id, 1) tree
                            INNER JOIN ref_cls_act ON ref_cls_act.cls_id = tree.id   AND ref_cls_act.period IS NOT NULL
                            INNER JOIN fav_act     ON ref_cls_act.act_id = fav_act.aid AND usr=CURRENT_USER

                            LEFT JOIN (SELECT MAX(timemark) AS previos , act_id AS aid , obj_id AS oid 
                                                     FROM log_main 
					             GROUP BY obj_id,act_id)
					     last_log ON last_log.oid= oo.id
						     AND last_log.aid=ref_cls_act.act_id 
					             AND last_log.aid<>0 
					
/*
                            LEFT JOIN LATERAL (SELECT MAX(timemark) AS previos --, act_id AS aid , obj_id AS oid 
                                                     FROM log_main 
                                                     WHERE
					             log_main.obj_id= oo.id
						     AND log_main.act_id=ref_cls_act.act_id 
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

     FROM obj oo
  LEFT JOIN acls ON acls.id=oo.cls_id
  
WHERE oo.pid=108 
--WHERE oo.cls_id=105
--WHERE oo.cls_id=257;

--VACUUM FULL ANALYZE




/**
DROP TABLE IF EXISTS fav_act;
DROP SEQUENCE IF EXISTS fav_act_id_seq CASCADE;
CREATE SEQUENCE fav_act_id_seq  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE TABLE fav_act
(
  id  bigint NOT NULL DEFAULT nextval('fav_act_id_seq'::regclass),
  usr name   NOT NULL DEFAULT "current_user"(),
  aid bigint NOT NULL,
  CONSTRAINT pk_fav_act__id PRIMARY KEY (id),
  CONSTRAINT fk_fav_act_aid__act_id FOREIGN KEY (aid)
      REFERENCES act (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_fav_act_usr__role_rolname FOREIGN KEY (usr)
      REFERENCES wh_role (rolname) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT uk_fav_act__usr_aid UNIQUE (usr, aid)
)
WITH (
  OIDS=FALSE
);
GRANT SELECT ON TABLE fav_act TO "Guest";
GRANT INSERT, DELETE ON TABLE fav_act TO "User";
GRANT UPDATE(usr,aid) ON fav_act TO "User";

CREATE UNIQUE INDEX idx_fav_act__aid_usr ON fav_act(aid,usr);


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




