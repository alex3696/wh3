
--CREATE LANGUAGE "plpgsql";
DROP TABLE IF EXISTS wh_measure	CASCADE;
DROP TABLE IF EXISTS wh_class 	CASCADE;
DROP TABLE IF EXISTS wh_objnum 	CASCADE;
DROP TABLE IF EXISTS wh_objqty 	CASCADE;
DROP TABLE IF EXISTS wh_slot 	CASCADE;
DROP TABLE IF EXISTS wh_perm 	CASCADE;
DROP TABLE IF EXISTS wh_movlog	CASCADE;


-----------------------------------------------------------------------------------------------------------------------------
/** единицы измерения */
CREATE TABLE wh_measure (
	measurename 	TEXT 	NOT NULL	PRIMARY KEY,
	measuredesc 	TEXT 	NOT NULL	UNIQUE

	--,classtype	SMALLINT NOT NULL DEFAULT 2
--,measurenamealt 	TEXT 	NOT NULL	UNIQUE
); 
GRANT SELECT ON TABLE wh_measure  		TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE wh_measure 	TO "TypeDesigner";


INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('ед.','номерованных единиц (units)');
INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('шт.','штук (pcs)');
INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('г.','грамм');
INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('л.','литр');
INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('м.','метр');
INSERT INTO wh_measure(measurename,measuredesc)   VALUES ('кг.','Килограмм');

-----------------------------------------------------------------------------------------------------------------------------

/** таблица содеоржащая иерархическую структуру наследования КЛАССОВ */
CREATE TABLE wh_class ( 
	classid		SERIAL 	NOT NULL CHECK (classid>=0)		UNIQUE,		
	
	-- имя класса
	classname		TEXT 	NOT NULL			PRIMARY KEY ,	
	-- родительский класс
	classparent		TEXT 	NOT NULL DEFAULT 'Object'	REFERENCES wh_class(classname) 	ON UPDATE CASCADE ON DELETE CASCADE,

	classdesc		TEXT DEFAULT NULL,
	
	measurename		TEXT	DEFAULT 'ед.'	REFERENCES wh_measure(measurename) 	ON UPDATE CASCADE ON DELETE NO ACTION,


	classdeleted	INTEGER NOT NULL DEFAULT 0,

	
	CONSTRAINT ck_class_root CHECK ( classname<>classparent OR (classname='root' AND classparent='root'))-- один корень

	--(classtype=0[abstract] classtype=1[numbered] OR classtype=2[quantity]),
	,classtype	SMALLINT NOT NULL DEFAULT 1
	,CONSTRAINT uk_class_label UNIQUE( classname)
	,CONSTRAINT uk_class_qty UNIQUE( classname,classtype)
	,CONSTRAINT ck_class_classtype_measurename CHECK (  
		   (measurename IS NULL AND classtype=0) 			--если нет единиц измерения класс должен быть абстрактным
		OR (measurename IS NOT NULL AND measurename= 'ед.' AND classtype=1)	--если номерной класс
		OR (measurename IS NOT NULL AND measurename<>'ед.' AND classtype=2)	--если весовой класс
		)
		
);
GRANT SELECT ON TABLE wh_class  		TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE wh_class 	TO "TypeDesigner";
GRANT USAGE ON TABLE wh_class_classid_seq	TO "TypeDesigner"; 		-- Позволяет читать,инкрементировать текущее значение очереди
SELECT setval('wh_class_classid_seq', 100); 

-----------------------------------------------------------------------------------------------------------------------------
/**
INSERT - добавление к классу	- создаём таблички при их отсутствии t_state_XXX(act_label) t_log_XXX(act_label)
				- добавление отсутствующие столбцы в t_state_XXX t_log_XXX
*/
DROP FUNCTION IF EXISTS ftr_ai_class() CASCADE;
CREATE OR REPLACE FUNCTION ftr_ai_class()  RETURNS trigger AS
$body$
DECLARE

	state_tablename 	NAME;
	log_tablename		NAME;
BEGIN
	IF NEW.type=1 THEN
		SELECT table_name INTO state_tablename from information_schema.tables WHERE table_name='t_state_'||NEW.id;
		IF NOT FOUND THEN
			state_tablename:='t_state_'||NEW.id;
			EXECUTE 'CREATE TABLE '||state_tablename||'(
			obj_id		BIGINT NOT NULL PRIMARY KEY   
			,log_id		BIGINT NOT NULL 
			,act_label	NAME NOT NULL REFERENCES t_act(label) 	MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
			,CONSTRAINT fk_id_log_id_'||NEW.id||' FOREIGN KEY (obj_id,log_id) REFERENCES wh_objnum(id,log_id)MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
				)INHERITS (t_state_base) ';
			EXECUTE 'GRANT SELECT ON TABLE '||state_tablename||' 	TO "Guest";';
			EXECUTE 'GRANT INSERT,DELETE,UPDATE ON TABLE '||state_tablename||'	TO "User";';
			EXECUTE 'CREATE TRIGGER tr_aiu_state_'||NEW.id||' BEFORE INSERT OR UPDATE
					      ON '||state_tablename||' FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_state();';
			
		END IF;
		
		SELECT table_name INTO log_tablename from information_schema.tables WHERE table_name='t_log_'||NEW.id;
		IF NOT FOUND THEN 
			log_tablename:='t_log_'||NEW.id;
			RAISE DEBUG 'CREATE TABLE %',log_tablename;
			EXECUTE 'CREATE TABLE '||log_tablename||'(
				CONSTRAINT  pk_id_log_'||NEW.id||' 		PRIMARY KEY (log_id)
				,CONSTRAINT fk_user_log_'||NEW.id||'		FOREIGN KEY (loguser)  	REFERENCES wh_role(rolname)MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
				,CONSTRAINT fk_act_log_'||NEW.id||'		FOREIGN KEY (act_label)	REFERENCES t_act(label)    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
				,CONSTRAINT fk_obj_log_'||NEW.id||'		FOREIGN KEY(class_label,obj_label) 				REFERENCES t_state(class_label,obj_label)MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
				,CONSTRAINT fk_parent_obj_log_'||NEW.id||'	FOREIGN KEY(parent_class_label,parent_obj_label) 		REFERENCES t_state(class_label,obj_label)MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE
				,CONSTRAINT fk_previos_obj_log_'||NEW.id||'	FOREIGN KEY(previos_parent_class_label,previos_parent_obj_label)REFERENCES t_state(class_label,obj_label)MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE
				) INHERITS (t_log);';
				
			EXECUTE 'GRANT SELECT ON TABLE '||log_tablename||' TO "Guest";';
			EXECUTE 'GRANT INSERT,DELETE,UPDATE ON TABLE '||log_tablename||' TO "User";';
			
		END IF;
		

	END IF;
	
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_ai_class AFTER INSERT ON wh_class FOR EACH ROW EXECUTE PROCEDURE ftr_ai_class();



INSERT INTO wh_class(classid,classname,classparent,classdesc)   VALUES (0,'root','root','main root');
INSERT INTO wh_class(classid,classname,classparent,classdesc)   VALUES (1,'Object','root','main virtual root');
INSERT INTO wh_class(classid,classname,classparent,classdesc)   VALUES (2,'WH3 DataBase','Object','this database consist DB settings don`t remove');

-----------------------------------------------------------------------------------------------------------------------------
-- порядковый номер действия
CREATE SEQUENCE seq_act_log_id
  INCREMENT 1
  MINVALUE 1
  MAXVALUE 9223372036854775807
  START 1
  CACHE 1;
GRANT USAGE ON TABLE seq_act_log_id	TO "User"; 		-- Позволяет читать,инкрементировать текущее значение очереди  
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------


/** таблица содеоржащая иерархическую структуру ОБЪЕКТОВ */
CREATE TABLE wh_objnum (
	id		BIGSERIAL 	NOT NULL CHECK (id>=0)		UNIQUE		
	,log_id		BIGINT 		NOT NULL CHECK (log_id>=0)	UNIQUE	
	,pid		BIGINT 		NOT NULL DEFAULT 0 REFERENCES wh_objnum(id) ON UPDATE CASCADE ON DELETE SET DEFAULT	-- идентификатор родительского ОБЪЕКТА
	
	,CONSTRAINT  uk_state_id_logid	UNIQUE (id,log_id)
	
	CONSTRAINT ck_objnum_one_root CHECK ( (id=0 AND pid=0) OR id<>pid   ),  -- один корень
	
	-- имя ОБЪЕКТА
	objname		TEXT 	NOT NULL,	
	classname	TEXT 	NOT NULL REFERENCES wh_class(classname) 	ON UPDATE CASCADE ON DELETE CASCADE,
	CONSTRAINT pk_objnum	PRIMARY KEY(classname,objname),	-- уникальность  + primarykey - у одного типа не может быть объектов с одинаковыми именами			
	
	
	objcomment	TEXT DEFAULT NULL,		-- комментарий последнего перемещения
	objdeleted	INTEGER NOT NULL DEFAULT 0

	,classtype	SMALLINT NOT NULL DEFAULT 1 CHECK (classtype=1 OR (classtype=0 AND id=0))
	,CONSTRAINT fk_objnum_classtype FOREIGN KEY (classname,classtype)
			REFERENCES wh_class(classname,classtype) ON UPDATE CASCADE ON DELETE CASCADE
	
	
												 
);
GRANT SELECT ON TABLE wh_objnum  		TO "Guest";
GRANT UPDATE(pid,objcomment) ON TABLE wh_objnum	TO "User";
GRANT INSERT,DELETE,UPDATE ON TABLE wh_objnum 	TO "ObjDesigner";
GRANT USAGE ON TABLE wh_objnum_id_seq 		TO "ObjDesigner"; 		-- Позволяет читать,инкрементировать текущее значение очереди


INSERT INTO wh_objnum(id,pid,objname,classname,objcomment)  VALUES (0,0,'Object0','Object','Корневой объект');

--DROP INDEX idx_wh_objnum_pid;
CREATE INDEX idx_wh_objnum_pid ON wh_objnum USING btree("pid") ;


-----------------------------------------------------------------------------------------------------------------------------

/** таблица содеоржащая иерархическую структуру НЕ НУМЕРНЫХ ОБЪЕКТОВ (qtyitative objects) */
CREATE TABLE wh_objqty (
	--id		SERIAL 	NOT NULL CHECK (id>=0)	UNIQUE,		-- идентификатор количественного типа NULL
	pid	INTEGER NOT NULL DEFAULT 0 REFERENCES wh_objnum(id) ON UPDATE CASCADE ON DELETE SET DEFAULT,	-- идентификатор родительского ОБЪЕКТА
	
	-- имя партии ОБЪЕКТА
	objname		TEXT 	NOT NULL,	
	classname	TEXT 	NOT NULL REFERENCES wh_class(classname) 	ON UPDATE CASCADE ON DELETE CASCADE,

	-- количество (для ненумерованных типов)
	objcount	NUMERIC NOT NULL CHECK (objcount>=0),
	
	objcomment	TEXT,			-- комментарий последнего перемещения
	objdeleted	INTEGER NOT NULL DEFAULT 0

	,CONSTRAINT pk_objqty	PRIMARY KEY(classname,objname,pid) -- не может быть одинаковых номеров партий у одного узла

	,classtype	SMALLINT NOT NULL DEFAULT 2 CHECK (classtype=2)
	,CONSTRAINT fk_objnum_classtype FOREIGN KEY (classname,classtype)
			REFERENCES wh_class(classname,classtype) ON UPDATE CASCADE ON DELETE CASCADE

												 
);
GRANT SELECT ON TABLE wh_objqty  					TO "Guest";
GRANT INSERT,UPDATE(pid,objcount,objcomment) ON TABLE wh_objqty 	TO "User";
GRANT DELETE,UPDATE ON TABLE wh_objqty 					TO "ObjDesigner";


--DROP INDEX   idx_wh_objqty_pid;
CREATE INDEX idx_wh_objqty_pid ON wh_objqty USING btree("pid") ;
-----------------------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS wh_obj;
CREATE OR REPLACE VIEW wh_obj AS 
	SELECT id,  pid, objname,classname,objcomment,objdeleted,1 AS objcount, classtype FROM wh_objnum 
	UNION ALL 
	SELECT NULL AS id,pid, objname,classname,objcomment,objdeleted,objcount,classtype FROM wh_objqty;

GRANT SELECT ON TABLE wh_obj  	TO "Guest";	
-----------------------------------------------------------------------------------------------------------------------------


/** табдица слотов(контейнеров),связана с таблицей классов по имени класса.
по аналогии с ООП - слот это переменная класса являющаяся указателем на массив указателей (переменных)
наследование возможно только единичное, с наследованием всеех переменных

TODO: процедура выбирающая в таблицу все слоты(включае отнаследованые)
TODO: тригер для того чтоб не создавать слоты имеющиеся у родительского класса

TODO: изменить тригер разрешения переноса

-- внешние ключи на типы и объекты - в случае удаления слота,вложения объектов сохраняются, но повторно вложен быть не может
*/
CREATE TABLE wh_slot
(
	-- тип объекта к которому принадлежит слот,указывается обязательно по нему идёт привязка
	slotownerclass	TEXT	NOT NULL	REFERENCES wh_class (classname) MATCH FULL  ON UPDATE CASCADE ON DELETE CASCADE,
	-- /* Тип слота */,указывается обязательно 
	slotclass	TEXT	NOT NULL	REFERENCES wh_class (classname)	MATCH FULL  ON UPDATE CASCADE ON DELETE CASCADE,
	/** первичный ключ - тип слота + тип*/
	CONSTRAINT pk_slot_type_slottype	PRIMARY KEY (slotownerclass,slotclass), 
	slotmaxqty 	NUMERIC DEFAULT NULL,		-- максимальное чилсо объектов в слоте (NULL- бесконечно)
	slotminqty 	NUMERIC NOT NULL DEFAULT 0  CHECK (slotminqty>=0)-- минимальное чилсо объектов в слоте
	,CONSTRAINT ck_slot_max_more_min CHECK ( slotmaxqty>slotminqty)


	-- указываем тип класса, т.к. только номерованные объекты могут включать в себя другие объекты
	,classtype	SMALLINT NOT NULL DEFAULT 1 CHECK (classtype=1 OR (classtype=0 AND slotclass='Object'))
	,CONSTRAINT fk_objnum_classtype FOREIGN KEY (slotownerclass,classtype)
			REFERENCES wh_class(classname,classtype) ON UPDATE CASCADE ON DELETE SET DEFAULT
);
GRANT SELECT ON TABLE wh_slot  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE wh_slot 	TO "TypeDesigner";


-----------------------------------------------------------------------------------------------------------------------------

/** Таблица разрешений 
можно/нельзя[permtype] ложить объект "movobj" В слот "slotclass" объекта "slotownerobj" типа "slotownerclass"
	*/
CREATE TABLE wh_perm
(
	permid		SERIAL 	NOT NULL CHECK (permid>0)		PRIMARY KEY,		
	-- группа для которой разрешимо данное правило, (внешний ключ на группу)
	rolname		NAME	NOT NULL REFERENCES wh_role(rolname) 	MATCH FULL  ON UPDATE CASCADE ON DELETE CASCADE,

	-- тип допуска 0 открыть(разрешить), 1 закрыть(запретить)
	permtype	SMALLINT NOT NULL DEFAULT 0,

	
	-- слот(класс+класс слота) к которому привязано разрешение (внешний ключ на слот) 
/*куда(тип)*/slotownerclass	TEXT 	NOT NULL,
/*что(тип)*/slotclass	TEXT 	NOT NULL,
	CONSTRAINT fk_perm_slotownerclass_slotclass FOREIGN KEY (slotownerclass,slotclass)      REFERENCES wh_slot(slotownerclass,slotclass) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE,

	-- объект к которому привязано разрешение ( необязательное - в случае NULL(0) - допускается вложение лубого объекта типа slotclass) (внешний ключ на объект)
/*что(объект)*/movobj		TEXT,
	CONSTRAINT fk_perm_movable_obj_class 	FOREIGN KEY (slotclass,movobj)      REFERENCES wh_objnum (classname,objname) MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE,
/*куда(объект)*/slotownerobj	TEXT,
	CONSTRAINT fk_perm_dst_obj_class 	FOREIGN KEY (slotownerclass,slotownerobj)     REFERENCES wh_objnum (classname,objname) MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE,

/*куда путь*/-- текущее положение(путь) объекта с слотом разделённое на объекты и их типы для удобства(чтоб не делать JOIN на таблицу объектов)
/** без учёта имени обьекта тип объекут уже учтён slotownerclass , имя в [ownerobj] */
	dstobj_path 	TEXT,-- ARRAY,

/*откуда путь*/-- весь путь источника (откуда и что перемещают)
/** без учёта имени и класса перемещаемого обьекта т.к. тип учтён в slotclass,а имя в [dstobj] */
	srcobj_path 	TEXT-- ARRAY
	
	
);
GRANT SELECT ON TABLE wh_perm  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE wh_perm 	TO "TypeDesigner";
GRANT USAGE ON TABLE wh_perm_permid_seq		TO "TypeDesigner"; 		-- Позволяет читать,инкрементировать текущее значение очереди


CREATE INDEX ON wh_perm USING btree(slotclass) ;


-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде путей и таблицы, по идентификация объекта (уникальность)
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(IN _id INTEGER) CASCADE;
CREATE OR REPLACE FUNCTION fget_objnum_pathinfo_table(IN node_id INTEGER)
 RETURNS TABLE(	_id		INTEGER,
		_pid		INTEGER,
		_objname	TEXT,
		_classname	TEXT,
		_objcomment	TEXT,
		_objdeleted	INTEGER,
		_classtype	SMALLINT,
		_path		TEXT[]
		--,_cycle	BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY 
	(WITH RECURSIVE parents AS 
		(SELECT
			*, 
			ARRAY[ ARRAY[t.classname,t.objname]::TEXT[] ]::TEXT[] AS path
			--, ARRAY[t.id] AS exist, 
			--, FALSE AS cycle,
			
		FROM 
			wh_objnum AS t 
			WHERE 
			id = node_id   --[item.pid]
			AND node_id IS NOT NULL
		UNION ALL
		SELECT 	
			t.*,
			path || ARRAY[t.classname,t.objname]::TEXT[]
			--, exist || t.id
			--, t.id = ANY(exist)
		FROM 
			parents AS p, wh_objnum AS t 
			WHERE 
			t.id = p.pid
			AND t.id>0   
			--AND NOT cycle 
		)
	SELECT * FROM parents 
	);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(INTEGER) TO "Guest";
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(INTEGER) TO "Admin" WITH GRANT OPTION;

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде путей и таблицы, по  имени и типу объекта (первичный ключ)
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(IN _objname TEXT, IN _classname TEXT) CASCADE;
CREATE OR REPLACE FUNCTION fget_objnum_pathinfo_table(IN _in_objname TEXT, IN _in_classname TEXT)
 RETURNS TABLE(	_id		INTEGER,
		_pid		INTEGER,
		_objname	TEXT,
		_classname	TEXT,
		_objcomment	TEXT,
		_objdeleted	INTEGER,
		_classtype	SMALLINT,
		_path		TEXT[]
		--,_cycle	BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY 
	(WITH RECURSIVE parents AS 
		(SELECT
			*, 
			ARRAY[ ARRAY[t.classname,t.objname]::TEXT[] ]::TEXT[] AS path
			--, ARRAY[t.id] AS exist, 
			--, FALSE AS cycle,
			
		FROM 
			wh_objnum AS t 
			WHERE 
			    _in_objname = objname
			AND _in_objname	IS NOT NULL
			AND _in_classname = classname
			AND _in_classname IS NOT NULL
		UNION ALL
		SELECT 	
			t.*,
			path || ARRAY[t.classname,t.objname]::TEXT[]
			--, exist || t.id
			--, t.id = ANY(exist)
		FROM 
			parents AS p, wh_objnum AS t 
			WHERE 
			t.id = p.pid
			AND t.id>0   
			--AND NOT cycle 
		)
	SELECT * FROM parents 
	);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(INTEGER) TO "Guest";
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(INTEGER) TO "Admin" WITH GRANT OPTION;

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде 2мерного массива
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_array(IN node_pid INTEGER );
CREATE OR REPLACE FUNCTION get_path_array(IN node_pid INTEGER ) RETURNS TEXT[]  AS $$ 
	SELECT 	
		_path
	FROM	
		fget_objnum_pathinfo_table($1)
	WHERE	
		_pid=0
$$ LANGUAGE SQL;

GRANT EXECUTE ON FUNCTION get_path_array(integer) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_array(integer) TO "Admin" WITH GRANT OPTION;


--SELECT * FROM get_path_array(60);  

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения информации о наследовании класса
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_class_pathinfo_table(IN _name TEXT) CASCADE;
CREATE OR REPLACE FUNCTION fget_class_pathinfo_table(IN _name TEXT)
 RETURNS TABLE(	_classid	INTEGER,
		_classname	TEXT,
		_classparent	TEXT,
		_classdesc	TEXT,
		_measurename	TEXT,
		_classdeleted	INTEGER,
		_classtype	SMALLINT,
		_path		TEXT[]
		--,_cycle		BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY(
	WITH RECURSIVE parents AS 
		(SELECT
			 *, ARRAY[t.classname] AS path	--,FALSE AS cycle
		FROM 
			wh_class AS t 
		WHERE 
			_name IS NOT NULL 
			AND  t.classname = _name 
			AND  t.classid > 1
			--AND t.classname<>'Object'
		UNION ALL
		SELECT 	
			 t.*,path || t.classname	--,t.classname = ANY(path) 
		FROM 
			parents AS p, wh_class AS t 
		WHERE 
			t.classname = p.classparent 
			--AND NOT cycle 
			--AND t.classname <> t.classparent 
			--AND t.classname<>'Object'
			AND t.classid > 1
		)
	SELECT  * FROM 	parents 
	);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_class_pathinfo_table(TEXT) TO "Guest";
--SELECT * FROM fget_class_pathinfo_table('SmartMonitor(rev 1.0)')

-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути предков класса
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_class_parent_array(IN _name TEXT );
CREATE OR REPLACE FUNCTION fget_class_parent_array(IN _name TEXT ) RETURNS TEXT[]  AS $$ 
	SELECT 	
		_path
	FROM	
		fget_class_pathinfo_table($1)
	WHERE	
		_classparent='Object'
$$ LANGUAGE SQL;
GRANT EXECUTE ON FUNCTION fget_class_parent_array(TEXT) TO "Guest";

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
-- вьюшка для просмотра правил
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS moverule_lockup;

CREATE OR REPLACE VIEW moverule_lockup AS 
SELECT 
		obj.id 		AS MOV_OBJID,
		obj.pid		AS MOV_PID,
		obj.objname	AS MOV_OBJNAME,
		obj.classname 	AS MOV_CLASSNAME,
		obj.classtype 	AS MOV_CLASSTYPE,
		obj.objcount 	AS MOV_QTY,

		parent.classname AS PARENT_CLASSNAME,
		parent.objname   AS PARENT_OBJNAME,

		--get_path_array(obj.pid)	AS MOV_PATH,
		ARRAY[ARRAY[obj.classname,obj.objname]] || get_path_array(obj.pid) AS MOV_PATH,

		dst_obj.id 		AS DST_OBJID,
		obj.pid			AS DST_PID,
		dst_obj.objname		AS DST_OBJNAME,
		dst_obj.classname 	AS DST_CLASSNAME,

		get_path_array(dst_obj.id) AS DST_PATH,

		perm.permid    AS _perm_id,
		perm.permtype  AS _perm_type,
		slot.slotclass AS  _dst_slot_class,
		slot.slotminqty AS _dst_slot_min,
		slot.slotmaxqty AS _dst_slot_max
		
	FROM wh_obj obj


RIGHT JOIN 	wh_slot slot 		-- джоиним слоты 
	ON 	slot.slotclass IN (SELECT _classname FROM fget_class_pathinfo_table(obj.classname)) --=  obj.classname

RIGHT JOIN 	wh_objnum dst_obj 	-- джоиним классы с выбранными слотам
	ON 	dst_obj.classname=slot.slotownerclass 

RIGHT JOIN 	wh_objnum parent 	-- джоиним родителя
	ON	obj.pid=parent.id	

RIGHT JOIN 	wh_perm perm 		-- джоиним с классами имеющие слот выбранного типа
	ON	perm.slotownerclass= slot.slotownerclass
	AND 	perm.slotclass     = slot.slotclass
	AND  	(perm.movobj       IS NULL OR perm.movobj     =obj.objname )		 	-- отсекаем объекты,если указано имя объекта
	AND 	(perm.slotownerobj IS NULL OR parent.objname  =perm.slotownerobj )		-- отсеиваем лишние по родительскому объекту
	AND 	(perm.srcobj_path  IS NULL OR get_path_array(obj.pid)::TEXT     LIKE perm.srcobj_path )
	AND 	(perm.dstobj_path  IS NULL OR get_path_array(dst_obj.pid)::TEXT LIKE perm.dstobj_path )

	
	
-- group permission				
LEFT JOIN 	wh_role _group 
	ON	perm.rolname=_group.rolname	-- определяем ИМЕНА разрешённых групп
RIGHT JOIN	wh_auth_members membership
	ON	_group.id=membership.roleid	-- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
RIGHT JOIN 	wh_role _user  			
	ON	_user.id=membership.member 	-- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
	AND	_user.rolname=CURRENT_USER	-- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО

;

-- SELECT * FROM moverule_lockup WHERE MOV_OBJNAME='001' AND  MOV_CLASSNAME='ФЭУ102'
-- SELECT * FROM moverule_lockup WHERE MOV_OBJNAME='поставка №2' AND  MOV_CLASSNAME='Спирт'

/*
SELECT DISTINCT ON (DST_CLASSNAME) DST_CLASSNAME AS "Тип", DST_OBJNAME,  DST_PATH  
	FROM moverule_lockup 
	WHERE MOV_OBJNAME='001' AND  MOV_CLASSNAME='ФЭУ102'
	ORDER BY DST_CLASSNAME
*/

GRANT SELECT ON TABLE moverule_lockup		TO "Guest";

-----------------------------------------------------------------------------------------------------------------------------
/** таблица содеоржащая историю перемещения ОБЪЕКТОВ
*/
DROP TABLE IF EXISTS wh_movlog CASCADE;
CREATE TABLE wh_movlog (
	-- autofill --
	movlogid	SERIAL 	NOT NULL CHECK (movlogid>=0)	UNIQUE,		
	movlogtime	timestamp with time zone NOT NULL DEFAULT now(),
	rolname		NAME NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) 	MATCH FULL  ON UPDATE CASCADE ON DELETE NO ACTION,
	-- autofill --
	classname	TEXT 	NOT NULL REFERENCES wh_class(classname) 	MATCH FULL  ON UPDATE CASCADE ON DELETE NO ACTION, 	
	objname		TEXT 	NOT NULL,
	qty		NUMERIC NOT NULL,
	classtype	SMALLINT,
	movlogcomment	TEXT,			-- комментарий последнего перемещения

	src_path TEXT[],
	dst_path TEXT[],

	CONSTRAINT pk_movlog PRIMARY KEY (classname , objname, movlogtime )
	,CONSTRAINT ck_objnum_qty_one CHECK (   (classtype=1 AND qty=1) OR classtype=2	)
);

GRANT SELECT ON TABLE wh_movlog  	TO "Guest";
GRANT INSERT,DELETE ON TABLE wh_movlog  TO "User";
GRANT UPDATE ON TABLE wh_movlog  	TO "Admin";
GRANT USAGE ON TABLE wh_movlog_movlogid_seq	TO "User"; 		-- Позволяет читать,инкрементировать текущее значение очереди
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция перемещения объекта
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS move_obj(IN _src_path TEXT[],IN _dst_path TEXT[],IN _curr_pid INTEGER,IN qty NUMERIC,IN movcomment TEXT);
CREATE OR REPLACE FUNCTION move_obj(IN _src_path TEXT[],IN _dst_path TEXT[],IN _curr_pid INTEGER,IN qty NUMERIC,IN movcomment TEXT)
 RETURNS  VOID AS $BODY$	
DECLARE
	mov_obj 	TEXT[];
	dst_obj		TEXT[];
	src_path_wo_obj TEXT[];

	--_str moverule_lockup%ROWTYPE ;
	data_rec  RECORD;
	old_qty	NUMERIC;

	sum_curr NUMERIC;
	sum_dst  NUMERIC;
	min_curr NUMERIC;
	max_dst  NUMERIC;
BEGIN	
-- testing path проверяем что как минимум по одному элементу есть и размерность элемента равна двум (2)
	RAISE DEBUG 'MOVING % --> %',_src_path,_dst_path; 
	IF array_lower(_src_path,1)<1 OR array_lower(_dst_path,1)<1 OR array_upper(_src_path,2) <>2 OR array_upper(_dst_path,2)<>2 
	THEN
		RAISE EXCEPTION 'Wrong format in path: _src_path=%  --->  _dst_path=%',_src_path,_dst_path; 
	END IF;
-- extract and check MOVABLE and DESTINATION objects
	mov_obj = ARRAY [ _src_path[1][1],_src_path[1][2]];
	dst_obj  = ARRAY [ _dst_path[1][1],_dst_path[1][2]];
	RAISE DEBUG 'EXTRACTED mov_obj=%   dst_obj=%',mov_obj,dst_obj; 
	IF mov_obj[1] IS NULL OR dst_obj[1] IS NULL
	THEN
		RAISE EXCEPTION 'Wrong MOV_CLASSNAME=%  OR  DST_CLASSNAME=%',mov_obj[1],dst_obj[1]; 
	END IF;
	src_path_wo_obj =  _src_path[2:array_upper(_src_path,1)] ;
	RAISE DEBUG 'EXTRACTED src_path_wo_obj=%',src_path_wo_obj; 
-- search permission,slots and other data for moving from _src_path to _dst_path 
	SELECT 	sum(_perm_type),MOV_OBJID,MOV_CLASSTYPE,MOV_QTY,PARENT_CLASSNAME,DST_OBJID,_dst_slot_class,_dst_slot_min,_dst_slot_max
		INTO data_rec
	FROM moverule_lockup 
	WHERE 			MOV_CLASSNAME=mov_obj[1] AND MOV_OBJNAME=mov_obj[2]  
			AND 	DST_CLASSNAME=dst_obj[1] AND DST_OBJNAME=dst_obj[2]
			AND	MOV_PATH=_src_path
			AND	DST_PATH=_dst_path
			AND	MOV_PID =_curr_pid 
	GROUP BY DST_OBJID,MOV_OBJID,MOV_CLASSTYPE,MOV_QTY,PARENT_CLASSNAME,_dst_slot_class,_dst_slot_min,_dst_slot_max;	

	RAISE DEBUG 'CHECKED DATA %',data_rec;
	IF data_rec IS NULL OR data_rec.sum>0 
	THEN
		RAISE EXCEPTION 'No permission or permission denied for move % --> %',_src_path,_dst_path;  
	END IF;

/*	
-- Проверка на количество для отправителя на MIN  и MAX для принимающего
	
	-- сумма объектов слота в текущем слоте родителя (проверка минимального количества содержания) 
	-- если слот тип1 (наследованние типов тип1->тип2->тип3)и перемещаем объект типа2, то проверятся будет только сумма типов (тип2,тип3)... косяк однако
	SELECT SUM(objcount) INTO sum_curr FROM wh_obj WHERE pid=_curr_pid AND classname IN 
			(WITH RECURSIVE children AS 
				(SELECT classname, ARRAY[classid] AS exist,FALSE AS cycle
					FROM wh_class
					WHERE classname = mov_obj[1]
				 UNION ALL
				 SELECT  t.classname,exist || t.classid,t.classid = ANY(exist)
					FROM children AS c, wh_class  AS t
					WHERE t.classparent = c.classname AND NOT cycle 
				) 
				SELECT classname FROM children  WHERE NOT cycle 
			);
	SELECT slotminqty INTO min_curr FROM wh_slot WHERE slotownerclass=data_rec.PARENT_CLASSNAME       AND slotclass=mov_obj[1];-- минимум данного слота

	RAISE DEBUG 'DST_SLOT _dst_slot_class= %   _dst_slot_min=%   _dst_slot_max= %',data_rec._dst_slot_class,  data_rec._dst_slot_min,  data_rec._dst_slot_max;
	SELECT SUM(objcount) INTO sum_dst FROM wh_obj WHERE pid=data_rec.DST_OBJID AND classname IN 
			(WITH RECURSIVE children AS 
				(SELECT classname, ARRAY[classid] AS exist,FALSE AS cycle
					FROM wh_class
					WHERE classname = data_rec._dst_slot_class
				 UNION ALL
				 SELECT  t.classname,exist || t.classid,t.classid = ANY(exist)
					FROM children AS c, wh_class  AS t
					WHERE t.classparent = c.classname AND NOT cycle 
				) 
				SELECT classname FROM children  WHERE NOT cycle 
			);
	max_dst:=data_rec._dst_slot_max;

	RAISE DEBUG 'CHECK (sum_curr-qty )<min_curr = (%-%) < % = % < %',sum_curr,qty,min_curr,sum_curr-qty,min_curr;
	RAISE DEBUG 'CHECK (sum_dst +qty )>max_dst  = (%+%) > % = % > %',sum_dst,qty,max_dst,sum_dst+qty,max_dst;
*/
	IF ( 	qty <= 0 
	    OR	qty >  data_rec.MOV_QTY
		--OR (sum_curr-qty)<0 
		--OR (min_curr IS NOT NULL AND (sum_curr-qty )<min_curr) --остаток менее минимума или менее нуля
		--OR (max_dst  IS NOT NULL AND (sum_dst+qty )>max_dst)   --сумма более максимума
	    )
	THEN
		RAISE EXCEPTION 'Mismatch quantity of movable or destination SLOT';
	END IF;


	CASE
	-- MOVE numbered--
	WHEN data_rec.MOV_CLASSTYPE=1 THEN
		RAISE DEBUG 'MOVE NUMBERED UPDATE wh_objnum SET pid=%,objcomment=% WHERE id=%',
									data_rec.DST_OBJID ,movcomment, 	data_rec.MOV_OBJID;
		UPDATE wh_objnum SET pid=data_rec.DST_OBJID,objcomment=movcomment WHERE id=data_rec.MOV_OBJID;			
		INSERT INTO wh_movlog	( classname, objname,   qty, classtype, movlogcomment, src_path, dst_path)
			VALUES 		(mov_obj[1], mov_obj[2], 1,   1, 	movcomment,   src_path_wo_obj	,_dst_path );

	-- MOVE -- перемещаем всё
	WHEN data_rec.MOV_CLASSTYPE=2 AND data_rec.MOV_QTY=qty	THEN 

		SELECT objcount INTO old_qty FROM wh_objqty WHERE classname=mov_obj[1] AND objname=mov_obj[2] AND pid=data_rec.DST_OBJID;
		IF old_qty IS NOT NULL	THEN --если там уже есть этот объект
			RAISE DEBUG 'MOVE QTY сложение UPDATE wh_objqty SET objcount=%,objcomment=% WHERE objname=% AND classname=% AND pid=%',
									old_qty+qty,movcomment,	mov_obj[2],mov_obj[1],data_rec.DST_OBJID;
			UPDATE wh_objqty SET objcount=old_qty+qty,objcomment=movcomment WHERE classname=mov_obj[1] AND objname=mov_obj[2] AND pid=data_rec.DST_OBJID;
			RAISE DEBUG 'MOVE QTY удаление DELETE FROM wh_objqty WHERE classname=% AND objname=% AND pid=%',	mov_obj[1],mov_obj[2],_curr_pid;
			DELETE FROM wh_objqty WHERE classname=mov_obj[1] AND objname=mov_obj[2] AND pid=_curr_pid;
		ELSE
			RAISE DEBUG 'MOVE QTY перенос UPDATE wh_objqty SET pid=%,objcomment=% WHERE objname=% AND classname=% AND pid=%',
										data_rec.DST_OBJID ,movcomment,	mov_obj[2],mov_obj[1],_curr_pid;
			UPDATE wh_objqty SET pid=data_rec.DST_OBJID,objcomment=movcomment WHERE objname=mov_obj[2] AND classname=mov_obj[1]  AND pid=_curr_pid;	
		END IF;

		INSERT INTO wh_movlog	( classname, objname,   qty, classtype, movlogcomment, src_path, dst_path)
			VALUES 		(mov_obj[1], mov_obj[2],qty,   2, 	movcomment,   src_path_wo_obj	,_dst_path );

	
	-- DIV --
	WHEN data_rec.MOV_CLASSTYPE=2 AND data_rec.MOV_QTY>qty THEN
		RAISE DEBUG 'MOVE QTY вычитание UPDATE wh_objqty SET objcount=% WHERE objname=% AND classname=% AND pid=%',
									data_rec.MOV_QTY-qty,		mov_obj[2],mov_obj[1],_curr_pid;
		UPDATE wh_objqty SET objcount=data_rec.MOV_QTY-qty WHERE objname=mov_obj[2] AND classname=mov_obj[1]  AND pid=_curr_pid;	

		SELECT objcount INTO old_qty FROM wh_objqty WHERE pid=data_rec.DST_OBJID AND objname=mov_obj[2] AND classname=mov_obj[1];
		IF old_qty IS NOT NULL	THEN 
			RAISE DEBUG 'MOVE QTY добавление UPDATE wh_objqty SET objcount=%,objcomment=% WHERE objname=% AND classname=% AND pid=%',
									old_qty+qty,movcomment,	mov_obj[2],mov_obj[1],data_rec.DST_OBJID;
			UPDATE wh_objqty SET objcount=old_qty+qty,objcomment=movcomment WHERE objname=mov_obj[2] AND classname=mov_obj[1]  AND pid=data_rec.DST_OBJID;
		ELSE
			RAISE DEBUG 'MOVE QTY вставка INSERT INTO wh_objqty(pid,objname,classname,objcount,objcomment)VALUES(%,%,%,%,%)',
								data_rec.DST_OBJID,mov_obj[2],mov_obj[1],qty,movcomment;
			INSERT INTO wh_objqty(pid, objname, classname, objcount, objcomment) VALUES (data_rec.DST_OBJID,mov_obj[2],mov_obj[1],qty,movcomment);
		END IF;

		INSERT INTO wh_movlog	( classname, objname,   qty, classtype, movlogcomment, src_path, dst_path)
			VALUES 		(mov_obj[1], mov_obj[2],qty,   2, 	movcomment,   src_path_wo_obj	,_dst_path );

	ELSE
		RAISE EXCEPTION 'Wrong MOV_CLASSTYPE or unknown error'; 
	END CASE;


RETURN;
END;	
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 500;

GRANT EXECUTE ON FUNCTION move_obj(IN _src_path TEXT[],IN _dst_path TEXT[],IN _curr_pid INTEGER,IN qty NUMERIC,IN movcomment TEXT) TO "User";
GRANT EXECUTE ON FUNCTION move_obj(IN _src_path TEXT[],IN _dst_path TEXT[],IN _curr_pid INTEGER,IN qty NUMERIC,IN movcomment TEXT) TO "Admin" WITH GRANT OPTION;
  

--  SELECT move_obj( '{{Спирт,поставка №2}}'::TEXT[],'{{Склад,ямбург}}'::TEXT[],0, 0.05, NULL);
--  SELECT move_obj( '{{Спирт,поставка №2},{Склад,уренгой}}'::TEXT[],'{{Склад,ямбург}}'::TEXT[],0, 0.05, NULL);
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
--функция собирания пути из 2 массивов
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION make_path(IN revarray TEXT[])
 RETURNS TEXT AS $BODY$	
DECLARE
	result TEXT;
	i INTEGER;
BEGIN	
	result:='';
	i:=1;
	WHILE(revarray[i][1] IS NOT NULL AND revarray[i][2] IS NOT NULL )
	LOOP
		result:= '/['||revarray[i][1]||']'||revarray[i][2]||result; --RAISE NOTICE 'oitem=%  titem=%  result=%',opath[i],tpath[i],result; 
		i:=i+1;
	END LOOP;
	--RAISE NOTICE 'RETURN result=%',result; 
	RETURN result;

END;	
$BODY$
  LANGUAGE plpgsql VOLATILE  COST 100;

GRANT EXECUTE ON FUNCTION make_path(IN revarray TEXT[]) TO "Guest";
GRANT EXECUTE ON FUNCTION make_path(IN revarray TEXT[]) TO "Admin" WITH GRANT OPTION;
  
--SELECT * FROM make_path('{1,2,3,4,5,6}','{gh,fgh,gfhfh,fhg}'); -- тест функции
--SELECT make_path('{1,2,3,4,5,6}','{gh,fgh,gfhfh,fhg}'); -- тест функции
--SELECT rolname,make_path(srcobj_opath,srcobj_tpath)  FROM wh_perm; -- тест функции
--SELECT make_path(opath,tpath) FROM get_path(6); -- тест функции















-----------------------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '--- Заполняем базу тестовыми данными';
PRINT '';
-----------------------------------------------------------------------------------------------------------------------------
INSERT INTO wh_class(classname,classparent)   VALUES ('Склад','Object');
INSERT INTO wh_class(classname,classparent)   VALUES ('ВИКИЗ','Object');
INSERT INTO wh_class(classname,classparent)   VALUES ('СРК2М','Object');
INSERT INTO wh_class(classname,classparent)   VALUES ('ФЭУ','Object');
INSERT INTO wh_class(classname,classparent)   VALUES ('ФЭУ102','ФЭУ');
INSERT INTO wh_class(classname,classparent)   VALUES ('Hamamatsu102','ФЭУ');
INSERT INTO wh_class(classname,classparent,classtype,measurename)   VALUES ('Спирт','Object',2,'л.');
INSERT INTO wh_class(classname,classparent,classtype,measurename)   VALUES ('масло','Object',2,'кг.');
INSERT INTO wh_class(classname,classparent)   VALUES ('PLT9','Object');

INSERT INTO wh_objnum(objname,classname)   VALUES ('ямбург','Склад');
INSERT INTO wh_objnum(objname,classname)   VALUES ('уренгой','Склад');

INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('001','ВИКИЗ', 1);
INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('002','ВИКИЗ', 1);
INSERT INTO wh_objnum(objname,classname)       VALUES ('003','ВИКИЗ');

INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('011','СРК2М', 1);
INSERT INTO wh_objnum(objname,classname)       VALUES ('012','СРК2М');

INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('p№ 1','PLT9',1);
INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('p№ 2','PLT9',2);


INSERT INTO wh_objnum(objname,classname)   VALUES ('001','ФЭУ102');
INSERT INTO wh_objnum(objname,classname)   VALUES ('002','ФЭУ102');
INSERT INTO wh_objnum(objname,classname)   VALUES ('001','Hamamatsu102');
INSERT INTO wh_objnum(objname,classname,pid)   VALUES ('002','Hamamatsu102',6);

INSERT INTO wh_objqty(objname,classname,objcount,pid)   VALUES ('поставка №1','Спирт',10,1);
INSERT INTO wh_objqty(objname,classname,objcount)   VALUES ('поставка №2','Спирт',15.389);

INSERT INTO wh_objqty(objname,classname,objcount,pid)   VALUES ('поставка №001','масло',1.1, 2);
INSERT INTO wh_objqty(objname,classname,objcount,pid)   VALUES ('поставка №002','масло',5.3, 2);
INSERT INTO wh_objqty(objname,classname,objcount,pid)   VALUES ('поставка №003','масло',6.1, 1);


INSERT INTO wh_slot(slotownerclass, slotclass, slotmaxqty, slotminqty)VALUES ('СРК2М', 'ФЭУ', 1, 0);
INSERT INTO wh_slot(slotownerclass, slotclass, slotmaxqty, slotminqty)VALUES ('СРК2М', 'ФЭУ102', 1, 0);

INSERT INTO wh_slot(slotownerclass, slotclass)VALUES ('Склад', 'ВИКИЗ');
INSERT INTO wh_slot(slotownerclass, slotclass)VALUES ('Склад', 'СРК2М');
INSERT INTO wh_slot(slotownerclass, slotclass)VALUES ('Склад', 'Спирт');
INSERT INTO wh_slot(slotownerclass, slotclass)VALUES ('Склад', 'масло');



INSERT INTO wh_perm(rolname, slotownerclass,slotclass,slotownerobj,movobj,  srcobj_path,dstobj_path )
	VALUES ('User', 'СРК2М','ФЭУ', NULL,NULL,'{{Склад,%}%}','{{Склад,%}%}' );

INSERT INTO wh_perm(rolname, slotownerclass,slotclass,slotownerobj,movobj,  srcobj_path,dstobj_path )
	VALUES ('User', 'Склад','Спирт', NULL,NULL,NULL,NULL );



-----------------------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '--- Заполняем базу тестовыми данными';
PRINT '';
-----------------------------------------------------------------------------------------------------------------------------
/*
DELETE FROM wh_class WHERE classname LIKE 'TEST%';
INSERT INTO wh_class(classname,classparent,classtype,measurename)VALUES('TEST','Object',0,NULL);


DECLARE @type_qty,@obj_qty,@obj_pid,@mins,@maxs,@begin_id,@type_rnd,@slot_qty,@CUR_TYPE_RND;
SET @type_qty = 100;

INSERT INTO wh_class(classname,classparent)VALUES('TEST_MAIN','TEST');
INSERT INTO wh_objnum(objname,classname)   VALUES ('TEST_MAIN_1','TEST_MAIN');
SET @begin_id=SELECT id FROM wh_objnum WHERE objname='TEST_MAIN_1' AND classname='TEST_MAIN';
PRINT 'begin_id=' + CAST(@begin_id AS STRING);
SET @mins= @begin_id;


WHILE (@type_qty>0)
BEGIN
	INSERT INTO wh_class(classname, classparent)  				VALUES ('TEST_NUM_@type_qty','TEST' );
	INSERT INTO wh_class(classname, classparent,classtype,measurename)  	VALUES ('TEST_QTY_@type_qty','TEST',2,'л.' );

	
	
	SET @maxs= (100 - @type_qty ) * 100   +  CAST (@mins AS INTEGER);
	--инициализируем генераторы случайных чисел
	SET @obj_pid=INTEGER(CAST (@mins AS INTEGER), CAST (@maxs AS INTEGER) , 1) ;
	PRINT 'id = ' + CAST (@mins AS STRING)+ CAST (@maxs AS STRING);


	SET @type_rnd=INTEGER(CAST (@type_qty AS INTEGER)-1,100,1);
	SET @slot_qty=10;
	WHILE (@slot_qty>0 AND @type_qty<90)
	BEGIN
		SET @CUR_TYPE_RND='TEST_NUM_'+ CAST (@type_rnd AS STRING);
		INSERT INTO wh_slot(slotownerclass, slotclass)VALUES ('TEST_NUM_@type_qty', '@CUR_TYPE_RND');
		INSERT INTO wh_perm(rolname, slotownerclass,slotclass,slotownerobj,movobj,  srcobj_path,dstobj_path )
			VALUES ('postgres', 'TEST_NUM_@type_qty', '@CUR_TYPE_RND', NULL,NULL,NULL,NULL );
		SET @slot_qty=@slot_qty-1;
		
	END 
	
	

	SET @obj_qty  = 100;
	WHILE (@obj_qty>0)
	BEGIN
		INSERT INTO wh_objnum(objname,classname,pid)   		 VALUES ('objn_@obj_qty','TEST_NUM_@type_qty',@obj_pid);
		INSERT INTO wh_objqty(objname,classname,objcount,pid )   VALUES ('objq_@obj_qty','TEST_QTY_@type_qty',111,@obj_pid);
		
		SET @obj_qty=@obj_qty-1;
	END



	SET @type_qty=@type_qty-1;
END
*/
VACUUM FULL ANALYSE ;










