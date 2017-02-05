SELECT log_date,log_time
,act_title,mcls_title,mobj_title
,CASE WHEN act_title='Изменить примечание' THEN prop->>'104' ELSE NULL END as "примечание"
,CASE WHEN act_title='Профилактика' THEN prop->>'117' ELSE NULL END as "Описание профилактики"
,CASE WHEN act_title='Проверка' THEN prop->>'118' ELSE NULL END as "Описание проверки"
,CASE WHEN act_title='Ремонт' THEN prop->>'115' ELSE NULL END as "Причина ремонта"
,CASE WHEN act_title='Ремонт' THEN prop->>'116' ELSE NULL END as "Описание ремонта"
,CASE WHEN act_title='Калибровка' THEN prop->>'119' ELSE NULL END as "Описание калибровки"
,CASE WHEN act_title='ГИС' THEN prop->>'104' ELSE NULL END as "Описание ГИС"
--,act_color,log_date,mcls_id,mobj_id,act_id,prop->>'104' 
FROM log 
WHERE 
act_id IS NOT null 
AND mcls_title ~~* 'кса-т%' 
OR mcls_title='МРВ-38-120/60' 
OR mcls_title='МРГ-43-150/90' 

ORDER BY 
mcls_id
, (substring(mobj_title, '^[0-9]+')::INT, mobj_title ) ASC 
,log_dt DESC 
--LIMIT 100 OFFSET 0




