----------------------------------------------------------------------------------
-- Триггер обеспечивающий уникальность действия (+периода) на всю подветку класса 

DROP FUNCTION IF EXISTS ftr_biu__ref_cls_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_biu__ref_cls_act()  RETURNS trigger AS
$body$
DECLARE
BEGIN
  PERFORM FROM 
    (SELECT * FROM get_path_cls_info(NEW.cls_id, 0) OFFSET 1) clstree
    LEFT JOIN ref_cls_act acttree ON clstree.id = acttree.cls_id
    WHERE acttree.act_id=NEW.act_id;
  IF FOUND THEN
    RAISE EXCEPTION ' %: act_id=% already present in UP cls_id=%',TG_NAME, NEW.act_id, NEW.cls_id ;
  END IF;

  PERFORM FROM 
    (SELECT * FROM get_childs_cls(NEW.cls_id) OFFSET 1) clstree
    LEFT JOIN ref_cls_act acttree ON clstree._id = acttree.cls_id
    WHERE acttree.act_id=NEW.act_id;
  IF FOUND THEN
    RAISE EXCEPTION ' %: act_id=% already present in DOWN cls_id=%',TG_NAME, NEW.act_id, NEW.cls_id ;
  END IF;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';

CREATE TRIGGER tr_biu__ref_cls_act 
BEFORE INSERT OR UPDATE ON ref_cls_act 
FOR EACH ROW EXECUTE PROCEDURE ftr_biu__ref_cls_act();

GRANT EXECUTE ON FUNCTION ftr_biu__ref_cls_act() TO "TypeDesigner";

DROP INDEX IF EXISTS  idx_ref_cls_act__ll;
DROP INDEX IF EXISTS  idx_ref_cls_act__cid_aid_nnperiod;
CREATE INDEX idx_ref_cls_act__cid_aid_nnperiod
  ON ref_cls_act (cls_id,act_id)
  WHERE ( period IS NOT NULL );

GRANT UPDATE(period) ON ref_cls_act TO "TypeDesigner";
----------------------------------------------------------------------------------

ALTER TABLE public.log_main DROP COLUMN IF EXISTS act_id;
DELETE FROM act WHERE id=0;

ALTER TABLE public.log_main ADD COLUMN act_id bigint;
ALTER TABLE public.log_main
  ADD CONSTRAINT fk_log_main__act_id__act__id FOREIGN KEY (act_id)
      REFERENCES public.act (id) MATCH FULL
      ON UPDATE CASCADE ON DELETE RESTRICT;


UPDATE public.log_main SET (act_id) = 
    (SELECT act_id FROM log_detail_act
     WHERE log_main.id = log_detail_act.id);


INSERT INTO act(id, title) VALUES (0,'Перемещение') ;
UPDATE public.log_main SET act_id = 0
        WHERE log_main.act_id  IS NULL;
ALTER TABLE public.log_main ALTER COLUMN act_id SET NOT NULL;

-- устраняем действия введённые в одно время для одного объекта
UPDATE log_main SET timemark = data.new_dt 
  FROM  
(
  SELECT log_id, dup.* 
  --,row_number() OVER (PARTITION BY obj_id, timemark ORDER BY log_id ASC)  AS minadd
  ,(row_number() OVER (PARTITION BY obj_id, timemark ORDER BY log_id ASC))*'00:01:00'::INTERVAL+log_dt  AS new_dt
  FROM LOG
  INNER JOIN (
    SELECT COUNT(*),obj_id, timemark
      FROM log_main  
      GROUP BY obj_id, timemark
      HAVING      (COUNT(*) > 1) 
    )dup ON dup.obj_id=log.mobj_id 
    AND dup.timemark=log.log_dt
    --ORDER BY obj_id,log_id
)data
WHERE log_main.id = data.log_id;


-- индекс для быстрого отображения истории объекта
DROP INDEX IF EXISTS idx_log_main__oid_dt;
--CREATE INDEX idx_log_main__oid_dt ON log_main (obj_id, timemark DESC);
CREATE UNIQUE  INDEX idx_log_main__oid_dt ON log_main (obj_id, timemark DESC);

DROP INDEX IF EXISTS idx_log_main__last_act ;
--CREATE INDEX idx_log_main__last_act ON log_main (obj_id, act_id, timemark DESC) WHERE ( act_id <> 0);
CREATE UNIQUE INDEX idx_log_main__last_act  ON log_main (obj_id, act_id, timemark DESC)  WHERE ( act_id <> 0);
  

--VACUUM FULL 
--ANALYZE;


/**
-- список объектов с массивом остатков периодов с дополнительным стодбцом в log_main + MAX +2
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
   WHERE oo.cls_id=257;
   ---WHERE oo.pid=108;
*/


/**
-- ПОСМОТРЕТЬ классы к которым нет информации о периоде калибровки в старой базе
SELECT * FROM get_childs_cls(101) 
LEFT OUTER JOIN (
               SELECT cls_id,acls.kind, 105::BIGINT AS act_id,((val::INT*30)::TEXT||'days')::INTERVAL as period
               --, title , prop_id, val
               FROM public.prop_cls
               LEFT JOIN acls ON acls.id=prop_cls.cls_id
               WHERE prop_id=102
) old ON old.cls_id=_id
WHERE _kind<>0 AND cls_id IS NULL

-- ПОСМОТРЕТЬ все разрешения калибровки 
SELECT perm_act.id, access_group, access_disabled, cls_id, obj_id, src_path, act_id,title
  FROM perm_act
  LEFT JOIN acls ON acls.id=perm_act.cls_id
  WHERE act_id=105;
*/




--
DELETE FROM ref_cls_act WHERE act_id=105;

INSERT INTO ref_cls_act (cls_id, cls_kind, act_id, period)
               SELECT cls_id,acls.kind, 105::BIGINT AS act_id,((val::INT*30)::TEXT||'days')::INTERVAL as period
               --, title , prop_id, val
               FROM public.prop_cls
               LEFT JOIN acls ON acls.id=prop_cls.cls_id
               WHERE prop_id=102;


INSERT INTO public.perm_act(access_group, access_disabled, cls_id, src_path, act_id)
    SELECT 'Инженер-метролог', 0, cls_id , '{{104,106},%}', 105
    FROM public.prop_cls
    LEFT JOIN acls ON acls.id=prop_cls.cls_id
    WHERE prop_id=102;

ANALYZE;

-- удалить из таблицы log_detail_act идентификатор действия, т.к. его уже перенесли в log_main
DROP VIEW log2;
DROP INDEX idx_logactdet__aid;
ALTER TABLE log_detail_act DROP CONSTRAINT log_detail_act_act_id_fkey;

--ALTER TABLE log_detail_act DROP COLUMN act_id;







                                