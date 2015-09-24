
SET default_transaction_isolation =serializable;
SET client_min_messages='debug1';
SHOW client_min_messages;
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
DROP SEQUENCE IF EXISTS seq_prop_cls_id CASCADE;
DROP SEQUENCE IF EXISTS seq_ref_cls_act_id CASCADE;
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
DROP TABLE IF EXISTS perm CASCADE;
DROP TABLE IF EXISTS prop_kind CASCADE;
DROP TABLE IF EXISTS prop CASCADE;
DROP TABLE IF EXISTS prop_cls CASCADE;
DROP TABLE IF EXISTS act CASCADE;
DROP TABLE IF EXISTS ref_cls_act CASCADE;
DROP TABLE IF EXISTS ref_act_prop CASCADE;
DROP TABLE IF EXISTS obj CASCADE;
DROP TABLE IF EXISTS obj_details_qty CASCADE;
DROP TABLE IF EXISTS log CASCADE;
DROP TABLE IF EXISTS lock_src CASCADE;
DROP TABLE IF EXISTS lock_dst CASCADE;
DROP TABLE IF EXISTS lock_dstact CASCADE;



---------------------------------------------------------------------------------------------------
CREATE TABLE cls ( 
    id     BIGINT   NOT NULL DEFAULT nextval('seq_cls_id')
    ,title WHNAME   NOT NULL                          
    ,note  TEXT              DEFAULT NULL
);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_tree ( 
  pid   BIGINT   NOT NULL DEFAULT 1

,CONSTRAINT pk_clstree__id    PRIMARY KEY ( id )
,CONSTRAINT uk_clstree__title UNIQUE ( title )
,CONSTRAINT fk_clstree__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_tree( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT ck_clstree__root  CHECK (  (id=1 AND pid=1) -- main root
                               OR(id>0 AND id<>pid)   )  -- один корень                             
)INHERITS (cls);
CREATE INDEX idx_clstree_pid ON cls_tree(pid);

INSERT INTO cls_tree(id,title,pid)VALUES (1,'RootClsTree',1);
--INSERT INTO cls_tree(id,title,pid)VALUES (1,'root',0);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_qtyi ( 

 CONSTRAINT pk_clsqtyi__id    PRIMARY KEY ( id )
,CONSTRAINT uk_clsqtyi__title UNIQUE ( title )
,CONSTRAINT fk_clsqtyi__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_tree( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (cls_tree);
CREATE INDEX idx_clsqtyi_pid ON cls_qtyi(pid);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_qtyf ( 
  measure WHNAME NOT NULL

,CONSTRAINT pk_clsqtyf__id    PRIMARY KEY ( id )
,CONSTRAINT uk_clsqtyf__title UNIQUE ( title )
,CONSTRAINT fk_clsqtyf__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_tree( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (cls_tree);
CREATE INDEX idx_cls_qtyf__measure ON cls_qtyf(measure);
CREATE INDEX idx_clsqtyf_pid ON cls_qtyf(pid);
---------------------------------------------------------------------------------------------------
CREATE TABLE cls_num ( 
 CONSTRAINT pk_clsnum__id    PRIMARY KEY ( id )
,CONSTRAINT uk_clsnum__title UNIQUE ( title )
,CONSTRAINT fk_clsnum__pid   FOREIGN KEY ( pid )
    REFERENCES                    cls_tree( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (cls_tree);
CREATE INDEX idx_clsnum_pid ON cls_num(pid);

INSERT INTO cls_num(id,title,pid)VALUES (1,'RootClsNum',1);

---------------------------------------------------------------------------------------------------
--DROP VIEW IF EXISTS cls_all CASCADE;
CREATE VIEW cls_all AS
SELECT id, title, note, pid, NULL::WHNAME  AS measure FROM ONLY cls_tree
UNION ALL
SELECT id, title, note, pid, 'ед.'::WHNAME AS measure FROM cls_num
UNION ALL
SELECT id, title, note, pid, 'шт.'::WHNAME AS measure FROM cls_qtyi
UNION ALL
SELECT id, title, note, pid, measure                  FROM cls_qtyf
;


---------------------------------------------------------------------------------------------------
-- основная типов свойств
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_kind( 
 id    BIGINT NOT NULL
,title WHNAME NOT NULL
,CONSTRAINT pk_propkind__id    PRIMARY KEY ( id )
);
INSERT INTO prop_kind (id, title) VALUES (0, 'text');
INSERT INTO prop_kind (id, title) VALUES (1, 'numberic');
INSERT INTO prop_kind (id, title) VALUES (2, 'date');
INSERT INTO prop_kind (id, title) VALUES (3, 'link');
INSERT INTO prop_kind (id, title) VALUES (4, 'file');
INSERT INTO prop_kind (id, title) VALUES (5, 'array');
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
--DROP TABLE IF EXISTS prop_cls CASCADE;
CREATE TABLE prop_cls ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id  INTEGER NOT NULL
,prop_id INTEGER NOT NULL
,val     TEXT
);
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_num ( 
 CONSTRAINT pk_propnum               PRIMARY KEY ( id )
,CONSTRAINT uk_propnum__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propnum__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propnum__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_num ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (prop_cls);
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_qtyi ( 
 CONSTRAINT pk_propqtyi               PRIMARY KEY ( id )
,CONSTRAINT uk_propqtyi__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propqtyi__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propqtyi__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_qtyi ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (prop_cls);
---------------------------------------------------------------------------------------------------
CREATE TABLE prop_qtyf ( 
 CONSTRAINT pk_propqtyf               PRIMARY KEY ( id )
,CONSTRAINT uk_propqtyf__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propqtyf__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_propqtyf__clsid        FOREIGN KEY ( cls_id )
    REFERENCES                           cls_qtyf ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
)INHERITS (prop_cls);




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
INSERT INTO act (id, title) VALUES (0, 'Move');
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
GRANT SELECT ON TABLE perm               TO "Guest";
GRANT INSERT,DELETE,UPDATE ON TABLE perm TO "TypeDesigner";
---------------------------------------------------------------------------------------------------
CREATE TABLE perm_move
(
   dst_cls_id      BIGINT   NOT NULL 
  ,dst_obj_id      BIGINT            DEFAULT NULL
  ,dst_path        TEXT              DEFAULT NULL
)INHERITS (perm);
---------------------------------------------------------------------------------------------------
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
---------------------------------------------------------------------------------------------------
CREATE TABLE perm_act
(
  act_id          INTEGER   NOT NULL 
,CONSTRAINT pk_permact__id PRIMARY KEY ( id )   
,CONSTRAINT fk_permact__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permact__clsid       FOREIGN KEY (cls_id)
    REFERENCES                               cls_num(id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
-- внешний ключ на действия
,CONSTRAINT fk_permact_clsact FOREIGN KEY (cls_id, act_id) 
    REFERENCES                ref_cls_act (cls_id, act_id) 
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (perm);
CREATE INDEX idx_permact__user ON perm_act(access_group);



---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- объект базовый
---------------------------------------------------------------------------------------------------
CREATE TABLE obj (
 id         BIGINT  NOT NULL DEFAULT nextval('seq_obj_id') 
,title      WHNAME  NOT NULL
,cls_id     INTEGER NOT NULL 
);
---------------------------------------------------------------------------------------------------
-- объект номерной
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_num (
 pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT  
,act_logid  BIGINT   
,CONSTRAINT pk_objnum__id          PRIMARY KEY(id)
,CONSTRAINT uk_objnum__clsid       UNIQUE (cls_id)
,CONSTRAINT uk_objnum__clsid_title UNIQUE (cls_id, title) 
,CONSTRAINT uk_objnum__movelogid   UNIQUE (move_logid) 
,CONSTRAINT uk_objnum__actlogid    UNIQUE (act_logid) 

,CONSTRAINT fk_objnum_pid      FOREIGN KEY (pid)
    REFERENCES                 obj_num      (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_objnum_clsnum   FOREIGN KEY (cls_id)
    REFERENCES                 cls_num     (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT ck_objnum_one_root CHECK (  (id=1 AND pid=1) 
                                      OR(id<>pid AND id>1 AND pid>0 ))
) INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num ("pid") ;
INSERT INTO obj_num(id,pid,cls_id,title)  
        VALUES (1,1,(SELECT id FROM cls_num WHERE title='RootClsNum'),'RootObjNum');


---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ названия
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_qtyi
(
 CONSTRAINT pk_objqtyi__id     PRIMARY KEY( id )
,CONSTRAINT uk_objqtyi__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_objqtyi__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyi  (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj);
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_qtyf
(
 CONSTRAINT pk_objqtyf__id     PRIMARY KEY( id )
,CONSTRAINT uk_objqtyf__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_objqtyf__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyi  (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj);

---------------------------------------------------------------------------------------------------
-- таблица ДЕТАЛЕЙ КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ 
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_details_qty (
 objqty_id BIGINT
,pid        BIGINT  NOT NULL DEFAULT 1 
,move_logid BIGINT
,CONSTRAINT ck_objnum_one_root CHECK (objqty_id>1 AND pid>0 AND objqty_id<>pid)
);


---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ ЭЛЕМЕНТЫ
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_details_qtyi (
 CONSTRAINT pk_detailobjqtyi_oid_pid   PRIMARY KEY(objqty_id, pid)
,CONSTRAINT uk_detailobjqtyi_log_id    UNIQUE (move_logid) --при разделении 2 объекта появится с одинаковым last_log_id
,CONSTRAINT fk_detailobjqtyi_items_pid FOREIGN KEY (pid)
    REFERENCES                  obj_num    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_detailobjqtyi_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                 obj_qtyi  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyi_pid ON obj_details_qtyi USING btree("pid") ;
CREATE INDEX idx_detailobjqtyi_movelogid ON obj_details_qtyi("move_logid") ;
---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ ЭЛЕМЕНТЫ
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_details_qtyf (
 qty        NUMERIC NOT NULL              CHECK (qty>=0)
,CONSTRAINT pk_detailobjqtyf_oid_pid   PRIMARY KEY(objqty_id, pid)
,CONSTRAINT fk_detailobjqtyf_items_pid FOREIGN KEY (pid)
    REFERENCES                  obj_num    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_detailobjqtyf_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                 obj_qtyf  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyf_pid ON obj_details_qtyf USING btree("pid") ;
CREATE INDEX idx_detailobjqtyf_id ON obj_details_qtyf USING btree("objqty_id") ;
CREATE INDEX idx_detailobjqtyf_movelogid ON obj_details_qtyf("move_logid") ;



---------------------------------------------------------------------------------------------------
-- базовая таблица для всех логов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log CASCADE;
CREATE TABLE log (
     id        BIGINT   NOT NULL DEFAULT nextval('seq_log_id')
    ,timemark  TIMESTAMPTZ NOT NULL DEFAULT now()
    ,username  NAME NOT NULL DEFAULT CURRENT_USER

    ,src_objnum_id BIGINT    NOT NULL 
    ,src_path      BIGINT[]  NOT NULL 
);
---------------------------------------------------------------------------------------------------
-- базовая таблица логов перемещения
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move CASCADE;
CREATE TABLE log_move (
  dst_objnum_id  BIGINT    NOT NULL 
  ,dst_path      BIGINT[]  NOT NULL 

)INHERITS (log);
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения количественных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_qtyi CASCADE;
CREATE TABLE log_move_qtyi (
   objqtyi_id     BIGINT    NOT NULL 
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
    REFERENCES                        obj_qtyi  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (log_move);
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения количественных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_qtyf CASCADE;
CREATE TABLE log_move_qtyf (
   objqtyf_id     BIGINT    NOT NULL 
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
    REFERENCES                        obj_qtyf  (id)
    MATCH SIMPLE ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (log_move);
  
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_move_num CASCADE;
CREATE TABLE log_move_num (
   objnum_id     BIGINT   NOT NULL 

,CONSTRAINT pk_logmovenum__id          PRIMARY KEY (id)
,CONSTRAINT fk_logmovenum__username    FOREIGN KEY (username)
    REFERENCES                             wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION
,CONSTRAINT fk_logmovenum__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_logmovenum__dstobjnumid FOREIGN KEY (dst_objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE    
,CONSTRAINT fk_logmovenum__objnumid    FOREIGN KEY (objnum_id)
    REFERENCES                            obj_num  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
)INHERITS (log_move);
---------------------------------------------------------------------------------------------------
-- базовая таблица логов действий номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_act_ CASCADE;
CREATE TABLE log_act (
   objnum_id     BIGINT   NOT NULL 
  ,act_id        BIGINT   NOT NULL 
/*
,CONSTRAINT pk_logact__id       PRIMARY KEY (id)
,CONSTRAINT fk_logact__username FOREIGN KEY (username)
    REFERENCES                 wh_role (rolname)                   
    MATCH FULL ON UPDATE CASCADE ON DELETE NO ACTION

,CONSTRAINT fk_logact__srcobjnumid FOREIGN KEY (src_objnum_id)
    REFERENCES                        t_objnum  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_logact__objnumid FOREIGN KEY (objnum_id)
    REFERENCES                           t_objnum  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

,CONSTRAINT fk_logact__actid FOREIGN KEY (act_id)
    REFERENCES                           t_act  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
*/
)INHERITS (log);


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
    REFERENCES              obj_qtyi_details  (objqty_id, pid)
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
    REFERENCES              obj_qtyf_details  (objqty_id, pid)
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











---------------------------------------------------------------------------------------------------
-- функция поиска корня дерева наследования
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_inherit_root(IN _name NAME) CASCADE;
CREATE OR REPLACE FUNCTION get_inherit_root(IN _name NAME) 
    RETURNS NAME
    AS $BODY$ 
WITH RECURSIVE parents AS (
  SELECT 1 AS idx,inhrelid, inhparent 
    ,ARRAY[ t.inhrelid ] AS path
    FROM pg_inherits AS t 
    WHERE t.inhrelid=(SELECT oid FROM pg_class WHERE relname=_name)
  UNION ALL
    SELECT p.idx+1 AS idx,t.inhrelid, t.inhparent 
    ,path || ARRAY[t.inhrelid]
    FROM parents AS p, pg_inherits AS t 
    WHERE t.inhrelid=p.inhparent 
  )
  SELECT relname /*inhparent,pg_class.**/ FROM parents 
 LEFT JOIN pg_class ON pg_class.oid=parents.inhparent
 ORDER BY idx desc LIMIT 1
 $BODY$ LANGUAGE sql STABLE;

SELECT * FROM get_inherit_root('prop_num');
---------------------------------------------------------------------------------------------------
-- функция поиска корня дерева наследования и проверки идентификатора
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_bi_uid() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bi_uid()  RETURNS trigger AS
$body$
DECLARE
  _root_table NAME;
  _id BIGINT;
BEGIN
  --_root_table:=quote_ident(SUBSTRING(TG_TABLE_NAME, '^[[:alnum:]]+'));
  _root_table:=quote_ident(get_inherit_root(TG_TABLE_NAME));
  
  EXECUTE 'SELECT id FROM '||_root_table||' WHERE id = '||NEW.id INTO _id; 
  IF _id IS NOT NULL THEN
    RAISE EXCEPTION '%: Unique id(%) for tables %_XXX',TG_NAME,NEW.id, _root_table;
  END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';


CREATE TRIGGER tr_bi_cls_tree BEFORE INSERT ON cls_tree FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_cls_num  BEFORE INSERT ON cls_num  FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_cls_qtyi BEFORE INSERT ON cls_qtyi FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_cls_qtyf BEFORE INSERT ON cls_qtyf FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();

CREATE TRIGGER tr_bi_prop_num  BEFORE INSERT ON prop_num  FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_prop_qtyi BEFORE INSERT ON prop_qtyi FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_prop_qtyf BEFORE INSERT ON prop_qtyf FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();

CREATE TRIGGER tr_bi_obj_num  BEFORE INSERT ON obj_num FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_obj_qtyi BEFORE INSERT ON obj_qtyi FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_obj_qtyf BEFORE INSERT ON obj_qtyf FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();

CREATE TRIGGER tr_bi_perm_act       BEFORE INSERT ON perm_act       FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_perm_move_num  BEFORE INSERT ON perm_move_num  FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_perm_move_qtyi BEFORE INSERT ON perm_move_qtyi FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_perm_move_qtyf BEFORE INSERT ON perm_move_qtyf FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();

CREATE TRIGGER tr_bi_log_move_num   BEFORE INSERT ON log_move_num   FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_log_move_qtyi  BEFORE INSERT ON log_move_qtyi  FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();
CREATE TRIGGER tr_bi_log_move_qtyf  BEFORE INSERT ON log_move_qtyf  FOR EACH ROW EXECUTE PROCEDURE ftr_bi_uid();






delete from cls_tree WHERE id>5000;
delete from cls_num WHERE id>5000;
delete from cls_qtyi WHERE id>5000;
delete from cls_qtyf WHERE id>5000;

delete from obj WHERE id>5000;

INSERT INTO cls_num(id,title)         VALUES (10000,'clsn10000');
INSERT INTO cls_qtyi(id,title)        VALUES (10001,'clsi10001');
INSERT INTO cls_qtyf(id,title)        VALUES (10000,'clsqf10000');


INSERT INTO obj_num(id,title,cls_id,pid) VALUES (10000,'objnum10000',10000,1);
INSERT INTO obj_qtyi(id,title,cls_id)    VALUES (10001,'objqtyi10001',10001);
INSERT INTO obj_qtyi(id,title,cls_id)    VALUES (10000,'objqtyi10000',10000);



/*
---------------------------------------------------------------------------------------------------
DECLARE @clstitle, @qty, @pid ;
--SET @qty= 400;
SET @qty= 10;

WHILE (@qty > 0)
BEGIN
  SET @pid = INSERT INTO cls_tree(title) VALUES ('clsa@qty') RETURNING id;
  INSERT INTO cls_num(title,pid)         VALUES ('clsn@qty',  @pid);
  INSERT INTO cls_qtyi(title,pid)        VALUES ('clsqi@qty', @pid);
  INSERT INTO cls_qtyf(title,pid,measure)VALUES ('clsqf@qty', @pid, '(mes.)');

  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;
*/


SELECT * FROM cls_all WHERE measure = 'шт.';


SELECT * FROM cls_all WHERE pid between 0 and 110;


SELECT * FROM cls WHERE id between 104 and 118;


SELECT * FROM cls_all WHERE pid = 0 OR pid<110;














