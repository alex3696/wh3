SELECT 
REGEXP_REPLACE(tcls._title, '"', '""','g')  AS "тип"
,quote_literal(REGEXP_REPLACE(obj.title, '"', '""','g'))  AS "объект"
,obj.qty  AS "количество"
, REGEXP_REPLACE( get_path_obj(obj.pid, 1), '"', '""','g') AS "местоположение"
--,get_path_cls(obj.cls_id,1)  AS "полный тип" 
--,tcls._path AS "полный тип" 
--,obj.dt_insert::DATE AS "Дата первой записи"
--,obj.dt_update::DATE AS "Дата изменения"
--,obj.id          AS oid
--,obj.cls_id     AS cid
--,obj.cls_kind   AS ckind

FROM obj
LEFT JOIN 
( SELECT *
    FROM get_childs_cls( 
        (SELECT acls.id FROM acls WHERE acls.title= 'ЗИП Sondex' LIMIT 1) )
        --(SELECT acls.id FROM acls WHERE acls.title= '?Корневой тип?STRING?ЗИП Sondex?' LIMIT 1) )
) tcls ON tcls._id=obj.cls_id 
WHERE tcls._kind<>0 AND tcls._id>1
ORDER BY "тип" 
,(substring(obj.title, '^[0-9]+')::INT, obj.title ) ASC 
  --LIMIT 100


