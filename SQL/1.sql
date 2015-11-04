DROP TABLE json_test CASCADE;
CREATE TABLE json_test (
  id serial primary key,
  data jsonb
);
CREATE INDEX idxginp1 ON json_test USING gin (data);
CREATE INDEX idxginp2 ON json_test USING gin ((data -> 'ГАБАРИТЫ'));

INSERT INTO json_test (data) VALUES 
  ('{"a":null}'),
  ('{}'),
  ('{"a": 1}'),
  ('{"a": 2, "b": ["c", "d"]}'),
  ('{"a": 1, "b": {"c": "d", "e": true}}'),
  ('{"b": 2, "s": 22}'),
  ('"quote_li\"teral"'),
  ('{"ГАБАРИТЫ": {"длина":1,"диаметр":2,"ширина":3 }}'),
  ('{"ГАБАРИТЫ": {"ширина":4,"высота":5,"глубина":6 }}'),
  ('{"a": {"ширина":4,"высота":5,"глубина":6 }}'),
  ('{"свойство1":1,"свойство2":2}'),
  ('{"свойство1":1,"свойство3":3}');


DECLARE @qty ;
SET @qty=1000;

WHILE (@qty > 0)
BEGIN
  INSERT INTO json_test (data) VALUES ('{"свойство@qty":1,"свойство@qty":22}');
  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;
  


SELECT data::TEXT FROM json_test ;

SELECT * FROM json_test WHERE data ?| array['a'];


SELECT  '{"a": {"b":"foo"}}'::json->'1'
SELECT * FROM json_test WHERE data ? 'ГАБАРИТЫ'


SELECT data->'ГАБАРИТЫ'->'высота' FROM json_test WHERE data->'ГАБАРИТЫ' ? 'высота';
SELECT * FROM json_test WHERE data->'a' ? 'высота';

DROP MATERIALIZED VIEW sss;
CREATE MATERIALIZED VIEW sss AS
SELECT distinct(jsonb_object_keys(data)::NAME) AS title  FROM json_test WHERE data @>'{}';
CREATE UNIQUE INDEX idx_sss ON  sss (title);
REFRESH MATERIALIZED VIEW CONCURRENTLY  sss ;
SELECT * FROM sss WHERE title='a';


select * from json_populate_record(null::json_test, '{"id":1,"b":2}')
select * from json_to_record('{"a":1,"b":[1,2,3],"c":"bar"}') as x(a int, b text, d text)





SELECT data->'a',jsonb_typeof(data->'a') FROM json_test WHERE data ?& array['a'];

SELECT data,jsonb_typeof(data->'a') FROM json_test WHERE id<50






SELECT * FROM json_test WHERE data ->> 'a' > '1';
SELECT * FROM json_test WHERE data -> 'b' > '1';








SELECT ARRAY(SELECT jsonb_object_keys(data)) FROM json_test WHERE data ?| array['a'];






SELECT o.id, o.pid, o.title, o.qty 
, o.move_logid --, get_path(o.pid)  AS path 
, parent.title 
, o.prop 
, x.*
FROM obj o 
LEFT JOIN jsonb_to_record(o.prop) as x("prop_id_11" text, "Счет ГК" text, "комментарий" text) ON true
LEFT JOIN obj_name parent ON parent.id = o.id 
WHERE o.cls_id = 2167









SELECT o.id, o.pid, o.title, o.qty 
, o.move_logid --, get_path(o.pid)  AS path 
--, parent.title 
, o.prop 
, xx.*
FROM obj o, LATERAL (SELECT * FROM jsonb_to_record(o.prop) as x("prop_id_11" text, "Счет ГК" text, "комментарий" text))xx
LEFT JOIN LATERAL obj_name ON obj.id = obj_name.id
WHERE o.cls_id = 2167

















SELECT 'retert' LIKE '%'




SELECT 'fdgdfgdsfg sfdg  '~ '^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$'


SELECT '567' ~ '^(%|[[:digit:]]+)$' -- number or %

SELECT '6%78,%,66,' ~ '^((%|[[:digit:]]+),)+$' -- number or % by ,
SELECT '678,%,66' ~ '^((%|[[:digit:]]+),)+(%|[[:digit:]]+)$' -- number or % by ,


SELECT res[1],res[2],res[3],res[4],res[5],res[6]
FROM regexp_matches (
'{%,{%,%},%%{%,106},{108,%}%{111,122}%}' 
,'^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$'
, 'g' ) res;


SELECT res[1],res[2],res[3],res[4],res[5],res[6]
FROM regexp_matches (
'{%,{%,%},%%{%,106},{108,%}%{111,122}%}' 
,'(((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)'
, 'g' ) res;


SELECT res[1],res[2],res[3],res[4],res[5],res[6],res[7],res[8]
FROM regexp_matches (
'{%,{%,%},%%{%,106},{108,%}%{111,122}%}' 
,'((%+|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)'
, 'g' ) res;





SELECT  
'{%,{%,%},%%{%,106},{108,%}%{111,122}%}'  ~
'^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$'





SELECT ORDINALITY as ord,res[1] as id1,res[2] as id2
                FROM 
                     regexp_matches('{%,{%,%},%%{%,106},{108,%}{111,122}%}' , 
                     '%+|{(%|[[:digit:]]+),(%|[[:digit:]]+)}','g') WITH ORDINALITY res







    SELECT cls.id AS cls_id
        , cls.title::NAME AS cls_title
        , obj.id AS obj_id
        , obj.title::NAME AS obj_title
        ,  arr.id1 ,  arr.id2
        FROM (
                SELECT ORDINALITY as ord,res[1] as id1,res[2] as id2
                FROM 
                     regexp_matches('{%,{%,%},%%{%,106},{108,%}{111,122}%}' , 
                     '%+|{(%|[[:digit:]]+),(%|[[:digit:]]+)}','g') WITH ORDINALITY res

             ) arr
        LEFT JOIN obj_name obj ON obj.id= CASE WHEN arr.id2='%' THEN NULL ELSE arr.id2::BIGINT END
        LEFT JOIN cls_name cls ON cls.id= CASE 
                                            WHEN arr.id1 IS NULL OR arr.id1='%'
                                            THEN obj.cls_id
                                            ELSE arr.id1::BIGINT END
        ORDER BY arr.ord



		SELECT perm_act.id, access_group, access_disabled, script_restrict 
		     , cls.id, cls.title, obj.id, obj.title--, src_path 
		     , act.id, act.title 
		     , arr_2title, arr_2id
		  FROM perm_act --, LATERAL ( SELECT  tmppath_to_2id_info(src_path) ) x
		    LEFT JOIN LATERAL tmppath_to_2id_info(src_path) x ON true 
		    LEFT JOIN cls   ON cls.id = perm_act.cls_id 
		    LEFT JOIN obj   ON obj.id = perm_act.obj_id 
		    LEFT JOIN act ON act.id = perm_act.act_id 
          WHERE perm_act.cls_id = 103




		SELECT perm_move.id, access_group, access_disabled, script_restrict 
		     , mov_cls.id, mov_cls.title, mov_obj.id, mov_obj.title 
		     , src_cls.id, src_cls.title, src_obj.id, src_obj.title
		     , dst_cls.id, dst_cls.title, dst_obj.id, dst_obj.title
		     , src.arr_2title, src.arr_2id
		     , dst.arr_2title, dst.arr_2id
		  FROM perm_move 
		  LEFT JOIN LATERAL tmppath_to_2id_info(src_path) src ON true 
		  LEFT JOIN LATERAL tmppath_to_2id_info(dst_path) dst ON true 
		  
		    LEFT JOIN cls      mov_cls ON mov_cls.id = perm_move.cls_id 
		    LEFT JOIN obj      mov_obj ON mov_obj.id = perm_move.obj_id 
		    LEFT JOIN cls      src_cls ON src_cls.id = perm_move.src_cls_id 
		    LEFT JOIN obj      src_obj ON src_obj.id = perm_move.src_obj_id 
		    LEFT JOIN cls      dst_cls ON dst_cls.id = perm_move.dst_cls_id 
		    LEFT JOIN obj      dst_obj ON dst_obj.id = perm_move.dst_obj_id 
		  WHERE dst_cls_id = 103




























