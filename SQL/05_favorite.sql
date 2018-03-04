SET default_transaction_isolation =serializable;
BEGIN TRANSACTION;
SET client_min_messages = 'warning';
SHOW client_min_messages;
-------------------------------------------------------------------------------
------ избранные свойства для действий ----------------------------------------
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS fav_act CASCADE;
CREATE TABLE fav_act
(
  usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,aid bigint NOT NULL
  ,info SMALLINT NOT NULL DEFAULT 1 CHECK 
    (    info=01 OR info=02 OR info=03 
      OR info=11 OR info=12 OR info=13
      OR info=21 OR info=22 OR info=23
      OR info=31 OR info=32 OR info=33 )
  ,CONSTRAINT pk_fav_act__usr_cid_aid_info PRIMARY KEY (usr, cid, aid, info)
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
GRANT SELECT, INSERT, DELETE, UPDATE ON TABLE fav_act TO "Guest";
-------------------------------------------------------------------------------
------ избранные свойства для ОБЬЕКТОВ ----------------------------------------
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS fav_oprop CASCADE;
CREATE TABLE fav_oprop
(
  usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,pid bigint NOT NULL

  ,CONSTRAINT pk_fav_oprop__usr_cid_pid PRIMARY KEY (usr, cid, pid)
  ,CONSTRAINT fk_fav_oprop__pid FOREIGN KEY (pid)
      REFERENCES prop (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_oprop_usr FOREIGN KEY (usr)
      REFERENCES wh_role (rolname) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_oprop__cid FOREIGN KEY (cid)
      REFERENCES acls (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);
GRANT SELECT, INSERT, DELETE, UPDATE ON TABLE fav_oprop TO "Guest";
-------------------------------------------------------------------------------
------ избранные свойства для КЛАССОВ  ----------------------------------------
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS fav_cprop CASCADE;
CREATE TABLE fav_cprop
(
  usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,pid bigint NOT NULL

  ,CONSTRAINT pk_fav_cprop__usr_cid_pid PRIMARY KEY (usr, cid, pid)
  ,CONSTRAINT fk_fav_cprop__pid FOREIGN KEY (pid)
      REFERENCES prop (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_cprop_usr FOREIGN KEY (usr)
      REFERENCES wh_role (rolname) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
  ,CONSTRAINT fk_fav_cprop__cid FOREIGN KEY (cid)
      REFERENCES acls (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE
)
WITH (
  OIDS=FALSE
);
GRANT SELECT, INSERT, DELETE, UPDATE ON TABLE fav_cprop TO "Guest";
-------------------------------------------------------------------------------
------ избранные свойства для обьектов  ----------------------------------------
-------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj_fav_info;
CREATE OR REPLACE VIEW  obj_fav_info AS 
  SELECT 
     obj.id
    ,obj.pid
    ,obj.title
    ,obj.cls_id
    ,obj.cls_kind
    ,obj.qty
    ,obj.move_logid
    ,obj.act_logid

,( WITH  cls_tree(id) AS (
    SELECT id FROM get_path_cls_info(obj.cls_id, 1)
   )
  SELECT(
          COALESCE(
            NULLIF(
              jsonb_build_object(
                                  'fav_oprop',(
                                                SELECT jsonb_object_agg( fav_oprop.pid ,prop->>fav_oprop.pid::TEXT  ) 
                                                  FROM fav_oprop
                                                  INNER JOIN cls_tree tree ON tree.id= fav_oprop.cid  AND usr=CURRENT_USER
                                                  WHERE prop ? fav_oprop.pid::TEXT 
                                              )
                                )
              ,'{"fav_oprop": null}'
            ) 
            ,'{}'::JSONB 
          )
        )
        ||
        (--jsonb_pretty(
          COALESCE(
            NULLIF(
              jsonb_build_object('fav_act', jsonb_object_agg(fav_act_prop.aid,fav_act_prop.val))
              ,'{"fav_act": null}'
            )
            ,'{}'::JSONB 
          )
         --)
        )         
        as last_act
        FROM (  
                WITH cls_fav(aid) AS (
                        SELECT aid,info
                        FROM fav_act 
                        INNER JOIN cls_tree ON fav_act.cid=cls_tree.id AND usr = CURRENT_USER 
                    )
                ,fav_distinct(aid,max_info) AS (
                SELECT aid,max(info)
                FROM fav_act 
                INNER JOIN cls_tree ON fav_act.cid=cls_tree.id AND usr = CURRENT_USER 
                GROUP BY aid
               )
            --SELECT * FROM fav_distinct
            , fav_period(aid,period) AS (
                    SELECT ref_cls_act.act_id, ref_cls_act.period
                    FROM cls_tree ct
                    INNER JOIN ref_cls_act ON ref_cls_act.period IS NOT NULL
                                          AND ref_cls_act.cls_id = ct.id
                    INNER JOIN fav_distinct ON fav_distinct.aid=ref_cls_act.act_id
                                          AND max_info>10
                  )
              --SELECT * FROM fav_period
            ,  last_log AS (
               SELECT MAX(timemark) AS previos, act_id AS aid, obj_id AS oid 
                    FROM fav_distinct
                    INNER JOIN log_main ON fav_distinct.aid=log_main.act_id AND act_id<>0 AND log_main.obj_id = obj.id
                    GROUP BY obj_id, act_id
                    )
              --  SELECT * FROM last_log
            , fav_info AS (
                SELECT cls_fav.*,period,previos
                FROM cls_fav
                LEFT JOIN fav_period ON fav_period.aid=cls_fav.aid
                LEFT JOIN last_log   ON last_log.aid=cls_fav.aid
                WHERE (period IS NULL AND info<10 ) or (period IS NOT NULL )
                )

            SELECT fav_info.aid
                 ,jsonb_object_agg(  info,
                                        CASE info
                                        WHEN 01 THEN previos::TIMESTAMPTZ::TEXT
                                        WHEN 02 THEN previos::DATE::TEXT
                                        WHEN 03 THEN previos::TIME::TEXT
                                        WHEN 11 THEN period::INTERVAL::TEXT
                                        WHEN 12 THEN EXTRACT(EPOCH FROM period)::TEXT
                                        WHEN 13 THEN ROUND( (EXTRACT(EPOCH FROM period )/86400)::NUMERIC,2 )::TEXT 
                                        WHEN 21 THEN (previos+period)::TIMESTAMPTZ::TEXT
                                        WHEN 22 THEN (previos+period)::DATE::TEXT
                                        WHEN 23 THEN (previos+period)::TIME::TEXT
                                        WHEN 31 THEN (previos+period-now())::INTERVAL::TEXT
                                        WHEN 32 THEN EXTRACT( EPOCH FROM (previos+period-now()) )::TEXT 
                                        WHEN 33 THEN ROUND ((EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 )::TEXT 
                                        ELSE '' END -- не должно выполняться, не существует
                                 ) as val
                    FROM fav_info
            GROUP BY fav_info.aid


        ) fav_act_prop     
 ) AS fav_prop_info

FROM obj;
GRANT SELECT ON TABLE obj_fav_info TO "Guest";
--SELECT * FROM obj_current_info WHERE cls_id=191;
-------------------------------------------------------------------------------
------ избранные свойства для классов  ----------------------------------------
-------------------------------------------------------------------------------
DROP VIEW IF EXISTS cls_fav_info;
CREATE OR REPLACE VIEW cls_fav_info AS 

SELECT  
    acls.* 
    ,(WITH  cls_tree(id) AS(SELECT id FROM get_path_cls_info(acls.id, 1))
    , fav_distinct  AS( 
        SELECT aid, info 
		FROM fav_act 
		INNER JOIN cls_tree ON fav_act.cid = cls_tree.id 
                           AND usr = CURRENT_USER AND info BETWEEN 10 AND 20 
        GROUP BY aid, info) 
	, fav_period(aid,val)  AS( 
        SELECT fav_distinct.aid 
						, jsonb_object_agg( 
							info 
							, CASE info 
								WHEN 11 THEN period::INTERVAL::TEXT 
								WHEN 12 THEN EXTRACT(EPOCH FROM period)::TEXT 
								WHEN 13 THEN ROUND((EXTRACT(EPOCH FROM period) / 86400)::NUMERIC, 2)::TEXT 
							ELSE '' END 
					)
		FROM ref_cls_act 
		INNER JOIN fav_distinct ON ref_cls_act.period IS NOT NULL AND fav_distinct.aid = ref_cls_act.act_id 
		INNER JOIN cls_tree ct ON  ref_cls_act.cls_id = ct.id 
		GROUP BY fav_distinct.aid )
    , fav_act_info AS(
    SELECT  jsonb_build_object('fav_act', jsonb_object_agg(aid,val)) AS val
        FROM fav_period
    )
    , fav_prop_info(val) AS(
        SELECT jsonb_build_object('fav_cprop',jsonb_object_agg( fav_cprop.pid ,prop_cls.val))
        FROM fav_cprop
        INNER JOIN cls_tree tree ON tree.id= fav_cprop.cid  AND usr=CURRENT_USER
        INNER JOIN prop_cls ON prop_cls.cls_id=acls.id AND prop_cls.prop_id=fav_cprop.pid )

    SELECT NULLIF (
        (SELECT COALESCE( NULLIF(fav_prop_info.val,'{"fav_cprop" : null}'::JSONB),'{}'::JSONB) FROM fav_prop_info)
        || 
        ( SELECT COALESCE( (SELECT fav_act_info.val FROM fav_act_info),'{}'::JSONB) )
        ,'{}'::JSONB
        )
    ) AS fav_prop_info
FROM acls
WHERE acls.id > 99 ;
GRANT SELECT ON TABLE cls_fav_info TO "Guest";
COMMIT TRANSACTION;