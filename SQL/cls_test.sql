

SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;

SELECT whgrant_grouptouser('Admin','postgres');
---------------------------------------------------------------------------------------------------
-- домены
---------------------------------------------------------------------------------------------------
DROP DOMAIN IF EXISTS WHNAME CASCADE; 

CREATE DOMAIN WHNAME AS NAME
   CHECK (VALUE ~ '^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$') ;

---------------------------------------------------------------------------------------------------
-- идентификаторы
---------------------------------------------------------------------------------------------------
DROP SEQUENCE IF EXISTS seq_cls_id CASCADE;
DROP SEQUENCE IF EXISTS seq_act_id CASCADE;
DROP SEQUENCE IF EXISTS seq_prop_id CASCADE;
DROP SEQUENCE IF EXISTS seq_prop_cls_id     CASCADE;
DROP SEQUENCE IF EXISTS seq_ref_cls_act_id  CASCADE;
DROP SEQUENCE IF EXISTS seq_ref_act_prop_id CASCADE;
DROP SEQUENCE IF EXISTS seq_obj_id CASCADE;
DROP SEQUENCE IF EXISTS seq_log_id CASCADE;
DROP SEQUENCE IF EXISTS seq_perm_id CASCADE;

CREATE SEQUENCE seq_cls_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_act_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_prop_id INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_prop_cls_id      INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_ref_cls_act_id   INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_ref_act_prop_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_obj_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_log_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_perm_id INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;

GRANT USAGE ON TABLE seq_cls_id  TO "User";
GRANT USAGE ON TABLE seq_act_id  TO "User";
GRANT USAGE ON TABLE seq_prop_id TO "User";
GRANT USAGE ON TABLE seq_prop_cls_id     TO "User";
GRANT USAGE ON TABLE seq_ref_cls_act_id  TO "User";
GRANT USAGE ON TABLE seq_ref_act_prop_id TO "User";
GRANT USAGE ON TABLE seq_obj_id  TO "User";
GRANT USAGE ON TABLE seq_log_id  TO "User";
GRANT USAGE ON TABLE seq_perm_id TO "User";


---------------------------------------------------------------------------------------------------
-- таблицы
---------------------------------------------------------------------------------------------------
-- классы
DROP TABLE IF EXISTS cls_name CASCADE;
DROP TABLE IF EXISTS cls_tree CASCADE;
DROP TABLE IF EXISTS cls_real CASCADE;
DROP TABLE IF EXISTS cls_qtyi  CASCADE;
DROP TABLE IF EXISTS cls_qtyf  CASCADE;
DROP TABLE IF EXISTS cls_num   CASCADE;
DROP TABLE IF EXISTS cls CASCADE;
DROP TABLE IF EXISTS cls_abstr CASCADE;

DROP VIEW IF EXISTS cls CASCADE;
DROP FUNCTION IF EXISTS ftg_ins_cls() CASCADE;
DROP FUNCTION IF EXISTS ftg_del_cls() CASCADE;
DROP FUNCTION IF EXISTS ftg_upd_cls() CASCADE;




DROP TABLE IF EXISTS log_act CASCADE;
DROP TABLE IF EXISTS log_move_num CASCADE;
DROP TABLE IF EXISTS log_move_qtyi CASCADE;
DROP TABLE IF EXISTS log_move_qtyf CASCADE;




DROP TABLE IF EXISTS perm CASCADE;
DROP TABLE IF EXISTS perm_act CASCADE;
DROP TABLE IF EXISTS perm_move CASCADE;
DROP TABLE IF EXISTS prop_kind CASCADE;
DROP TABLE IF EXISTS prop CASCADE;
DROP TABLE IF EXISTS prop_cls CASCADE;
DROP TABLE IF EXISTS prop_num CASCADE;
DROP TABLE IF EXISTS prop_qtyf CASCADE;
DROP TABLE IF EXISTS prop_qtyi CASCADE;
DROP TABLE IF EXISTS act CASCADE;
DROP TABLE IF EXISTS ref_cls_act CASCADE;
DROP TABLE IF EXISTS ref_act_prop CASCADE;
DROP TABLE IF EXISTS obj CASCADE;
DROP TABLE IF EXISTS obj_num CASCADE;
DROP TABLE IF EXISTS obj_names_qtyi CASCADE;
DROP TABLE IF EXISTS obj_names_qtyf CASCADE;
DROP TABLE IF EXISTS obj_details_qty CASCADE;
DROP TABLE IF EXISTS obj_details_qtyi CASCADE;
DROP TABLE IF EXISTS obj_details_qtyf CASCADE;
DROP TABLE IF EXISTS log CASCADE;
DROP TABLE IF EXISTS log_act_detail CASCADE;
DROP TABLE IF EXISTS lock_src CASCADE;
DROP TABLE IF EXISTS lock_dst CASCADE;
DROP TABLE IF EXISTS lock_dstact CASCADE;
DROP TABLE IF EXISTS favorite_prop CASCADE;

---------------------------------------------------------------------------------------------------
-- функции
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_array1_to_table(anyarray);
DROP FUNCTION IF EXISTS get_path_array(bigint);
DROP FUNCTION IF EXISTS get_path(bigint);






---------------------------------------------------------------------------------------------------
-- перечень всех классов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_name CASCADE;
CREATE TABLE cls_name ( 
  id     BIGINT NOT NULL DEFAULT nextval('seq_cls_id')
    PRIMARY KEY
  ,title WHNAME NOT NULL
    UNIQUE 
  ,note  TEXT            DEFAULT NULL
  ,kind           SMALLINT NOT NULL CHECK ( kind BETWEEN 0 AND 3 )
  ,CONSTRAINT uk_cls_name_id_kind UNIQUE (id,kind)
);
---------------------------------------------------------------------------------------------------
-- дерево абстрактных классов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_tree CASCADE;
CREATE TABLE cls_tree ( 
  id  BIGINT NOT NULL CHECK (  (id=0 AND pid=0) OR(id>0 AND id<>pid)   )  -- один корень 
    PRIMARY KEY
    REFERENCES cls_name( id )       MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,pid BIGINT NOT NULL DEFAULT 1
    REFERENCES cls_tree( id )       MATCH FULL ON UPDATE RESTRICT ON DELETE SET DEFAULT

 ,kind           SMALLINT NOT NULL DEFAULT 0 CHECK ( kind=0 )

 ,CONSTRAINT cls_tree_id_kind_fkey FOREIGN KEY (id,kind)
      REFERENCES cls_name (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE CASCADE
);
CREATE INDEX idx_cls_tree_pid ON cls_tree(pid);

---------------------------------------------------------------------------------------------------
-- классы имеющие объекты
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_real CASCADE;
CREATE TABLE cls_real ( 
  id             BIGINT NOT NULL CHECK ( id>0 AND id<>pid )
    PRIMARY KEY
    REFERENCES cls_name( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,pid            BIGINT NOT NULL DEFAULT 1
    REFERENCES cls_tree( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,default_objid BIGINT NOT NULL DEFAULT 1
 ,measure        WHNAME NOT NULL

 ,kind           SMALLINT NOT NULL CHECK ( kind BETWEEN 1 AND 3 )

 ,CONSTRAINT cls_real_id_kind_fkey FOREIGN KEY (id,kind)
      REFERENCES cls_name (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE CASCADE
 
);
CREATE INDEX idx_clsreal_pid ON cls_real(pid);
---------------------------------------------------------------------------------------------------
-- классы нумерные
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_num   CASCADE;
CREATE TABLE cls_num ( 
  id  BIGINT NOT NULL 
    PRIMARY KEY
    REFERENCES cls_real( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,kind           SMALLINT NOT NULL DEFAULT 1 CHECK ( kind=1 )

 ,CONSTRAINT cls_num_id_kind_fkey FOREIGN KEY (id,kind)
      REFERENCES cls_name (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE CASCADE
);
---------------------------------------------------------------------------------------------------
-- классы количественные
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_qtyi  CASCADE;
CREATE TABLE cls_qtyi ( 
  id  BIGINT NOT NULL 
    PRIMARY KEY
    REFERENCES cls_real( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,kind           SMALLINT NOT NULL DEFAULT 2 CHECK ( kind=2 )

 ,CONSTRAINT cls_qtyi_id_kind_fkey FOREIGN KEY (id,kind)
      REFERENCES cls_name (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE CASCADE
);
---------------------------------------------------------------------------------------------------
-- классы количественные дробные
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS cls_qtyf  CASCADE;
CREATE TABLE cls_qtyf ( 
  id  BIGINT NOT NULL 
    PRIMARY KEY
    REFERENCES cls_real( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE
 ,kind           SMALLINT NOT NULL DEFAULT 3 CHECK ( kind=3 )

 ,CONSTRAINT cls_qtyf_id_kind_fkey FOREIGN KEY (id,kind)
      REFERENCES cls_name (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE CASCADE
);


---------------------------------------------------------------------------------------------------
-- основная типов свойств
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop_kind CASCADE;
CREATE TABLE prop_kind( 
 id    BIGINT NOT NULL
,title WHNAME NOT NULL
,CONSTRAINT pk_propkind__id    PRIMARY KEY ( id )
);
INSERT INTO prop_kind (id, title) VALUES (0, 'text')
                                        ,(1, 'numberic')
                                        ,(2, 'date')
                                        ,(3, 'link')
                                        ,(4, 'file')
                                        ,(5, 'array');
---------------------------------------------------------------------------------------------------
-- основная описания свойств(переменных) действий
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop CASCADE;
CREATE TABLE prop ( 
 id    BIGINT   NOT NULL DEFAULT nextval('seq_prop_id')
,title WHNAME   NOT NULL
,kind  SMALLINT NOT NULL DEFAULT 0 
,CONSTRAINT pk_prop__id    PRIMARY KEY ( id )
,CONSTRAINT uk_prop__title UNIQUE ( title )
,CONSTRAINT fk_prop__kind   FOREIGN KEY ( kind )
    REFERENCES                 prop_kind( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
);
---------------------------------------------------------------------------------------------------
-- таблица свойств классов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop_cls CASCADE;
CREATE TABLE prop_cls ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id  INTEGER NOT NULL
,prop_id INTEGER NOT NULL
,val     TEXT
,CONSTRAINT pk_propcls               PRIMARY KEY ( id )
,CONSTRAINT uk_propcls__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propcls__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_propcls__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_real( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);


---------------------------------------------------------------------------------------------------
-- основная таблица действий 
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS act CASCADE;
CREATE TABLE act (
 id     BIGINT NOT NULL DEFAULT nextval('seq_act_id')
,title  WHNAME NOT NULL
,note   TEXT            DEFAULT NULL
,color  NAME            DEFAULT NULL
,script TEXT            DEFAULT NULL
,CONSTRAINT pk_act__id    PRIMARY KEY ( id  )
,CONSTRAINT uk_act__title UNIQUE ( title )
);

---------------------------------------------------------------------------------------------------
-- табличка связи классов с действиями
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS ref_cls_act CASCADE;
CREATE TABLE ref_cls_act ( 
    id        BIGINT   NOT NULL DEFAULT nextval('seq_ref_cls_act_id')
    ,cls_id   INTEGER  NOT NULL
    ,act_id   INTEGER  NOT NULL

,CONSTRAINT pk_refclsact__id    PRIMARY KEY ( id )
,CONSTRAINT uk_refclsact_clsid_actid UNIQUE (cls_id, act_id)
,CONSTRAINT fk_refclsact__clsid FOREIGN KEY (cls_id )
    REFERENCES                      cls_num (    id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_refclsact__actid FOREIGN KEY ( act_id )
    REFERENCES                           act( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);
---------------------------------------------------------------------------------------------------
-- линки М:М , для действий и свойств
---------------------------------------------------------------------------------------------------
CREATE TABLE ref_act_prop ( 
    id       BIGINT  NOT NULL DEFAULT nextval('seq_ref_act_prop_id')
    ,act_id  INTEGER NOT NULL
    ,prop_id INTEGER NOT NULL

,CONSTRAINT pk_refactprop__id          PRIMARY KEY ( id )
,CONSTRAINT uk_refactprop__actid_propid UNIQUE( act_id,prop_id) 
,CONSTRAINT fk_refactprop__actid  FOREIGN KEY ( act_id )
    REFERENCES                             act(     id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_refactprop__propid FOREIGN KEY ( prop_id )
    REFERENCES                            prop(      id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);

---------------------------------------------------------------------------------------------------
-- разрешения перемещений
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS perm_move CASCADE;
CREATE TABLE perm_move
(
  id               BIGINT   NOT NULL DEFAULT nextval('seq_perm_id')
  ,access_group    NAME     NOT NULL -- группа для которой разрешимо данное правило
  ,access_disabled SMALLINT NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
  ,script_restrict TEXT              DEFAULT NULL

  ,src_cls_id  BIGINT   NOT NULL 
  ,src_obj_id  BIGINT            DEFAULT NULL
  ,src_path    TEXT              DEFAULT NULL

  ,cls_id      BIGINT   NOT NULL 
  ,obj_id      BIGINT            DEFAULT NULL
  
  ,dst_cls_id  BIGINT   NOT NULL 
  ,dst_obj_id  BIGINT            DEFAULT NULL
  ,dst_path    TEXT              DEFAULT NULL

,CONSTRAINT pk_permmove__id PRIMARY KEY ( id )   
,CONSTRAINT fk_permmove__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmove__srcclsid       FOREIGN KEY (src_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmove__dstclsid       FOREIGN KEY (dst_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (perm);
CREATE INDEX idx_permmove__user ON perm_move(access_group);
CREATE INDEX idx_permmove__srcclsob ON perm_move(src_cls_id, src_obj_id);
CREATE INDEX idx_permmove__clsobj ON perm_move(cls_id,     obj_id);
CREATE INDEX idx_permmove__dstclsobj ON perm_move(dst_cls_id, dst_obj_id);

---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS perm_act CASCADE;
CREATE TABLE perm_act
(
  id               BIGINT   NOT NULL DEFAULT nextval('seq_perm_id')
  ,access_group    NAME     NOT NULL -- группа для которой разрешимо данное правило
  ,access_disabled SMALLINT NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
  ,script_restrict TEXT              DEFAULT NULL

  ,src_cls_id  BIGINT   NOT NULL 
  ,src_obj_id  BIGINT            DEFAULT NULL
  ,src_path    TEXT              DEFAULT NULL

  ,act_id          INTEGER   NOT NULL 
,CONSTRAINT pk_permact__id PRIMARY KEY ( id )   
,CONSTRAINT fk_permact__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_permact__srcclsid       FOREIGN KEY (src_cls_id)
    REFERENCES                               cls_num(id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE

-- внешний ключ на действия
,CONSTRAINT fk_permact_clsact FOREIGN KEY (src_cls_id, act_id) 
    REFERENCES                ref_cls_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (perm);
CREATE INDEX idx_permact__user ON perm_act(access_group);
CREATE INDEX idx_permact__srcclsobj ON perm_act(src_cls_id, src_obj_id);
CREATE INDEX idx_permact__actid ON perm_act(act_id);
---------------------------------------------------------------------------------------------------



---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- общие сведения об объектах 
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS obj_name CASCADE;
CREATE TABLE obj_name (
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 
,move_logid BIGINT

,CONSTRAINT pk_obj__id           PRIMARY KEY(id)
,CONSTRAINT uk_obj__title_clsid  UNIQUE (title, cls_id)
--,CONSTRAINT uk_obj__id_clsid     UNIQUE (id,    cls_id) 
,CONSTRAINT uk_objnum__movelogid UNIQUE (move_logid) 

,CONSTRAINT fk_obj__cls         FOREIGN KEY (cls_id)
    REFERENCES                  cls_real    ( id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_objname_clsid ON obj_name ("cls_id") ;

---------------------------------------------------------------------------------------------------
-- детальные сведения объект номерной
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS obj_num CASCADE;
CREATE TABLE obj_num (
  id         BIGINT  NOT NULL  CHECK (  (id=0 AND pid=0) OR( id>0 AND id<>pid ))
    PRIMARY KEY
    REFERENCES obj_name( id )       MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,pid        BIGINT  NOT NULL DEFAULT 1 
     REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

 ,act_logid  BIGINT   UNIQUE
 ,prop       JSONB


);-- INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num ("pid") ;


ALTER TABLE cls_real
  ADD CONSTRAINT cls_real_default_objid_fkey FOREIGN KEY (default_objid)
      REFERENCES obj_num (id) MATCH FULL
      ON UPDATE RESTRICT ON DELETE SET DEFAULT;

---------------------------------------------------------------------------------------------------
-- детальные сведения объект количественный целочисленный
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS obj_qtyi CASCADE;
CREATE TABLE obj_qtyi (
  id         BIGINT  NOT NULL 
    REFERENCES obj_name( id )       MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,pid        BIGINT        NOT NULL CHECK(pid>0) DEFAULT 1 
      REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
 ,qty        NUMERIC(20,0) NOT NULL CHECK (qty>=0)
);
CREATE INDEX idx_objqtyi_id ON obj_qtyi("id") ;
CREATE INDEX idx_objqtyi_pid ON obj_qtyi("pid") ;


---------------------------------------------------------------------------------------------------
-- детальные сведения объект количественный дробных
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS obj_qtyf CASCADE;
CREATE TABLE obj_qtyf (
  id         BIGINT  NOT NULL 
    REFERENCES obj_name( id )       MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,pid        BIGINT        NOT NULL CHECK(pid>0) DEFAULT 1 
      REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
 ,qty        NUMERIC NOT NULL CHECK (qty>=0)
);
CREATE INDEX idx_objqtyf_id ON obj_qtyf("id") ;
CREATE INDEX idx_objqtyf_pid ON obj_qtyf("pid") ;




---------------------------------------------------------------------------------------------------
-- базовая таблица для всех логов
---------------------------------------------------------------------------------------------------
/*
DROP TABLE IF EXISTS log CASCADE;
CREATE TABLE log (
     id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
    ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
    ,username  NAME NOT NULL DEFAULT CURRENT_USER

    ,src_objnum_id BIGINT    NOT NULL 
    ,src_path      BIGINT[]  NOT NULL 
);
*/
---------------------------------------------------------------------------------------------------
-- таблица логов действий номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_act CASCADE;
CREATE TABLE log_act (
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id') PRIMARY KEY
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

  ,src_objnum_id BIGINT   NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,src_path      BIGINT[] NOT NULL 

  ,obj_id        BIGINT   NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,act_id        BIGINT   NOT NULL REFERENCES act( id )     MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE 
  ,prop          JSONB

);--INHERITS (log);
--CREATE INDEX idx_permmove__user ON perm_move(src_cls_id, src_obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(cls_id,     obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(dst_cls_id, dst_obj_id);

---------------------------------------------------------------------------------------------------
-- таблица логов перемещения номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move CASCADE;
CREATE TABLE log_move (
   id            BIGINT      NOT NULL DEFAULT nextval('seq_log_id') PRIMARY KEY 
  ,timemark      TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username      NAME        NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

  ,src_objnum_id BIGINT    NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,src_path      BIGINT[]  NOT NULL 

  ,dst_objnum_id BIGINT    NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,dst_path      BIGINT[]  NOT NULL 

  ,obj_id        BIGINT    NOT NULL REFERENCES obj_name( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,qty           NUMERIC   NOT NULL 
  -- state
  ,act_logid     BIGINT    NOT NULL REFERENCES log_act( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE RESTRICT 
);



-----------------------------------------------------------------------------------------------------------------------------
-- таблица избранных свойст объектов отображаемых при просмотре пользователем каталога объектов по типу или по местоположению
-----------------------------------------------------------------------------------------------------------------------------
--DROP TABLE IF EXISTS favorite_prop CASCADE;
CREATE TABLE favorite_prop(
    id           BIGSERIAL NOT NULL UNIQUE
    ,user_label  NAME      NOT NULL DEFAULT CURRENT_USER 
    ,cls_id      INTEGER       NOT NULL
    ,act_id      INTEGER   NOT NULL
    ,prop_id     INTEGER   NOT NULL

,CONSTRAINT pk_favoriteprop__id           PRIMARY KEY ( id ) 
,CONSTRAINT uk_favoriteprop__id           UNIQUE (user_label, cls_id, act_id, prop_id )

,CONSTRAINT fk_favoriteprop__user_label   FOREIGN KEY (user_label) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favoriteprop__ref_act_prop FOREIGN KEY (act_id, prop_id) 
    REFERENCES                           ref_act_prop (act_id, prop_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_favoriteprop__ref_class_act FOREIGN KEY (cls_id, act_id) 
    REFERENCES                           ref_cls_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);


---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS cls CASCADE;
CREATE VIEW cls AS
SELECT id, pid, measure, kind, title, note,default_objid FROM
(
SELECT id, pid, NULL::WHNAME AS measure,NULL::BIGINT AS default_objid  FROM cls_tree
UNION ALL
SELECT id, pid, measure, default_objid FROM cls_real
) cdif
LEFT JOIN cls_name USING (id);
/*
DROP VIEW IF EXISTS cls CASCADE;
CREATE VIEW cls AS
SELECT id, pid, measure, kind, title, note,default_objid FROM
(
SELECT id, pid, NULL::WHNAME AS measure, 0::SMALLINT AS kind,NULL::BIGINT AS default_objid  FROM cls_tree
UNION ALL
SELECT id, pid, measure,1::SMALLINT AS kind,default_objid FROM cls_num LEFT JOIN cls_real USING (id)
UNION ALL
SELECT id, pid, measure,2::SMALLINT AS kind,default_objid FROM cls_qtyi LEFT JOIN cls_real USING (id)
UNION ALL
SELECT id, pid, measure,3::SMALLINT AS kind,default_objid FROM cls_qtyf LEFT JOIN cls_real USING (id)
) cdif
LEFT JOIN cls_name USING (id);
*/












---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj CASCADE;
CREATE VIEW obj AS
SELECT id, pid, title, cls_id, prop, qty, move_logid, act_logid,cls_kind FROM
(
SELECT id, pid, act_logid, prop, 1::SMALLINT AS cls_kind,1::NUMERIC AS qty FROM obj_num L
UNION ALL
SELECT id, pid, NULL::BIGINT AS act_logid, NULL::JSONB AS prop,2::SMALLINT AS cls_kind, qty FROM obj_qtyi
UNION ALL
SELECT id, pid, NULL::BIGINT AS act_logid, NULL::JSONB AS prop,3::SMALLINT AS cls_kind, qty FROM obj_qtyf
) cdif
LEFT JOIN obj_name USING (id);





---------------------------------------------------------------------------------------------------
-- тригер прикрепления действий к классам
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_biu_perm_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_biu_perm_act()  RETURNS trigger AS
$body$
DECLARE

BEGIN
  IF TG_OP='INSERT' OR TG_OP='UPDATE' THEN
        PERFORM * FROM ref_cls_act WHERE cls_id = NEW.src_cls_id AND act_id = NEW.act_id;
        -- если не нашлось, то добавляем 
        IF NOT FOUND THEN
            INSERT INTO ref_cls_act(cls_id, act_id) VALUES (NEW.src_cls_id, NEW.act_id);
        END IF;
  END IF;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_biu_perm_act BEFORE INSERT OR UPDATE ON perm_act FOR EACH ROW EXECUTE PROCEDURE ftr_biu_perm_act();

---------------------------------------------------------------------------------------------------
-- тригер очищающий ненужные ссылки КЛАСС-ДЕЙСТВИЕ
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_aud_perm_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aud_perm_act()  RETURNS trigger AS
$body$
DECLARE
BEGIN
  IF TG_OP='DELETE' OR (TG_OP='UPDATE' AND (NEW.src_cls_id<>OLD.src_cls_id OR NEW.act_id<>OLD.act_id)) THEN
    DELETE FROM ref_cls_act WHERE (act_id, cls_id) NOT IN (
                SELECT act_id, src_cls_id FROM perm_act GROUP BY act_id, src_cls_id );
  END IF;
RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_aud_perm_act AFTER DELETE OR UPDATE ON perm_act FOR EACH ROW EXECUTE PROCEDURE ftr_aud_perm_act();







-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде путей и таблицы, по идентификация объекта (уникальность)
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_objnum_pathinfo_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fget_objnum_pathinfo_table(IN node_id BIGINT)
 RETURNS TABLE(
     _obj_id      BIGINT
    ,_obj_pid     BIGINT
    ,_obj_title   WHNAME
    ,_cls_id      BIGINT
    ,_cls_title   WHNAME
    ,_patharray   NAME[]
    ,pathid       NAME[]
    ,_path        TEXT
    
) AS $BODY$ 
BEGIN
RETURN QUERY 
    WITH RECURSIVE parents AS 
    (SELECT
        o.id, o.pid, o.title, c.id, c.title
        ,ARRAY[ ARRAY[c.title,o.title]::NAME[] ]::NAME[] AS path
        ,ARRAY[ ARRAY[c.id,o.id]::NAME[] ]::NAME[] AS pathid
        , '/['||c.title||']'||o.title AS _path
        FROM obj AS o
        LEFT JOIN cls c ON c.id=o.cls_id
         WHERE o.id = node_id   --[item.pid]
        AND o.id>1
     UNION ALL
     SELECT
        o.id, o.pid, o.title, cls_real.id, cls_name.title
        ,p.path || ARRAY[cls_name.title,o.title]::NAME[]
        ,p.pathid || ARRAY[cls_real.id,o.id]::NAME[]
        ,'/['||cls_name.title||']'||o.title|| p._path
        FROM 
        parents AS p, obj AS o 
        LEFT JOIN cls_real ON cls_real.id=o.cls_id
        LEFT JOIN cls_name ON cls_real.id=cls_name.id
        WHERE 
        o.id = p.pid
        AND o.id<>1   
        )
   SELECT * FROM parents;

END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 100;
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION fget_objnum_pathinfo_table(BIGINT) TO "Admin" WITH GRANT OPTION;

SELECT * FROM fget_objnum_pathinfo_table(100);


-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения пути объектов и типов в виде 2мерного массива
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_array(IN node_pid BIGINT ) CASCADE;
CREATE OR REPLACE FUNCTION get_path_array(IN node_pid BIGINT ) RETURNS NAME[]  AS $$ 
 SELECT _patharray FROM fget_objnum_pathinfo_table($1) WHERE _obj_pid=1
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
      WHERE _obj_pid=1
$$ LANGUAGE SQL;

GRANT EXECUTE ON FUNCTION get_path(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path(BIGINT) TO "Admin" WITH GRANT OPTION;

-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------    
-- функция получения информации о наследовании класса
-----------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fget_cls_pathinfo_table(IN _in_id INTEGER) CASCADE;
CREATE OR REPLACE FUNCTION fget_cls_pathinfo_table(IN _in_id INTEGER)
 RETURNS TABLE(
   _id       BIGINT,
   _title   WHNAME,
   _kind    SMALLINT,
   _pid     BIGINT,
   _note    TEXT,
   _measure WHNAME,
   _path NAME[]
) AS $BODY$ 
BEGIN
RETURN QUERY(
  WITH RECURSIVE parents AS 
    (SELECT b.id, b.title, b.kind, b.pid, b.note, b.measure
            ,ARRAY[b.title]::NAME[] AS path --,FALSE AS cycle
       FROM cls AS b
       WHERE  b.id = _in_id AND _in_id IS NOT NULL AND  b.id > 1 
     UNION ALL
     SELECT c.id, cls.title, 0::SMALLINT AS kind, c.pid, cls.note, NULL::WHNAME
            ,path || cls.title::NAME --,t.classname = ANY(path) 
       FROM parents AS p, cls_tree AS c 
       LEFT JOIN cls ON cls.id=c.id
       WHERE c.id = p.pid AND c.id > 1
     )
     SELECT  * FROM parents 
);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_cls_pathinfo_table(INTEGER) TO "Guest";

SELECT * FROM fget_cls_pathinfo_table(137);



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


---------------------------------------------------------------------------------------------------
-- тригер создания класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_ins_cls() CASCADE;
CREATE FUNCTION ftg_ins_cls() RETURNS TRIGGER AS $$
BEGIN
  NEW.id := COALESCE(NEW.id,nextval('seq_cls_id'));
  NEW.default_objid := COALESCE(NEW.default_objid,1);
    
  INSERT INTO cls_name (id, title, note, kind) VALUES ( NEW.id, NEW.title, NEW.note,NEW.kind);
  
  CASE NEW.kind 
   WHEN 0 THEN INSERT INTO cls_tree (id, pid)VALUES (NEW.id, NEW.pid); NEW.measure:=NULL;
   WHEN 1 THEN
     INSERT INTO cls_real(id, pid, default_objid, measure,kind)  VALUES (NEW.id, NEW.pid, NEW.default_objid, NEW.measure,NEW.kind);
     INSERT INTO cls_num  (id)VALUES (NEW.id);
   WHEN 2 THEN 
     INSERT INTO cls_real(id, pid, default_objid, measure,kind)  VALUES (NEW.id, NEW.pid, NEW.default_objid, NEW.measure,NEW.kind);
     INSERT INTO cls_qtyi  (id)VALUES (NEW.id);
   WHEN 3 THEN 
     INSERT INTO cls_real(id, pid, default_objid, measure,kind)  VALUES (NEW.id, NEW.pid, NEW.default_objid, NEW.measure,NEW.kind);
     INSERT INTO cls_qtyf  (id)VALUES (NEW.id);
   ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW.kind ;
  END CASE;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_ii_cls INSTEAD OF INSERT ON cls FOR EACH ROW EXECUTE PROCEDURE ftg_ins_cls();
---------------------------------------------------------------------------------------------------
-- тригер редактирования класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_upd_cls() CASCADE;
CREATE FUNCTION ftg_upd_cls() RETURNS TRIGGER AS $$
BEGIN
  IF NEW.id<>OLD.id OR NEW.kind<>OLD.kind THEN
    RAISE EXCEPTION ' %: can`t change id and kind',TG_NAME;
  END IF;
  UPDATE cls_name SET title=NEW.title, note=NEW.note WHERE id=NEW.id;
  CASE 
   WHEN NEW.kind=0 THEN UPDATE cls_tree SET pid=NEW.pid WHERE id=NEW.id; 
   WHEN (NEW.kind=1 OR NEW.kind=2 OR NEW.kind=3) THEN 
     UPDATE cls_real SET pid=NEW.pid, default_objid=NEW.default_objid, measure=NEW.measure WHERE id=NEW.id;
   ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW.kind ;
  END CASE;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_iu_cls INSTEAD OF UPDATE ON cls FOR EACH ROW EXECUTE PROCEDURE ftg_upd_cls();
---------------------------------------------------------------------------------------------------
-- тригер удаления класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_del_cls() CASCADE;
CREATE FUNCTION ftg_del_cls() RETURNS TRIGGER AS $$
BEGIN
  DELETE FROM cls_name WHERE id = OLD.id;
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_id_cls INSTEAD OF DELETE ON cls FOR EACH ROW EXECUTE PROCEDURE ftg_del_cls();

/*
DROP RULE rl_id_cls_tree ON cls_tree;
CREATE RULE rl_id_cls_tree AS 
 ON DELETE TO cls_tree
 DO INSTEAD
 DELETE FROM cls WHERE id = OLD.id;
*/



---------------------------------------------------------------------------------------------------
-- тригер создания объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_ins_obj() CASCADE;
CREATE FUNCTION ftg_ins_obj() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
  NEW_obj_id BIGINT;
BEGIN
  SELECT kind INTO _kind FROM cls WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, NEW.cls_id ;
  END IF;

  NEW.id := COALESCE(NEW.id,nextval('seq_obj_id'));

  INSERT INTO obj_name(id, title, cls_id)VALUES(NEW.id, NEW.title, NEW.cls_id) RETURNING id INTO NEW_obj_id;

  CASE _kind
    WHEN 1 THEN INSERT INTO obj_num(id, pid)VALUES (NEW_obj_id,NEW.pid);NEW.qty:=1;
    WHEN 2 THEN INSERT INTO obj_qtyi(id, pid, qty) VALUES (NEW_obj_id,NEW.pid,NEW.qty);
    WHEN 3 THEN INSERT INTO obj_qtyi(id, pid, qty) VALUES (NEW_obj_id,NEW.pid,NEW.qty);
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
  RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_ii_obj INSTEAD OF INSERT ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_ins_obj();
---------------------------------------------------------------------------------------------------
-- тригер редактирования объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_upd_obj() CASCADE;
CREATE FUNCTION ftg_upd_obj() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
BEGIN
  SELECT kind INTO _kind FROM cls WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, NEW.cls_id ;
  END IF;
  
  IF NEW.id<>OLD.id OR NEW.cls_id<>OLD.cls_id THEN
    RAISE EXCEPTION ' %: can`t change id/cls_id',TG_NAME;
  END IF;

  IF _kind>0 AND _kind<4 THEN
    UPDATE obj_name SET title=NEW.title, move_logid=NEW.move_logid WHERE id =  NEW.id;
  END IF;

  CASE _kind
    WHEN 1 THEN 
      UPDATE obj_num  SET pid=NEW.pid, act_logid=NEW.act_logid, prop=NEW.prop WHERE id =  NEW.id;
    WHEN 2 THEN 
      UPDATE obj_qtyi SET pid=NEW.pid, qty=NEW.qty WHERE id = NEW.id;
    WHEN 3 THEN 
      UPDATE obj_qtyf SET pid=NEW.pid, qty=NEW.qty WHERE id = NEW.id;
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_iu_obj INSTEAD OF UPDATE ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_upd_obj();
---------------------------------------------------------------------------------------------------
-- тригер удаления объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_del_obj() CASCADE;
CREATE FUNCTION ftg_del_obj() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
BEGIN
  SELECT kind INTO _kind FROM cls WHERE id=OLD.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, OLD.cls_id ;
  END IF;

  CASE _kind
    WHEN 1 THEN DELETE FROM obj_name WHERE id = OLD.id;
    WHEN 2 THEN DELETE  FROM obj_qtyi WHERE objqty_id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_name WHERE id = OLD.id AND pid=OLD.pid ;
                IF NOT FOUND THEN
                  DELETE FROM obj_name WHERE id=OLD.id;
                END IF;
    WHEN 3 THEN DELETE  FROM obj_qtyf WHERE objqty_id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_name WHERE id = OLD.id AND pid=OLD.pid ;
                IF NOT FOUND THEN
                  DELETE FROM obj_name WHERE id=OLD.id;
                END IF;
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,_kind ;
  END CASE;
RETURN OLD;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_id_obj INSTEAD OF DELETE ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_del_obj();




---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Вставка базовых классов и объектов';
PRINT '';
---------------------------------------------------------------------------------------------------

INSERT INTO cls(id,pid,title,kind) VALUES (0,0,'nullClsRoot',0);
INSERT INTO cls(id,pid,title,kind) VALUES (1,0,'ClsRoot',0);
INSERT INTO cls(id,pid,title,kind,measure) VALUES (2,1,'RootNumType',1,'шт.');

INSERT INTO obj(id,pid,title,cls_id)VALUES (0,0,'nullNumRoot',2);
INSERT INTO obj(id,pid,title,cls_id)VALUES (1,0,'ObjRoot',2);


---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- создаём 1 пустое и одно действие с свойствами';
PRINT '';
---------------------------------------------------------------------------------------------------

DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
DELETE FROM cls WHERE title='TestCls';

DECLARE @cls_id_1;
SET @cls_id_1 = INSERT INTO cls(pid,title,kind,measure) VALUES (1,'TestCls',1,'coco') RETURNING id;

DECLARE @prop_id_1,@prop_id_2;
SET @prop_id_1 = INSERT INTO prop(title, kind)VALUES ('prop_id_1', 0)RETURNING id;
SET @prop_id_2 = INSERT INTO prop(title, kind)VALUES ('prop_id_2', 0)RETURNING id;

DECLARE @act_id_1,@act_id_2;
SET @act_id_1 = INSERT INTO act (title) VALUES ('Ремонт')RETURNING id;
SET @act_id_2 = INSERT INTO act (title) VALUES ('Проверка')RETURNING id;

INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_1);

INSERT INTO ref_cls_act(cls_id, act_id) VALUES (@cls_id_1, @act_id_1);
INSERT INTO ref_cls_act(cls_id, act_id) VALUES (@cls_id_1, @act_id_2);

INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_1);    
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_2);    

DELETE FROM ref_act_prop WHERE act_id=@act_id_2 AND  prop_id=@prop_id_2;    

UPDATE prop SET title='prop_id_11' WHERE title='prop_id_1';

DELETE FROM prop WHERE title='prop_id_2';



/*
delete from cls_abstr WHERE id>5000;
delete from cls_num WHERE id>5000;
delete from cls_qtyi WHERE id>5000;
delete from cls_qtyf WHERE id>5000;

delete from obj WHERE id>5000;

INSERT INTO cls_num(id,title,pid)         VALUES (10000,'clsn10000',1);
INSERT INTO cls_qtyi(id,title,pid)        VALUES (10001,'clsi10001',1);
INSERT INTO cls_qtyf(id,title,pid)        VALUES (10000,'clsqf10000',1);


INSERT INTO obj_num(id,title,cls_id,pid) VALUES (10000,'objnum10000',10000,1);
INSERT INTO obj_names_qtyi(id,title,cls_id)    VALUES (10001,'objqtyi10001',10001);
INSERT INTO obj_names_qtyi(id,title,cls_id)    VALUES (10000,'objqtyi10000',10000);


INSERT INTO obj_num(title,cls_id,pid) VALUES ('objnum10001',10000,1);
INSERT INTO obj_num(title,cls_id,pid) VALUES ('objnum10002',10000,1);
INSERT INTO obj_num(title,cls_id,pid) VALUES ('objnum10003',10000,1);
*/


---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Вставка тестовых свойств';
PRINT '';
---------------------------------------------------------------------------------------------------
/*
DECLARE @prop_qty ;
SET @prop_qty= 10;
WHILE (@prop_qty > 0)
BEGIN
  INSERT INTO prop(title, kind)VALUES ('TestProp@prop_qty', 0);
  SET @prop_qty=@prop_qty-1;
END
*/


---------------------------------------------------------------------------------------------------
DECLARE @clstitle, @qty, @pid, @cnumid, @cqtyiid, @cqtyfid , @oqtyiid, @oqtyfid, @opid ;
SET @opid= 1;
SET @pid= 1;
SET @qty= 510;
--SET @qty= 10;

WHILE (@qty > 0)
BEGIN

  SET @pid =     INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsa@qty',0,NULL)RETURNING id;
  SET @pid = CAST (@pid AS INTEGER);
  SET @cnumid =  INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsn@qty',1,'ед')RETURNING id;
  SET @cqtyiid = INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsqi@qty',2,'шт')RETURNING id;
  SET @cqtyfid = INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsqf@qty',3, 'mes.')RETURNING id;

  SET @cnumid = CAST (@cnumid AS INTEGER);
  SET @cqtyiid = CAST (@cqtyiid AS INTEGER);
  SET @cqtyfid = CAST (@cqtyfid AS INTEGER);

  SET @opid= INSERT INTO obj(title,cls_id,pid) VALUES ('objnum@cnumid',   @cnumid,@opid)RETURNING id;
  INSERT INTO obj(title,cls_id,pid,qty) VALUES ('objqtyi@cqtyiid', @cqtyiid, @opid, 2);
  INSERT INTO obj(title,cls_id,pid,qty) VALUES ('objqtyi@cqtyfid', @cqtyfid, @opid, 3);

/**
  SET @oqtyiid = INSERT INTO obj_names_qtyi(title,cls_id,qty) VALUES ('objqtyi@cqtyiid', @cqtyiid)     RETURNING id;
  SET @oqtyfid = INSERT INTO obj_names_qtyf(title,cls_id) VALUES ('objqtyf@cqtyfid', @cqtyfid)     RETURNING id;

  INSERT INTO obj_details_qtyi(objqty_id, pid, qty) VALUES (@oqtyiid, @opid, 1);
  INSERT INTO obj_details_qtyf(objqty_id, pid, qty) VALUES (@oqtyfid, @opid, 1);
*/


  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;















