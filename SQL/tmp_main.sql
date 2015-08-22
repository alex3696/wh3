
SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;

DROP TABLE IF EXISTS t_cls 		CASCADE;
DROP TABLE IF EXISTS t_clsnum 		CASCADE;
DROP TABLE IF EXISTS t_clsqty 		CASCADE;

DROP TABLE IF EXISTS t_act 		CASCADE;
DROP TABLE IF EXISTS t_prop 		CASCADE;

DROP TABLE IF EXISTS t_cls_prop 	CASCADE;

DROP TABLE IF EXISTS t_ref_class_act 	CASCADE;
DROP TABLE IF EXISTS t_ref_act_prop 	CASCADE;


DROP TABLE IF EXISTS t_objqtykey        CASCADE;
DROP TABLE IF EXISTS t_objnum 		CASCADE;
DROP TABLE IF EXISTS t_objqty		CASCADE;
DROP TABLE IF EXISTS t_log 		CASCADE;
DROP TABLE IF EXISTS t_logqty		CASCADE;

DROP TABLE IF EXISTS t_state 		CASCADE;


DROP TABLE IF EXISTS t_access_act 	CASCADE;
DROP TABLE IF EXISTS t_access_slot 	CASCADE;

DROP TABLE IF EXISTS t_favorite_prop 	CASCADE;
DROP TABLE IF EXISTS t_favorite_obj 	CASCADE;
DROP TABLE IF EXISTS t_favorite_objnum 	CASCADE;
DROP TABLE IF EXISTS t_favorite_objqty 	CASCADE;



DROP VIEW IF EXISTS w_obj CASCADE;



DROP SEQUENCE IF EXISTS seq_log_id 	CASCADE;
DROP SEQUENCE IF EXISTS seq_obj_id 	CASCADE;


DROP TABLE IF EXISTS t_rule 		CASCADE;
DROP TABLE IF EXISTS t_slot	CASCADE;
DROP TABLE IF EXISTS t_perm	CASCADE;
DROP VIEW IF EXISTS moverule_lockup CASCADE;



DROP FUNCTION IF EXISTS fget_cls_parent_array(name) CASCADE;
DROP FUNCTION IF EXISTS fget_cls_pathinfo_table(text) CASCADE;
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(bigint) CASCADE;
DROP FUNCTION IF EXISTS get_path_array(bigint) CASCADE;
DROP FUNCTION IF EXISTS isnumber(text) CASCADE;
DROP FUNCTION IF EXISTS move_obj(name[], name[], bigint, numeric, text) CASCADE;

DROP FUNCTION IF EXISTS ftr_bi_class() CASCADE;
DROP FUNCTION IF EXISTS ftr_ai_class() CASCADE;
DROP FUNCTION IF EXISTS ftr_bu_class() CASCADE;
DROP FUNCTION IF EXISTS ftr_bd_class() CASCADE;


DROP FUNCTION IF EXISTS ftr_biu_ref_class_act() CASCADE;
DROP FUNCTION IF EXISTS ftr_aiu_act_ref_prop() CASCADE;

DROP FUNCTION IF EXISTS ftr_aiu_state() CASCADE;

DROP FUNCTION IF EXISTS ftr_bi_obj() CASCADE;

DROP FUNCTION IF EXISTS ftr_bu_act_prop() CASCADE;


DROP FUNCTION IF EXISTS ftr_bu_prop() CASCADE;


DROP FUNCTION IF EXISTS fget_cls_childs(integer);
DROP FUNCTION IF EXISTS fget_cls_pathinfo_table(integer);
DROP FUNCTION IF EXISTS fget_get_oid_path(bigint);
DROP FUNCTION IF EXISTS fn_append_prop(bigint);
DROP FUNCTION IF EXISTS fn_array1_to_table(anyarray);
DROP FUNCTION IF EXISTS fn_array2_to_table(anyarray);
DROP FUNCTION IF EXISTS fn_clear_prop(bigint);
DROP FUNCTION IF EXISTS fn_create_log_table(bigint);
DROP FUNCTION IF EXISTS fn_create_state_table(bigint);

DROP FUNCTION IF EXISTS fn_delete_objqty(bigint, bigint);
DROP FUNCTION IF EXISTS fn_get_oid_path_table(bigint);
DROP FUNCTION IF EXISTS fn_insert_objqty(integer, name, bigint, numeric);
DROP FUNCTION IF EXISTS fn_oidarray_to_path(anyarray);
DROP FUNCTION IF EXISTS fn_update_objqty(bigint, bigint, name, bigint, numeric);
DROP FUNCTION IF EXISTS move_num_object(anyarray, anyarray);

-----------------------------------------------------------------------------------------------------------------------------

-- таблица содеоржащая иерархическую структуру наследования КЛАССОВ 
CREATE TABLE t_cls ( 
    id           SERIAL   NOT NULL           -- идентификатор записи
    ,vid         SERIAL   NOT NULL           CHECK (vid>=0)  -- идентификатор версии
    ,label       NAME     NOT NULL                          -- имя класса
    ,type        SMALLINT NOT NULL DEFAULT 1 --(class_type=0[abstract] class_type=1[numbered] OR class_type=2[quantity]),
    ,pid         INTEGER  NOT NULL DEFAULT 1
    ,description TEXT              DEFAULT NULL
    ,measurename TEXT              DEFAULT 'ед.'

    ,default_pid BIGINT   NOT NULL DEFAULT 1 -- местоположение объектов по умолчанию


,CONSTRAINT pk_cls_label      PRIMARY KEY ( label )
,CONSTRAINT uk_class_id       UNIQUE ( id )
,CONSTRAINT uk_class_lbl_type UNIQUE ( label, type )
,CONSTRAINT uk_class_id_type  UNIQUE ( id, type )


,CONSTRAINT fk_class__parent FOREIGN KEY ( pid )
    REFERENCES                    t_cls( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

,CONSTRAINT ck_cls_root CHECK (  (id=0 AND pid=0) -- main root
                               OR(id=1 AND pid=0) -- Object0
                               OR(id>1 AND pid>0 AND id<>pid)   )  -- один корень
,CONSTRAINT ck_class_mess    CHECK (  
       ( type=0            AND measurename IS NULL ) --если нет единиц измерения класс должен быть абстрактным
    OR ( type=1            AND measurename = 'ед.' ) --если номерной класс
    OR ((type=2 OR type=3) AND measurename <> 'ед.' )--если количественный класс
    )

);
GRANT SELECT ON TABLE t_cls  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_cls 	TO "TypeDesigner";
GRANT USAGE ON TABLE t_cls_id_seq		TO "TypeDesigner"; 		-- Позволяет читать,инкрементировать текущее значение очереди
SELECT setval('t_cls_id_seq', 100); 

CREATE INDEX idx_t_cls_pid ON t_cls USING btree (pid);

INSERT INTO t_cls(id,label,pid,description)   VALUES (0,'root',0,'main root');
INSERT INTO t_cls(id,label,pid,description)   VALUES (1,'Object',0,'main virtual root');
--INSERT INTO t_cls(id,label,pid,description)   VALUES (2,'WH3 DataBase',1,'this database consist DB settings don`t remove');




-----------------------------------------------------------------------------------------------------------------------------
-- классы нумерные
CREATE TABLE t_clsnum
(
    cls_id   INTEGER NOT NULL
   ,cls_type SMALLINT NOT NULL DEFAULT 1 CHECK (cls_type=1)

,CONSTRAINT pk_clsnum_id   PRIMARY KEY ( cls_id )

,CONSTRAINT fk_clsnum__cls FOREIGN KEY ( cls_id, cls_type )
    REFERENCES                    t_cls( id, type )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);

INSERT INTO t_clsnum(cls_id)   VALUES (0); --root
INSERT INTO t_clsnum(cls_id)   VALUES (1); --object

-----------------------------------------------------------------------------------------------------------------------------
-- классы количественные
CREATE TABLE t_clsqty
(
    cls_id   INTEGER NOT NULL
   ,cls_type SMALLINT NOT NULL DEFAULT 2 CHECK ( (cls_type>1 AND cls_id>3) OR (cls_id<=3) )

,CONSTRAINT pk_clsqty_id   PRIMARY KEY ( cls_id )

,CONSTRAINT fk_clsnum__cls FOREIGN KEY ( cls_id, cls_type )
    REFERENCES                    t_cls( id, type )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);


INSERT INTO t_clsqty(cls_id,cls_type)   VALUES (0,1); --root
INSERT INTO t_clsqty(cls_id,cls_type)   VALUES (1,1); --object


-----------------------------------------------------------------------------------------------------------------------------
-- основная таблица действий 
/**
insert Создание ДЕЙСТВИЯ - нет действий
update Изменение ДЕЙСТВИЯ	- каскадное изменение записей d t_ref_act_prop(act_label) и t_ref_class_act(act_label) 
delete Удаление ДЕЙСТВИЯ 	- каскадное удаление записей из из t_ref_act_prop и t_ref_class_act
*/
CREATE TABLE t_act (
    id           SERIAL NOT NULL
    ,label       NAME   NOT NULL
    ,description TEXT            DEFAULT NULL
    ,color       NAME            DEFAULT NULL
    ,script      TEXT            DEFAULT NULL
    ,vid         SERIAL NOT NULL              -- версия записи    

,CONSTRAINT pk_act__label PRIMARY KEY ( label )
,CONSTRAINT uk_act__id    UNIQUE ( id )
,CONSTRAINT uk_act__vid   UNIQUE ( vid )

);

INSERT INTO t_act (id, label) VALUES (0, 'Move');

GRANT SELECT ON TABLE               t_act TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_act TO "TypeDesigner";

--INSERT INTO t_act(label,description)VALUES('Перемещение','встроенное действие по перемещению объёктов');
-----------------------------------------------------------------------------------------------------------------------------
-- основная описания свойств(переменных????) действий
/** insert Создание свойств - нет действий */
/** update Изменение свойств	- каскадное изменение prop_label переименование колонок в таблицах состояния t_act_state_XXX и истории t_act_log_XXX
/** delete Удаление свойств 	- каскадное удаление записей из t_ref_act_prop */
CREATE TABLE t_prop ( 
    id     SERIAL   NOT NULL
    ,label NAME     NOT NULL
    ,type  SMALLINT NOT NULL DEFAULT 0 -- 0=text, 1=num, 2=date, 3=link, 4=file 5=array

,CONSTRAINT pk_prop__label PRIMARY KEY ( label )
,CONSTRAINT uk_prop__id    UNIQUE ( id )
    
);

GRANT SELECT ON TABLE t_prop  			TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_prop 	TO "TypeDesigner";

-----------------------------------------------------------------------------------------------------------------------------
-- линки М:М , для действий и свойств
/**
INSERT || UPDATE прямое изменение prop_label/act_label   добавление отсутствующие столбцы в t_act_state_XXX t_act_log_XXX
/** delete - удаление свойства из действия - ничего не делаем */
CREATE TABLE t_ref_act_prop ( 
    id       SERIAL  NOT NULL
    ,act_id  INTEGER NOT NULL REFERENCES  t_act(id)  MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE 
    ,prop_id INTEGER NOT NULL REFERENCES  t_prop(id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE 

,CONSTRAINT pk_act_ref_prop     PRIMARY KEY (act_id,prop_id) 
,CONSTRAINT uk_ref_act_prop__id UNIQUE ( id )
);
GRANT SELECT               ON TABLE t_ref_act_prop TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_ref_act_prop TO "TypeDesigner";

DROP VIEW IF EXISTS w_ref_act_prop;
CREATE OR REPLACE VIEW w_ref_act_prop AS 
 SELECT t_ref_act_prop.id, t_act.id AS act_id, t_act.label AS act_label, t_prop.id AS prop_id, t_prop.label AS prop_label
   FROM t_ref_act_prop
   LEFT JOIN t_act  ON t_act.id  = t_ref_act_prop.act_id
   LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id;
GRANT SELECT ON TABLE w_ref_act_prop TO "Guest";

-----------------------------------------------------------------------------------------------------------------------------
-- табличка связи классов с действиями
/**
INSERT - добавление дествия к классу 	- создаём таблички при их отсутствии t_act_state_XXX(act_label) t_act_log_XXX(act_label)
					- добавление отсутствующие столбцы в t_act_state_XXX t_act_log_XXX
UPDATE	- каскадное изменение act_label	- равнозначно добавлению другого действия этому классу

/**delete*/

CREATE TABLE t_ref_class_act ( 
    id        SERIAL   NOT NULL
    ,cls_id   INTEGER  NOT NULL
    ,act_id   INTEGER  NOT NULL REFERENCES t_act(id)   MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE 

,CONSTRAINT pk_ref_class_act      PRIMARY KEY (cls_id, act_id)
,CONSTRAINT uk_ref_class_act__id  UNIQUE ( id )

,CONSTRAINT uk_ref_class_act__cls FOREIGN KEY ( cls_id )
    REFERENCES                    t_clsnum( cls_id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);
GRANT SELECT               ON TABLE t_ref_class_act TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_ref_class_act TO "TypeDesigner";


DROP VIEW IF EXISTS w_ref_class_act;
CREATE OR REPLACE VIEW w_ref_class_act AS 
    SELECT 
    t_ref_class_act.id,
    t_cls.id AS cls_id, 
    t_cls.label AS t_cls,
    t_act.id AS act_id, 
    t_act.label AS act_label
   FROM t_ref_class_act
   LEFT JOIN t_act  ON t_act.id  = t_ref_class_act.act_id
   LEFT JOIN t_cls ON t_cls.id = t_ref_class_act.cls_id;
GRANT SELECT ON TABLE w_ref_class_act TO "Guest";

-----------------------------------------------------------------------------------------------------------------------------
-- таблица свойств классов
-----------------------------------------------------------------------------------------------------------------------------

CREATE TABLE t_cls_prop ( 
    id           SERIAL NOT NULL
    ,cls_id  INTEGER NOT NULL REFERENCES  t_cls(id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
    ,prop_id INTEGER NOT NULL REFERENCES  t_prop(id)  MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
    ,val         TEXT

,CONSTRAINT pk_cls_prop      PRIMARY KEY(cls_id,prop_id)
,CONSTRAINT uk_cls_prop__id UNIQUE ( id )

);
GRANT SELECT               ON TABLE t_cls_prop TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_cls_prop TO "ObjDesigner";
-----------------------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS w_class_prop ;
CREATE OR REPLACE VIEW w_class_prop AS 
SELECT  t_cls_prop.id,
        cls_label,
        t_prop.id AS prop_id,
        t_prop.type AS prop_type,
        prop_label, 
        val
  FROM t_cls_prop 
  LEFT JOIN t_prop ON t_prop.label = t_cls_prop.prop_label;

  GRANT SELECT ON TABLE w_class_prop TO "Guest";


---------------------------------------------------------------------------------------------------
-- порядковый номер объекта
---------------------------------------------------------------------------------------------------
CREATE SEQUENCE seq_obj_id
    INCREMENT 1
    MINVALUE 0
    NO MAXVALUE
    START 100;
-- Позволяет получать идентификаторы для новых объектов
GRANT USAGE ON TABLE seq_obj_id TO "ObjDesigner"; 

---------------------------------------------------------------------------------------------------
-- порядковый номер действия
---------------------------------------------------------------------------------------------------
CREATE SEQUENCE seq_log_id
    INCREMENT 1
    MINVALUE 0
    NO MAXVALUE
    START 100;
-- Позволяет получать идентификаторы для новых логов
GRANT USAGE ON TABLE seq_log_id TO "User";


---------------------------------------------------------------------------------------------------
-- таблица содеоржащая иерархическую структуру ОБЪЕКТОВ их состояние, местоположение и количество
CREATE TABLE t_objnum (
     cls_id      INTEGER
    ,obj_label   NAME     NOT NULL
    ,last_log_id BIGINT                                          CHECK (last_log_id>=0)
    ,pid         BIGINT   NOT NULL DEFAULT 1 
    ,id          BIGINT   NOT NULL DEFAULT nextval('seq_obj_id') 


,CONSTRAINT pk_objnum        PRIMARY KEY(obj_label, cls_id) 

,CONSTRAINT uk_objnum_id_cls_id     UNIQUE (id,cls_id)

,CONSTRAINT uk_objnum_id     UNIQUE (id)
,CONSTRAINT uk_objnum_log_id UNIQUE (last_log_id) 

,CONSTRAINT fk_objnum_pid      FOREIGN KEY (pid)
    REFERENCES                 t_objnum    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_objnum_clsnum   FOREIGN KEY (cls_id)
    REFERENCES                 t_clsnum    (cls_id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT ck_objnum_one_root CHECK (  (id=0 AND pid=0) -- main root
                                      OR(id=1 AND pid=0) -- Object0
                                      OR(id>1 AND pid>0 AND id<>pid)   )  -- один корень

);
GRANT SELECT                  ON TABLE t_objnum TO "Guest";
GRANT UPDATE(pid,last_log_id) ON TABLE t_objnum TO "User";
GRANT INSERT,DELETE,UPDATE    ON TABLE t_objnum TO "ObjDesigner";

CREATE INDEX idx_t_objnum_pid ON t_objnum USING btree("pid") ;
--CREATE INDEX idx_t_objnum_id_more_one ON t_objnum(id) WHERE id>1;


INSERT INTO t_objnum(id,pid,cls_id,obj_label)  VALUES (0,0,0,'root');
INSERT INTO t_objnum(id,pid,cls_id,obj_label)  VALUES (1,0,1,'Object0');

ALTER TABLE t_cls
  ADD CONSTRAINT fk_class__default_pid FOREIGN KEY (default_pid)
      REFERENCES                          t_objnum (id) 
      MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT;




---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ ЭЛЕМЕНТЫ
---------------------------------------------------------------------------------------------------

CREATE TABLE t_objqtykey
(
    id BIGINT  DEFAULT nextval('seq_obj_id'::regclass)
    ,cls_id    INTEGER NOT NULL
    ,obj_label NAME    NOT NULL

,CONSTRAINT uk_objqtykey_id     UNIQUE(id)
,CONSTRAINT pk_objqtykey        PRIMARY KEY(obj_label, cls_id)

,CONSTRAINT fk_objqtykey_cls_id FOREIGN KEY (cls_id)
    REFERENCES                 t_clsqty    (cls_id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
    
);


CREATE TABLE t_objqty (
    objqty_id    BIGINT
    ,last_log_id BIGINT            DEFAULT NULL CHECK (last_log_id>=0)
    ,pid         BIGINT   NOT NULL DEFAULT 1 
    ,qty         NUMERIC  NOT NULL              CHECK (qty>=0)

,CONSTRAINT pk_obj_items        PRIMARY KEY(objqty_id, pid)

,CONSTRAINT fk_obj_items_pid    FOREIGN KEY (pid)
    REFERENCES                  t_objnum    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
    
,CONSTRAINT fk_objqty_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                 t_objqtykey  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_t_objqty_pid ON t_objqty USING btree("pid") ;
CREATE INDEX idx_t_objqty_id ON t_objqty USING btree("objqty_id") ;

---------------------------------------------------------------------------------------------------
-- базовая таблица состояний
---------------------------------------------------------------------------------------------------
CREATE TABLE t_state(
    obj_id BIGINT
    --, cls_id INTEGER
    
);


CREATE TABLE t_log (
     log_id       BIGINT     NOT NULL --DEFAULT nextval('seq_log_id'::regclass)
    ,logtime     TIMESTAMPTZ NOT NULL DEFAULT now()
    ,loguser     NAME        NOT NULL DEFAULT CURRENT_USER  
    ,cls_id      INTEGER     NOT NULL 

    --,note        TEXT                 DEFAULT NULL
    ,src_path    INTEGER ARRAY
    ,dst_path    INTEGER ARRAY

);




-----------------------------------------------------------------------------------------------------------------------------
-- таблица содеоржащая историю изменения состояниz и свойств ОБЪЕКТОВ 
CREATE TABLE t_logqty (
    old_obj_pid  INTEGER NOT NULL
    ,new_obj_pid INTEGER NOT NULL
    ,objqty_id   BIGINT  NOT NULL 
    ,qty         NUMERIC NOT NULL CHECK (qty>0)

,CONSTRAINT pk_logqty_id   PRIMARY KEY(log_id)

,CONSTRAINT fk_logqty_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                 t_objqtykey  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


,CONSTRAINT fk_logqty_user FOREIGN KEY (loguser)
    REFERENCES                 wh_role (rolname)                   
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION




)INHERITS (t_log);







-----------------------------------------------------------------------------------------------------------------------------
-- таблица правил 
-- можно/нельзя[permtype] ложить объект "mov_obj" В слот "mov_cls" объекта "dst_obj" типа "dst_cls"
-----------------------------------------------------------------------------------------------------------------------------
CREATE TABLE t_access_slot
(
    id               BIGSERIAL NOT NULL CHECK (id>0)
    ,access_group     NAME     NOT NULL -- группа для которой разрешимо данное правило
    ,access_disabled SMALLINT  NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
    ,script_restrict TEXT               DEFAULT NULL

    ,dst_cls_id      INTEGER   NOT NULL -- владелец слота
    ,dst_obj_id      INTEGER            DEFAULT NULL
    ,dst_path        TEXT               DEFAULT NULL

    ,mov_cls_id      INTEGER   NOT NULL -- тип слота
    ,mov_obj_id      INTEGER            DEFAULT NULL
    ,src_path        TEXT               DEFAULT NULL

    ,src_min      NUMERIC      NOT NULL DEFAULT 0
    ,dst_max      NUMERIC               DEFAULT NULL

,CONSTRAINT pk_access_slot              PRIMARY KEY ( id ) 

,CONSTRAINT fk_access_slot__acess_group FOREIGN KEY (access_group) 
    REFERENCES                              wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_access_dst_cls           FOREIGN KEY (dst_cls_id)
    REFERENCES                               t_clsnum(cls_id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_access_mov_cls           FOREIGN KEY (mov_cls_id)
    REFERENCES                               t_cls(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

--,CONSTRAINT fk_rule_src_class_ref    FOREIGN KEY (src_cls)
--    REFERENCES                            t_cls(label)
--    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE


);
GRANT SELECT ON TABLE t_access_slot               TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_access_slot TO "TypeDesigner";

CREATE INDEX ON t_access_slot(access_group);

CREATE INDEX ON t_access_slot USING btree(mov_cls_id) ;

CREATE INDEX ON t_access_slot(dst_path text_pattern_ops);
CREATE INDEX ON t_access_slot(src_path text_pattern_ops);


---------------------------------------------------------------------------------------------------
-- табличка разрешений для связи классов с действиями
---------------------------------------------------------------------------------------------------
CREATE TABLE t_access_act ( 
    id               BIGSERIAL NOT NULL UNIQUE
    ,access_group    NAME      NOT NULL 
    ,access_disabled SMALLINT  NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
    ,script_restrict TEXT               DEFAULT NULL

    ,act_id          INTEGER   NOT NULL 
    ,cls_id          INTEGER   NOT NULL 

    ,obj_id          BIGINT             DEFAULT NULL
    ,src_path        TEXT               DEFAULT NULL -- текущее местоположение тип+имя объекта{{act_cls,act_obj}src_path}

,CONSTRAINT pk_access_act              PRIMARY KEY ( id ) 

-- внешний ключ на действия
,CONSTRAINT fk_access_act_cls FOREIGN KEY (cls_id, act_id) 
    REFERENCES              t_ref_class_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

-- внешний ключ на группы которым разрешено/запрещено
,CONSTRAINT fk_access_act_role FOREIGN KEY (access_group)
    REFERENCES                     wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

-- внешний ключ на объекты
,CONSTRAINT fk_access_act_objnum FOREIGN KEY (obj_id)
    REFERENCES                     t_objnum (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

);
CREATE INDEX idx_t_access_act__access_group ON t_access_act USING btree (access_group);
CREATE INDEX idx_t_access_act__access_group ON t_access_act USING btree (cls_id);



GRANT SELECT ON TABLE t_access_act               TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_access_act TO "TypeDesigner";




-----------------------------------------------------------------------------------------------------------------------------
-- таблица избранных свойст объектов отображаемых при просмотре пользователем каталога объектов по типу или по местоположению
-----------------------------------------------------------------------------------------------------------------------------
CREATE TABLE t_favorite_prop(
    id           BIGSERIAL NOT NULL UNIQUE
    ,user_label  NAME      NOT NULL DEFAULT CURRENT_USER 
    ,cls_id      INTEGER       NOT NULL
    ,act_id      INTEGER   NOT NULL
    ,prop_id     INTEGER   NOT NULL

,CONSTRAINT pk_favorite_prop               PRIMARY KEY (user_label, cls_id, act_id, prop_id )
,CONSTRAINT uk_favorite_prop__id           UNIQUE ( id ) 

,CONSTRAINT fk_favorite_prop__user_label   FOREIGN KEY (user_label) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favorite_prop__ref_act_prop FOREIGN KEY (act_id, prop_id) 
    REFERENCES                           t_ref_act_prop (act_id, prop_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favorite_prop__ref_class_act FOREIGN KEY (cls_id, act_id) 
    REFERENCES                           t_ref_class_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);
GRANT SELECT ON TABLE t_favorite_prop               TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorite_prop TO "TypeDesigner";

---------------------------------------------------------------------------------------------------
-- таблица содержащая иерархическую структуру избранного пользователей 
---------------------------------------------------------------------------------------------------
CREATE TABLE t_favorite_obj ( 
     id           BIGSERIAL NOT NULL 
    ,pid          INTEGER   NOT NULL DEFAULT 1 
    ,user_label   NAME      NOT NULL DEFAULT CURRENT_USER 
    ,expanded     BOOLEAN   NOT NULL DEFAULT TRUE
    ,custom_label NAME               DEFAULT NULL
    ,is_objnum    BOOLEAN   NOT NULL DEFAULT TRUE

,CONSTRAINT pk_favorite_obj       PRIMARY KEY (id)
,CONSTRAINT uk_favorite_obj__node UNIQUE ( id, is_objnum ) -- один id не может быть в obnum и objid 

,CONSTRAINT fk_favorite_obj__pid         FOREIGN KEY (pid) 
    REFERENCES                 t_favorite_obj (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favorite_prop__user_label FOREIGN KEY (user_label) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT  ck_favorites_root           CHECK ( 
    (  ( id>0 AND pid>0 ) AND   id<>pid) 
    OR ( id=0 AND pid=0 ) OR  ( id=1 AND pid=0) )-- один корень

 );
---------------------------------------------------------------------------------------------------
/*
-- детализация избранного ( избранные НОМЕРНЫЕ объекты каталог ПО ТИПУ/МЕСТОПОЛЕЖЕНИЮ )
CREATE TABLE t_favorite_objnum ( 
    id           BIGINT  NOT NULL
     ,is_objnum  BOOLEAN NOT NULL DEFAULT TRUE CHECK (is_objnum IS TRUE)

    ,cls_label NAME    NOT NULL
    ,obj_label   NAME    NOT NULL
    ,is_type_lnk BOOLEAN NOT NULL DEFAULT TRUE -- ПО ТИПУ/МЕСТОПОЛЕЖЕНИЮ
    

,CONSTRAINT fk_favorite_objnum__objnum FOREIGN KEY (cls_label, obj_label) 
    REFERENCES                            t_objnum (cls_label, obj_label) 
    MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_favorite_objnum__node   FOREIGN KEY (id, is_objnum) 
    REFERENCES                      t_favorite_obj (id, is_objnum) 
    MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE
    
);    
---------------------------------------------------------------------------------------------------
-- детализация избранного ( избранные КОЛИЧЕСТВЕННЫЕ объекты ПО ТИПУ/МЕСТОПОЛЕЖЕНИЮ )
---------------------------------------------------------------------------------------------------
CREATE TABLE t_favorite_objqty (
    id           BIGINT  NOT NULL
    ,is_objnum   BOOLEAN NOT NULL DEFAULT TRUE CHECK (is_objnum IS FALSE)

    ,cls_label NAME    NOT NULL
    ,obj_label   NAME    NOT NULL
    ,obj_pid     BIGINT           DEFAULT NULL -- МЕСТОПОЛЕЖЕНИЮ

,CONSTRAINT fk_favorite_objnum__objqty FOREIGN KEY (cls_label, obj_label) 
    REFERENCES                     t_obj (cls_label, obj_label) 
    MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_favorite_objnum__node   FOREIGN KEY (id, is_objnum) 
    REFERENCES                      t_favorite_obj (id, is_objnum) 
    MATCH SIMPLE  ON UPDATE CASCADE ON DELETE CASCADE
);


 
GRANT SELECT ON TABLE t_favorite_obj                  TO "Guest";
GRANT SELECT ON TABLE t_favorite_objnum               TO "Guest";
GRANT SELECT ON TABLE t_favorite_objqty               TO "Guest";

GRANT INSERT,DELETE,UPDATE ON TABLE t_favorite_obj    TO "User";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorite_objnum TO "User";
GRANT INSERT,DELETE,UPDATE ON TABLE t_favorite_objqty TO "User";

GRANT USAGE ON TABLE t_favorite_obj_id_seq            TO "User"; 
SELECT setval('t_favorite_obj_id_seq', 100); 

INSERT INTO t_favorite_obj(id,pid,custom_label)   VALUES (0,0,'root');
INSERT INTO t_favorite_obj(id,pid,custom_label)   VALUES (1,0,'virtual_root'); -- favorites_id=1

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
DROP FUNCTION IF EXISTS ftr_bi_class() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bi_class()  RETURNS trigger AS
$body$
DECLARE
BEGIN
-- подставляем единицу измерения по умолчанию
    CASE NEW.type
    WHEN 0 THEN NEW.measurename=NULL;
    WHEN 1 THEN NEW.measurename='ед.';
    WHEN 2 THEN NEW.measurename='шт.';
    ELSE END CASE;       
-- проверяем, что новый класс не является дочерним абстрактного
    PERFORM * FROM t_cls WHERE NEW.pid=id AND type>0 AND pid>1;
    IF FOUND THEN
        RAISE EXCEPTION '%: Can`t create child to non abstract class ',TG_NAME;
    END IF;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bi_class AFTER INSERT ON t_cls FOR EACH ROW EXECUTE PROCEDURE ftr_bi_class();
-----------------------------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------
-- функция создания таблицы состояния для нумерованного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_create_state_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_create_state_table(IN _id BIGINT)
    RETURNS VOID AS
$body$	
DECLARE
    new_tablename    NAME;
    creating_script VARCHAR;
BEGIN
    SELECT table_name INTO new_tablename -- проверяем наличие таблички
        FROM information_schema.tables WHERE table_name = 't_state_'||_id;
    IF FOUND THEN
        RAISE EXCEPTION 'fn_create_state_table: Таблица "%" уже сужествует',new_tablename;
    END IF;

    creating_script := 'CREATE TABLE t_state_'||_id||'(
                              CONSTRAINT pk_state_'||_id||' PRIMARY KEY( obj_id )
                             ,CONSTRAINT fk_state_'||_id||' FOREIGN KEY (obj_id )
                                 REFERENCES              t_objnum (id)
                                 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
                        )INHERITS (t_state);
                        GRANT SELECT ON TABLE t_state_'       ||_id||' TO "Guest"; 
                        GRANT INSERT,DELETE ON TABLE t_state_'||_id||' TO "User"; ';
    --RAISE DEBUG 'fn_create_log_table: %',_creating_script;
    EXECUTE creating_script;
END;
$body$
LANGUAGE 'plpgsql';

COMMENT ON FUNCTION fn_create_state_table(IN _id BIGINT) 
    IS '-- функция создания таблицы состояния для нумерованного объекта';
---------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------
-- функция создания таблицы ИСТОРИИ для нумерованного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_create_log_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_create_log_table(IN _id BIGINT)
    RETURNS VOID AS
$body$	
DECLARE
    new_tablename   NAME;
    _creating_script VARCHAR;
BEGIN
    SELECT table_name INTO new_tablename -- проверяем наличие таблички
        FROM information_schema.tables WHERE table_name = 't_state_'||_id;
    IF FOUND THEN
        RAISE EXCEPTION 'fn_create_log_table: Таблица "%" уже сужествует',new_tablename;
    END IF;
    _creating_script := 'CREATE TABLE t_log_'||_id||'(
                         act_id      INTEGER NOT NULL
                         ,obj_id     BIGINT  NOT NULL
                         ,old_obj_pid INTEGER NOT NULL
                         ,new_obj_pid INTEGER NOT NULL
                         ,CONSTRAINT pk_lognum_'||_id||' PRIMARY KEY ( log_id )
                         ,CONSTRAINT fk_lognum_'||_id||' FOREIGN KEY (obj_id, cls_id)
                                                 REFERENCES t_objnum (id, cls_id)
                                                 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
                         ,CONSTRAINT fk_logusr_'||_id||' FOREIGN KEY (loguser) 
                                                 REFERENCES wh_role (rolname)
                                                 MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
                         ,CONSTRAINT fk_logact_'||_id||' FOREIGN KEY (act_id) 
                                                 REFERENCES t_act(id)
                                                 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

                         )INHERITS (t_log);
                        GRANT SELECT ON TABLE t_log_'       ||_id||' TO "Guest"; 
                        GRANT INSERT,DELETE ON TABLE t_log_'||_id||' TO "User"; ';
    --RAISE DEBUG 'fn_create_log_table: %',_creating_script;
    EXECUTE _creating_script;
END;
$body$
LANGUAGE 'plpgsql';

COMMENT ON FUNCTION fn_create_log_table(IN _id BIGINT) 
    IS '-- функция создания таблицы ИСТОРИИ для нумерованного объекта';
---------------------------------------------------------------------------------------------------

--SELECT fn_create_log_table(555);


/**
INSERT - добавление к классу - создаём таблички при их отсутствии t_state_XXX(act_label) t_log_XXX(act_label)
  добавление отсутствующие столбцы в t_state_XXX t_log_XXX
*/
DROP FUNCTION IF EXISTS ftr_ai_class() CASCADE;
CREATE OR REPLACE FUNCTION ftr_ai_class()  RETURNS trigger AS
$body$
DECLARE
    log_tablename NAME;
BEGIN
    CASE 
    WHEN NEW.type=1 THEN
        INSERT INTO t_clsnum(cls_id, cls_type)VALUES(NEW.id,NEW.type);
    WHEN NEW.type=2 OR NEW.type=3 THEN
        INSERT INTO t_clsqty(cls_id, cls_type)VALUES(NEW.id,NEW.type);
    ELSE 
    END CASE;

    IF NEW.type=1 THEN
        PERFORM fn_create_log_table(NEW.id);
        PERFORM fn_create_state_table(NEW.id);
    END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_ai_class AFTER INSERT ON t_cls FOR EACH ROW EXECUTE PROCEDURE ftr_ai_class();




-----------------------------------------------------------------------------------------------------------------------------
--
DROP FUNCTION IF EXISTS ftr_bu_class() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bu_class()  RETURNS trigger AS
$body$
DECLARE
BEGIN
    RAISE DEBUG '%: создать тригер обновления класса с обновлением имён в истории ',TG_NAME;
    --t_lognum_, t_logqty_ + t_perm
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bu_class BEFORE UPDATE ON t_cls FOR EACH ROW EXECUTE PROCEDURE ftr_bu_class();



-----------------------------------------------------------------------------------------------------------------------------
--
DROP FUNCTION IF EXISTS ftr_bd_class() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bd_class()  RETURNS trigger AS
$body$
DECLARE
BEGIN
    IF OLD.type=1 THEN
        EXECUTE 'DROP TABLE IF EXISTS t_log_'||OLD.id||' CASCADE';
        EXECUTE 'DROP TABLE IF EXISTS t_state_'||OLD.id||' CASCADE';
    END IF;
RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bd_class AFTER DELETE ON t_cls FOR EACH ROW EXECUTE PROCEDURE ftr_bd_class();


---------------------------------------------------------------------------------------------------
-- функция удаления ненужных столбцов из таблицы состояния (в истории не удаляютсья)
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_clear_prop (IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_clear_prop(IN _id BIGINT)
    RETURNS VOID AS
$body$
DECLARE
    rec        RECORD;
    col_name   NAME;
    tbl_name   NAME;
    
    -- ищем все свойства которые надо удалить, т.е. те что удалены или изменены в действии
    del_prop CURSOR (_cls_id INTEGER, tbl NAME)  IS 
        SELECT column_name FROM information_schema.columns -- выбрать все столбцы таблицы 
          WHERE table_name=tbl||'_'||_cls_id
        EXCEPT ALL                                         -- исключая те столбцы, 
        ( SELECT distinct t_prop.label FROM t_ref_act_prop -- которые есть в действиях класса
            LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id 
            WHERE act_id IN 
              (SELECT act_id FROM t_ref_class_act WHERE _cls_id=_cls_id  )
          UNION ALL SELECT column_name FROM information_schema.columns WHERE table_name=tbl
        );
BEGIN
    tbl_name := quote_ident('t_state'||'_'||_id);
    
    FOR rec IN del_prop(_id,'t_state') LOOP
        col_name := quote_ident(rec.column_name);
        RAISE DEBUG 'fn_clear_prop: DEL COLUMN % FROM TABLE %',col_name,tbl_name;
        EXECUTE 'ALTER TABLE '||tbl_name||' DROP COLUMN IF EXISTS '||col_name;
    END LOOP;

END;
$body$
LANGUAGE 'plpgsql';
COMMENT ON FUNCTION fn_clear_prop(IN _id BIGINT) 
    IS '-- функция удаления ненужных столбцов из таблицы состояния (в истории не удаляютсья)';
/**
ALTER TABLE t_state_105 ADD COLUMN q1 TEXT;
ALTER TABLE t_state_105 ADD COLUMN q2 TEXT;
ALTER TABLE t_state_105 ADD COLUMN q3 TEXT;
SELECT fn_clear_prop(105);
 */
---------------------------------------------------------------------------------------------------
-- функция добавлени нужных столбцов в таблицы состояния и истории
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_append_prop (IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_append_prop(IN _id BIGINT)
    RETURNS VOID AS
$body$
DECLARE
    rec        RECORD;
    col_name   NAME;
    tbl_name   NAME;

    -- ищем все свойства всех действий этого класса которые надо добавлять
    new_prop CURSOR (_cls_id INTEGER, tbl NAME)  IS 
        SELECT distinct t_prop.label AS prop_label FROM t_ref_act_prop             -- которые есть в действиях класса
          LEFT JOIN t_prop ON t_prop.id = t_ref_act_prop.prop_id 
          WHERE act_id IN 
            (SELECT act_id FROM t_ref_class_act WHERE cls_id = _cls_id )
        EXCEPT ALL -- исключая те столбцы что уже есть
        ( SELECT column_name FROM information_schema.columns 
            WHERE table_name=tbl||'_'||_cls_id
          EXCEPT ALL SELECT column_name FROM information_schema.columns WHERE table_name=tbl
        );

BEGIN
    tbl_name := quote_ident('t_state'||'_'||_id);
    
    FOR rec IN new_prop(_id,'t_state') LOOP
        col_name := quote_ident(rec.prop_label);
        RAISE DEBUG 'fn_append_prop: ADD COLUMN % TO TABLE %',col_name,tbl_name;
        EXECUTE 'ALTER TABLE '||tbl_name||' ADD COLUMN '||col_name||' TEXT;';
    END LOOP;

    tbl_name := quote_ident('t_log'||'_'||_id);

    FOR rec IN new_prop(_id,'t_log') LOOP
        col_name := quote_ident(rec.prop_label);
        RAISE DEBUG 'fn_append_prop: ADD COLUMN % TO TABLE %',col_name,tbl_name;
        EXECUTE 'ALTER TABLE '||tbl_name||' ADD COLUMN '||col_name||' TEXT;';
    END LOOP;

END;
$body$
LANGUAGE 'plpgsql';

COMMENT ON FUNCTION fn_append_prop(IN _id BIGINT) 
    IS '-- функция добавлени нужных столбцов в таблицы состояния и истории';
---------------------------------------------------------------------------------------------------




-----------------------------------------------------------------------------------------------------------------------------
/** update Изменение свойств	- каскадное изменение prop_label 
переименование колонок в таблицах состояния t_state_XXX и истории t_log_XXX */
DROP FUNCTION IF EXISTS ftr_au_prop() CASCADE;
CREATE OR REPLACE FUNCTION ftr_au_prop()  RETURNS trigger AS
$body$
DECLARE
    -- ищем все классы в котором есть свойство 
    cursor_of_id CURSOR IS 
        SELECT DISTINCT(t_ref_class_act.cls_id) AS class_id
        FROM t_prop
        RIGHT JOIN t_ref_act_prop  ON t_prop.id              = t_ref_act_prop.prop_id
        RIGHT JOIN t_ref_class_act ON t_ref_class_act.act_id = t_ref_act_prop.act_id
        WHERE t_ref_act_prop.prop_id=OLD.id;

    changed        RECORD;
BEGIN
    IF OLD.label<>NEW.label THEN

        RAISE DEBUG 'tr_bu_act_prop: ИЗМЕНЕНО СВОЙСТВО % --> % ',OLD.label,NEW.label;
        
        FOR changed IN cursor_of_id 
        LOOP
            RAISE DEBUG 'tr_bu_act_prop: ИЗМЕНЕНО ПОЛЕ % --> % в таблицах "%" состояния/истории ',
                OLD.label, NEW.label, changed.class_id;
            EXECUTE 'ALTER TABLE  t_log_'||changed.class_id||' RENAME COLUMN '
                ||quote_ident(OLD.label)||' TO '||quote_ident(NEW.label);
            EXECUTE 'ALTER TABLE  t_state_'||changed.class_id||' RENAME COLUMN '
                ||quote_ident(OLD.label)||' TO '||quote_ident(NEW.label);
    END LOOP;
    END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_au_prop AFTER UPDATE ON t_prop FOR EACH ROW EXECUTE PROCEDURE ftr_au_prop();





DROP FUNCTION IF EXISTS ftr_biu_access_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_biu_access_act()  RETURNS trigger AS
$body$
DECLARE

BEGIN
    CASE TG_OP
    WHEN 'INSERT' THEN -- проверяем наличие 
        PERFORM * FROM t_ref_class_act WHERE cls_id = NEW.cls_id AND act_id = NEW.act_id;
        -- если не нашлось, то добавляем 
        IF NOT FOUND THEN
            INSERT INTO t_ref_class_act(cls_id, act_id) VALUES (NEW.cls_id, NEW.act_id);
        END IF;
    WHEN 'UPDATE' THEN
        IF NEW.cls_id<>OLD.cls_id OR NEW.act_id<>old.act_id THEN
            UPDATE t_ref_class_act SET cls_id=NEW.cls_id, act_id=NEW.act_id WHERE 
                cls_id=OLD.cls_id AND act_id=OLD.act_id;
        END IF;                
    ELSE
    END CASE;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_biu_access_act BEFORE INSERT OR UPDATE ON t_access_act FOR EACH ROW EXECUTE PROCEDURE ftr_biu_access_act();
-----------------------------------------------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS ftr_ad_access_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_ad_access_act()  RETURNS trigger AS
$body$
DECLARE

BEGIN


    DELETE FROM t_ref_class_act WHERE (act_id, cls_id) NOT IN (
                     SELECT act_id, cls_id FROM t_access_act GROUP BY act_id, cls_id
                      );
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_ad_access_act AFTER DELETE ON t_access_act FOR EACH ROW EXECUTE PROCEDURE ftr_ad_access_act();



-----------------------------------------------------------------------------------------------------------------------------
/**
AFTER INSERT || UPDATE прямое изменение prop_label/act_label   добавление отсутствующие столбцы в t_state_XXX t_log_XXX
*/
DROP FUNCTION IF EXISTS ftr_aiu_act_ref_prop() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aiu_act_ref_prop()  RETURNS trigger AS
$body$
DECLARE

    -- находим все классы, в которых есть действие изменённое или вставленное
    cursor_changed_class CURSOR ( _act_id INTEGER ) IS
        SELECT cls_id FROM t_ref_class_act 
        WHERE act_id = _act_id ;

BEGIN
    CASE TG_OP
    WHEN 'INSERT' THEN
        RAISE DEBUG '%: В ДЕЙСТВИИ % ДОБАВЛЕНО СВОЙСТВО % ',TG_NAME,NEW.act_id,NEW.prop_id;
    WHEN 'UPDATE' THEN
        RAISE DEBUG '%: В ДЕЙСТВИИ % ОБНОВЛЕНО СВОЙСТВО % ',TG_NAME,NEW.act_id,NEW.prop_id;
    WHEN 'DELETE' THEN
        RAISE DEBUG '%: В ДЕЙСТВИИ % УДАЛЕНО СВОЙСТВО % ',TG_NAME,OLD.act_id,OLD.prop_id;
    ELSE
        RAISE DEBUG '%: неизвестная операция ',TG_NAME;
    END CASE;

    -- добавление нового действия - добавление столбцов связанных с NEW.act_label 

    IF TG_OP='UPDATE' OR TG_OP='DELETE' THEN
        FOR changed IN cursor_changed_class(OLD.act_id) LOOP
            PERFORM fn_clear_prop(changed.cls_id);
        END LOOP;
    END IF;
    
    IF TG_OP='UPDATE' OR TG_OP='INSERT' THEN
        FOR changed IN cursor_changed_class(NEW.act_id) LOOP
            PERFORM fn_append_prop(changed.cls_id);
        END LOOP;
    END IF;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tg_aiu_act_ref_prop AFTER INSERT OR UPDATE OR DELETE 
   ON t_ref_act_prop FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_act_ref_prop();




-----------------------------------------------------------------------------------------------------------------------------
/**
INSERT - добавление дествия к классу 	- создаём таблички при их отсутствии t_state_XXX(act_label) t_log_XXX(act_label)
					- добавление отсутствующие столбцы в t_state_XXX t_log_XXX

UPDATE	- каскадное изменение act_label	- равнозначно добавлению другого действия этому классу
*/
DROP FUNCTION IF EXISTS ftr_aiu_ref_class_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aiu_ref_class_act()  RETURNS trigger AS
$body$
DECLARE
    class_id INTEGER;
BEGIN

    IF TG_OP='UPDATE' OR TG_OP='DELETE' THEN
        PERFORM fn_clear_prop(OLD.cls_id);
    END IF;
    
    IF TG_OP='UPDATE' OR TG_OP='INSERT' THEN
        PERFORM fn_append_prop(NEW.cls_id);
    END IF;

    
    

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
-----------------------------------------------------------------------------------------------------------------------------
CREATE TRIGGER tr_aiu_ref_class_act 
    AFTER INSERT OR UPDATE OR DELETE ON t_ref_class_act 
    FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_ref_class_act();
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------


---------------------------------------------------------------------------------------------------
-- функция создания количественного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_insert_objqty(IN _cls_id INTEGER, IN _label NAME, IN _pid BIGINT, IN _qty NUMERIC) CASCADE;
CREATE OR REPLACE FUNCTION fn_insert_objqty(IN _cls_id INTEGER, IN _label NAME, IN _pid BIGINT, IN _qty NUMERIC)
    RETURNS TABLE(id_ BIGINT, pid_ BIGINT, obj_label_ name, qty_ NUMERIC) AS
$body$
DECLARE
    
    v_objqty_id BIGINT;
BEGIN
    SELECT id INTO v_objqty_id FROM t_objqtykey WHERE cls_id = _cls_id AND obj_label = _label;
    IF NOT FOUND THEN
        INSERT INTO t_objqtykey(cls_id, obj_label ) 
               VALUES ( _cls_id, _label ) RETURNING id INTO v_objqty_id;
    END IF;

    INSERT INTO t_objqty(objqty_id, pid, qty) VALUES (v_objqty_id, _pid, _qty);
    id_:=v_objqty_id;
    pid_:=_pid;
    obj_label_:=_label;
    qty_=_qty;
    return next;
    RETURN ;
END;
$body$
LANGUAGE 'plpgsql';
GRANT EXECUTE ON FUNCTION fn_insert_objqty(INTEGER, NAME, BIGINT, NUMERIC ) TO "User";
COMMENT ON FUNCTION fn_insert_objqty(INTEGER, NAME, BIGINT, NUMERIC )
    IS '-- функция создания количественного объекта';


---------------------------------------------------------------------------------------------------
-- функция редактирования количественного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_update_objqty( IN _objqty_id BIGINT, IN old_pid BIGINT,
                                          IN new_label NAME, IN new_pid BIGINT, IN new_qty NUMERIC) CASCADE;
                                          
CREATE OR REPLACE FUNCTION fn_update_objqty( IN _objqty_id BIGINT, IN old_pid BIGINT,
                                          IN new_label NAME, IN new_pid BIGINT, IN new_qty NUMERIC)
    RETURNS VOID 
    AS 
$body$
--DECLARE
BEGIN
    PERFORM id FROM t_objqtykey WHERE id = _objqty_id;
    IF FOUND THEN
        UPDATE t_objqtykey SET obj_label=new_label WHERE id=_objqty_id;
        UPDATE t_objqty    SET pid= new_pid, qty=new_qty  WHERE objqty_id=_objqty_id AND pid=old_pid;
    END IF;
END;
$body$
LANGUAGE 'plpgsql';
GRANT EXECUTE ON FUNCTION fn_update_objqty(BIGINT, BIGINT, NAME, BIGINT, NUMERIC ) TO "User";
COMMENT ON FUNCTION fn_update_objqty(BIGINT, BIGINT, NAME, BIGINT, NUMERIC )
    IS '-- функция редактирования количественного объекта';


---------------------------------------------------------------------------------------------------
-- функция удаления количественного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_delete_objqty(IN _objqty_id BIGINT, IN _pid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_delete_objqty(IN _objqty_id BIGINT, IN _pid BIGINT)
    RETURNS VOID AS
$body$
DECLARE
    
    v_objqty_id BIGINT;
BEGIN
    DELETE FROM t_objqty WHERE objqty_id=_objqty_id AND pid = _pid;

    DELETE FROM t_objqtykey 
        WHERE (id) NOT IN (SELECT objqty_id FROM t_objqty GROUP BY objqty_id);
END;
$body$
LANGUAGE 'plpgsql';
GRANT EXECUTE ON FUNCTION fn_delete_objqty(BIGINT, BIGINT ) TO "User";
COMMENT ON FUNCTION fn_delete_objqty(BIGINT, BIGINT )
    IS '-- функция удаления количественного объекта';





-----------------------------------------------------------------------------------------------------------------------------
/** динамический тригер сохраняет данные соответствующие лействию BEFORE INSERT OR UPDATE ON t_state_XX */

DROP FUNCTION IF EXISTS ftr_aiu_state() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aiu_state()  RETURNS trigger AS
$body$
DECLARE

	-- находим свойста данного действия
	cursor_act_prop 	CURSOR  IS 
					SELECT prop_label 
						FROM t_ref_act_prop
					WHERE 	act_label=NEW.act_label;


	-- ищем все свойства данного класса, которые не участвуют в действии
	cursor_cleaned_prop 	CURSOR (_cls_label NAME)  IS 
					SELECT prop_label 
						FROM t_ref_act_prop
					WHERE act_label IN (SELECT DISTINCT(act_label) FROM t_ref_class_act WHERE cls_label = _cls_label)
				EXCEPT ALL 
					SELECT prop_label 
						FROM t_ref_act_prop
					WHERE 	act_label=NEW.act_label;


					
	
	
	perm_group	NAME;

	insert_script_1 TEXT;
	insert_script_2 TEXT;

	_rec_act_prop	RECORD;
	_new_prop_val	TEXT;
	_old_prop_val	TEXT;

	tmp_rec RECORD;

	obj 		RECORD;
	class_id 	BIGINT;

	vlog_id 	BIGINT;

BEGIN
	IF TG_OP='UPDATE' AND NEW.obj_id<>OLD.obj_id THEN
		RAISE EXCEPTION '%: Нельзя менять идентификатор объекта',TG_NAME;
	END IF;	

	RAISE DEBUG '%: Ищем идентификатор класса % ',TG_NAME,TG_TABLE_NAME;
	SELECT CAST( trim(leading 't_state_' from TG_TABLE_NAME) AS INTEGER) INTO class_id;
	IF NOT FOUND THEN
		RAISE EXCEPTION '%: Идентификатор класса не найден в %',TG_NAME,TG_TABLE_NAME;
	END IF;		

	RAISE DEBUG '%: Ищем и имя и тип класса для obj_id=%',TG_NAME,NEW.obj_id;	
	SELECT * INTO obj FROM t_objnum WHERE id=NEW.obj_id ; --AND t_state.class_type=1
	IF NOT FOUND THEN
		RAISE EXCEPTION '%: Идентификатор объекта % не найден в t_objnum',TG_NAME,NEW.obj_id;
	END IF;	
	

	RAISE DEBUG '%: Ищем разрешение выполнения "%" в классе "%" пользоватем "%" ',TG_NAME,NEW.act_label,obj.cls_label,CURRENT_USER;	
	SELECT t_ref_class_act_perm.perm_group INTO perm_group 
		FROM t_ref_class_act_perm
		LEFT JOIN 	wh_role _group -- определяем разрешённые группы
			ON	_group.rolname=t_ref_class_act_perm.perm_group	AND not _group.rolcanlogin 
		RIGHT JOIN	wh_auth_members membership -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп-пользователей
			ON	_group.id=membership.roleid	
		RIGHT JOIN 	wh_role _user  	-- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей		
			ON	_user.id=membership.member AND _user.rolcanlogin
			AND	_user.rolname=CURRENT_USER	-- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО
		WHERE t_ref_class_act_perm.cls_label=cls_label AND t_ref_class_act_perm.act_label=NEW.act_label;
	IF NOT FOUND THEN
		RAISE EXCEPTION '%: Действие "%" не определено/не разрешено для %',TG_NAME,NEW.act_label,obj.cls_label;
	END IF;

	--инициализируем переменные чтоб небыло NULL || 'данные' = NULL
	insert_script_1:='';
	insert_script_2:='';

	FOR _rec_act_prop IN cursor_act_prop LOOP
		insert_script_1:=insert_script_1||','||quote_ident(_rec_act_prop.prop_label);
		EXECUTE 'SELECT ('||quote_literal(NEW)||'::'||TG_RELID::regclass||').'|| quote_ident(_rec_act_prop.prop_label) INTO _new_prop_val;

		IF _new_prop_val IS NOT NULL THEN
			insert_script_2:=insert_script_2||','||quote_literal(_new_prop_val);
		ELSE
			insert_script_2:=insert_script_2||',NULL';
		END IF;
			
		RAISE DEBUG '%: Лог свойства %=% ',TG_NAME,_rec_act_prop.prop_label,_new_prop_val;	

	END LOOP;

	RAISE DEBUG '%: Проверка изменения свойств не участвующик в действии',TG_NAME;

	FOR _rec_act_prop IN cursor_cleaned_prop(obj.cls_label) LOOP
		EXECUTE 'SELECT ('||quote_literal(NEW)||'::'||TG_RELID::regclass||').'|| quote_ident(_rec_act_prop.prop_label) INTO _new_prop_val;
		
		IF TG_OP='INSERT' AND _new_prop_val IS NOT NULL THEN
			RAISE EXCEPTION '%: Нельзя менять % в действии %',TG_NAME,_rec_act_prop.prop_label,NEW.act_label;	
		END IF;
		
		IF TG_OP='UPDATE' THEN
			EXECUTE 'SELECT ('||quote_literal(OLD)||'::'||TG_RELID::regclass||').'|| quote_ident(_rec_act_prop.prop_label) INTO _old_prop_val;
			IF _new_prop_val<>_old_prop_val THEN
				RAISE EXCEPTION '%: Нельзя менять свойство "%" в действии "%"',TG_NAME,_rec_act_prop.prop_label,NEW.act_label;	
			END IF;
		END IF;
	END LOOP;
	
	vlog_id:= nextval('seq_log_id'::regclass);

	RAISE DEBUG '%: Обновляем состояние объекта (%) в t_objnum ',TG_NAME, obj;
	UPDATE t_objnum SET log_id=vlog_id, log_time=CURRENT_TIMESTAMP, log_user=CURRENT_USER	WHERE t_objnum.id=NEW.obj_id;

	insert_script_1:='INSERT INTO t_log_'||class_id||'(log_id, cls_label, obj_label, comment, act_label'
					||insert_script_1;
	insert_script_2:=')VALUES('	   ||vlog_id
					||','||quote_literal(obj.cls_label)
					||','||quote_literal(obj.obj_label)
					||','||(CASE WHEN obj.comment IS NULL THEN 'NULL' ELSE quote_literal(obj.comment) END)
					||','||quote_literal(NEW.act_label)
					||insert_script_2||')';
	RAISE DEBUG '%: Вставляем в лог изменения состояния и свойств объекта: 
				% 
				%'
				,TG_NAME,insert_script_1, insert_script_2;
	EXECUTE  insert_script_1||insert_script_2;

	RAISE DEBUG '%: Обновляем свойства объекта % в t_state_% на NEW %',TG_NAME, obj.id, class_id,  NEW;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
--CREATE TRIGGER tr_aiu_state BEFORE INSERT OR UPDATE ON t_state_XX FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_state();


-----------------------------------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS w_obj;
CREATE OR REPLACE VIEW w_obj AS 
SELECT obj.*, t_cls.label AS cls_label, t_cls.type, t_cls.measurename, t_cls.default_pid AS cls_default_pid
  FROM t_cls
  RIGHT JOIN ( SELECT id AS obj_id, pid AS obj_pid, cls_id , obj_label, last_log_id, 1::NUMERIC AS qty
                 FROM t_objnum   
                 UNION ALL 
                 SELECT objqty_id AS obj_id, pid AS obj_pid, cls_id , obj_label, last_log_id, qty
                 FROM t_objqty
                   LEFT JOIN t_objqtykey ON t_objqtykey.id=t_objqty.objqty_id
              )obj  
                    ON obj.cls_id=t_cls.id;

GRANT SELECT ON TABLE w_obj TO "Guest";

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде путей и таблицы, по идентификация объекта (уникальность)
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fget_objnum_pathinfo_table(IN node_id BIGINT)
 RETURNS TABLE(
    _id           BIGINT
    ,_pid         BIGINT
    ,_obj_label   NAME
    ,_cls_label   NAME
    ,_cls_id      INTEGER
    ,_last_log_id BIGINT
    ,_patharray   NAME[]
    ,pathid       NAME[]
    ,_path        TEXT
    
) AS $BODY$ 
BEGIN
RETURN QUERY 
    WITH RECURSIVE parents AS 
    (SELECT
        t.id, t.pid, t.obj_label, t_cls.label, t.cls_id, t.last_log_id
        ,ARRAY[ ARRAY[t_cls.label,t.obj_label]::NAME[] ]::NAME[] AS path
        ,ARRAY[ ARRAY[t.cls_id,t.id]::NAME[] ]::NAME[] AS pathid
        , '/['||t_cls.label||']'||t.obj_label AS _path
        --, ARRAY[t.id] AS exist, 
        --, FALSE AS cycle,
        FROM t_objnum AS t 
        LEFT JOIN t_cls ON t_cls.id=cls_id
        WHERE t.id = node_id   --[item.pid]
        AND t.id>1
     UNION ALL
     SELECT
        t.id, t.pid, t.obj_label, t_cls.label, t.cls_id, t.last_log_id
        ,p.path || ARRAY[t_cls.label,t.obj_label]::NAME[]
        ,p.pathid || ARRAY[t.cls_id,t.id]::NAME[]
        ,'/['||t_cls.label||']'||t.obj_label || p._path
        --, exist || t.id
        --, t.id = ANY(exist)
        FROM 
        parents AS p, t_objnum AS t 
        LEFT JOIN t_cls ON t_cls.id=t.cls_id
        WHERE 
        t.id = p.pid
        AND t.id>1   
        --AND NOT cycle 
        )
   SELECT * FROM parents;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 100;
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(BIGINT) TO "Admin" WITH GRANT OPTION;

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде 2мерного массива
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_array(IN node_pid BIGINT ) CASCADE;
CREATE OR REPLACE FUNCTION get_path_array(IN node_pid BIGINT ) RETURNS NAME[]  AS $$ 
	SELECT 	
		_patharray
	FROM	
		fget_objnum_pathinfo_table($1)
	WHERE	
		_pid=1
$$ LANGUAGE SQL;

GRANT EXECUTE ON FUNCTION get_path_array(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_array(BIGINT) TO "Admin" WITH GRANT OPTION;


--SELECT * FROM get_path_array(101);  


-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде строки
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path(IN node_pid BIGINT ) CASCADE;
CREATE OR REPLACE FUNCTION get_path(IN node_pid BIGINT ) RETURNS TEXT  AS $$ 
    SELECT _path 
      FROM fget_objnum_pathinfo_table($1) 
      WHERE _pid=1
$$ LANGUAGE SQL;

GRANT EXECUTE ON FUNCTION get_path(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path(BIGINT) TO "Admin" WITH GRANT OPTION;










-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения дочерних типов
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_cls_childs(IN node_id INTEGER) CASCADE;
CREATE OR REPLACE FUNCTION fget_cls_childs(IN node_id INTEGER)
 RETURNS TABLE(	
		_id		INTEGER,
		_vid		INTEGER,
		_label 		NAME,
		_type 		SMALLINT,
		_pid 		INTEGER,
		_description 	TEXT,
		_measurename 	TEXT
		--_path		NAME[]
		--,_cycle		BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY(
    WITH RECURSIVE children AS (
    SELECT id, vid, label, type, pid, description, measurename
           --pid || '/' || label          AS ord,
           ,ARRAY[id]                            AS exist
           ,FALSE                                AS cycle
    FROM t_cls
    WHERE id = node_id
    UNION ALL
        SELECT t.id, t.vid, t.label, t.type, t.pid, t.description, t.measurename
               --ord || '[' || t.pid || ']' || '/' || t.label,
               ,exist || t.id 
               ,t.id = ANY(exist)
        FROM children AS c, 
             t_cls  AS t
        WHERE t.pid = c.id AND 
              NOT cycle AND
              array_length(exist, 1) < 1000
)
SELECT id, vid, label, type, pid, description, measurename 
    FROM children WHERE NOT cycle --ORDER BY ord LIMIT 100;
    );
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_cls_childs(INTEGER) TO "Guest";

--SELECT * FROM fget_cls_childs(102);



-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения информации о наследовании класса
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_cls_pathinfo_table(IN _in_id INTEGER) CASCADE;
CREATE OR REPLACE FUNCTION fget_cls_pathinfo_table(IN _in_id INTEGER)
 RETURNS TABLE(	
		_id		INTEGER,
		_vid		INTEGER,
		_label 		NAME,
		_type 		SMALLINT,
		_pid 		INTEGER,
		_description 	TEXT,
		_measurename 	TEXT,
		_path		NAME[]
		--,_cycle		BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY(
	WITH RECURSIVE parents AS 
		(SELECT	id, vid, label, type, pid, description, measurename
			,ARRAY[t.label] AS path	--,FALSE AS cycle
		FROM 
			t_cls AS t 
		WHERE 
			_in_id IS NOT NULL 
			AND  t.id = _in_id
			AND  t.id > 1
			--AND t.classname<>'Object'
		UNION ALL
		SELECT 	t.id, t.vid, t.label, t.type, t.pid, t.description, t.measurename
			,path || t.label	--,t.classname = ANY(path) 
		FROM 
			parents AS p, t_cls AS t 
		WHERE 
			t.id = p.pid 
			--AND NOT cycle 
			--AND t.classname <> t.classparent 
			--AND t.classname<>'Object'
			AND t.id > 1
		)
	SELECT  * FROM 	parents 
	);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_cls_pathinfo_table(INTEGER) TO "Guest";
--SELECT * FROM fget_cls_pathinfo_table('SmartMonitor(rev 1.0)')
SELECT * FROM fget_cls_pathinfo_table(104);




/**

-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути предков класса
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_cls_parent_array(IN _in_id NAME ) CASCADE;
CREATE OR REPLACE FUNCTION fget_cls_parent_array(IN _in_id NAME ) RETURNS NAME[]  AS $$ 
    SELECT _path FROM fget_cls_pathinfo_table($1) WHERE _pid=1
$$ LANGUAGE SQL;
GRANT EXECUTE ON FUNCTION fget_cls_parent_array(NAME) TO "Guest";

-- SELECT * FROM fget_cls_parent_array('ФЭУ102')
*/



-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования одномерного массива идентификаторов класса в таблицу
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_array1_to_table(IN anyarray);
CREATE OR REPLACE FUNCTION fn_array1_to_table(IN anyarray)
  RETURNS TABLE(idx integer, id anyelement) 
  AS $BODY$ 
  SELECT row, $1[row] from generate_subscripts($1, 1) as row
$BODY$ LANGUAGE sql IMMUTABLE;
--SELECT * FROM fn_array1_to_table('{101,102,103,104}'::int[]);

-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования двумерного массива идентификаторов объектов таблицу
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_array2_to_table(IN anyarray);
CREATE OR REPLACE FUNCTION fn_array2_to_table(IN anyarray)
  RETURNS TABLE(idx integer, col1 anyelement, col2 anyelement) 
  AS $BODY$
  SELECT row, $1[row][1],$1[row][2] from generate_subscripts($1, 1) as row;
--SELECT $1[row][1],$1[row][2]    from generate_series(1,array_length($1,1), 1) as row;
  $BODY$ LANGUAGE sql IMMUTABLE;
--SELECT * FROM fn_array2_to_table('{{101,100},{21,22},{104,102},{41,42}}'::int[]);

-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования одномерного массива объектоа в таблицу-путь
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_oidarray_to_path(IN anyarray);
CREATE OR REPLACE FUNCTION fn_oidarray_to_path(IN idpath anyarray)
  RETURNS NAME []
    AS
$BODY$
DECLARE
    --rec      RECORD;
    result   NAME [];
    get_obj_info CURSOR IS 
    SELECT t_cls.id, t_cls.label, t_objnum.id, obj_label,  arr.id
        FROM fn_array1_to_table(idpath) arr
        LEFT JOIN t_objnum USING (id)
        LEFT JOIN t_cls ON t_cls.id=t_objnum.cls_id 
        ORDER BY idx;
BEGIN
    FOR rec IN get_obj_info LOOP
        result := result || ARRAY[ ARRAY[rec.label,rec.obj_label] ];
    END LOOP;
    RETURN result;
END;
$BODY$
  LANGUAGE plpgsql STABLE ;
--SELECT fn_oidarray_to_path('{101,102,103,104}'::int[]);

-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути идентификаторов объекта
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_get_oid_path_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_get_oid_path_table(IN node_id BIGINT)
    RETURNS 
    TABLE(idx integer, id  BIGINT, pid BIGINT,  pthid  BIGINT[])  
    AS $BODY$ 
    WITH RECURSIVE parents AS 
    (SELECT
        1 AS idx, t.id, t.pid
        ,ARRAY[t.id]:: BIGINT[] AS pathid
        , FALSE AS cycle
        FROM t_objnum AS t 
        WHERE t.id = node_id   --[item.pid]
     UNION ALL
     SELECT
        p.idx+1 AS idx, t.id, t.pid
        ,p.pathid || t.id
        , t.id = ANY(pathid)
        FROM 
        parents AS p, t_objnum AS t 
        WHERE 
        t.id = p.pid
        AND t.id>1   
        AND NOT cycle 
        )
   SELECT p.idx, p.id, p.pid, p.pathid FROM parents p;
$BODY$ LANGUAGE sql STABLE;


--SELECT * FROM fn_get_oid_path_table(103);


-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути предков класса
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_get_oid_path(IN _in_id BIGINT ) CASCADE;
CREATE OR REPLACE FUNCTION fget_get_oid_path(IN _in_id BIGINT ) RETURNS BIGINT[]  AS $BODY$ 
BEGIN
    RETURN (SELECT pthid FROM fn_get_oid_path_table($1) WHERE pid=1);
END    
$BODY$  LANGUAGE plpgsql STABLE COST 100;



--SELECT fn_oidarray_to_path( (SELECT fget_get_oid_path(102)) );




-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
-- вьюшка для просмотра правил
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE VIEW moverule_lockup AS 
SELECT 
    obj.obj_id      AS mov_obj_id
    ,obj.obj_label  AS mov_obj_label
    ,obj.obj_pid    AS mov_obj_pid
    ,obj.cls_id     AS mov_cls_id
    ,obj.cls_label  AS mov_cls_label
    ,obj.type       AS mov_cls_type
    ,obj.qty        AS mov_obj_qty 
    ,perm.src_path  AS src_path

    ,dst.id         AS dst_obj_id
    ,dst.obj_label  AS dst_obj_label
    ,dst.pid        AS dst_obj_pid
    ,dst.cls_id     AS dst_cls_id
    ,perm.dst_path  AS dst_path
    
    ,perm.id              AS perm_id
    ,perm.access_disabled AS perm_access_disabled
    ,perm.script_restrict AS perm_script

    --,(SELECT fn_oidarray_to_path(fget_get_oid_path(obj.obj_pid))) AS CURR_PATH
    --,(SELECT fn_oidarray_to_path(fget_get_oid_path(dst.pid)))     AS DST_PATH

FROM w_obj obj -- откуда + что

RIGHT JOIN t_access_slot perm -- находим все объекты классы которых удовлетворяют правилу (которые можно перемещать)
    ON   obj.cls_id IN (SELECT _id FROM fget_cls_childs(perm.mov_cls_id))
    AND perm.mov_obj_id IS NULL OR obj.obj_id = perm.mov_obj_id -- отсеиваем по имени
    AND (perm.src_path IS NULL OR get_path(obj.obj_pid) LIKE perm.src_path ) -- отсеиваем по местоположению

LEFT JOIN t_objnum dst -- куда
    ON perm.dst_cls_id = dst.cls_id
    AND perm.dst_obj_id IS NULL OR  dst.id = perm.dst_obj_id -- отсеиваем по имени
    AND (perm.dst_path IS NULL OR get_path(dst.pid) LIKE perm.dst_path ) -- отсеиваем по местоположению
    --AND dst.id>0 
    


-- group permission
LEFT JOIN wh_role _group 
    ON perm.access_group=_group.rolname-- определяем ИМЕНА разрешённых групп
RIGHT JOIN    wh_auth_members membership
    ON _group.id=membership.roleid -- определяем ИДЕНТИФИКАТОРЫ разрешённых групп
RIGHT JOIN wh_role _user  
    ON  _user.id=membership.member -- определяем ИДЕНТИФИКАТОРЫ разрешённых пользователей
    AND _user.rolname=CURRENT_USER -- определяем ИМЕНА разрешённых пользователей ВКЛЮЧАЯ ТЕКУЩЕГО

WHERE obj.obj_pid <> dst.id AND dst.id>0 
;

/**


SELECT 
    dst_cls_id, cls.label as dst_cls_label ,
    dst_obj_id,              dst_obj_label
  FROM moverule_lockup 
  LEFT JOIN t_cls cls ON cls.id=dst_cls_id
  WHERE mov_obj_label='001' AND  mov_cls_label='ФЭУ102'
  ORDER BY dst_cls_id

*/

GRANT SELECT ON TABLE moverule_lockup TO "Guest";












------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления КЛАССОВ';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO t_cls(label)   VALUES ('Склад'); --101
INSERT INTO t_cls(label,pid,type,measurename)   VALUES ('ФЭУ',1,0,NULL); --102
INSERT INTO t_cls(label,pid,type,measurename)   VALUES ('Спирт',1,3,'л.'); --103
INSERT INTO t_cls(label,pid)VALUES ('ФЭУ102',102); --104
INSERT INTO t_cls(label,pid)VALUES ('СРК2М',1); --105
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления НОМЕРНЫХ ОБЪЕКТОВ';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO t_objnum(obj_label,cls_id)   VALUES ('уренгой',101); --100
INSERT INTO t_objnum(obj_label,cls_id)   VALUES ('001', 104 /*ФЭУ102*/); -- 101
INSERT INTO t_objnum(obj_label,cls_id)   VALUES ('002',104 /*ФЭУ102*/);
INSERT INTO t_objnum(obj_label,cls_id,pid)   VALUES ('012', 105, (SELECT id FROM t_objnum WHERE obj_label='уренгой') );/*СРК2М*/
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ';
PRINT '';
------------------------------------------------------------------------------------------------------------
--SELECT fn_insert_objqty(103,'поставка №1', 100, 10);
SELECT fn_insert_objqty(103,'поставка №1', 1, 20);
SELECT fn_insert_objqty(103,'поставка №2', 1, 16.334);
SELECT fn_insert_objqty(103,'поставка №3', 100, 1.1);
SELECT fn_insert_objqty(103,'поставка №4', 100, 1.2);
SELECT fn_insert_objqty(103,'поставка №5', 100, 1.3);
SELECT fn_insert_objqty(103,'поставка №6', 100, 1.4);

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления СВОЙСТВ КЛАССА';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO t_prop(label)VALUES('длина(мм)');
INSERT INTO t_prop(label)VALUES('диаметр(мм)');
INSERT INTO t_cls_prop(cls_label, prop_label, val)VALUES('СРК2М','длина(мм)',382);
INSERT INTO t_cls_prop(cls_label, prop_label, val)VALUES('СРК2М','диаметр(мм)',42);




------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления ПРАВИЛ перемещения';
PRINT '';
------------------------------------------------------------------------------------------------------------


INSERT INTO t_access_slot(access_group, access_disabled, dst_cls_id, mov_cls_id)
    VALUES ('User', 0,(SELECT id FROM t_cls WHERE label='СРК2М')
                  , (SELECT id FROM t_cls WHERE label='ФЭУ'));



INSERT INTO t_access_slot(access_group, access_disabled, dst_cls_id, mov_cls_id)
    VALUES ('User', 0, (SELECT id FROM t_cls WHERE label='Object')
                     , (SELECT id FROM t_cls WHERE label='ФЭУ102'));



INSERT INTO t_access_slot(access_group, access_disabled, dst_cls_id, mov_cls_id)
    VALUES ('User', 0, (SELECT id FROM t_cls WHERE label='Склад')
                     , (SELECT id FROM t_cls WHERE label='Спирт'));

INSERT INTO t_access_slot(access_group, access_disabled, dst_cls_id, mov_cls_id)
    VALUES ('User', 0, (SELECT id FROM t_cls WHERE label='Object')
                     , (SELECT id FROM t_cls WHERE label='Спирт'));


------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления ИЗБРАННЫХ ОБЪЕКТОВ/ТИПОВ перемещения';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_1');
INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_2');
INSERT INTO t_favorites(pid,label)   VALUES (1,'folder_3');
INSERT INTO t_favorites(pid,classid)   VALUES (2,103);
INSERT INTO t_favorites(pid,objnumid)   VALUES (3,101);



------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления ДЕЙСТВИЙ';
PRINT '';
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '-- создаём 4 свойства';
INSERT INTO t_prop(label)VALUES('пров1');--=3;
INSERT INTO t_prop(label)VALUES('пров2');--=4;
INSERT INTO t_prop(label)VALUES('рем1');--=5;
INSERT INTO t_prop(label)VALUES('рем2');--=6;
PRINT '';
PRINT '-- создаём 2  действия ';
INSERT INTO t_act(label)VALUES('проверка');--1;
INSERT INTO t_act(label)VALUES('ремонт');--2;
PRINT '';
PRINT '-- добавляем в действие проверки свойства пров1 и пров2';
INSERT INTO t_ref_act_prop(act_id,prop_id)VALUES(1,3);
INSERT INTO t_ref_act_prop(act_id,prop_id)VALUES(1,4);
PRINT '';
PRINT '-- добавляем в класс ФЭУ102 действие проверка - должны появиться в столбца(пров1 и пров2) в t_state_104';
INSERT INTO t_ref_class_act(act_id,cls_id)VALUES(1,101);
PRINT '';
PRINT 'добавляем в класс СРК2М действие ремонт - в t_state_105 без изменений';
INSERT INTO t_ref_class_act(act_id,cls_id)VALUES(2,105);
PRINT '';
PRINT 'добавляем в класс СРК2М действие проверка - должны появиться в столбца(пров1 и пров2) в t_state_105';
INSERT INTO t_ref_class_act(act_id,cls_id)VALUES(1,105);



PRINT '';
PRINT '-- добавляем в действие ремонта свойства рем1 и рем2 в t_state_105  должны появиться в столбца(рем1 и рем2)';
INSERT INTO t_ref_act_prop(act_id,prop_id)VALUES(2,5);
INSERT INTO t_ref_act_prop(act_id,prop_id)VALUES(2,6);
-- переименовываем свойства пров1 и пров2 в t_state_105 и t_state_104   должны переименоваться столбцы пров1->тест1_1, пров2->тест1_2
UPDATE t_prop SET label='тест1_1' WHERE  label='пров1';
UPDATE t_prop SET label='тест1_2' WHERE  label='пров2';




-- FAIL в проверке муже есть тест1_2
UPDATE t_ref_act_prop SET prop_id=4 WHERE  prop_id=3 AND act_id=1 ;-- FAIL

-- перемещаем свойство тест1_2 из проверка в ремонт - никаих изменений не должно быть
UPDATE t_ref_act_prop SET act_id=2 WHERE  prop_id=4 AND act_id=1;



-- должны появиться в столбца(рем1 и рем2 и тест1_2)
INSERT INTO t_ref_class_act(act_id,cls_id)VALUES(2,101);

DELETE FROM t_ref_act_prop WHERE act_id=1 AND prop_id=3;

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки  РАЗРЕШЕНИЙ ДЕЙСТВИЙ';
PRINT '';
------------------------------------------------------------------------------------------------------------



INSERT INTO t_access_act(access_group, access_disabled, script_restrict, 
                         act_id,                        
                         cls_id, 
                         obj_id, src_path)
    VALUES ('User', 0, NULL 
              ,(SELECT id FROM t_act WHERE label='ремонт')
              ,(SELECT id FROM t_cls WHERE label='СРК2М')
              ,NULL, '/[Склад]%' );


INSERT INTO t_access_act(access_group, access_disabled, script_restrict, 
                         act_id,                        
                         cls_id, 
                         obj_id, src_path)
    VALUES ('User', 0, NULL 
              ,(SELECT id FROM t_act WHERE label='проверка')
              ,(SELECT id FROM t_cls WHERE label='СРК2М')
              ,NULL, '/[Склад]%' ); --{101}=='{{Склад,уренгой}}'


DELETE FROM t_ref_class_act WHERE cls_id IN (SELECT id FROM t_cls WHERE label='СРК2М') 
                              AND act_id IN (SELECT id FROM t_act WHERE label='проверка');


------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты вставки/обновления/удаления СОСТОЯНИЙ И ИСТОРИИ';
PRINT '';
------------------------------------------------------------------------------------------------------------




------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты производительности свойств';
PRINT '';
------------------------------------------------------------------------------------------------------------

/*
DECLARE @maxprop_qty,@currprop_qty;
SET @maxprop_qty = 10000;
SET @currprop_qty = @maxprop_qty;

WHILE (@currprop_qty>0)
BEGIN
	INSERT INTO t_prop(label, type)VALUES ('prop_@currprop_qty', 0);
	SET @currprop_qty=@currprop_qty-1;		-- инкрементируем количество папок
END
*/























