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
    RAISE EXCEPTION ' %: act_id=% already present in cls_id=%',TG_NAME, NEW.act_id, NEW.cls_id ;
  END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';

CREATE TRIGGER tr_biu__ref_cls_act 
BEFORE INSERT OR UPDATE ON ref_cls_act 
FOR EACH ROW EXECUTE PROCEDURE ftr_biu__ref_cls_act();

GRANT EXECUTE ON FUNCTION ftr_biu__ref_cls_act() TO public ;--"TypeDesigner";

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


UPDATE log_main SET timemark=timemark + '00:05:00' 
 WHERE id IN (SELECT MAX(id) AS max_lid 
                FROM log_main  
                GROUP BY obj_id, act_id, timemark
                HAVING      (COUNT(*) > 1)
             )

DROP INDEX IF EXISTS idx_log_main__oid_dt;
CREATE INDEX idx_log_main__oid_dt ON log_main (obj_id, timemark DESC);

DROP INDEX IF EXISTS idx_log_main__last_act ;
CREATE /* UNIQUE */INDEX idx_log_main__last_act 
  ON log_main (obj_id, act_id, timemark DESC)
  WHERE ( act_id <> 0);

--VACUUM FULL 
ANALYZE;

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
                  INNER JOIN LATERAL(SELECT MAX(timemark) AS log_dt,
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

   ;

DROP INDEX idx_logactdet__aid;
ALTER TABLE log_detail_act DROP CONSTRAINT log_detail_act_act_id_fkey;
ALTER TABLE log_detail_act DROP COLUMN act_id;










                                