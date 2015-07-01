SET client_min_messages='debug1';
SHOW client_min_messages;

DROP TABLE IF EXISTS t_class 	CASCADE;
DROP TABLE IF EXISTS t_act CASCADE;
DROP TABLE IF EXISTS t_prop CASCADE;



DROP TABLE IF EXISTS t_ref_class_act 	CASCADE;
DROP TABLE IF EXISTS t_act_access 	CASCADE;
DROP TABLE IF EXISTS t_ref_act_prop 	CASCADE;

DROP TABLE IF EXISTS t_class_prop CASCADE;			

DROP TABLE IF EXISTS t_favorite_prop CASCADE;


DROP VIEW IF EXISTS w_obj;
DROP TABLE IF EXISTS t_obj 		CASCADE;
DROP TABLE IF EXISTS t_objnum		CASCADE;
DROP TABLE IF EXISTS t_objqty 		CASCADE;


DROP TABLE IF EXISTS t_state_base 	CASCADE;
DROP TABLE IF EXISTS t_log_base 	CASCADE;

DROP TABLE IF EXISTS t_state0_num 	CASCADE;
DROP TABLE IF EXISTS t_state0_qty 	CASCADE;

 

DROP SEQUENCE IF EXISTS seq_obj_id 	CASCADE;
DROP SEQUENCE IF EXISTS seq_log_id 	CASCADE;
DROP TABLE IF EXISTS t_log 		CASCADE;

DROP TABLE IF EXISTS t_rule 		CASCADE;
DROP TABLE IF EXISTS t_slot	CASCADE;
DROP TABLE IF EXISTS t_perm	CASCADE;
DROP VIEW IF EXISTS moverule_lockup CASCADE;


DROP TABLE IF EXISTS t_favorites CASCADE;



DROP FUNCTION IF EXISTS fget_class_parent_array(name);
DROP FUNCTION IF EXISTS fget_class_pathinfo_table(text);
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(bigint);
DROP FUNCTION IF EXISTS get_path_array(bigint);
DROP FUNCTION IF EXISTS isnumber(text);
DROP FUNCTION IF EXISTS move_obj(name[], name[], bigint, numeric, text);

DROP FUNCTION IF EXISTS ftr_bi_class();
DROP FUNCTION IF EXISTS ftr_ai_class();
DROP FUNCTION IF EXISTS ftr_bu_class();
DROP FUNCTION IF EXISTS ftr_bd_class();



DROP FUNCTION IF EXISTS ftr_biu_class_ref_act();
DROP FUNCTION IF EXISTS ftr_aiu_act_ref_prop();

DROP FUNCTION IF EXISTS ftr_aiu_state();

DROP FUNCTION IF EXISTS ftr_bi_obj();

DROP FUNCTION IF EXISTS ftr_bu_act_prop();


DROP FUNCTION IF EXISTS ftr_bu_prop();


-----------------------------------------------------------------------------------------------------------------------------
-- таблица содеоржащая иерархическую структуру наследования КЛАССОВ 
CREATE TABLE t_class ( 
 id	BIGSERIAL 	NOT NULL		-- идентификатор записи		
,type	SMALLINT 	NOT NULL DEFAULT 1 	--(class_type=0[abstract] class_type=1[numbered] OR class_type=2[quantity]),
,label	NAME 		NOT NULL 		-- имя класса	
,vid	BIGSERIAL 	NOT NULL 		-- версия записи
,parent	NAME 		NOT NULL DEFAULT 'Object'
,description	TEXT DEFAULT NULL
,measurename	TEXT DEFAULT 'ед.'

,CONSTRAINT pk_class__label		PRIMARY KEY(label) 
,CONSTRAINT pk_class__label_type	UNIQUE(label,type) 
,CONSTRAINT uk_class__id		UNIQUE(id)
,CONSTRAINT uk_class__vid		UNIQUE(vid) 

,CONSTRAINT fk_class__parent 	FOREIGN KEY
		(parent) REFERENCES t_class(label) MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

,CONSTRAINT ck_class_root 	CHECK 
		( label<>parent OR (label='root' AND parent='root'))		-- один корень
,CONSTRAINT ck_class_type_measurename CHECK (  
		   ( type=0 AND measurename IS NULL ) 				--если нет единиц измерения класс должен быть абстрактным
		OR ( type=1 AND measurename IS NOT NULL AND measurename= 'ед.')	--если номерной класс
		OR ( type=2 AND measurename IS NOT NULL AND measurename<>'ед.')	--если весовой класс
		)

);
GRANT SELECT ON TABLE t_class  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_class 	TO "TypeDesigner";
GRANT USAGE ON TABLE t_class_id_seq		TO "TypeDesigner"; 		-- Позволяет читать,инкрементировать текущее значение очереди
SELECT setval('t_class_id_seq', 100); 

INSERT INTO t_class(id,label,parent,description)   VALUES (0,'root','root','main root');
INSERT INTO t_class(id,label,parent,description)   VALUES (1,'Object','root','main virtual root');
INSERT INTO t_class(id,label,parent,description)   VALUES (2,'WH3 DataBase','Object','this database consist DB settings don`t remove');


-----------------------------------------------------------------------------------------------------------------------------
-- основная таблица действий 
/**
insert Создание ДЕЙСТВИЯ - нет действий
update Изменение ДЕЙСТВИЯ	- каскадное изменение записей d t_ref_act_prop(act_label) и t_ref_class_act(act_label) 
delete Удаление ДЕЙСТВИЯ 	- каскадное удаление записей из из t_ref_act_prop и t_ref_class_act
*/
CREATE TABLE t_act (
id		BIGSERIAL 	NOT NULL
,vid		BIGSERIAL 	NOT NULL
,label		NAME		NOT NULL
,description	TEXT 	DEFAULT NULL
,color		NAME 	DEFAULT NULL
,script		TEXT 	DEFAULT NULL

,CONSTRAINT pk_act__label	PRIMARY KEY(label) 
,CONSTRAINT uk_act__id	UNIQUE(id)
,CONSTRAINT uk_act__vid	UNIQUE(vid) 	
);
GRANT SELECT ON TABLE t_act  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_act 	TO "TypeDesigner";

INSERT INTO t_act(id,label,description)VALUES(0,'Перемещение','встроенное действие по перемещению объёктов');
-----------------------------------------------------------------------------------------------------------------------------
-- основная описания свойств(переменных????) действий
/** insert Создание свойств - нет действий */
/** update Изменение свойств	- каскадное изменение prop_label переименование колонок в таблицах состояния t_act_state_XXX и истории t_act_log_XXX
/** delete Удаление свойств 	- каскадное удаление записей из t_ref_act_prop */
CREATE TABLE t_prop ( 
 id		BIGSERIAL 	NOT NULL 
,label		TEXT
,type 		SMALLINT 	NOT NULL DEFAULT 0 -- 0=text, 1=num, 2=date, 3=link, 4=file 5=array

,CONSTRAINT uk_prop__id		UNIQUE(id)
,CONSTRAINT pk_prop__label	PRIMARY KEY(label)
	
);

GRANT SELECT ON TABLE t_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_prop 	TO "TypeDesigner";
-----------------------------------------------------------------------------------------------------------------------------
-- линки М:М , для действий и свойств
/**
INSERT || UPDATE прямое изменение prop_label/act_label   добавление отсутствующие столбцы в t_act_state_XXX t_act_log_XXX
/** delete - удаление свойства из действия - ничего не делаем */
CREATE TABLE t_ref_act_prop ( 
id		BIGSERIAL 	NOT NULL
,act_label	NAME 		NOT NULL 
,prop_label 	NAME 		NOT NULL

,CONSTRAINT pk_ref_act_prop 		PRIMARY KEY (act_label,prop_label) 
,CONSTRAINT uk_ref_act_prop__id		UNIQUE(id)

,CONSTRAINT fk_ref_act_prop__prop	FOREIGN KEY
	(act_label) 	REFERENCES t_act(label)  MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_ref_act_prop__act	FOREIGN KEY
	(prop_label)	REFERENCES t_prop(label) MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
	
);
GRANT SELECT ON TABLE t_ref_act_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_ref_act_prop 	TO "TypeDesigner";
-----------------------------------------------------------------------------------------------------------------------------
-- табличка связи классов с действиями
/**
INSERT - добавление дествия к классу 	- создаём таблички при их отсутствии t_act_state_XXX(act_label) t_act_log_XXX(act_label)
					- добавление отсутствующие столбцы в t_act_state_XXX t_act_log_XXX
UPDATE	- каскадное изменение act_label	- равнозначно добавлению другого действия этому классу

/**delete*/

CREATE TABLE t_ref_class_act ( 
id		BIGSERIAL 	NOT NULL
,class_label 	NAME 		NOT NULL 
,class_type 	SMALLINT	NOT NULL DEFAULT 1 CHECK ( class_type=1 )
,act_label	NAME 		NOT NULL 

,CONSTRAINT pk_ref_class_act 		PRIMARY KEY (class_label, act_label) 
,CONSTRAINT uk_ref_class_act__id	UNIQUE(id)

,CONSTRAINT fk_ref_class_act__class	FOREIGN KEY
	(class_label, class_type) 	REFERENCES t_class(label,type)  MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_ref_class_act__act	FOREIGN KEY
	(act_label)	REFERENCES t_act(label) MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

);
GRANT SELECT ON TABLE t_ref_class_act  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_ref_class_act 	TO "TypeDesigner";
-----------------------------------------------------------------------------------------------------------------------------
-- табличка разрешений для связи классов с действиями
CREATE TABLE t_act_access ( 
id		BIGSERIAL 	NOT NULL
,act_label	NAME 		NOT NULL 
,class_label 	NAME 		NOT NULL 

,access_group	NAME 		NOT NULL 
,access_disabled SMALLINT 	NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)

,access_path	NAME ARRAY -- паттерн местоположения и имя текущего объекта типа {{class_label,%}%} == любой объект в любом месте

,CONSTRAINT uk_act_access__id	UNIQUE(id)

,CONSTRAINT fk_act_access__class	
	FOREIGN KEY 	(class_label, act_label) REFERENCES 
	t_ref_class_act(class_label, act_label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_act_access__role
	FOREIGN KEY 	(access_group) REFERENCES 
	wh_role		(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);

GRANT SELECT ON TABLE t_act_access  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_act_access 	TO "TypeDesigner";
-----------------------------------------------------------------------------------------------------------------------------
-- таблица избранных свойст объектов отображаемых при просмотре пользователем каталога объектов по типу или по местоположению
-----------------------------------------------------------------------------------------------------------------------------
CREATE TABLE t_favorite_prop (
 id		BIGSERIAL NOT NULL UNIQUE
,user_label	NAME NOT NULL DEFAULT CURRENT_USER 
,class_label 	NAME NOT NULL
,act_label 	NAME NOT NULL
,prop_label 	NAME NOT NULL

,CONSTRAINT pk_favorite_prop PRIMARY KEY (user_label, class_label, act_label, prop_label )

,CONSTRAINT fk_favorite_prop__user 
	FOREIGN KEY 	(user_label) 		REFERENCES 
	wh_role 	(rolname) 		MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favorite_prop__ref_act_prop 
	FOREIGN KEY 	(act_label, prop_label) REFERENCES 
	t_ref_act_prop 	(act_label, prop_label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favorite_prop__ref_class_act
	FOREIGN KEY 	(class_label, act_label)REFERENCES 
	t_ref_class_act	(class_label, act_label)MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


  	
);
GRANT SELECT ON TABLE t_favorite_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorite_prop 	TO "TypeDesigner";
-----------------------------------------------------------------------------------------------------------------------------
-- таблица свойств классов
-----------------------------------------------------------------------------------------------------------------------------
CREATE TABLE t_class_prop ( 
	id		BIGSERIAL NOT NULL UNIQUE
	,class_label	NAME	NOT NULL REFERENCES  t_class(label) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
	,prop_label 	NAME	NOT NULL REFERENCES  t_prop(label) 	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
	,val		TEXT

,CONSTRAINT pk_t_classprop		
	PRIMARY KEY(class_label,prop_label)

);
GRANT SELECT ON TABLE t_class_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_class_prop 	TO "ObjDesigner";
-----------------------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS w_class_prop ;
CREATE OR REPLACE VIEW w_class_prop AS 
SELECT 	t_class_prop.id,
	class_label,

	t_prop.id 	AS 	prop_id,
	t_prop.type	AS 	prop_type,
	prop_label, 
	val
  FROM t_class_prop 
  LEFT JOIN t_prop ON t_prop.label = t_class_prop.prop_label;

  GRANT SELECT ON TABLE w_class_prop  				TO "Guest";
-----------------------------------------------------------------------------------------------------------------------------


 
-----------------------------------------------------------------------------------------------------------------------------
-- порядковый номер объекта
CREATE SEQUENCE seq_obj_id
  INCREMENT 1
  MINVALUE 0
  NO MAXVALUE
  START 1;
GRANT USAGE ON TABLE seq_obj_id	TO "User"; 		-- Позволяет читать,инкрементировать текущее значение очереди  

-----------------------------------------------------------------------------------------------------------------------------
-- порядковый номер действия
CREATE SEQUENCE seq_log_id
  INCREMENT 1
  MINVALUE 1
  NO MAXVALUE
  START 1
  CACHE 1;

GRANT USAGE ON TABLE seq_log_id	TO "User"; 		-- Позволяет читать,инкрементировать текущее значение очереди  
-----------------------------------------------------------------------------------------------------------------------------
-- базовая табличка состояния и логов, чтоб удалялось каскадно 
CREATE TABLE t_state_base();
CREATE TABLE t_log_base();

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
-- таблица содеоржащая иерархическую структуру ОБЪЕКТОВ их состояние, местоположение и количество
CREATE TABLE t_obj (
 id		BIGSERIAL 	NOT NULL
,class_label	NAME 		NOT NULL 
,class_type	SMALLINT	NOT NULL
,obj_label	NAME 		NOT NULL

,CONSTRAINT pk_obj 		PRIMARY KEY ( class_label, class_type, obj_label ) 
,CONSTRAINT uk_obj__id 		UNIQUE(id) 
,CONSTRAINT fk_obj__class 	FOREIGN KEY(class_label,class_type) 	REFERENCES t_class(label,type)	MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT ck_obj__class_type	CHECK ( class_type>0 )

);


-----------------------------------------------------------------------------------------------------------------------------
-- таблица состояния ИСТОРИИ ПЕРЕМЕЩЕНИЯ
CREATE TABLE t_log_common (

/** общие данные по логированию */
 log_id		BIGINT NOT NULL DEFAULT nextval('seq_log_id'::regclass)
,log_time	timestamp with time zone NOT NULL DEFAULT now()
,log_user	NAME NOT NULL DEFAULT CURRENT_USER  

,class_label	NAME 		NOT NULL 		/** ЧТО 	*/
,class_type	SMALLINT	NOT NULL
,obj_label	NAME 		NOT NULL

,act_id		BIGINT 		NOT NULL
--,act_label	NAME 		NOT NULL


,CONSTRAINT pk_logcommon	 	PRIMARY KEY(log_id) 
,CONSTRAINT fk_logcommon__obj_label 	FOREIGN KEY ( class_label, class_type, obj_label ) REFERENCES t_obj(class_label, class_type, obj_label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_logcommon__act_id 	FOREIGN KEY ( act_id ) REFERENCES t_act(id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
--,CONSTRAINT fk_logcommon__act_label 	FOREIGN KEY ( act_label ) REFERENCES t_act(label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

) INHERITS (t_log_base);



-----------------------------------------------------------------------------------------------------------------------------
-- таблица состояния ИСТОРИИ ПЕРЕМЕЩЕНИЯ
CREATE TABLE t_log0_num (

/** общие данные по логированию */
 log_id		BIGINT 		NOT NULL

,src_path	NAME ARRAY				/** ОТКУДА 	*/
,dst_path	NAME ARRAY				/** КУДА 	*/
,note		VARCHAR DEFAULT NULL			/** ПРИМЕЧАНИЕ 	*/


,CONSTRAINT pk_lognum0	PRIMARY KEY ( log_id ) 

,CONSTRAINT fk_lognum0__logid 	FOREIGN KEY ( log_id ) REFERENCES t_log_common(log_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE



) INHERITS (t_log_base);

-----------------------------------------------------------------------------------------------------------------------------
-- таблица содеоржащая все нумерные объекты
CREATE TABLE t_state0_num (
 class_label	NAME 		NOT NULL 
,class_type	SMALLINT	NOT NULL DEFAULT 1 CHECK ( class_type=1 )
,obj_label	NAME 		NOT NULL

,last_log_id	BIGINT 		NOT NULL 

,id		BIGSERIAL 	NOT NULL
,pid		BIGINT 		NOT NULL DEFAULT 0
,note		VARCHAR 		 DEFAULT NULL



,CONSTRAINT pk_state0num 	PRIMARY KEY ( class_label, obj_label ) 
,CONSTRAINT uk_state0num__id 	UNIQUE(id) 
,CONSTRAINT fk_state0num__obj 	FOREIGN KEY ( class_label, class_type, obj_label ) REFERENCES t_obj(class_label, class_type, obj_label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_state0num__pid 	FOREIGN KEY (pid) REFERENCES t_state0_num(id) MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

,CONSTRAINT fk_state0num__logid	FOREIGN KEY (last_log_id ) REFERENCES t_log_common(log_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);





-----------------------------------------------------------------------------------------------------------------------------
-- таблица состояния ИСТОРИИ ПЕРЕМЕЩЕНИЯ
CREATE TABLE t_log0_qty (

/** общие данные по логированию */
 log_id		BIGINT 		NOT NULL

,src_path	NAME ARRAY				/** ОТКУДА 	*/
,dst_path	NAME ARRAY				/** КУДА 	*/
,qty		NUMERIC 	NOT NULL CHECK (qty>=0)	/** СКОЛЬКО 	*/
,note		VARCHAR DEFAULT NULL			/** ПРИМЕЧАНИЕ 	*/

,CONSTRAINT pk_logqty0	PRIMARY KEY ( log_id ) 

,CONSTRAINT fk_logqty0__logid 	FOREIGN KEY ( log_id ) REFERENCES t_log_common(log_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


) INHERITS (t_log_base);



--,CONSTRAINT ck_log0__qty	CHECK ( class_type=3 OR ( (class_type=2 OR class_type=1) AND qty=1 ) )
--,CONSTRAINT ck_logXXX__qty	CHECK ( class_type=1 )

-----------------------------------------------------------------------------------------------------------------------------
-- таблица содеоржащая всеколичественные объекты
CREATE TABLE t_state0_qty (
 class_type	SMALLINT	NOT NULL DEFAULT 1 CHECK ( class_type>1 )
,class_label	NAME 		NOT NULL 
,obj_label	NAME 		NOT NULL

,last_log_id	BIGINT 		NOT NULL 

,id		BIGSERIAL	NOT NULL
,objnum_pid	BIGINT 		NOT NULL DEFAULT 0
,note		VARCHAR 		 DEFAULT NULL
,qty		NUMERIC 	NOT NULL CHECK (qty>=0)	

,CONSTRAINT pk_state0qty	PRIMARY KEY ( class_label, obj_label, objnum_pid ) 
,CONSTRAINT uk_state0qty__id 	UNIQUE(id) 
,CONSTRAINT fk_state0qty__obj 	FOREIGN KEY ( class_label, class_type, obj_label ) REFERENCES t_obj(class_label, class_type, obj_label) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_state0qty__objnum_pid FOREIGN KEY (objnum_pid) REFERENCES t_state0_num(id) MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT


,CONSTRAINT fk_state0qty__logid FOREIGN KEY ( last_log_id ) REFERENCES t_log_common(log_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);








-----------------------------------------------------------------------------------------------------------------------------
-- таблица состояния ИСТОРИИ ПЕРЕМЕЩЕНИЯ
CREATE TABLE t_log_1 (

/** общие данные по логированию */
 log_id		BIGINT 	NOT NULL

,"Примечание"	VARCHAR DEFAULT NULL

,CONSTRAINT pk_log1		PRIMARY KEY ( log_id ) 
,CONSTRAINT fk_log1__logid 	FOREIGN KEY ( log_id ) REFERENCES t_log_common(log_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

) INHERITS (t_log_base);

GRANT SELECT ON TABLE 		t_log_1  	TO "Guest";
GRANT INSERT,DELETE ON TABLE 	t_log_1 	TO "User";

-----------------------------------------------------------------------------------------------------------------------------
-- таблица состояния ДЕЙСТВИЯ ПЕРЕМЕЩЕНИЯ
CREATE TABLE t_state_1 (
class_type	SMALLINT	NOT NULL DEFAULT 1 CHECK ( class_type=1 )
,class_label	NAME 		NOT NULL 
,obj_label	NAME 		NOT NULL

,last_log_id		BIGINT NOT NULL 
,"Примечание"		VARCHAR DEFAULT NULL

,CONSTRAINT pk_state1 		PRIMARY KEY ( class_label, obj_label ) 
,CONSTRAINT fk_state1__obj 	FOREIGN KEY ( class_label, class_type, obj_label ) REFERENCES t_obj ( class_label, class_type, obj_label ) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_state1__logid 	FOREIGN KEY ( last_log_id ) REFERENCES t_log_common( log_id ) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

) INHERITS (t_state_base);

GRANT INSERT,DELETE,UPDATE 			ON TABLE t_state_1 TO "User";
REVOKE UPDATE (class_type,class_label,obj_label)ON TABLE t_state_1 FROM "User" ;
GRANT SELECT ON TABLE 			t_state_1  	TO "Guest";





PRINT '';
PRINT '-- создаём действия ';
INSERT INTO t_act(label)VALUES('примечание');
INSERT INTO t_act(label)VALUES('проверка');
INSERT INTO t_act(label)VALUES('ремонт');




/*
GRANT SELECT ON TABLE t_obj  			TO "Guest";

SELECT setval('t_obj_id_seq', 100); 
GRANT USAGE ON TABLE t_obj_id_seq	TO "User";

GRANT SELECT 			ON TABLE t_obj 		TO "Guest";


CREATE INDEX idx_obj__pid ON t_state_0("obj_pid") ;

INSERT INTO t_obj(id, obj_label, class_label, class_type, qty)  VALUES (0,'Object0','Object', 1, 1);
*/