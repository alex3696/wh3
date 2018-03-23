CREATE OR REPLACE VIEW obj_items AS
SELECT obj_num.id,
            obj_num.pid,
            1::numeric AS qty,
            obj_num.cls_id,
            obj_num.cls_kind
           FROM obj_num
        UNION ALL
         SELECT obj_qtyi.id,
            obj_qtyi.pid,
            obj_qtyi.qty,
            obj_qtyi.cls_id,
            obj_qtyi.cls_kind
           FROM obj_qtyi
        UNION ALL
         SELECT obj_qtyf.id,
            obj_qtyf.pid,
            obj_qtyf.qty,
            obj_qtyf.cls_id,
            obj_qtyf.cls_kind
           FROM obj_qtyf;

GRANT SELECT        ON obj_items  TO Guest;





---------------------
-- все классы ветки
----------------------
SELECT 
cls.id, cls.title, cls.kind, cls.measure,obj.qty
,fav_prop_info
FROM
(
SELECT cls_id AS cid, cls_kind AS ckind, SUM(qty) AS qty
FROM obj_items obj 
WHERE obj.pid = 1
GROUP BY cls_id ,cls_kind
)obj
INNER JOIN cls_fav_info cls ON cls.id   =cid AND cls.kind =ckind
ORDER BY (substring(title, '^[0-9]{1,9}')::INT, title ) ASC 


---------------------
-- все объекты класса ветки
----------------------
		SELECT obj.id, obj.title, obj.qty, obj.pid 
		       --,get_path_objnum(obj.pid,1)  AS path
		       ,fav_prop_info
		 FROM obj_fav_info obj 
		 WHERE obj.id>0  AND obj.pid = 1 --AND obj.cls_id = 102
		 ORDER BY (substring(obj.title, '^[0-9]{1,9}')::INT, obj.title ) ASC 


--SET enable_seqscan = ON;
--SET enable_seqscan = OFF;
---------------------
-- поиск v1 obj catalog
----------------------
WITH RECURSIVE children AS (
    SELECT obj.id, objn.pid, obj.title, obj.cls_id, obj.prop, 1::NUMERIC AS qty, obj.move_logid, obj.act_logid, obj.cls_kind
           ,acls.title AS ctitle, acls.pid AS parent_cid, acls.kind AS kind, acls.measure AS measure 
           ,ARRAY[obj.id] AS exist
           ,FALSE         AS cycle
           , '' AS path
           --, ARRAY[obj.id] AS arr_id
           --, ARRAY[title::NAME] AS arr_title
    FROM obj_name obj
    INNER JOIN obj_num objn USING (id, cls_id, cls_kind)
    INNER JOIN acls ON acls.id=obj.cls_id
    WHERE obj.id = 1
    UNION ALL
        SELECT t.id, t.pid, t.title, t.cls_id, t.prop, t.qty, t.move_logid, t.act_logid, t.cls_kind
        ,acls.title AS ctitle, acls.pid AS parent_cid, acls.kind AS kind, acls.measure AS measure 
               ,exist || t.id 
               ,t.id = ANY(exist)
               , c.path||'/['||acls.title||']'||t.title
               --, c.arr_id     || ARRAY[t.id]::BIGINT[]
               --, c.arr_title  || ARRAY[t.title::NAME]::NAME[]
        FROM children AS c , obj  AS t 
        INNER JOIN acls ON acls.id=t.cls_id
        WHERE t.pid = c.id AND NOT cycle 
)
SELECT ch.id, ch.pid, ch.title, ch.cls_id, ch.prop, ch.qty, ch.move_logid, ch.act_logid, ch.cls_kind
    ,ctitle, parent_cid, kind, measure 
    ,COALESCE(get_path_objnum(1,1),''::TEXT)|| path AS path
    , fav_prop_info 
    FROM children ch 
    INNER JOIN obj_fav_info ON obj_fav_info.cls_id = ch.cls_id AND obj_fav_info.id = ch.id 
    WHERE NOT cycle 
    --AND ( (ctitle~~*'%срк%' AND ch.title~~*'%12%') OR(ctitle~~*'%12%' AND ch.title~~*'%срк%') ) 
    AND ( (ctitle~~*'%1002%' OR ch.title~~*'%1002%') ) 

 ORDER BY 
  (substring(ctitle, '^[0-9]{1,9}')::INT, ctitle) ASC 
 ,(substring(ch.title, '^[0-9]{1,9}')::INT, ch.title) ASC 
---------------------
-- поиск v1 CLS catalog
----------------------
WITH RECURSIVE children AS (
    SELECT id,  title, kind, pid,  note, measure,dobj
           ,ARRAY[id]                            AS exist
           ,FALSE                                AS cycle
           , '' AS path
           --, ARRAY[id] AS arr_id
           --, ARRAY[title::NAME] AS arr_title
    FROM acls
    WHERE id = 1
    UNION ALL
        SELECT t.id, t.title, t.kind, t.pid, t.note, t.measure, t.dobj
               ,exist || t.id 
               ,t.id = ANY(exist)
               , c.path||'/'||t.title
               --, c.arr_id     || ARRAY[t.id]::BIGINT[]
               --, c.arr_title  || ARRAY[t.title::NAME]::NAME[]
        FROM children AS c, 
             acls  AS t
        WHERE t.pid = c.id AND 
              NOT cycle 
              --AND t.kind<= _ckind
              --AND array_length(exist, 1) < 1000 -- глубина дерева
)
SELECT cls.id AS cid,cls.pid AS parent_cid, cls.title AS ctitle, cls.kind, cls.measure
      ,obj.id AS oid,obj.pid AS parent_oid,obj.title AS otitle, qty
      , get_path_objnum(obj.pid, 1) AS path 
      , sum(qty) OVER(PARTITION BY cls.id)  AS allqty  
      , fav_prop_info
    FROM children cls
    INNER JOIN obj_fav_info obj ON obj.cls_id=cls.id AND obj.cls_kind=cls.kind
    WHERE NOT cls.cycle AND kind = 1
    AND ( (cls.title~~*'%1002%' OR obj.title~~*'%1002%') ) 
    --AND ( (ctitle~~*'%срк%' AND obj.title~~*'%12%') OR(ctitle~~*'%12%' AND obj.title~~*'%срк%') ) 
		 ORDER BY 
		  (substring(cls.title, '^[0-9]{1,9}')::INT, cls.title) ASC 
		 ,(substring(obj.title, '^[0-9]{1,9}')::INT, obj.title) ASC 














---------------------
-- поиск v3 obj catalog
----------------------

SELECT  
*
,get_path_objnum(parent_oid,1)
,get_path_cls(cid,1)
FROM
(
  SELECT  cls.id AS cid,cls.pid AS parent_cid, cls.title AS ctitle, cls.kind, cls.measure
      ,obj.id AS oid,obj.pid AS parent_oid,obj.title AS otitle, qty
  ,(SELECT arr_id FROM get_path_cls_info(cls.id,1)ipc WHERE ipc.pid=1 ) AS cls_arr_id
  --,(SELECT arr_id FROM get_path_objnum_info(obj.pid,1)ipo WHERE ipo.opid=1 ) AS obj_arr_id
  --obj.*,cls.* 
  --,cls_arr_id
  FROM obj
  INNER join acls cls  ON obj.cls_id=cls.id AND obj.cls_kind=cls.kind 
  WHERE obj.title~~*('%1002%') OR cls.title~~*('%1002%')
  --WHERE ((cls.title~~*'%срк%' AND obj.title~~*'%12%')
  --   OR(cls.title~~*'%12%' AND obj.title~~*'%срк%'))
)ss
WHERE cls_arr_id && ARRAY[108::BIGINT]
--WHERE obj_arr_id && ARRAY[110::BIGINT]
--WHERE (SELECT arr_id FROM get_path_cls_info(cid,0)pc WHERE pc.pid=0 ) && ARRAY[108::BIGINT]
--WHERE (SELECT arr_id FROM get_path_objnum_info(oid,1)po WHERE po.opid=1 ) && ARRAY[103::BIGINT]
--WHERE 108::BIGINT = ANY(cls_arr_id) 
--WHERE 103::BIGINT = ANY(obj_arr_id) 
--WHERE 108::BIGINT = ANY((SELECT arr_id FROM get_path_cls_info(cid,0)pc WHERE pc.pid=0 )::BIGINT[])
--WHERE 103::BIGINT = ANY((SELECT arr_id FROM get_path_objnum_info(oid,0)pc WHERE pc.opid=0 )::BIGINT[])





SELECT  cls.id AS cid,cls.pid AS parent_cid, cls.title AS ctitle, cls.kind, cls.measure
   , sum(qty) OVER(PARTITION BY cls.id)  AS cqty 
  ,obj.id AS oid,obj.pid AS parent_oid,obj.title AS otitle, qty
  ,get_path_objnum(obj.pid,1)
  ,get_path_cls(cls.id,1)
  ,fav_prop_info
FROM obj_fav_info obj
INNER join acls cls  ON obj.cls_id=cls.id AND obj.cls_kind=cls.kind 
--join LATERAL get_path_objnum_info(obj.pid,0)ipo ON ipo.opid=0 AND 1::BIGINT = ANY(ipo.arr_id) 
join LATERAL get_path_cls_info(cls.id,0)ipc ON ipc.pid=0 AND 1::BIGINT = ANY(ipc.arr_id) 
WHERE obj.title~~*('%180%') OR cls.title~~*('%180%')
--WHERE ((cls.title~~*'%срк%' AND obj.title~~*'%12%')
--   OR(cls.title~~*'%12%' AND obj.title~~*'%срк%'))
ORDER BY 
   (substring(cls.title, '^[0-9]{1,9}')::INT, cls.title) ASC 
  ,(substring(obj.title, '^[0-9]{1,9}')::INT, obj.title) ASC 












INSERT INTO fav_act (usr,cid,aid,info)

SELECT username,myfav.* FROM wh_user
LEFT JOIN LATERAL
(SELECT cid,aid,info FROM fav_act WHERE usr='a.savinov') myfav
ON TRUE
 ON CONFLICT DO NOTHING;



INSERT INTO fav_cprop (usr,cid,pid)

SELECT username,myfav.* FROM wh_user
INNER JOIN LATERAL
(SELECT cid,pid FROM fav_cprop WHERE usr='a.savinov') myfav
ON TRUE
 ON CONFLICT DO NOTHING;



INSERT INTO fav_oprop (usr,cid,pid)

SELECT username,myfav.* FROM wh_user
INNER JOIN LATERAL
(SELECT cid,pid FROM fav_oprop WHERE usr='a.savinov') myfav
ON TRUE
 ON CONFLICT DO NOTHING;


UPDATE fav_oprop SET cid=101
WHERE cid=124 AND pid=124


SELECT username,myfav.*,prop.title, acls.title FROM wh_user
INNER JOIN LATERAL
(SELECT cid,pid FROM fav_oprop WHERE usr='a.savinov') myfav
ON TRUE
INNER JOIN prop ON prop.id=pid
INNER JOIN acls ON acls.id=cid


VACUUM FULL ANALYZE 



UPDATE log_detail_act 
SET prop = jsonb_set(prop, '{107}'
             , to_jsonb(replace(prop->>'107', 'M:\', '\\pfsgg.ru\fs\Метрология\'))
             );
UPDATE obj_name 
SET prop = jsonb_set(prop, '{107}'
             , to_jsonb(replace(prop->>'107', 'M:\', '\\pfsgg.ru\fs\Метрология\'))
             );




UPDATE log_detail_act 
SET prop = jsonb_set(prop, '{107}'
             , to_jsonb(replace(prop->>'107', '\\Fs1\', '\\pfsgg.ru\'))
             );
UPDATE obj_name 
SET prop = jsonb_set(prop, '{107}'
             , to_jsonb(replace(prop->>'107', '\\Fs1\', '\\pfsgg.ru\'))
             );












