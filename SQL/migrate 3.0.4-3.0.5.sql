-------------------------------------------------------------------------------
------ избранные свойства для действий ----------------------------------------
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS fav_act CASCADE;
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
GRANT SELECT, INSERT, DELETE, UPDATE ON TABLE fav_act TO "Guest";
--ALTER TABLE fav_act ENABLE ROW LEVEL SECURITY;
--CREATE POLICY fav_act ON fav_act USING (usr = current_user);
--SET enable_seqscan = OFF;
--SET enable_seqscan = ON;
-------------------------------------------------------------------------------
------ избранные свойства для ОБЬЕКТОВ ----------------------------------------
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS fav_oprop CASCADE;
DROP SEQUENCE IF EXISTS fav_oprop_id_seq CASCADE;
CREATE SEQUENCE fav_oprop_id_seq  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE TABLE fav_oprop
(
  id  bigint NOT NULL DEFAULT nextval('fav_oprop_id_seq'::regclass)
  ,usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,pid bigint NOT NULL

  ,CONSTRAINT pk_fav_oprop__id PRIMARY KEY (id)
  ,CONSTRAINT uk_fav_oprop__usr_cid_pid UNIQUE (usr, cid, pid)
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
DROP SEQUENCE IF EXISTS fav_cprop_id_seq CASCADE;
CREATE SEQUENCE fav_cprop_id_seq  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE TABLE fav_cprop
(
  id  bigint NOT NULL DEFAULT nextval('fav_cprop_id_seq'::regclass)
  ,usr name   NOT NULL DEFAULT "current_user"()
  ,cid bigint NOT NULL
  ,pid bigint NOT NULL

  ,CONSTRAINT pk_fav_cprop__id PRIMARY KEY (id)
  ,CONSTRAINT uk_fav_cprop__usr_cid_pid UNIQUE (usr, cid, pid)
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
------ избранные свойства для КЛАССОВ  ----------------------------------------
-------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj_current_info;
CREATE OR REPLACE VIEW  obj_current_info AS 
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
                                              )
                                )
              ,'{"fav_oprop": null}'
            ) 
            ,'{}'::JSONB 
          )
        )
        ||
        (
          COALESCE(
            NULLIF(
              jsonb_build_object(
                                  'fav_act',
                                  jsonb_object_agg( 
                                                    all_fav_bitor.aid  --ref_cls_act.act_id
                                                    ,CASE WHEN((visible & 1)>0) THEN jsonb_build_object('1',CASE WHEN((visible & 16)>0) THEN previos::TEXT ELSE previos::DATE::TEXT END  ) ELSE '{}'::jsonb END 
                                                    ||CASE WHEN(period IS NOT NULL) THEN 
                                                           CASE WHEN((visible & 2)>0) THEN jsonb_build_object('2',EXTRACT(EPOCH FROM period) ) ELSE '{}' END 
                                                        || CASE WHEN((visible & 4)>0) THEN jsonb_build_object('4',CASE WHEN((visible & 16)>0) THEN (previos+period)::TEXT ELSE (previos+period)::DATE::TEXT END  ) ELSE '{}'::jsonb END 
                                                        || CASE WHEN((visible & 8)>0) THEN jsonb_build_object('8',ROUND( (EXTRACT(EPOCH FROM (previos+period-now()) )/86400)::NUMERIC,2 ) )  ELSE '{}' END 
                                                    ELSE '{}' END 
                                  )
              )
              ,'{"fav_act": null}'
            )
            ,'{}'::JSONB 
          )
        )         
        as last_act
        FROM (SELECT aid, bit_or(visible) AS visible FROM cls_tree cls
                       INNER JOIN fav_act ON fav_act.cid=cls.id AND fav_act.usr = CURRENT_USER 
                       GROUP BY aid)all_fav_bitor
        LEFT JOIN  LATERAL (SELECT ref_cls_act.period
                            FROM cls_tree ct
                            INNER JOIN ref_cls_act ON ref_cls_act.period IS NOT NULL
                                              AND ref_cls_act.cls_id = ct.id
                                              AND ref_cls_act.act_id = all_fav_bitor.aid
                     )ref_ca ON TRUE
        LEFT JOIN LATERAL
              (SELECT MAX(timemark) AS previos, act_id AS aid, obj_id AS oid
                 FROM log_main 
                 WHERE act_id<>0
                   AND log_main.obj_id = obj.id 
                   AND log_main.act_id = all_fav_bitor.aid 
                 GROUP BY obj_id, act_id
              ) last_log             
              ON TRUE       
 ) AS fav_prop_info

FROM obj;



--SELECT * FROM obj_current_info WHERE cls_id=159;











