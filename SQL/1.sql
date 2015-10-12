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

