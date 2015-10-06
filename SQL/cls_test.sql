

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
) INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num ("pid") ;
---------------------------------------------------------------------------------------------------
-- таблица КОЛИЧЕСТВЕННЫХ ОБЪЕКТОВ названия
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_names_qtyi
(
 CONSTRAINT pk_namesobjqtyi__id     PRIMARY KEY( id )
,CONSTRAINT uk_namesobjqtyi__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_namesobjqtyi__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyi  (    id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj);
---------------------------------------------------------------------------------------------------
CREATE TABLE obj_names_qtyf
(
 CONSTRAINT pk_namesobjqtyf__id     PRIMARY KEY( id )
,CONSTRAINT uk_namesobjqtyf__clsid_label UNIQUE( cls_id, title )
,CONSTRAINT fk_namesobjqtyf__clsid FOREIGN KEY (cls_id)
    REFERENCES                  cls_qtyf  (    id)
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
 qty        NUMERIC(20,0) NOT NULL              CHECK (qty>=0)
,CONSTRAINT pk_detailobjqtyi_oid_pid   PRIMARY KEY(objqty_id, pid)
,CONSTRAINT uk_detailobjqtyi_log_id    UNIQUE (move_logid) --при разделении 2 объекта появится с одинаковым last_log_id
,CONSTRAINT fk_detailobjqtyi_items_pid FOREIGN KEY (pid)
    REFERENCES                          obj_num    (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
,CONSTRAINT fk_detailobjqtyi_objqtykey FOREIGN KEY (objqty_id)
    REFERENCES                     obj_names_qtyi  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyi_pid ON obj_details_qtyi("pid") ;
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
    REFERENCES                 obj_names_qtyf  (id)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
) INHERITS (obj_details_qty);
CREATE INDEX idx_detailobjqtyf_pid ON obj_details_qtyf("pid") ;
CREATE INDEX idx_detailobjqtyf_id ON obj_details_qtyf("objqty_id") ;
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
    REFERENCES                        obj_names_qtyi  (id)
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
    REFERENCES                        obj_names_qtyf  (id)
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
SELECT id, pid, measure, kind, title, note FROM
(
SELECT id, pid, 'ед.'::WHNAME AS measure,1::SMALLINT AS kind FROM cls_num
UNION ALL
SELECT id, pid, 'шт.'::WHNAME AS measure,2::SMALLINT AS kind FROM cls_qtyi
UNION ALL
SELECT id, pid, measure                 ,3::SMALLINT AS kind FROM cls_qtyf
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
SELECT id, pid, measure, kind, title, note FROM
(
SELECT id, pid, NULL::WHNAME AS measure, 0::SMALLINT AS kind FROM cls_abstr
UNION ALL
SELECT id, pid, 'ед.'::WHNAME AS measure,1::SMALLINT AS kind FROM cls_num
UNION ALL
SELECT id, pid, 'шт.'::WHNAME AS measure,2::SMALLINT AS kind FROM cls_qtyi
UNION ALL
SELECT id, pid, measure                 ,3::SMALLINT AS kind FROM cls_qtyf
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
SELECT id, title, cls_id, pid, move_logid, 1::NUMERIC AS qty, act_logid, 1::SMALLINT AS cls_kind FROM ONLY obj_num
UNION ALL
SELECT id, title, cls_id, pid, move_logid, qty,            NULL::BIGINT, 2::SMALLINT AS cls_kind FROM ONLY obj_qtyf
UNION ALL
SELECT id, title, cls_id, pid, move_logid, qty,            NULL::BIGINT, 3::SMALLINT AS cls_kind FROM ONLY obj_qtyi;


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

--SELECT * FROM fget_objnum_pathinfo_table(553);


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
		_obj_pid=1
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
		_id		BIGINT,
		_title 		WHNAME,
		_kind 		SMALLINT,
		_pid 		BIGINT,
		_note 	        TEXT,
		_measure 	WHNAME,
		_path		NAME[]
		--,_cycle		BOOLEAN
		) AS $BODY$ 
BEGIN
RETURN QUERY(
	WITH RECURSIVE parents AS 
		(SELECT	c.id, cls.title, c.kind, c.pid, cls.note, c.measure
			,ARRAY[cls.title]::NAME[] AS path	--,FALSE AS cycle
		FROM cls_tree AS c 
		LEFT JOIN cls ON cls.id=c.id
		WHERE 
			_in_id IS NOT NULL 
			AND  c.id = _in_id
			AND  c.id > 1

		UNION ALL
		SELECT 	c.id, cls.title, 1::SMALLINT AS kind, c.pid, cls.note, 'шт.'::WHNAME
			,path || cls.title::NAME	--,t.classname = ANY(path) 
		FROM parents AS p, cls_num AS c 
		LEFT JOIN cls ON cls.id=c.id
		WHERE 
			c.id = p.pid 
			AND c.id > 1
		)
	SELECT  * FROM 	parents 
	);
END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION fget_cls_pathinfo_table(INTEGER) TO "Guest";
--SELECT * FROM fget_cls_pathinfo_table('SmartMonitor(rev 1.0)')
SELECT * FROM fget_cls_pathinfo_table(553);


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
PRINT '';
PRINT '- Вставка тестовых классов и объектов';
PRINT '';
---------------------------------------------------------------------------------------------------

INSERT INTO cls_tree(id,pid,title,kind) VALUES (0,0,'nullClsRoot',0);
INSERT INTO cls_tree(id,pid,title,kind) VALUES (1,0,'AbstrClsRoot',0);
INSERT INTO cls_tree(id,pid,title,kind) VALUES (2,1,'RootNumType',1);

INSERT INTO obj_num(id,pid,title,cls_id)VALUES (0,0,'nullNumRoot',2);
INSERT INTO obj_num(id,pid,title,cls_id)VALUES (1,0,'RootObj',2);
INSERT INTO act (id, title) VALUES (0, 'Move');

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
  SET @cnumid =  INSERT INTO cls_tree(pid,title,kind) VALUES (@pid,'clsn@qty',1)RETURNING id;
  SET @cqtyiid = INSERT INTO cls_tree(pid,title,kind) VALUES (@pid,'clsqi@qty',2)RETURNING id;
  SET @cqtyfid = INSERT INTO cls_tree(pid,title,kind,measure) VALUES (@pid,'clsqf@qty',3, 'mes.')RETURNING id;

  SET @opid = INSERT INTO obj_num(title,cls_id,pid)        VALUES ( 'objnum@cnumid',  @cnumid,@opid)RETURNING id;
  SET @oqtyiid = INSERT INTO obj_names_qtyi(title,cls_id) VALUES ('objqtyi@cqtyiid', @cqtyiid)RETURNING id;
  SET @oqtyfid = INSERT INTO obj_names_qtyf(title,cls_id) VALUES ('objqtyf@cqtyfid', @cqtyfid)RETURNING id;

  INSERT INTO obj_details_qtyi(objqty_id, pid, qty) VALUES (@oqtyiid, @opid, 1);
  INSERT INTO obj_details_qtyf(objqty_id, pid, qty) VALUES (@oqtyfid, @opid, 1);



  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;



SELECT * FROM cls_all WHERE measure = 'шт.';


SELECT * FROM cls_tree WHERE pid between 0 and 110;


SELECT * FROM cls_real WHERE id between 104 and 118;


SELECT * FROM ONLY cls_abstr WHERE pid = 1;














