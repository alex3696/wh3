DROP TABLE IF EXISTS t_favorites CASCADE;

-----------------------------------------------------------------------------------------------------------------------------
-- таблица содержащая иерархическую структуру избранного для пользователей 
-----------------------------------------------------------------------------------------------------------------------------

CREATE TABLE t_favorites ( 
  id           SERIAL  PRIMARY KEY
 ,pid          INTEGER NOT NULL DEFAULT 1 
   REFERENCES t_favorites(id)    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE --SET DEFAULT
 ,owner        NAME    NOT NULL DEFAULT CURRENT_USER 
   REFERENCES wh_role(rolname)   MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,expanded     BOOLEAN NOT NULL DEFAULT TRUE
 ,label        TEXT    DEFAULT NULL 

 ,node_type   SMALLINT NOT NULL DEFAULT 0 CHECK (node_type>=0 AND node_type<3)
 ,cls_id      INTEGER  DEFAULT NULL REFERENCES  t_cls(id)   MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,obj_id      INTEGER  DEFAULT NULL REFERENCES  t_objnum(id)MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


,CONSTRAINT ck_fav_type CHECK (  (node_type=0 AND cls_id IS NULL AND obj_id IS NULL)
                               OR(node_type=1 AND cls_id IS NOT NULL AND obj_id IS NULL)
                               OR(node_type=2 AND cls_id IS NULL AND obj_id IS NOT NULL))
 
,CONSTRAINT ck_fav_root CHECK (  (id=0 AND pid=0) -- main root
                               OR(id=1 AND pid=0) -- Object0
                               OR(id>1 AND pid>0 AND id<>pid)   )  -- один корень

);
GRANT SELECT ON TABLE t_favorites  TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorites TO "User";
GRANT USAGE ON TABLE t_favorites_id_seq TO "User"; -- Позволяет читать,инкрементировать текущее значение очереди
SELECT setval('t_favorites_id_seq', 100); 

INSERT INTO t_favorites(id,pid,label)   VALUES (0,0,'root');
INSERT INTO t_favorites(id,pid,label)   VALUES (1,0,'virtual_root');

INSERT INTO t_favorites(pid,label,node_type) VALUES (1,'folder_1',0);
INSERT INTO t_favorites(pid,label,node_type) VALUES (1,'folder_2',0);
INSERT INTO t_favorites(pid,label,node_type) VALUES (1,'folder_3',0);
INSERT INTO t_favorites(pid,cls_id,node_type)VALUES (102,103,1);
INSERT INTO t_favorites(pid,obj_id,node_type)VALUES (103,103,2);

-----------------------------------------------------------------------------------------------------------------------------
-- Вьюшка с сылками избранного
-----------------------------------------------------------------------------------------------------------------------------
--DROP VIEW w_favorites;
CREATE OR REPLACE VIEW w_favorites AS 
SELECT 
  t_favorites.id
 ,t_favorites.pid
 ,t_favorites.owner
 ,t_favorites.expanded
 ,t_favorites.label
 ,t_favorites.node_type

 ,t_favorites.cls_id
 ,t_cls.label AS favcls_label
 ,t_cls.type AS favcls_type
 ,t_cls.pid AS favcls_pid
 ,t_cls.measurename AS favcls_mes

 ,t_favorites.obj_id
  ,t_objnum.obj_label AS favobj_label
  ,t_objnum.pid AS favobj_pid
/*
 ,objcls.label AS favobj_cls_label
 ,objcls.type AS favobj_cls_type
 ,objcls.pid AS favobj_cls_pid
 ,objcls.measurename AS favobj_cls_mes
*/
FROM t_favorites
  LEFT JOIN t_objnum ON t_favorites.obj_id=t_objnum.id
  LEFT JOIN t_cls  ON t_favorites.cls_id =t_cls.id 
  OR t_objnum.cls_id=t_cls.id 
--  LEFT JOIN t_cls objcls  ON t_objnum.cls_id =objcls.id 
WHERE t_favorites.owner=CURRENT_USER;

GRANT SELECT ON TABLE w_favorites  TO "Guest";


/**
CREATE OR REPLACE FUNCTION isarray(text) RETURNS BOOLEAN AS '
select $1 ~ ''^(({){(1)}(}))?$'' as result
' LANGUAGE SQL;

SELECT isarray('{{11}}')

SELECT SUBSTRING('XY1234Z', 'Y*([0-9]{1,3})');
SELECT SUBSTRING('XY12223224Z', '[[:alnum:]_]*?([2]{1})');

SELECT SUBSTRING('{XY 122 23 224Z}', '(({)[\w\s]*(}))*');
SELECT SUBSTRING('{{XпрY,22},{3fgh45,34 5345}}', '(({)(({)[\w|\s|\\k]+(,)[\w|\s]+(}))+((,)({)[\w|\s]+(,)[\w|\s]+(}))*(}))*');
*/
-----------------------------------------------------------------------------------------------------------------------------
-- функция проверки числа 
-----------------------------------------------------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION isnumber(text) RETURNS BOOLEAN AS '
select $1 ~ ''^(-)?[\d]+((\.)[\d]+)?((e|E)(-)?[\d]+)?$'' as result
' LANGUAGE SQL;

--SELECT isnumber('66')
--SELECT isnumber('6.6')
--SELECT isnumber('6.6E6')








