DROP TABLE IF EXISTS t_favorites CASCADE;
DROP TABLE IF EXISTS t_whclass_prop CASCADE;

-----------------------------------------------------------------------------------------------------------------------------
-- таблица содержащая иерархическую структуру избранного для пользователей 
-----------------------------------------------------------------------------------------------------------------------------

CREATE TABLE t_favorites ( 
	favorites_id	SERIAL 	PRIMARY KEY		
	,pid		INTEGER NOT NULL DEFAULT 1 REFERENCES t_favorites(favorites_id) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE --SET DEFAULT
	,owner		NAME	NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
	,expanded	BOOLEAN	NOT NULL DEFAULT TRUE
	,label		TEXT 	 DEFAULT NULL 
	,CONSTRAINT  ck_favorites_root CHECK ( (favorites_id>0 AND pid>0 AND favorites_id<>pid) OR (favorites_id=0 AND pid=0) OR (favorites_id=1 AND pid=0) )-- один корень

	,classid 	INTEGER	 DEFAULT NULL REFERENCES  wh_class(classid) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
	,objnumid 	INTEGER	 DEFAULT NULL REFERENCES  wh_objnum(id) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
GRANT SELECT ON TABLE t_favorites  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorites 	TO "User";
GRANT USAGE ON TABLE t_favorites_favorites_id_seq	TO "User"; 		-- Позволяет читать,инкрементировать текущее значение очереди

INSERT INTO t_favorites(favorites_id,pid,label)   VALUES (0,0,'root');
INSERT INTO t_favorites(pid,label)   VALUES (0,'virtual_root'); 		-- favorites_id=1

INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_1');
INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_2');
INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_3');
INSERT INTO t_favorites(pid,classid)   VALUES (2,103);
INSERT INTO t_favorites(pid,objnumid)   VALUES (3,11);

-----------------------------------------------------------------------------------------------------------------------------
-- Вьюшка с сылками избранного
-----------------------------------------------------------------------------------------------------------------------------
--DROP VIEW w_favorites;
CREATE OR REPLACE VIEW w_favorites AS 
SELECT 
	 t_favorites.favorites_id
	,t_favorites.pid		
	,t_favorites.owner		
	,t_favorites.expanded	
	,t_favorites.label	
	,t_favorites.classid	
	,t_favorites.objnumid	
	

	,wh_objnum.objname 	AS objname
	,wh_objnum.classname 	AS objclass

	,wh_class.classname	AS classfilter

FROM
	t_favorites
	LEFT JOIN wh_class  ON t_favorites.classid =wh_class.classid 
	LEFT JOIN wh_objnum ON t_favorites.objnumid=wh_objnum.id
WHERE t_favorites.owner=CURRENT_USER;

GRANT SELECT ON TABLE w_favorites  			TO "Guest";


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


-----------------------------------------------------------------------------------------------------------------------------
-- таблица свойств классов
-----------------------------------------------------------------------------------------------------------------------------

CREATE TABLE t_whclass_prop ( 
	classid 	INTEGER	NOT NULL REFERENCES  wh_class(classid) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
	,label		TEXT 	NOT NULL
	,val		TEXT
	,type 		SMALLINT NOT NULL DEFAULT 0 -- 0=text, 1=num, 2=date, 3=link, 4=file
	,time 		TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT now()
	,editor 	NAME NOT NULL DEFAULT "current_user"()
	,prop_id	SERIAL 	UNIQUE		

	,CONSTRAINT pk_t_whclass_prop		PRIMARY KEY(classid,label)
	,CONSTRAINT ck_t_whclass_prop_type 	CHECK ( (type=1 AND isnumber(val)) OR type<>1  )

);
GRANT SELECT ON TABLE t_whclass_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_whclass_prop 	TO "Admin";




-- Свойства базы данных --
INSERT INTO t_whclass_prop(classid, label, val)   	VALUES (2, 'FTP server', '192.168.0.18');
INSERT INTO t_whclass_prop(classid, label, val)		VALUES (2, 'FTP port', '21');
INSERT INTO t_whclass_prop(classid, label, val)		VALUES (2, 'FTP username', 'postgres');
INSERT INTO t_whclass_prop(classid, label, val)		VALUES (2, 'FTP password', '1');

-- Тестовые cвойства --
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '12', 12, 1);
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '1', 1, 1);
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '11', 11, 1);
INSERT INTO t_whclass_prop(classid, label, val)         VALUES (103, 'str', 'str');
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '21', 21, 1);
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '3', 3, 1);
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, '2', 2, 1);
INSERT INTO t_whclass_prop(classid, label, val, type)   VALUES (103, 'c2', 'c2', 0);
--SELECT label,val, CASE  WHEN isnumber(val) THEN val::NUMERIC ELSE NULL END AS _data  FROM t_prop_class ORDER BY _data 









	