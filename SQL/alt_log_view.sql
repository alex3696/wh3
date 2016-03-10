
SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;



--SET enable_seqscan = ON;
--SET enable_seqscan = OFF;

DROP VIEW IF EXISTS log;

CREATE OR REPLACE VIEW log AS 
SELECT log.id::BIGINT       AS log_id
      ,date_trunc('second' ,log.timemark)::timestamp  AS log_dt
      ,log.username AS log_user
      
      ,log.act_id
      ,log.act_title
      ,log.act_color
      ,log.obj_id AS mobj_id
      ,mcls.id    AS mcls_id
      ,mobj.title AS mobj_title
      ,mcls.title AS mcls_title
      ,log.qty 
--      ,xsrc.path  AS src_path
--      ,xdst.path  AS dst_path
,(SELECT path FROM tmppath_to_2id_info(log.src_path::TEXT,1)) AS src_path
,(SELECT path FROM tmppath_to_2id_info(log.dst_path::TEXT,1)) AS dst_path
,log.curr_prop AS prop
       ,log.timemark::timestamptz::date  AS log_date
       ,date_trunc('second' ,log.timemark)::timestamptz::time AS log_time

FROM
(
SELECT log_move.id, log_move.timemark, log_move.username, 0::BIGINT AS act_id, 'Перемещение'::WHNAME AS act_title,'white'::VARCHAR(64) AS act_color
     , log_move.obj_id --, cls.title, obj_name.title 
     , log_move.qty
     , log_move.src_path 
     , log_move.dst_path
     , la.prop AS curr_prop
  FROM log_move
  LEFT JOIN log_act la ON la.id=act_logid
UNION ALL
SELECT log_act.id, timemark, username, act_id, act.title, act.color
     , log_act.obj_id
     , 1::NUMERIC AS qty
     , log_act.src_path 
     , NULL::BIGINT[]
     , log_act.prop
  FROM log_act
  LEFT JOIN act ON act.id=act_id
) log
--LEFT JOIN LATERAL tmppath_to_2id_info(log.src_path::TEXT) as xsrc ON true 
--LEFT JOIN LATERAL tmppath_to_2id_info(log.dst_path::TEXT) as xdst ON true 

  LEFT JOIN obj_name mobj ON mobj.id=obj_id
  LEFT JOIN cls mcls ON mcls.id=mobj.cls_id
--WHERE act_title ~~* '%рем%'
--WHERE username ~~* '%iva%'
--WHERE xsrc.path ~~* '%сц%рем%' AND act_title !~~* '%перем%'
;




--DELETE FROM log WHERE log_id='101';



/**
SELECT *  FROM log;

--SELECT src_path||'->'||dst_path  FROM log;
SELECT log_time::timestamptz::date
      ,date_trunc('second' ,log_time)::timestamptz::time
      ,* 
FROM log --WHERE log_time::timestamptz::date='2016.01-23'
ORDER BY log_time::timestamptz::date ASC

SELECT log_time::timestamptz::date AS ldate
      ,date_trunc('second' ,log_time)::timestamptz::time AS ltime
      ,* 
FROM log --WHERE log_time::timestamptz::date='2016.01-23'
ORDER BY log_time;

*/




















