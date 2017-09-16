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
  ,acls.title AS ctitle
  ,oo.title AS otitle
  ,jperiod
  FROM obj oo
  LEFT JOIN acls ON acls.id=oo.cls_id
  LEFT JOIN LATERAL
( SELECT
                  json_object_agg( ref_cls_act.act_id, ROUND( (EXTRACT(EPOCH FROM (lastlog.log_dt + ref_cls_act.period -now()) )/86400)::NUMERIC,2 ) ) AS jperiod
                  FROM get_path_cls_info(oo.cls_id, 0) tree
                  INNER JOIN ref_cls_act ON ref_cls_act.cls_id = tree.id  AND ref_cls_act.period IS NOT NULL
                     --AND ( ref_cls_act.act_id =105 OR ref_cls_act.act_id =104 OR ref_cls_act.act_id =108 )
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
   WHERE oo.pid=108;
   --WHERE oo.cls_id=257;
   
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
DROP VIEW IF EXISTS log2;
DROP INDEX IF EXISTS idx_logactdet__aid;
ALTER TABLE log_detail_act DROP CONSTRAINT log_detail_act_act_id_fkey;

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция выполнения действия
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) CASCADE;

CREATE OR REPLACE FUNCTION do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB)
                  RETURNS  VOID AS 
$BODY$
DECLARE
    _lock_info   RECORD;
    _last_log_id BIGINT;
    _prop_str    TEXT;
    _prop_item   TEXT;
    _curr_pathid BIGINT[];
    _cls_id      BIGINT;
    _perm_sum    INTEGER;
    _old_prop    JSONB;

    _chk_props CURSOR IS

SELECT distinct( ref_act_prop.prop_id) as id, pold.value
       ,all_new.id as new_id,all_new.value
       ,CASE WHEN all_new.id IS NOT NULL THEN all_new.value ELSE pold.value END AS result
       --, COALESCE( all_new.value, pold.value) as result
  FROM  ref_cls_act
  RIGHT JOIN ref_act_prop  ON ref_act_prop.act_id  = ref_cls_act.act_id
  LEFT JOIN (select key::BIGINT, value::TEXT from jsonb_each(_old_prop)) pold ON pold.key=ref_act_prop.prop_id
  LEFT JOIN(
             SELECT ref_act_prop.prop_id as id, pnew.value
             FROM  ref_cls_act
             RIGHT JOIN ref_act_prop  ON ref_act_prop.act_id  = ref_cls_act.act_id
             LEFT JOIN (select key::BIGINT, value::TEXT from jsonb_each(_prop)) pnew ON pnew.key=ref_act_prop.prop_id
             WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(_cls_id)) AND ref_cls_act.act_id=_act_id
             AND ref_act_prop.prop_id IS NOT NULL 
            ) all_new ON all_new.id=ref_act_prop.prop_id
  WHERE ref_cls_act.cls_id IN (SELECT id FROM get_path_cls_info(_cls_id)) 
  AND ( (all_new.id IS NOT NULL AND all_new.value IS NOT NULL) OR (all_new.id IS NULL  AND pold.value IS NOT NULL)) ;

BEGIN
  -- проверяем - заблокирован ли объект для действия
  SELECT * INTO _lock_info 
    FROM lock_obj
    LEFT JOIN lock_act USING (oid)
    WHERE
      oid = _obj_id
      AND lock_time +'00:10:00.00' > now()
      AND lock_session = pg_backend_pid()
      --AND ((src_path IS NULL AND _src_path IS NULL) OR src_path=_src_path)
      AND act_id = _act_id;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not locked obj_id=% ', _obj_id;
  END IF;
  -- находим текущее положение и состояние
  SELECT cls_id,prop,get_path_obj_arr_2id(pid) INTO _cls_id,_old_prop,_curr_pathid 
    FROM obj WHERE id=_obj_id;
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not exists obj_id=% ', _obj_id;
  END IF;
  _curr_pathid:=COALESCE(_curr_pathid::TEXT,'{}');
  RAISE DEBUG '_curr_pathid: %',_curr_pathid;
  -- проверяем возможность выполнения выбранного действия в текущем местоположении
    SELECT sum INTO _perm_sum FROM (
    SELECT perm.act_id , sum(access_disabled) 
        FROM perm_act perm
        RIGHT JOIN wh_membership ON groupname=perm.access_group AND wh_membership.username=CURRENT_USER
      WHERE 
        perm.cls_id IN(SELECT id FROM get_path_cls_info(_cls_id,0))
        AND perm.act_id = _act_id
        AND  (perm.obj_id IS NULL OR perm.obj_id = _obj_id)
        AND  _curr_pathid::TEXT LIKE perm.src_path
        --AND perm.access_group IN (SELECT groupname FROM wh_membership WHERE username=CURRENT_USER)
      GROUP BY act_id
      ) t 
      WHERE t.sum=0;

   RAISE DEBUG '_perm_sum: %',COALESCE(_perm_sum,-1);

  IF _perm_sum IS NULL OR _perm_sum>0 THEN
    RAISE EXCEPTION ' Path was changed for obj_id=% _cls_id=% _act_id=% try again ', _obj_id,_cls_id,_act_id;
  END IF;

  FOR rec IN _chk_props LOOP
    _prop_item:= concat_ws(':',quote_ident(rec.id::TEXT), rec.result );
    _prop_str := concat_ws(',',_prop_str, _prop_item );
  END LOOP;
  _prop_str:='{'||_prop_str||'}';
  RAISE DEBUG '_prop_str: %',_prop_str;

  INSERT INTO log_main(src_path, obj_id,act_id)
    VALUES ( _curr_pathid, _obj_id, _act_id) RETURNING id INTO _last_log_id;
  INSERT INTO log_detail_act(id, prop) 
    VALUES (_last_log_id, _prop_str::JSONB);
  UPDATE obj_name SET act_logid=_last_log_id, prop=_prop_str::JSONB WHERE id=_obj_id;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;

GRANT EXECUTE ON FUNCTION do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) TO "User";
-------------------------------------------------------------------------------
-- перемещение
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    do_move( IN _oid  BIGINT, 
                                    IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;

CREATE OR REPLACE FUNCTION do_move( IN _oid  BIGINT, 
                                    IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) 
                  RETURNS  VOID AS $BODY$
DECLARE
  _lock_info   RECORD;
  _curr_path BIGINT[];
  _dst_path BIGINT[];
  _ckind SMALLINT;
  _act_logid BIGINT;
  
  _src_qty NUMERIC;
  _dst_qty NUMERIC;

  _move_logid BIGINT;
BEGIN
--  определяем заблокирован ли объект для действия
  SELECT * INTO _lock_info 
    FROM lock_obj
    WHERE
      oid = _oid
      AND pid = _old_opid
      AND lock_time +'00:10:00.00' > now()
      AND lock_session = pg_backend_pid();
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object not locked obj_id=% ', _oid;
  END IF;
-- проверяем изменился ли текущий путь объекта
  _curr_path:=(SELECT get_path_obj_arr_2id(_old_opid));
  _curr_path:=COALESCE(_curr_path::TEXT,'{}');
  RAISE DEBUG '_curr_pathid: %',_curr_path;
  if(_curr_path<>_lock_info.path) THEN
    RAISE EXCEPTION ' Object src_path was changed obj_id=%, old_path=% curr_path=% '
                                             , _oid, _lock_info.path,_curr_path;
  END IF;
-- проверяем есть ли среди заблокированных пунктов назначений выбранный 
  _dst_path:=get_path_obj_arr_2id(_new_opid);
  _dst_path:=COALESCE(_dst_path::TEXT,'{}');
  PERFORM FROM lock_dst WHERE oid=_oid AND pid=_old_opid AND dst_path = _dst_path 
                              OR (dst_path IS NULL AND _dst_path IS NULL);
  IF NOT FOUND THEN
    RAISE EXCEPTION ' Object dst_path was changed obj_id=%, _dst_path=%'
                                             , _oid, _dst_path;
  END IF;
-- проверяем существование объекта и берём его класс
  SELECT cls_kind,act_logid,qty INTO _ckind, _act_logid,_src_qty FROM obj WHERE id=_oid AND pid=_old_opid;
  IF NOT FOUND OR _ckind IS NULL OR _ckind<1 OR _ckind>3 THEN
    RAISE EXCEPTION ' Object not exist obj_id=%', _oid;
  END IF;
-- находим исходное количество и количество в месте назначения
  SELECT qty INTO _dst_qty FROM obj WHERE pid=_new_opid AND id=_oid;
  RAISE DEBUG ' _dst_qty=% ',_dst_qty;

  CASE _ckind
   WHEN 1 THEN PERFORM do_move_objnum(_oid, _new_opid);
   WHEN 2 THEN PERFORM do_move_objqtyi(_oid, _old_opid, _new_opid, _qty);
   WHEN 3 THEN PERFORM do_move_objqtyf(_oid, _old_opid, _new_opid, _qty);
   ELSE RAISE EXCEPTION 'Wrong kind or unknown error'; 
  END CASE;

  

  

  INSERT INTO log_main(src_path, obj_id, act_id)
      VALUES (_curr_path, _oid, 0) RETURNING id INTO _move_logid;
  INSERT INTO log_detail_move(id, dst_path, qty, prop_lid)
      VALUES (_move_logid,  _dst_path, _qty, _act_logid);
-- обновляем ссылку на последнее действие в исходном и конечном объектах
  UPDATE obj_name SET move_logid=_move_logid  WHERE id=_oid;

END;
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION do_move( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC )  TO "User";
------------------------------------------------------------------------------------------------------------
--
------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS log;
CREATE OR REPLACE VIEW log AS 
 SELECT 
  lm.id       AS log_id
 ,lm.timemark AS log_dt
 ,lm.username AS log_user
 ,act.id      AS act_id
 ,act.title   AS act_title
 ,act.color   AS act_color
 ,lm.obj_id   AS mobj_id
 ,mobj.cls_id AS mcls_id
 ,mobj.title  AS mobj_title
 ,mcls.title  AS mcls_title
 ,CASE WHEN mcls.kind=1 THEN 1::NUMERIC ELSE lmd.qty END AS qty
 --,CASE WHEN lad.act_id IS NOT NULL THEN lad.prop ELSE NULL::JSONB END AS prop --,det.prop    AS prop
 ,CASE WHEN lm.act_id<>0 THEN lad.prop ELSE move_prop.prop END AS prop 
 ,lm.timemark::timestamptz::date  AS log_date
 ,date_trunc('second' ,lm.timemark)::timestamptz::time AS log_time
 ,lm.src_path[1][1]  AS src_cid
 ,lm.src_path[1][2]  AS src_oid
 ,CASE WHEN lm.act_id<>0 THEN NULL ELSE lmd.dst_path[1][1] END AS dst_cid -- ,det.dst_path[1][1] AS dst_cid
 ,CASE WHEN lm.act_id<>0 THEN NULL ELSE lmd.dst_path[1][2] END AS dst_oid -- ,det.dst_path[1][2] AS dst_oid
 ,lm.src_path        AS src_ipath
 ,CASE WHEN lm.act_id<>0 THEN NULL ELSE lmd.dst_path END AS dst_ipath --,det.dst_path       AS dst_ipath
 ,(SELECT path FROM tmppath_to_2id_info(lm.src_path::TEXT,1)) AS src_path
 ,(SELECT path FROM tmppath_to_2id_info(lmd.dst_path::TEXT,1)) AS dst_path

 ,lmd.prop_lid AS prop_lid
 ,mcls.kind    AS mcls_kind
 ,mcls.measure AS mcls_measure
FROM log_main lm
  LEFT JOIN log_detail_act  lad ON lad.id=lm.id
  LEFT JOIN log_detail_move lmd ON lmd.id=lm.id
  LEFT JOIN log_detail_act move_prop ON lmd.prop_lid=move_prop.id

  LEFT JOIN act ON act.id=lm.act_id
  LEFT JOIN obj_name mobj ON mobj.id=lm.obj_id
  LEFT JOIN acls     mcls ON mcls.id=mobj.cls_id
  ;

GRANT SELECT        ON log  TO "Guest";
GRANT DELETE        ON log  TO "User";

------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------
CREATE TRIGGER tr_bd_log INSTEAD OF DELETE ON public.log FOR EACH ROW EXECUTE PROCEDURE public.ftg_del_log();

------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------
ALTER TABLE log_detail_act DROP COLUMN act_id;







                                