SELECT 
avg( (kalibr_qty)*2 ) AS avg_kalibr
,avg( (check_qty+service_qty+repair_qty)*3 ) AS avg_rem
,avg(total_actions-check_qty*3-service_qty*3-repair_qty*3-kalibr_qty*2 ) AS avg_disp
FROM
(
SELECT log_dt::DATE
       ,EXTRACT(DOY FROM log_dt)::INTEGER
       --,act_id,act.title
       ,count(1) AS total_actions
       ,count(1)-count(act_id) AS moves
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Проверка%') THEN 1 END) as check_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Профилактика%') THEN 1 END) as service_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Ремонт%') THEN 1 END) as repair_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Калибровка%') THEN 1 END) as kalibr_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%ГИС%') THEN 1 END) as gis_qty
       
  FROM public.log 
  LEFT JOIN act ON act.id=act_id
  WHERE log_dt::date >= '2015-01-01'::TIMESTAMP AND log_dt::date < '2016-03-15'::TIMESTAMP
  GROUP BY log_dt::DATE, EXTRACT(DOY FROM log_dt) --, act_id, act.title
  --HAVING act_id IN (SELECT id FROM act WHERE title~~*'%Проверка%')
  ORDER BY log_dt::DATE  
)ass






 SELECT log_dt::DATE
       ,EXTRACT(DOY FROM log_dt)::INTEGER
       ,*
  FROM public.log 
  LEFT JOIN act ON act.id=act_id
  WHERE log_dt::date = '2016-08-13'::TIMESTAMP 



  
/**
Отчет по количеству действий в абстрактных классах
*/
SELECT 
_id
,_kind
,acls._pid
,_title
,check_qty_bypid
,service_qty_bypid
,repair_qty_bypid
,kalibr_qty_bypid
,gis_qty_bypid
,get_path_cls(_id)AS cpath 

FROM get_childs_cls(101) acls
LEFT JOIN
(

SELECT distinct(_pid)
--,get_path_cls(_id)AS cpath 
,sum(check_qty) OVER (PARTITION BY _pid) as check_qty_bypid
,sum(service_qty) OVER (PARTITION BY _pid) as service_qty_bypid
,sum(repair_qty) OVER (PARTITION BY _pid) as repair_qty_bypid
,sum(kalibr_qty) OVER (PARTITION BY _pid) as kalibr_qty_bypid
,sum(gis_qty) OVER (PARTITION BY _pid) as gis_qty_bypid
FROM get_childs_cls(101) tree
LEFT JOIN (SELECT mcls_id
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Проверка%') THEN 1 END) as check_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Профилактика%') THEN 1 END) as service_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Ремонт%') THEN 1 END) as repair_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Калибровка%') THEN 1 END) as kalibr_qty
       ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%ГИС%') THEN 1 END) as gis_qty
       FROM public.log 
       LEFT JOIN act ON act.id=act_id
       WHERE log_dt::date >= '2016-01-01'::TIMESTAMP AND log_dt::date <= '2016-12-31'::TIMESTAMP
       GROUP BY mcls_id)cls_stat ON cls_stat.mcls_id=_id
WHERE _kind=1
) by_acls ON by_acls._pid = acls._id
WHERE acls._kind=0
ORDER BY cpath

/**
Отчет по количеству действий во всех классах
*/
SELECT 
_id, _kind, acls._pid, _title
, check_qty, service_qty, repair_qty, kalibr_qty, gis_qty
,get_path_cls(_id)AS cpath 
FROM get_childs_cls(101) acls
LEFT JOIN (SELECT mcls_id 
            ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Проверка%') THEN 1 END) as check_qty
            ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Профилактика%') THEN 1 END) as service_qty
            ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Ремонт%') THEN 1 END) as repair_qty
           ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%Калибровка%') THEN 1 END) as kalibr_qty
           ,count(CASE WHEN act_id=(SELECT id FROM act WHERE title~~*'%ГИС%') THEN 1 END) as gis_qty
           FROM public.log 
           LEFT JOIN act ON act.id=act_id
           WHERE log_dt::date >= '2016-01-01'::TIMESTAMP AND log_dt::date <= '2016-12-31'::TIMESTAMP
           GROUP BY mcls_id
          ) by_acls 
          ON by_acls.mcls_id = acls._id
ORDER BY cpath




