SELECT *
FROM
(
SELECT log_id, log.log_dt, log_user 
, mcls_title, mobj_title , mcls_id,mobj_id, qty 
, prop->'116' AS "Описание ремонта"
--,act_id, act_title, act_color, prop 
--,src_cid, src_oid, src_ipath, src_path ,dst_cid, dst_oid, dst_ipath, dst_path 
--,prop_lid, mcls_kind, mcls_measure  
FROM log  
WHERE    
( (log.log_dt>='01.01.2017 00:00:21' AND log.log_dt<='16.05.2017 11:34:21') ) 
AND  ( act_title='Ремонт') --AND  ( act_id=102) 
AND  ( prop->>'115'='Небрежное отношение') 
ORDER BY  log.log_dt DESC  
--LIMIT 10
) repairs
LEFT JOIN LATERAL 
  ( SELECT prev.log_dt,src_path FROM LOG prev
    WHERE 
      prev.mobj_id=repairs.mobj_id
      AND prev.log_dt<repairs.log_dt 
      AND prev.act_id IS NULL
      AND dst_path ~~* '%/[Участок СЦ]Ремонтный участок'
      ORDER BY prev.log_dt DESC LIMIT 1) previos
ON true
ORDER BY src_path, mcls_title, repairs.log_dt



