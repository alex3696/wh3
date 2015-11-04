

SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;

ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON SEQUENCES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON TABLES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON FUNCTIONS FROM public;

SELECT whgrant_grouptouser('Admin','postgres');
---------------------------------------------------------------------------------------------------
-- домены
---------------------------------------------------------------------------------------------------
DROP DOMAIN IF EXISTS WHNAME CASCADE; 
DROP DOMAIN IF EXISTS TMPPATH CASCADE; 

CREATE DOMAIN WHNAME AS NAME
   CHECK (VALUE ~ '^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$') ;

CREATE DOMAIN TMPPATH AS TEXT
   CHECK (VALUE ~ '^{((((%*)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') ;

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
DROP SEQUENCE IF EXISTS favorite_prop_id_seq CASCADE;

CREATE SEQUENCE seq_cls_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_act_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_prop_id INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_prop_cls_id      INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_ref_cls_act_id   INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_ref_act_prop_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_obj_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_log_id  INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE seq_perm_id INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;
CREATE SEQUENCE favorite_prop_id_seq INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;

GRANT USAGE ON TABLE seq_cls_id  TO "TypeDesigner";
GRANT USAGE ON TABLE seq_act_id  TO "TypeDesigner";
GRANT USAGE ON TABLE seq_prop_id TO "TypeDesigner";
GRANT USAGE ON TABLE seq_prop_cls_id     TO "TypeDesigner";
GRANT USAGE ON TABLE seq_ref_cls_act_id  TO "TypeDesigner";
GRANT USAGE ON TABLE seq_ref_act_prop_id TO "TypeDesigner";
GRANT USAGE ON TABLE seq_obj_id  TO "ObjDesigner";
GRANT USAGE ON TABLE seq_log_id  TO "User";
GRANT USAGE ON TABLE seq_perm_id TO "TypeDesigner";
GRANT USAGE ON TABLE favorite_prop_id_seq  TO "User";


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

DROP VIEW IF EXISTS cls CASCADE;
DROP FUNCTION IF EXISTS ftg_ins_cls() CASCADE;
DROP FUNCTION IF EXISTS ftg_del_cls() CASCADE;
DROP FUNCTION IF EXISTS ftg_upd_cls() CASCADE;

-- объекты
DROP TABLE IF EXISTS obj_name CASCADE;
DROP TABLE IF EXISTS obj_num CASCADE;
DROP TABLE IF EXISTS obj_qtyi  CASCADE;
DROP TABLE IF EXISTS obj_qtyf  CASCADE;

DROP VIEW IF EXISTS obj CASCADE;
DROP FUNCTION IF EXISTS ftg_ins_obj() CASCADE;
DROP FUNCTION IF EXISTS ftg_del_obj() CASCADE;
DROP FUNCTION IF EXISTS ftg_upd_obj() CASCADE;




DROP TABLE IF EXISTS log_act CASCADE;
DROP TABLE IF EXISTS log_move CASCADE;

DROP TABLE IF EXISTS perm_act CASCADE;
DROP TABLE IF EXISTS perm_move CASCADE;

DROP TABLE IF EXISTS prop CASCADE;
DROP TABLE IF EXISTS prop_cls CASCADE;
DROP TABLE IF EXISTS prop_kind CASCADE;
DROP TABLE IF EXISTS favorite_prop CASCADE;

DROP TABLE IF EXISTS act CASCADE;
DROP TABLE IF EXISTS ref_cls_act CASCADE;
DROP TABLE IF EXISTS ref_act_prop CASCADE;



---------------------------------------------------------------------------------------------------
-- функции
---------------------------------------------------------------------------------------------------


DROP FUNCTION IF EXISTS ftr_aud_perm_act() CASCADE;
DROP FUNCTION IF EXISTS ftr_biu_perm_act() CASCADE;




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

GRANT SELECT        ON TABLE cls_name  TO "Guest";
GRANT INSERT        ON TABLE cls_name  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_name  TO "Admin";
GRANT UPDATE(title,note,kind) ON TABLE cls_name  TO "TypeDesigner";
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

GRANT SELECT        ON TABLE cls_tree  TO "Guest";
GRANT INSERT        ON TABLE cls_tree  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_tree  TO "Admin";
GRANT UPDATE(pid)   ON TABLE cls_tree  TO "TypeDesigner";
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

GRANT SELECT        ON TABLE cls_real  TO "Guest";
GRANT INSERT        ON TABLE cls_real  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_real  TO "Admin";
GRANT UPDATE(pid,default_objid,measure)   ON TABLE cls_real  TO "TypeDesigner";
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

GRANT SELECT        ON TABLE cls_num  TO "Guest";
GRANT INSERT        ON TABLE cls_num  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_num  TO "Admin";
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
GRANT SELECT        ON TABLE cls_qtyi  TO "Guest";
GRANT INSERT        ON TABLE cls_qtyi  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_qtyi  TO "Admin";
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
GRANT SELECT        ON TABLE cls_qtyf  TO "Guest";
GRANT INSERT        ON TABLE cls_qtyf  TO "TypeDesigner";
GRANT DELETE        ON TABLE cls_qtyf  TO "Admin";
---------------------------------------------------------------------------------------------------
-- основная типов свойств
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop_kind CASCADE;
CREATE TABLE prop_kind( 
 id    BIGINT NOT NULL
,title WHNAME NOT NULL
,CONSTRAINT pk_propkind__id    PRIMARY KEY ( id )
);
GRANT SELECT        ON TABLE prop_kind  TO "Guest";
GRANT INSERT        ON TABLE prop_kind  TO "Admin";
GRANT DELETE        ON TABLE prop_kind  TO "Admin";
GRANT UPDATE        ON TABLE prop_kind  TO "Admin";

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
GRANT SELECT        ON TABLE prop  TO "Guest";
GRANT INSERT        ON TABLE prop  TO "TypeDesigner";
GRANT DELETE        ON TABLE prop  TO "TypeDesigner";
GRANT UPDATE(title,kind)
                    ON TABLE prop  TO "TypeDesigner";
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
GRANT SELECT        ON TABLE prop_cls  TO "Guest";
GRANT INSERT        ON TABLE prop_cls  TO "TypeDesigner";
GRANT DELETE        ON TABLE prop_cls  TO "TypeDesigner";
GRANT UPDATE(cls_id,prop_id,val)
                    ON TABLE prop_cls  TO "TypeDesigner";

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
GRANT SELECT        ON TABLE act  TO "Guest";
GRANT INSERT        ON TABLE act  TO "TypeDesigner";
GRANT DELETE        ON TABLE act  TO "TypeDesigner";
GRANT UPDATE(title,note,color,script)
                    ON TABLE act  TO "TypeDesigner";
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
    REFERENCES                      cls_name (    id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_refclsact__actid FOREIGN KEY ( act_id )
    REFERENCES                           act( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
GRANT SELECT        ON TABLE ref_cls_act  TO "Guest";
GRANT INSERT        ON TABLE ref_cls_act  TO "TypeDesigner";
GRANT DELETE        ON TABLE ref_cls_act  TO "TypeDesigner";
GRANT UPDATE(cls_id,act_id)
                    ON TABLE ref_cls_act  TO "TypeDesigner";
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
GRANT SELECT        ON TABLE ref_act_prop  TO "Guest";
GRANT INSERT        ON TABLE ref_act_prop  TO "TypeDesigner";
GRANT DELETE        ON TABLE ref_act_prop  TO "TypeDesigner";
GRANT UPDATE(act_id,prop_id)
                    ON TABLE ref_act_prop  TO "TypeDesigner";
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
  ,src_path    TMPPATH  NOT NULL DEFAULT '{%}'

  ,cls_id      BIGINT   NOT NULL 
  ,obj_id      BIGINT            DEFAULT NULL
  
  ,dst_cls_id  BIGINT   NOT NULL 
  ,dst_obj_id  BIGINT            DEFAULT NULL
  ,dst_path    TMPPATH  NOT NULL DEFAULT '{%}'

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

--CONSTRAINT ck_perm_act_src_path
--    CHECK (src_path ~ '^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') 
-- CONSTRAINT ck_perm_act_dst_path
--    CHECK (dst_path ~ '^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') 
);--INHERITS (perm);
CREATE INDEX idx_permmove__user ON perm_move(access_group);
CREATE INDEX idx_permmove__srcclsob ON perm_move(src_cls_id, src_obj_id);
CREATE INDEX idx_permmove__clsobj ON perm_move(cls_id,     obj_id);
CREATE INDEX idx_permmove__dstclsobj ON perm_move(dst_cls_id, dst_obj_id);

GRANT SELECT        ON TABLE perm_move  TO "Guest";
GRANT INSERT        ON TABLE perm_move  TO "TypeDesigner";
GRANT DELETE        ON TABLE perm_move  TO "TypeDesigner";
GRANT UPDATE (  access_group, access_disabled, script_restrict
              , src_cls_id, src_obj_id, src_path
              , cls_id, obj_id
              , dst_cls_id, dst_obj_id, dst_path)
                    ON TABLE perm_move  TO "TypeDesigner";
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS perm_act CASCADE;
CREATE TABLE perm_act
(
  id               BIGINT   NOT NULL DEFAULT nextval('seq_perm_id')
  ,access_group    NAME     NOT NULL -- группа для которой разрешимо данное правило
  ,access_disabled SMALLINT NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
  ,script_restrict TEXT              DEFAULT NULL

  ,cls_id  BIGINT   NOT NULL 
  ,obj_id  BIGINT            DEFAULT NULL
  ,src_path TMPPATH  NOT NULL DEFAULT '{%}'

  ,act_id          INTEGER   NOT NULL 
,CONSTRAINT pk_permact__id PRIMARY KEY ( id )   
,CONSTRAINT fk_permact__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

-- внешний ключ на действия
,CONSTRAINT fk_permact_clsact FOREIGN KEY (cls_id, act_id) 
    REFERENCES                ref_cls_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

--CONSTRAINT ck_perm_act_src_path
--    CHECK (src_path ~ '^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') 
);--INHERITS (perm);
CREATE INDEX idx_permact__user ON perm_act(access_group);
CREATE INDEX idx_permact__srcclsobj ON perm_act(cls_id, obj_id);
CREATE INDEX idx_permact__actid ON perm_act(act_id);
CREATE INDEX perm_act_src_path_idx ON perm_act (src_path text_pattern_ops);

GRANT SELECT        ON TABLE perm_act  TO "Guest";
GRANT INSERT        ON TABLE perm_act  TO "TypeDesigner";
GRANT DELETE        ON TABLE perm_act  TO "TypeDesigner";
GRANT UPDATE (  access_group, access_disabled, script_restrict
              , cls_id, obj_id
              , src_path
              , act_id)
                    ON TABLE perm_act  TO "TypeDesigner";
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
,act_logid  BIGINT   UNIQUE
,prop       JSONB

,CONSTRAINT pk_obj__id           PRIMARY KEY(id)
,CONSTRAINT uk_obj__title_clsid  UNIQUE (title, cls_id)
--,CONSTRAINT uk_obj__id_clsid     UNIQUE (id,    cls_id) 
,CONSTRAINT uk_objnum__movelogid UNIQUE (move_logid) 

,CONSTRAINT fk_obj__cls         FOREIGN KEY (cls_id)
    REFERENCES                  cls_real    ( id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_objname_clsid ON obj_name ("cls_id") ;
CREATE INDEX idx_objname_prop ON obj_name USING gin ("prop") ;

GRANT SELECT        ON TABLE obj_name  TO "Guest";
GRANT INSERT        ON TABLE obj_name  TO "ObjDesigner";
GRANT DELETE        ON TABLE obj_name  TO "ObjDesigner";
GRANT UPDATE (  title, cls_id, move_logid, act_logid, prop)
                    ON TABLE obj_name  TO "User";
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

 

);-- INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num ("pid") ;

GRANT SELECT        ON TABLE obj_num  TO "Guest";
GRANT INSERT        ON TABLE obj_num  TO "ObjDesigner";
GRANT DELETE        ON TABLE obj_num  TO "ObjDesigner";
GRANT UPDATE (pid)  ON TABLE obj_num  TO "User";

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
 ,CONSTRAINT uk_obj_qtyi__id_pid UNIQUE ( id, pid )   
);
CREATE INDEX idx_objqtyi_id ON obj_qtyi("id") ;
CREATE INDEX idx_objqtyi_pid ON obj_qtyi("pid") ;

GRANT SELECT        ON TABLE obj_qtyi  TO "Guest";
GRANT INSERT        ON TABLE obj_qtyi  TO "User";
GRANT DELETE        ON TABLE obj_qtyi  TO "User";
GRANT UPDATE (pid, qty)
                    ON TABLE obj_qtyi  TO "User";

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
 ,CONSTRAINT uk_obj_qtyf__id_pid UNIQUE ( id, pid )   
);
CREATE INDEX idx_objqtyf_id ON obj_qtyf("id") ;
CREATE INDEX idx_objqtyf_pid ON obj_qtyf("pid") ;

GRANT SELECT        ON TABLE obj_qtyf  TO "Guest";
GRANT INSERT        ON TABLE obj_qtyf  TO "User";
GRANT DELETE        ON TABLE obj_qtyf  TO "User";
GRANT UPDATE (pid, qty)
                    ON TABLE obj_qtyf  TO "User";


---------------------------------------------------------------------------------------------------
-- таблица логов действий номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_act CASCADE;
CREATE TABLE log_act (
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id') PRIMARY KEY
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

  ,src_path  BIGINT[] 

  ,obj_id    BIGINT   NOT NULL REFERENCES obj_name( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,act_id    BIGINT   NOT NULL REFERENCES act( id )     MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE 
  ,prop      JSONB

);--INHERITS (log);
--CREATE INDEX idx_permmove__user ON perm_move(src_cls_id, src_obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(cls_id,     obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(dst_cls_id, dst_obj_id);

GRANT SELECT        ON TABLE log_act  TO "Guest";
GRANT INSERT        ON TABLE log_act  TO "User";
GRANT DELETE        ON TABLE log_act  TO "User";
                    
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move CASCADE;
CREATE TABLE log_move (
   id            BIGINT      NOT NULL DEFAULT nextval('seq_log_id') PRIMARY KEY 
  ,timemark      TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username      NAME        NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

  ,src_objnum_id BIGINT    NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,src_path      BIGINT[]  

  ,dst_objnum_id BIGINT    NOT NULL REFERENCES obj_num( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,dst_path      BIGINT[]  

  ,obj_id        BIGINT    NOT NULL REFERENCES obj_name( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE CASCADE 
  ,qty           NUMERIC   NOT NULL 
  -- state
  ,act_logid     BIGINT             REFERENCES log_act( id ) MATCH SIMPLE ON UPDATE RESTRICT ON DELETE RESTRICT 
);

GRANT SELECT        ON TABLE log_move  TO "Guest";
GRANT INSERT        ON TABLE log_move  TO "User";
GRANT DELETE        ON TABLE log_move  TO "User";

-----------------------------------------------------------------------------------------------------------------------------
-- таблица избранных свойст объектов отображаемых при просмотре пользователем каталога объектов по типу или по местоположению
-----------------------------------------------------------------------------------------------------------------------------
--DROP TABLE IF EXISTS favorite_prop CASCADE;
CREATE TABLE favorite_prop(
    id           BIGINT NOT NULL DEFAULT nextval('favorite_prop_id_seq') UNIQUE
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

GRANT SELECT        ON TABLE favorite_prop  TO "Guest";
GRANT INSERT        ON TABLE favorite_prop  TO "User";
GRANT DELETE        ON TABLE favorite_prop  TO "User";
GRANT UPDATE (user_label, cls_id, act_id, prop_id )
                    ON TABLE favorite_prop  TO "User";

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
GRANT SELECT        ON cls  TO "Guest";
GRANT INSERT        ON cls  TO "TypeDesigner";
GRANT DELETE        ON cls  TO "Admin";
GRANT UPDATE        ON cls  TO "TypeDesigner";











---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj CASCADE;
CREATE VIEW obj AS
SELECT id, pid, title, cls_id, prop, qty, move_logid, act_logid,cls_kind FROM
(
SELECT id, pid, 1::SMALLINT AS cls_kind,1::NUMERIC AS qty FROM obj_num
UNION ALL
SELECT id, pid, 2::SMALLINT AS cls_kind, qty FROM obj_qtyi
UNION ALL
SELECT id, pid, 3::SMALLINT AS cls_kind, qty FROM obj_qtyf
) cdif
LEFT JOIN obj_name USING (id);

GRANT SELECT        ON obj  TO "Guest";
GRANT INSERT        ON obj  TO "User";
GRANT DELETE        ON obj  TO "User";
GRANT UPDATE        ON obj  TO "User";




---------------------------------------------------------------------------------------------------
-- тригер прикрепления действий к классам
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_biu_perm_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_biu_perm_act()  RETURNS trigger AS
$body$
DECLARE

BEGIN
  IF TG_OP='INSERT' OR TG_OP='UPDATE' THEN
        PERFORM * FROM ref_cls_act WHERE cls_id = NEW.cls_id AND act_id = NEW.act_id;
        -- если не нашлось, то добавляем 
        IF NOT FOUND THEN
            INSERT INTO ref_cls_act(cls_id, act_id) VALUES (NEW.cls_id, NEW.act_id);
        END IF;
  END IF;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_biu_perm_act BEFORE INSERT OR UPDATE ON perm_act FOR EACH ROW EXECUTE PROCEDURE ftr_biu_perm_act();

GRANT EXECUTE ON FUNCTION ftr_biu_perm_act() TO "TypeDesigner";
---------------------------------------------------------------------------------------------------
-- тригер очищающий ненужные ссылки КЛАСС-ДЕЙСТВИЕ
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_aud_perm_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aud_perm_act()  RETURNS trigger AS
$body$
DECLARE
BEGIN
  IF TG_OP='DELETE' OR (TG_OP='UPDATE' AND (NEW.cls_id<>OLD.cls_id OR NEW.act_id<>OLD.act_id)) THEN
    DELETE FROM ref_cls_act WHERE (act_id, cls_id) NOT IN (
                SELECT act_id, cls_id FROM perm_act GROUP BY act_id, cls_id );
  END IF;
RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_aud_perm_act AFTER DELETE OR UPDATE ON perm_act FOR EACH ROW EXECUTE PROCEDURE ftr_aud_perm_act();


GRANT EXECUTE ON FUNCTION ftr_aud_perm_act() TO "TypeDesigner";


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

GRANT EXECUTE ON FUNCTION ftg_ins_cls() TO "TypeDesigner";
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

GRANT EXECUTE ON FUNCTION ftg_upd_cls() TO "TypeDesigner";
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

GRANT EXECUTE ON FUNCTION ftg_upd_cls() TO "Admin";
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
BEGIN
  SELECT kind INTO _kind FROM cls WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, NEW.cls_id ;
  END IF;

  NEW.id := COALESCE(NEW.id,nextval('seq_obj_id'));

  PERFORM FROM obj WHERE id=NEW.id;-- проверяем существование количественного объета
  IF NOT FOUND THEN
    INSERT INTO obj_name(id, title, cls_id)VALUES(NEW.id, NEW.title, NEW.cls_id);
  END IF;
  
  CASE _kind
    WHEN 1 THEN INSERT INTO obj_num(id, pid)VALUES (NEW.id,NEW.pid);NEW.qty:=1;
    WHEN 2 THEN 
      IF (ceil(NEW.qty)<>NEW.qty)THEN
        RAISE EXCEPTION ' qty is not integer NEW.qty', NEW.qty;
      END IF;
      INSERT INTO obj_qtyi(id, pid, qty) VALUES (NEW.id,NEW.pid,NEW.qty);
    WHEN 3 THEN INSERT INTO obj_qtyi(id, pid, qty) VALUES (NEW.id,NEW.pid,NEW.qty);
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
  RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_ii_obj INSTEAD OF INSERT ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_ins_obj();

GRANT EXECUTE ON FUNCTION ftg_ins_obj() TO "User";
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
    UPDATE obj_name SET 
      title=NEW.title
      ,move_logid=NEW.move_logid 
      ,act_logid=NEW.act_logid
      ,prop=NEW.prop
      WHERE id =  NEW.id;
  END IF;

  CASE _kind
    WHEN 1 THEN 
      UPDATE obj_num  SET pid=NEW.pid WHERE id =  NEW.id;
    WHEN 2 THEN 
      IF (ceil(NEW.qty)<>NEW.qty)THEN
        RAISE EXCEPTION ' qty is not integer NEW.qty', NEW.qty;
      END IF;
      UPDATE obj_qtyi SET pid=NEW.pid, qty=NEW.qty WHERE id = NEW.id AND pid=OLD.pid;
    WHEN 3 THEN 
      UPDATE obj_qtyf SET pid=NEW.pid, qty=NEW.qty WHERE id = NEW.id AND pid=OLD.pid;
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_iu_obj INSTEAD OF UPDATE ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_upd_obj();

GRANT EXECUTE ON FUNCTION ftg_upd_obj() TO "User";
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
    WHEN 1 THEN DELETE FROM obj_name  WHERE id = OLD.id;
    WHEN 2 THEN DELETE  FROM obj_qtyi WHERE id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_qtyi WHERE id = OLD.id;
                IF NOT FOUND THEN
                  DELETE FROM obj_name WHERE id=OLD.id;
                END IF;
    WHEN 3 THEN DELETE  FROM obj_qtyf WHERE id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_qtyf WHERE id = OLD.id;
                IF NOT FOUND THEN
                  DELETE FROM obj_name WHERE id=OLD.id;
                END IF;
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,_kind ;
  END CASE;
RETURN OLD;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_id_obj INSTEAD OF DELETE ON obj FOR EACH ROW EXECUTE PROCEDURE ftg_del_obj();

GRANT EXECUTE ON FUNCTION ftg_del_obj() TO "User";


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


ALTER TABLE cls_real
  ADD CONSTRAINT cls_real_default_objid_fkey FOREIGN KEY (default_objid)
      REFERENCES obj_num (id) MATCH FULL
      ON UPDATE RESTRICT ON DELETE SET DEFAULT;
---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Заполняем тестовыми данными';
PRINT '';
---------------------------------------------------------------------------------------------------

DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
DELETE FROM cls WHERE title='TestCls';

DECLARE @cls_id_1,@cls_id_2,@cls_id_3,@cls_id_4,@cls_id_5,@cls_id_6,@cls_id_7,@cls_id_8;
SET @cls_id_1 = INSERT INTO cls(pid,title,kind,measure) VALUES (1,'TestCls',1,'coco') RETURNING id;
SET @cls_id_2 = INSERT INTO cls(pid,title,kind) VALUES (1,'Приборы',0) RETURNING id;
SET @cls_id_3 = INSERT INTO cls(pid,title,kind) VALUES (1,'ЗИП',0) RETURNING id;
SET @cls_id_4 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_2,'СРК2М',1,'ед.') RETURNING id;
SET @cls_id_5 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_2,'ДИНА-К4-89',1,'ед.') RETURNING id;
SET @cls_id_6 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'ФЭУ-102',2,'шт.') RETURNING id;
SET @cls_id_7 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'упл.кольцо 105-115-58',2,'шт.') RETURNING id;
SET @cls_id_8 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'Спирт',3,'л.') RETURNING id;

DECLARE @oid_1,@oid_2,@oid_3,@oid_4,@oid_5,@oid_6,@oid_7,@oid_8,@oid_9,@oid_10,@oid_11,@oid_12,@oid_13;
SET @oid_1 = INSERT INTO obj(title,cls_id,pid) VALUES ('01',@cls_id_4, 1 )RETURNING id;
SET @oid_2 = INSERT INTO obj(title,cls_id,pid) VALUES ('02',@cls_id_4, 1 )RETURNING id;
SET @oid_3 = INSERT INTO obj(title,cls_id,pid) VALUES ('03',@cls_id_4, 1 )RETURNING id;
SET @oid_4 = INSERT INTO obj(title,cls_id,pid) VALUES ('01',@cls_id_5, 1 )RETURNING id;

SET @oid_5 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_6, 1, 9)RETURNING id;
SET @oid_6 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_7, 1, 50)RETURNING id;
SET @oid_7 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_8, 1 , 3.3)RETURNING id;
SET @oid_8 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_8, 1 , 3.1)RETURNING id;

SET @oid_9 = INSERT INTO obj(title,cls_id,pid) VALUES ('1',@cls_id_1, 1 )RETURNING id;
SET @oid_10 = INSERT INTO obj(title,cls_id,pid) VALUES ('2',@cls_id_1, 1 )RETURNING id;


DECLARE @prop_id_1,@prop_id_2,@prop_id_3,@prop_id_4;
SET @prop_id_1 = INSERT INTO prop(title, kind)VALUES ('Наработка(ч.)', 1)RETURNING id;
SET @prop_id_2 = INSERT INTO prop(title, kind)VALUES ('Счёт ГК', 1)RETURNING id;
SET @prop_id_3 = INSERT INTO prop(title, kind)VALUES ('Комментарий', 0)RETURNING id;
SET @prop_id_4 = INSERT INTO prop(title, kind)VALUES ('Описание ремонта', 0)RETURNING id;

DECLARE @act_id_1,@act_id_2,@act_id_3,@act_id_4;
SET @act_id_1 = INSERT INTO act (title) VALUES ('Ремонт')RETURNING id;
SET @act_id_2 = INSERT INTO act (title) VALUES ('Проверка')RETURNING id;
SET @act_id_3 = INSERT INTO act (title) VALUES ('Профилактика')RETURNING id;
SET @act_id_4 = INSERT INTO act (title) VALUES ('ГИС')RETURNING id;

-- 'Ремонт' --Наработка+Счёт ГК+Комментарий+Описание ремонта
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_3);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_4);
-- 'Проверка' --Наработка+Счёт ГК+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_3);
-- 'Профилактика' --Наработка+Счёт ГК+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_3);
-- 'ГИС' --Наработка+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_4, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_4, @prop_id_3);


INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_1);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_2);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_3);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_4);


INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_6, NULL, @act_id_2);

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_5, NULL, @act_id_1);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_5, NULL, @act_id_3);


INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL        -- whar [ФЭУ-102]%
                       ,2, NULL, '{%}'          -- from [ROOT]%
                       , @cls_id_4, NULL, '{%}' -- to   [СРК2М]% 
                      );

INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id
                      ,dst_cls_id, dst_obj_id)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL        -- whar [ФЭУ-102]%
                       ,2, NULL                -- from [ROOT]%
                       , @cls_id_5, NULL       -- to   [ДИНА-К4-89]% 
                      );

INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id
                      ,dst_cls_id, dst_obj_id)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL       -- whar [ФЭУ-102]%
                       ,@cls_id_5, NULL       -- from [ДИНА-К4-89]% 
                       ,2, NULL             -- to   [ROOT]%
                       
                      );



/*
delete from cls_abstr WHERE id>5000;
delete from cls_num WHERE id>5000;
delete from cls_qtyi WHERE id>5000;
delete from cls_qtyf WHERE id>5000;

delete from obj WHERE id>5000;

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
SET @qty= 10;
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















