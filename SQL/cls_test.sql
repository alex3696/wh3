

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
DROP TABLE IF EXISTS cls CASCADE;
DROP TABLE IF EXISTS cls_abstr CASCADE;
DROP TABLE IF EXISTS cls_qtyi  CASCADE;
DROP TABLE IF EXISTS cls_qtyf  CASCADE;
DROP TABLE IF EXISTS cls_num   CASCADE;


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



DROP FUNCTION IF EXISTS ftg_ins_cls_tree();
DROP FUNCTION IF EXISTS ftg_del_cls_tree();
DROP FUNCTION IF EXISTS ftg_upd_cls_tree();

---------------------------------------------------------------------------------------------------
CREATE TABLE cls ( 
    id     BIGINT   NOT NULL DEFAULT nextval('seq_cls_id')
    ,title WHNAME   NOT NULL                          
    ,note  TEXT              DEFAULT NULL

,CONSTRAINT pk_cls__id    PRIMARY KEY ( id )   
,CONSTRAINT uk_cls__title UNIQUE ( title ) 
);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_abstr ( 
 id  BIGINT NOT NULL 
,pid BIGINT NOT NULL DEFAULT 1

,CONSTRAINT pk_clsabstr__id    PRIMARY KEY ( id )
,CONSTRAINT fk_clsabstr__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_abstr( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT ck_clsabstr__root  CHECK (  (id=0 AND pid=0) -- main root
                               OR(id>0 AND id<>pid)   )  -- один корень                             
,CONSTRAINT fk_clsabstr__id   FOREIGN KEY ( id )
    REFERENCES                    cls( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);
CREATE INDEX idx_clsabstr_pid ON cls_abstr(pid);

---------------------------------------------------------------------------------------------------
CREATE TABLE cls_qtyi ( 
 id  BIGINT NOT NULL 
,pid BIGINT NOT NULL DEFAULT 1
,default_pid BIGINT  DEFAULT 1

,CONSTRAINT pk_clsqtyi__id    PRIMARY KEY ( id )
,CONSTRAINT fk_clsqtyi__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_abstr( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT ck_clsqtyi__root  CHECK ( id>0 AND id<>pid )
,CONSTRAINT fk_clsqtyi__id   FOREIGN KEY ( id )
    REFERENCES                    cls( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);
CREATE INDEX idx_clsqtyi_pid ON cls_qtyi(pid);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_qtyf ( 
 id  BIGINT NOT NULL 
,pid BIGINT NOT NULL DEFAULT 1
,measure WHNAME NOT NULL
,default_pid BIGINT  DEFAULT 1

,CONSTRAINT pk_clsqtyf__id    PRIMARY KEY ( id )
,CONSTRAINT fk_clsqtyf__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_abstr( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

,CONSTRAINT ck_clsqtyf__root  CHECK ( id>0 AND id<>pid )
,CONSTRAINT fk_clsqtyf__id   FOREIGN KEY ( id )
    REFERENCES                    cls( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
    
);
CREATE INDEX idx_cls_qtyf__measure ON cls_qtyf(measure);
CREATE INDEX idx_clsqtyf_pid ON cls_qtyf(pid);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_num ( 
 id  BIGINT NOT NULL 
,pid BIGINT NOT NULL DEFAULT 1
,default_pid BIGINT  DEFAULT 1

,CONSTRAINT pk_clsnum__id    PRIMARY KEY ( id )
,CONSTRAINT fk_clsnum__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_abstr( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

,CONSTRAINT ck_clsnum__root  CHECK ( id>0 AND id<>pid )
,CONSTRAINT fk_clsnum__id   FOREIGN KEY ( id )
    REFERENCES                    cls( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_clsnum_pid ON cls_num(pid);


---------------------------------------------------------------------------------------------------
-- основная типов свойств
---------------------------------------------------------------------------------------------------
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
    REFERENCES                           cls ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
/*
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_num ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id  INTEGER NOT NULL
,prop_id INTEGER NOT NULL
,val     TEXT

,CONSTRAINT pk_propnum               PRIMARY KEY ( id )
,CONSTRAINT uk_propnum__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propnum__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propnum__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_num ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
);--INHERITS (prop_cls);
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_qtyi ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id  INTEGER NOT NULL
,prop_id INTEGER NOT NULL
,val     TEXT

,CONSTRAINT pk_propqtyi               PRIMARY KEY ( id )
,CONSTRAINT uk_propqtyi__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propqtyi__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propqtyi__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_qtyi ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
);--INHERITS (prop_cls);
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_qtyf ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id  INTEGER NOT NULL
,prop_id INTEGER NOT NULL
,val     TEXT

,CONSTRAINT pk_propqtyf               PRIMARY KEY ( id )
,CONSTRAINT uk_propqtyf__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propqtyf__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propqtyf__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_qtyf ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
);--INHERITS (prop_cls);

*/


---------------------------------------------------------------------------------------------------
-- основная таблица действий 
---------------------------------------------------------------------------------------------------
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
-- таблица правил 
-- можно/нельзя[permtype] ложить объект "mov_obj" В слот "mov_cls" объекта "dst_obj" типа "dst_cls"
---------------------------------------------------------------------------------------------------
/**
DROP TABLE IF EXISTS perm CASCADE;
---------------------------------------------------------------------------------------------------
CREATE TABLE perm
(
  id               BIGINT   NOT NULL DEFAULT nextval('seq_perm_id')
  ,access_group    NAME     NOT NULL -- группа для которой разрешимо данное правило
  ,access_disabled SMALLINT NOT NULL DEFAULT 0 CHECK (access_disabled=0 OR access_disabled=1)
  ,script_restrict TEXT              DEFAULT NULL

  ,cls_id      BIGINT       NOT NULL 
  ,obj_id      BIGINT                DEFAULT NULL
  ,src_path        TEXT              DEFAULT NULL
);
*/
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
,CONSTRAINT fk_permact__srcclsid       FOREIGN KEY (src_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permact__dstclsid       FOREIGN KEY (dst_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (perm);
CREATE INDEX idx_permmove__user ON perm_move(access_group);
CREATE INDEX idx_permmove__srcclsob ON perm_move(src_cls_id, src_obj_id);
CREATE INDEX idx_permmove__clsobj ON perm_move(cls_id,     obj_id);
CREATE INDEX idx_permmove__dstclsobj ON perm_move(dst_cls_id, dst_obj_id);

---------------------------------------------------------------------------------------------------
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
-- внешний ключ на действия
,CONSTRAINT fk_permact_clsact FOREIGN KEY (src_cls_id, act_id) 
    REFERENCES                ref_cls_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (perm);
CREATE INDEX idx_permact__user ON perm_act(access_group);
CREATE INDEX idx_permact__srcclsobj ON perm_act(src_cls_id, src_obj_id);
---------------------------------------------------------------------------------------------------
/*
CREATE TABLE perm_move_num
(

 CONSTRAINT pk_permmovenum__id PRIMARY KEY ( id ) 
,CONSTRAINT fk_permmovenum__acess_group FOREIGN KEY (access_group) 
    REFERENCES                              wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_permmovenum__clsid       FOREIGN KEY (cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmovenum__dstcls       FOREIGN KEY (dst_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (perm_move);
CREATE INDEX idx_permmovenum__user ON perm_move_num(access_group);
---------------------------------------------------------------------------------------------------
CREATE TABLE perm_move_qtyi
(
 CONSTRAINT pk_permmoveqtyi__id PRIMARY KEY ( id ) 
,CONSTRAINT fk_permmoveqtyi__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_permmoveqtyi__clsid       FOREIGN KEY (cls_id)
    REFERENCES                                cls_qtyi(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmoveqtyi__dstcls       FOREIGN KEY (dst_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

    
)INHERITS (perm_move);
CREATE INDEX idx_permmoveqtyi__user ON perm_move_qtyi (access_group);
---------------------------------------------------------------------------------------------------
CREATE TABLE perm_move_qtyf
(
  src_min      NUMERIC      NOT NULL DEFAULT 0
 ,dst_max      NUMERIC               DEFAULT NULL

,CONSTRAINT pk_permmoveqtyf__id PRIMARY KEY ( id ) 
,CONSTRAINT fk_permmoveqtyf__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_permmoveqtyf__clsid       FOREIGN KEY (cls_id)
    REFERENCES                                cls_qtyf(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmoveqtyf__dstcls       FOREIGN KEY (dst_cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE    
)INHERITS (perm_move);
CREATE INDEX idx_permmoveqtyf__user ON perm_move_qtyf(access_group);
*/




---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- объект базовый
---------------------------------------------------------------------------------------------------
/**
CREATE TABLE obj (
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 
);
*/
---------------------------------------------------------------------------------------------------
-- объект номерной
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_num (
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 

,pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT  
,act_logid  BIGINT   
,prop       JSONB
,CONSTRAINT pk_objnum__id          PRIMARY KEY(id)
,CONSTRAINT uk_objnum__clsid_title UNIQUE (cls_id, title) 
,CONSTRAINT uk_objnum__movelogid   UNIQUE (move_logid) 
,CONSTRAINT uk_objnum__actlogid    UNIQUE (act_logid) 

,CONSTRAINT fk_objnum_pid      FOREIGN KEY (pid)
    REFERENCES                 obj_num      (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_objnum_clsnum   FOREIGN KEY (cls_id)
    REFERENCES                 cls_num     (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT ck_objnum_one_root CHECK (  (id=0 AND pid=0) 
                                      OR( id>0 AND id<>pid ))
);-- INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num ("pid") ;
---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ названия
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_names_qtyi
(
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 

,CONSTRAINT pk_namesobjqtyi__id     PRIMARY KEY( id )
,CONSTRAINT uk_namesobjqtyi__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_namesobjqtyi__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyi  (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);-- INHERITS (obj);
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_names_qtyf
(
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 

,CONSTRAINT pk_namesobjqtyf__id     PRIMARY KEY( id )
,CONSTRAINT uk_namesobjqtyf__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_namesobjqtyf__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyf  (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);-- INHERITS (obj);

---------------------------------------------------------------------------------------------------
-- таблица ДЕТАЛЕЙ КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ 
---------------------------------------------------------------------------------------------------
/*
CREATE TABLE obj_details_qty (
 objqty_id BIGINT
,pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT
,CONSTRAINT ck_objnum_one_root CHECK (objqty_id>1 AND pid>0 AND objqty_id<>pid)
);
*/

---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ ЭЛЕМЕНТЫ
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_details_qtyi (
 objqty_id BIGINT
,pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT
,CONSTRAINT ck_objqtyi_one_root CHECK (objqty_id>1 AND pid>0 AND objqty_id<>pid)

,qty        NUMERIC(20,0) NOT NULL              CHECK (qty>=0)
,CONSTRAINT pk_detailobjqtyi_oid_pid   PRIMARY KEY(objqty_id, pid)
,CONSTRAINT uk_detailobjqtyi_log_id    UNIQUE (move_logid) --при разделении 2 объекта появится с одинаковым last_log_id
,CONSTRAINT fk_detailobjqtyi_items_pid FOREIGN KEY (pid)
    REFERENCES                          obj_num    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_detailobjqtyi_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                     obj_names_qtyi  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);-- INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyi_pid ON obj_details_qtyi("pid") ;
CREATE INDEX idx_detailobjqtyi_movelogid ON obj_details_qtyi("move_logid") ;
---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ ЭЛЕМЕНТЫ
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_details_qtyf (
 objqty_id BIGINT
,pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT
,CONSTRAINT ck_objqtyf_one_root CHECK (objqty_id>1 AND pid>0 AND objqty_id<>pid)

,qty        NUMERIC NOT NULL              CHECK (qty>=0)
,CONSTRAINT pk_detailobjqtyf_oid_pid   PRIMARY KEY(objqty_id, pid)
,CONSTRAINT fk_detailobjqtyf_items_pid FOREIGN KEY (pid)
    REFERENCES                  obj_num    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_detailobjqtyf_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                 obj_names_qtyf  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);-- INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyf_pid ON obj_details_qtyf("pid") ;
CREATE INDEX idx_detailobjqtyf_id ON obj_details_qtyf("objqty_id") ;
CREATE INDEX idx_detailobjqtyf_movelogid ON obj_details_qtyf("move_logid") ;



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
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER

  ,src_objnum_id BIGINT    NOT NULL 
  ,src_path      BIGINT[]  NOT NULL 

  ,objnum_id     BIGINT   NOT NULL 
  ,act_id        BIGINT   NOT NULL 
  ,prop          JSONB

,CONSTRAINT pk_logact__id       PRIMARY KEY (id)
,CONSTRAINT fk_logact__username FOREIGN KEY (username)
    REFERENCES                 wh_role (rolname)                   
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

,CONSTRAINT fk_logact__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                        obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_logact__objnumid FOREIGN KEY (objnum_id)
    REFERENCES                        obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_logact__actid FOREIGN KEY (act_id)
    REFERENCES                           act  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

);--INHERITS (log);
--CREATE INDEX idx_permmove__user ON perm_move(src_cls_id, src_obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(cls_id,     obj_id);
--CREATE INDEX idx_permmove__user ON perm_move(dst_cls_id, dst_obj_id);

---------------------------------------------------------------------------------------------------
-- базовая таблица логов перемещения
---------------------------------------------------------------------------------------------------
/*
DROP TABLE IF EXISTS log_move CASCADE;
CREATE TABLE log_move (
  ,dst_objnum_id  BIGINT    NOT NULL 
  ,dst_path      BIGINT[]  NOT NULL 

)INHERITS (log);
*/
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения количественных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_qtyi CASCADE;
CREATE TABLE log_move_qtyi (
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER

  ,src_objnum_id BIGINT    NOT NULL 
  ,src_path      BIGINT[]  NOT NULL 

  ,dst_objnum_id  BIGINT    NOT NULL 
  ,dst_path      BIGINT[]  NOT NULL 

  ,objqtyi_id     BIGINT    NOT NULL 
,CONSTRAINT pk_logmoveqtyi__id       PRIMARY KEY (id)
,CONSTRAINT fk_logmoveqtyi__username FOREIGN KEY (username)
    REFERENCES                 wh_role (rolname)                   
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
,CONSTRAINT fk_logmoveqtyi__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                              obj_num  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_logmoveqtyi__dstobjnumid FOREIGN KEY (dst_objnum_id)
    REFERENCES                              obj_num  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_logmoveqtyi__objqtyid FOREIGN KEY (objqtyi_id)
    REFERENCES                        obj_names_qtyi  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (log_move);
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения количественных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_qtyf CASCADE;
CREATE TABLE log_move_qtyf (
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER

  ,src_objnum_id BIGINT    NOT NULL 
  ,src_path      BIGINT[]  NOT NULL 

  ,dst_objnum_id  BIGINT    NOT NULL 
  ,dst_path      BIGINT[]  NOT NULL 
  
   ,objqtyf_id     BIGINT    NOT NULL 
   ,qty           NUMERIC   NOT NULL 
,CONSTRAINT pk_logmoveqtyf__id       PRIMARY KEY (id)
,CONSTRAINT pk_logmoveqtyf__username FOREIGN KEY (username)
    REFERENCES                 wh_role (rolname)                   
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
,CONSTRAINT fk_pk_logmoveqtyf__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                              obj_num  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_pk_logmoveqtyf__dstobjnumid FOREIGN KEY (dst_objnum_id)
    REFERENCES                              obj_num  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_pk_logmoveqtyf__objqtyid FOREIGN KEY (objqtyf_id)
    REFERENCES                        obj_names_qtyf  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (log_move);
  
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_num CASCADE;
CREATE TABLE log_move_num (
   id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
  ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username  NAME NOT NULL DEFAULT CURRENT_USER

  ,src_objnum_id BIGINT    NOT NULL 
  ,src_path      BIGINT[]  NOT NULL 

  ,dst_objnum_id  BIGINT    NOT NULL 
  ,dst_path      BIGINT[]  NOT NULL 
  
  ,objnum_id    BIGINT   NOT NULL 
  ,logact_id    BIGINT   NOT NULL 

,CONSTRAINT pk_logmovenum__id          PRIMARY KEY (id)
,CONSTRAINT fk_logmovenum__username    FOREIGN KEY (username)
    REFERENCES                             wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
,CONSTRAINT fk_logmovenum__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_logmovenum__logactid    FOREIGN KEY (logact_id)
    REFERENCES                             log_act (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET NULL
,CONSTRAINT fk_logmovenum__dstobjnumid FOREIGN KEY (dst_objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE    
,CONSTRAINT fk_logmovenum__objnumid    FOREIGN KEY (objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);--INHERITS (log_move);
/*
-------------------------------------------------------------------------------
--таблица блокировки перемещаемых объектов
-------------------------------------------------------------------------------
CREATE TABLE lock_src(
   lock_session INTEGER     NOT NULL DEFAULT pg_backend_pid()
  ,lock_user    NAME        NOT NULL DEFAULT CURRENT_USER
  ,lock_time    TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP
-- What
  ,oid          BIGINT
  ,src_path     BIGINT[]    
);
-------------------------------------------------------------------------------
--таблица блокировки перемещаемых номерных объектов
-------------------------------------------------------------------------------
CREATE TABLE lock_srcnum(
  CONSTRAINT pk_srcnum__oid     PRIMARY KEY (oid)

  ,CONSTRAINT fk_srcnum__username FOREIGN KEY (lock_user)
    REFERENCES                       wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

  ,CONSTRAINT fk_bnsrc__oid      FOREIGN KEY (oid)
    REFERENCES                      obj_num ( id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

)INHERITS (lock_src);
-------------------------------------------------------------------------------
--таблица блокировки перемещаемых QTYI объектов
-------------------------------------------------------------------------------
CREATE TABLE lock_srcqtyi(
  pid   BIGINT      NOT NULL

,CONSTRAINT pk_srcqtyi__oid_pid   PRIMARY KEY (oid, pid)
,CONSTRAINT fk_srcqtyi__oid_pid   FOREIGN KEY (oid, pid)
    REFERENCES              obj_details_qtyi  (objqty_id, pid)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_srcnum__username   FOREIGN KEY (lock_user)
    REFERENCES                       wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (lock_src);
-------------------------------------------------------------------------------
--таблица блокировки перемещаемых QTYF объектов
-------------------------------------------------------------------------------
CREATE TABLE lock_srcqtyf(
  pid   BIGINT      NOT NULL
  ,qty  NUMERIC
  
,CONSTRAINT pk_srcqtyf__oid_pid   PRIMARY KEY (oid, pid)
,CONSTRAINT fk_srcqtyf__oid_pid   FOREIGN KEY (oid, pid)
    REFERENCES              obj_details_qtyf  (objqty_id, pid)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_srcnum__username   FOREIGN KEY (lock_user)
    REFERENCES                       wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE  
)INHERITS (lock_src);

-------------------------------------------------------------------------------
--таблица блокировки перемещаемых QTYF объектов
-------------------------------------------------------------------------------
CREATE TABLE lock_srcact(
 CONSTRAINT pk_srcact__oid     PRIMARY KEY (oid)
,CONSTRAINT fk_srcact__username FOREIGN KEY (lock_user)
    REFERENCES                       wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_srcact__oid      FOREIGN KEY (oid)
    REFERENCES                      obj_num ( id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (lock_src);



CREATE TABLE lock_dst(
 oid  BIGINT  NOT NULL
,dst_path BIGINT[]
);

CREATE TABLE lock_dstnum(
 CONSTRAINT fk_lock_dstnum_oid FOREIGN KEY (oid)
 REFERENCES                    lock_srcnum (oid) 
 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (lock_dst);

CREATE TABLE lock_dstqtyi(
 pid  BIGINT  NOT NULL
,CONSTRAINT fk_lock_dstqtyi_oid FOREIGN KEY (oid,pid)
 REFERENCES                    lock_srcqtyi (oid,pid) 
 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (lock_dst);

CREATE TABLE lock_dstqtyf(
 pid  BIGINT  NOT NULL
,CONSTRAINT fk_lock_dstqtyf_oid FOREIGN KEY (oid,pid)
 REFERENCES                    lock_srcqtyf (oid,pid) 
 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (lock_dst);

CREATE TABLE lock_dstact(
 oid  BIGINT  NOT NULL
,actid  BIGINT  NOT NULL
 
,CONSTRAINT fk_lock_dstact_oid FOREIGN KEY (oid)
 REFERENCES                    lock_srcact (oid) 
 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_lock_dstact_actid FOREIGN KEY (actid)
 REFERENCES                    act (id) 
 MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 
);
*/
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
--DROP VIEW IF EXISTS cls_tree CASCADE;
CREATE VIEW cls_real AS
SELECT id, pid, measure, kind, title, note,default_pid FROM
(
SELECT id, pid, 'ед.'::WHNAME AS measure,1::SMALLINT AS kind,default_pid FROM cls_num
UNION ALL
SELECT id, pid, 'шт.'::WHNAME AS measure,2::SMALLINT AS kind,default_pid FROM cls_qtyi
UNION ALL
SELECT id, pid, measure                 ,3::SMALLINT AS kind,default_pid FROM cls_qtyf
) cdif
LEFT JOIN cls USING (id);

/*
SELECT id, title, note, pid, 'ед.'::WHNAME AS measure,1::SMALLINT AS kind FROM cls_num
UNION ALL
SELECT id, title, note, pid, 'шт.'::WHNAME AS measure,2::SMALLINT AS kind FROM cls_qtyi
UNION ALL
SELECT id, title, note, pid, measure                 ,3::SMALLINT AS kind FROM cls_qtyf
*/

---------------------------------------------------------------------------------------------------

CREATE VIEW cls_tree AS
SELECT id, pid, measure, kind, title, note,default_pid FROM
(
SELECT id, pid, NULL::WHNAME AS measure, 0::SMALLINT AS kind,NULL::BIGINT AS default_pid  FROM cls_abstr
UNION ALL
SELECT id, pid, 'ед.'::WHNAME AS measure,1::SMALLINT AS kind,default_pid  FROM cls_num
UNION ALL
SELECT id, pid, 'шт.'::WHNAME AS measure,2::SMALLINT AS kind,default_pid  FROM cls_qtyi
UNION ALL
SELECT id, pid, measure                 ,3::SMALLINT AS kind,default_pid  FROM cls_qtyf
) cdif
LEFT JOIN cls USING (id);

/*
SELECT id, title, note, pid, NULL::WHNAME  AS measure,0::SMALLINT AS kind  FROM ONLY cls_abstr
UNION ALL
SELECT id, title, note, pid, measure, kind FROM cls_real
*/
---------------------------------------------------------------------------------------------------

CREATE VIEW obj_qtyf AS
SELECT id, title, cls_id, pid, move_logid, qty
FROM ONLY obj_names_qtyf n
LEFT JOIN obj_details_qtyf d ON n.id=objqty_id;

---------------------------------------------------------------------------------------------------

CREATE VIEW obj_qtyi AS
SELECT id, title, cls_id, pid, move_logid, qty
FROM  obj_names_qtyi n
LEFT JOIN obj_details_qtyi d ON n.id=objqty_id;

---------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj_tree CASCADE;
CREATE OR REPLACE VIEW obj_tree AS
SELECT id, title, cls_id, pid, move_logid, 1::NUMERIC AS qty, act_logid, 1::SMALLINT AS cls_kind, prop FROM ONLY obj_num
UNION ALL
SELECT id, title, cls_id, pid, move_logid, qty,            NULL::BIGINT, 2::SMALLINT AS cls_kind,NULL::jsonb AS prop FROM ONLY obj_qtyf
UNION ALL
SELECT id, title, cls_id, pid, move_logid, qty,            NULL::BIGINT, 3::SMALLINT AS cls_kind,NULL::jsonb AS prop FROM ONLY obj_qtyi;




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
    ,_move_logid  BIGINT
    ,_patharray   NAME[]
    ,pathid       NAME[]
    ,_path        TEXT
    
) AS $BODY$ 
BEGIN
RETURN QUERY 
    WITH RECURSIVE parents AS 
    (SELECT
        o.id, o.pid, o.title, c.id, cls.title, o.move_logid
        ,ARRAY[ ARRAY[cls.title,o.title]::NAME[] ]::NAME[] AS path
        ,ARRAY[ ARRAY[c.id,o.id]::NAME[] ]::NAME[] AS pathid
        , '/['||cls.title||']'||o.title AS _path
        FROM obj_num AS o
        LEFT JOIN cls_num c ON c.id=o.cls_id
        LEFT JOIN cls ON cls.id=c.id
         WHERE o.id = node_id   --[item.pid]
        AND o.id>1
     UNION ALL
     SELECT
        o.id, o.pid, o.title, c.id, cls.title, o.move_logid
        ,p.path || ARRAY[cls.title,o.title]::NAME[]
        ,p.pathid || ARRAY[c.id,o.id]::NAME[]
        ,'/['||cls.title||']'||o.title|| p._path
        FROM 
        parents AS p, obj_num AS o 
        LEFT JOIN cls_num c  ON c.id=o.cls_id
        LEFT JOIN cls ON cls.id=c.id
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
       FROM cls_tree AS b
       WHERE _in_id IS NOT NULL AND  b.id > 1 AND b.id = _in_id
     UNION ALL
     SELECT c.id, cls.title, 0::SMALLINT AS kind, c.pid, cls.note, NULL::WHNAME
            ,path || cls.title::NAME --,t.classname = ANY(path) 
       FROM parents AS p, cls_abstr AS c 
       LEFT JOIN cls ON cls.id=c.id
       WHERE c.id = p.pid AND c.id > 1
     )
     SELECT  * FROM parents 
);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_cls_pathinfo_table(INTEGER) TO "Guest";
--SELECT * FROM fget_cls_pathinfo_table('SmartMonitor(rev 1.0)')
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
DROP FUNCTION IF EXISTS ftg_ins_cls_tree() CASCADE;
CREATE FUNCTION ftg_ins_cls_tree() RETURNS TRIGGER AS $$
BEGIN
  NEW.id := COALESCE(NEW.id,nextval('seq_cls_id'));
  INSERT INTO cls (id, title, note) VALUES ( NEW.id, NEW.title, NEW.note);
  CASE NEW.kind 
   WHEN 0 THEN INSERT INTO cls_abstr(id, pid)VALUES (NEW.id, NEW.pid); NEW.measure:=NULL;
   WHEN 1 THEN INSERT INTO cls_num  (id, pid)VALUES (NEW.id, NEW.pid); NEW.measure:=NULL;
   WHEN 2 THEN INSERT INTO cls_qtyi (id, pid)VALUES (NEW.id, NEW.pid); NEW.measure:=NULL;
   WHEN 3 THEN INSERT INTO cls_qtyf(id, pid, measure)VALUES (NEW.id, NEW.pid, NEW.measure);
   ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW.kind ;
  END CASE;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_ii_cls_tree INSTEAD OF INSERT ON cls_tree FOR EACH ROW EXECUTE PROCEDURE ftg_ins_cls_tree();
---------------------------------------------------------------------------------------------------
-- тригер редактирования класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_upd_cls_tree() CASCADE;
CREATE FUNCTION ftg_upd_cls_tree() RETURNS TRIGGER AS $$
BEGIN
  IF NEW.id<>OLD.id THEN
    RAISE EXCEPTION ' %: can`t change id',TG_NAME;
  END IF;
  UPDATE cls SET title=NEW.title, note=NEW.note WHERE id=NEW.id;
  CASE NEW.kind 
   WHEN 0 THEN UPDATE cls_abstr SET pid=NEW.pid; NEW.measure:=NULL;
   WHEN 1 THEN UPDATE cls_num   SET pid=NEW.pid; NEW.measure:=NULL;
   WHEN 2 THEN UPDATE cls_qtyi  SET pid=NEW.pid; NEW.measure:=NULL;
   WHEN 3 THEN UPDATE cls_qtyf  SET pid=NEW.pid, measure=NEW.measure ;
   ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW.kind ;
  END CASE;
  RETURN NEW;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_iu_cls_tree INSTEAD OF UPDATE ON cls_tree FOR EACH ROW EXECUTE PROCEDURE ftg_upd_cls_tree();
---------------------------------------------------------------------------------------------------
-- тригер удаления класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_del_cls_tree() CASCADE;
CREATE FUNCTION ftg_del_cls_tree() RETURNS TRIGGER AS $$
BEGIN
  DELETE FROM cls WHERE id = OLD.id;
  RETURN OLD;
END;
$$ LANGUAGE plpgsql;
CREATE TRIGGER tr_id_cls_tree INSTEAD OF DELETE ON cls_tree FOR EACH ROW EXECUTE PROCEDURE ftg_del_cls_tree();

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
DROP FUNCTION IF EXISTS ftg_ins_obj_tree() CASCADE;
CREATE FUNCTION ftg_ins_obj_tree() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
  NEW_objqty_id BIGINT;
BEGIN
  SELECT kind INTO _kind FROM cls_real WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, NEW.cls_id ;
  END IF;

  CASE _kind
    WHEN 1 THEN 
      INSERT INTO obj_num(title, cls_id, pid ) VALUES (NEW.title, NEW.cls_id, NEW.pid);
    WHEN 2 THEN
      INSERT INTO obj_names_qtyi  (title, cls_id)VALUES(NEW.title, NEW.cls_id) RETURNING id INTO NEW_objqty_id;
      INSERT INTO obj_details_qtyi (objqty_id, pid, qty)VALUES(NEW_objqty_id, NEW.pid, NEW.qty);
    WHEN 3 THEN
      INSERT INTO obj_names_qtyf(title,cls_id)VALUES(NEW.title, NEW.cls_id) RETURNING id INTO NEW_objqty_id;
      INSERT INTO obj_details_qtyf(objqty_id, pid, qty)VALUES(NEW_objqty_id, NEW.pid, NEW.qty);
  ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
  RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_ii_obj_tree INSTEAD OF INSERT ON obj_tree FOR EACH ROW EXECUTE PROCEDURE ftg_ins_obj_tree();
---------------------------------------------------------------------------------------------------
-- тригер редактирования объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_upd_obj_tree() CASCADE;
CREATE FUNCTION ftg_upd_obj_tree() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
BEGIN
  SELECT kind INTO _kind FROM cls_real WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, NEW.cls_id ;
  END IF;
  
  IF NEW.id<>OLD.id OR NEW.cls_id<>OLD.cls_id THEN
    RAISE EXCEPTION ' %: can`t change id/cls_id',TG_NAME;
  END IF;

  CASE _kind
    WHEN 1 THEN 
      UPDATE obj_num SET 
        title=NEW.title, pid=NEW.pid, move_logid=NEW.move_logid, act_logid=NEW.act_logid, prop=NEW.prop
        WHERE id =  NEW.id;
    WHEN 2 THEN 
      UPDATE obj_names_qtyi   SET title=NEW.title WHERE id = NEW.id;
      UPDATE obj_details_qtyi SET pid=NEW.pid, move_logid=NEW.move_logid, qty=NEW.qty WHERE objqty_id = NEW.id;
    WHEN 3 THEN 
      UPDATE obj_names_qtyf   SET title=NEW.title WHERE id = NEW.id;
      UPDATE obj_details_qtyf SET pid=NEW.pid, move_logid=NEW.move_logid, qty=NEW.qty WHERE objqty_id = NEW.id;
  ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,NEW_kind ;
  END CASE;
RETURN NEW;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_iu_obj_tree INSTEAD OF UPDATE ON obj_tree FOR EACH ROW EXECUTE PROCEDURE ftg_upd_obj_tree();
---------------------------------------------------------------------------------------------------
-- тригер удаления объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_del_obj_tree() CASCADE;
CREATE FUNCTION ftg_del_obj_tree() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
BEGIN
  SELECT kind INTO _kind FROM cls_real WHERE id=OLD.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls_real',TG_NAME, OLD.cls_id ;
  END IF;

  CASE _kind
    WHEN 1 THEN DELETE FROM obj_num WHERE id = OLD.id;
    WHEN 2 THEN DELETE  FROM obj_details_qtyi WHERE objqty_id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_details_qtyi WHERE objqty_id = OLD.id AND pid=OLD.pid ;
                IF NOT FOUND THEN
                  DELETE FROM obj_names_qtyi WHERE id=OLD.id;
                END IF;
    WHEN 3 THEN DELETE  FROM obj_details_qtyf WHERE objqty_id = OLD.id AND pid=OLD.pid;
                PERFORM FROM obj_details_qtyf WHERE objqty_id = OLD.id AND pid=OLD.pid ;
                IF NOT FOUND THEN
                  DELETE FROM obj_names_qtyf WHERE id=OLD.id;
                END IF;
    ELSE RAISE EXCEPTION ' %: wrong kind %',TG_NAME,_kind ;
  END CASE;
RETURN OLD;
END;
$body$ LANGUAGE plpgsql;
CREATE TRIGGER tr_id_obj_tree INSTEAD OF DELETE ON obj_tree FOR EACH ROW EXECUTE PROCEDURE ftg_del_obj_tree();




















/*
---------------------------------------------------------------------------------------------------
-- базовая таблица деталей ИСТОРИИ действий
---------------------------------------------------------------------------------------------------
CREATE TABLE log_act_detail(
  id BIGINT NOT NULL 
);
---------------------------------------------------------------------------------------------------
-- функция создания таблицы ИСТОРИИ действий для нумерованного объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_create_log_table(IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_create_log_table(IN _id BIGINT)
    RETURNS VOID AS
$body$	
DECLARE
    new_tablename   NAME;
    --_creating_script VARCHAR;
BEGIN
    -- проверяем наличие таблички -- SELECT table_name INTO new_tablename 
    SELECT table_name INTO new_tablename
        FROM information_schema.tables WHERE table_name = 'log_act_'||_id;
    IF FOUND THEN
        RAISE EXCEPTION 'fn_create_log_table: Таблица "%" уже сужествует',new_tablename;
    END IF;
    --_creating_script :=
    EXECUTE 
       'CREATE TABLE log_act_'||_id||'(
          CONSTRAINT pk_logact_id__'||_id||'   PRIMARY KEY (id)
         ,CONSTRAINT fk_logact_id__'||_id||'   FOREIGN KEY (id)
                      REFERENCES                   log_act (id)
                      MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
           )INHERITS (log_act_detail);
        GRANT SELECT ON TABLE log_act_'       ||_id||' TO "Guest"; 
        GRANT INSERT,DELETE ON TABLE log_act_'||_id||' TO "User"; '::VARCHAR;
    --RAISE DEBUG 'fn_create_log_table: %',_creating_script;
    --EXECUTE _creating_script;
END;
$body$
LANGUAGE 'plpgsql';

--SELECT fn_create_log_table(55);
--DROP TABLE log_act_55;


---------------------------------------------------------------------------------------------------
-- тригер при удалении класса удаляет таблицу логов действий
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_bd_clsnum() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bd_clsnum()  RETURNS trigger AS
$body$
DECLARE
BEGIN
  EXECUTE 'DROP TABLE IF EXISTS log_act_'||OLD.id||' CASCADE';
RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bd_clsnum BEFORE DELETE ON cls_num FOR EACH ROW EXECUTE PROCEDURE ftr_bd_clsnum();

---------------------------------------------------------------------------------------------------
-- функция удаления ненужных столбцов из таблицы состояния (в истории удаляются)
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_clear_prop (IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_clear_prop(IN _id BIGINT)
    RETURNS VOID AS
$body$
DECLARE
    --rec        RECORD;
    col_name   NAME;
    tbl_name   NAME;
    
    -- ищем все свойства которые надо удалить, т.е. те что удалены или изменены в действии
    del_prop CURSOR (_cls_id INTEGER, tbl NAME)  IS 
        SELECT column_name FROM information_schema.columns -- выбрать все столбцы таблицы 
          WHERE table_name=tbl||'_'||_cls_id
        EXCEPT ALL                                         -- исключая те столбцы, 
        ( SELECT distinct prop.title FROM ref_act_prop -- которые есть в действиях класса
            LEFT JOIN prop ON prop.id = ref_act_prop.prop_id 
            WHERE act_id IN 
              (SELECT act_id FROM ref_cls_act WHERE cls_id=_cls_id  )
          UNION ALL SELECT column_name FROM information_schema.columns WHERE table_name='log_act_detail'
        );
BEGIN
    tbl_name := quote_ident('log_act_'||_id);
    
    FOR rec IN del_prop(_id,'log_act') LOOP
        col_name := quote_ident(rec.column_name);
        RAISE DEBUG 'fn_clear_prop: DEL COLUMN % FROM TABLE %',col_name,tbl_name;
        EXECUTE 'ALTER TABLE '||tbl_name||' DROP COLUMN IF EXISTS '||col_name;
    END LOOP;

END;
$body$
LANGUAGE 'plpgsql';

--ALTER TABLE log_act_55 ADD COLUMN TEST1 TEXT;
--SELECT fn_clear_prop(55);

---------------------------------------------------------------------------------------------------
-- функция добавлени нужных столбцов в таблицы истории
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_append_prop (IN _id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION fn_append_prop(IN _id BIGINT)
    RETURNS VOID AS
$body$
DECLARE
    --rec        RECORD;
    col_name   NAME;
    tbl_name   NAME;

    -- ищем все свойства всех действий этого класса которые надо добавлять
    new_prop CURSOR (_cls_id INTEGER, tbl NAME)  IS 
        SELECT distinct prop.title AS prop_label FROM ref_act_prop             -- которые есть в действиях класса
          LEFT JOIN prop ON prop.id = ref_act_prop.prop_id 
          WHERE act_id IN 
            (SELECT act_id FROM ref_cls_act WHERE cls_id = _cls_id )
        EXCEPT ALL -- исключая те столбцы что уже есть
        ( SELECT column_name FROM information_schema.columns 
            WHERE table_name=tbl||'_'||_cls_id
          EXCEPT ALL SELECT column_name FROM information_schema.columns WHERE table_name='log_act_detail'
        );

BEGIN
    tbl_name := quote_ident('log_act_'||_id);

    FOR rec IN new_prop(_id,'log_act') LOOP
        col_name := quote_ident(rec.prop_label);
        RAISE DEBUG 'fn_append_prop: ADD COLUMN % TO TABLE %',col_name,tbl_name;
        EXECUTE 'ALTER TABLE '||tbl_name||' ADD COLUMN '||col_name||' TEXT;';
    END LOOP;

END;
$body$
LANGUAGE 'plpgsql';
---------------------------------------------------------------------------------------------------
-- триггер срабатывающий при изменениии свойст действия
---------------------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS ftr_aiu_ref_act_prop() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aiu_ref_act_prop()  RETURNS trigger AS
$body$
DECLARE

    -- находим все классы, в которых есть действие изменённое или вставленное
    cursor_changed_class CURSOR ( _act_id INTEGER ) IS
        SELECT cls_id FROM ref_cls_act 
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
CREATE TRIGGER tg_aiu_ref_act_prop AFTER INSERT OR UPDATE OR DELETE 
   ON ref_act_prop FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_ref_act_prop();

---------------------------------------------------------------------------------------------------
-- триггер срабатывающий при изменениии действий класса
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_aiu_ref_cls_act() CASCADE;
CREATE OR REPLACE FUNCTION ftr_aiu_ref_cls_act()  RETURNS trigger AS
$body$
DECLARE
    class_id INTEGER;
BEGIN
  IF TG_OP='INSERT' THEN
    PERFORM table_name FROM information_schema.tables WHERE table_name = 'log_act_'||NEW.cls_id;
    IF NOT FOUND THEN
        PERFORM fn_create_log_table(NEW.cls_id);
    END IF;
    PERFORM fn_append_prop(NEW.cls_id);
  END IF;

  IF TG_OP='UPDATE' THEN
    PERFORM fn_clear_prop(OLD.cls_id);
    PERFORM fn_append_prop(NEW.cls_id);
  END IF;

  IF TG_OP='DELETE' THEN
    PERFORM fn_clear_prop(OLD.cls_id);
  END IF;
  
  
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_aiu_ref_cls_act 
    AFTER INSERT OR UPDATE OR DELETE ON ref_cls_act 
    FOR EACH ROW EXECUTE PROCEDURE ftr_aiu_ref_cls_act();

---------------------------------------------------------------------------------------------------
-- триггер срабатывающий при изменениии свойства
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_au_prop() CASCADE;
CREATE OR REPLACE FUNCTION ftr_au_prop()  RETURNS trigger AS
$body$
DECLARE
    -- ищем все классы в котором есть свойство 
    cursor_of_id CURSOR IS 
        SELECT DISTINCT(ref_cls_act.cls_id) AS class_id
        FROM prop
        RIGHT JOIN ref_act_prop ON prop.id = ref_act_prop.prop_id
        RIGHT JOIN ref_cls_act  ON ref_cls_act.act_id = ref_act_prop.act_id
        WHERE ref_act_prop.prop_id=OLD.id;

    changed        RECORD;
BEGIN
    IF OLD.title<>NEW.title THEN

        RAISE DEBUG 'tr_bu_act_prop: ИЗМЕНЕНО СВОЙСТВО % --> % ',OLD.title,NEW.title;
        
        FOR changed IN cursor_of_id 
        LOOP
            RAISE DEBUG 'tr_bu_act_prop: ИЗМЕНЕНО ПОЛЕ % --> % в таблицах "%" истории ',
                OLD.title, NEW.title, changed.class_id;
            EXECUTE 'ALTER TABLE  log_act_'||changed.class_id||' RENAME COLUMN '
                ||quote_ident(OLD.title)||' TO '||quote_ident(NEW.title);
    END LOOP;
    END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_au_prop AFTER UPDATE ON prop FOR EACH ROW EXECUTE PROCEDURE ftr_au_prop();

*/


---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Вставка тестовых классов и объектов';
PRINT '';
---------------------------------------------------------------------------------------------------

INSERT INTO cls_tree(id,pid,title,kind) VALUES (0,0,'nullClsRoot',0);
INSERT INTO cls_tree(id,pid,title,kind) VALUES (1,0,'AbstrClsRoot',0);
INSERT INTO cls_tree(id,pid,title,kind) VALUES (2,1,'RootNumType',1);

INSERT INTO obj_num(id,pid,title,cls_id)VALUES (0,0,'nullNumRoot',2);
INSERT INTO obj_num(id,pid,title,cls_id)VALUES (1,0,'RootObj',2);


---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- создаём 1 пустое и одно действие с свойствами';
PRINT '';
---------------------------------------------------------------------------------------------------

DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
DELETE FROM cls_tree WHERE title='TestCls';

DECLARE @cls_id_1;
SET @cls_id_1 = INSERT INTO cls_tree(pid,title,kind) VALUES (1,'TestCls',1) RETURNING id;

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
SET @qty= 10;
--SET @qty= 10;

WHILE (@qty > 0)
BEGIN

  SET @pid =     INSERT INTO cls_tree(pid,title,kind) VALUES (@pid,'clsa@qty',0)RETURNING id;
  SET @pid = CAST (@pid AS INTEGER);
  SET @cnumid =  INSERT INTO cls_tree(pid,title,kind) VALUES (@pid,'clsn@qty',1)RETURNING id;
  SET @cqtyiid = INSERT INTO cls_tree(pid,title,kind) VALUES (@pid,'clsqi@qty',2)RETURNING id;
  SET @cqtyfid = INSERT INTO cls_tree(pid,title,kind,measure) VALUES (@pid,'clsqf@qty',3, 'mes.')RETURNING id;

  SET @cnumid = CAST (@cnumid AS INTEGER);
  SET @cqtyiid = CAST (@cqtyiid AS INTEGER);
  SET @cqtyfid = CAST (@cqtyfid AS INTEGER);

  SET @opid = INSERT INTO obj_num(title,cls_id,pid)       VALUES ('objnum@cnumid',   @cnumid,@opid)RETURNING id;
  SET @oqtyiid = INSERT INTO obj_names_qtyi(title,cls_id) VALUES ('objqtyi@cqtyiid', @cqtyiid)     RETURNING id;
  SET @oqtyfid = INSERT INTO obj_names_qtyf(title,cls_id) VALUES ('objqtyf@cqtyfid', @cqtyfid)     RETURNING id;

  INSERT INTO obj_details_qtyi(objqty_id, pid, qty) VALUES (@oqtyiid, @opid, 1);
  INSERT INTO obj_details_qtyf(objqty_id, pid, qty) VALUES (@oqtyfid, @opid, 1);



  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;


/**
SELECT * FROM cls_all WHERE measure = 'шт.';


SELECT * FROM cls_tree WHERE pid between 0 and 110;


SELECT * FROM cls_real WHERE id between 104 and 118;


SELECT * FROM ONLY cls_abstr WHERE pid = 1;
*/













