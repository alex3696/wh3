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
  