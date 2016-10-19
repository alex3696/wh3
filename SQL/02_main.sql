SET default_transaction_isolation =serializable;

BEGIN TRANSACTION;

--SET client_min_messages = 'error';
SET client_min_messages = 'warning';
--SET client_min_messages = 'notice';
--SET client_min_messages = 'debug';
--SHOW client_min_messages;
SHOW client_min_messages;
--SET enable_seqscan = ON;
---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- очистка
---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
-- расширения
DROP EXTENSION IF EXISTS pg_trgm CASCADE;
-- домены
DROP DOMAIN IF EXISTS WHNAME CASCADE; 
DROP DOMAIN IF EXISTS TMPPATH CASCADE; 
-- идентификаторы
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
-- таблицы(вьюшки) + их триггеры
  -- классы
  DROP TABLE IF EXISTS acls CASCADE;
    DROP FUNCTION IF EXISTS ftg_ins_cls() CASCADE;
    DROP FUNCTION IF EXISTS ftg_del_cls() CASCADE;
    DROP FUNCTION IF EXISTS ftg_upd_cls() CASCADE;
    DROP FUNCTION IF EXISTS ftr_bu_acls() CASCADE;
  -- объекты
  DROP TABLE IF EXISTS obj_name CASCADE;
  DROP TABLE IF EXISTS obj_num CASCADE;
  DROP TABLE IF EXISTS obj_qtyi  CASCADE;
  DROP TABLE IF EXISTS obj_qtyf  CASCADE;

  DROP VIEW IF EXISTS obj CASCADE;
    DROP FUNCTION IF EXISTS ftg_ins_obj() CASCADE;
    DROP FUNCTION IF EXISTS ftg_del_obj() CASCADE;
    DROP FUNCTION IF EXISTS ftg_upd_obj() CASCADE;
  -- свойства
  DROP TABLE IF EXISTS prop CASCADE;
  DROP TABLE IF EXISTS prop_cls CASCADE;
  DROP TABLE IF EXISTS prop_kind CASCADE;
  DROP TABLE IF EXISTS favorite_prop CASCADE;

-- действия
  DROP TABLE IF EXISTS act CASCADE;

  -- разрешения действий
  DROP TABLE IF EXISTS perm_act CASCADE;
    DROP FUNCTION IF EXISTS ftr_aud_perm_act() CASCADE;
    DROP FUNCTION IF EXISTS ftr_biu_perm_act() CASCADE;

  -- разрешения перемещений
  DROP TABLE IF EXISTS perm_move CASCADE;

  -- связывающие таблицы
  DROP TABLE IF EXISTS ref_cls_act CASCADE;
  DROP TABLE IF EXISTS ref_act_prop CASCADE;

  -- история
  DROP TABLE IF EXISTS log_main CASCADE;
    DROP FUNCTION IF EXISTS ftr_bd_log_main() CASCADE;
  DROP TABLE IF EXISTS log_detail_act CASCADE;
  DROP TABLE IF EXISTS log_detail_move CASCADE;

  DROP VIEW IF EXISTS log CASCADE;
    DROP FUNCTION IF EXISTS ftg_del_log() CASCADE;
    DROP FUNCTION IF EXISTS ftr_bu_any_obj() CASCADE;

  -- бизнес блокировки
  DROP TABLE IF EXISTS lock_obj CASCADE;
  DROP TABLE IF EXISTS lock_dst CASCADE;
  DROP TABLE IF EXISTS lock_act CASCADE;
  
  DROP VIEW IF EXISTS moverule_lockup CASCADE;

  DROP FUNCTION IF EXISTS    try_lock_obj(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
  DROP FUNCTION IF EXISTS    lock_reset(IN _oid  BIGINT, IN _pid BIGINT) CASCADE;
  DROP FUNCTION IF EXISTS    lock_for_act(IN _oid  BIGINT, IN _opid  BIGINT) CASCADE;
  DROP FUNCTION IF EXISTS    do_act(IN _obj_id BIGINT, _act_id BIGINT, IN _prop JSONB) CASCADE;
  DROP FUNCTION IF EXISTS    lock_for_move( IN _obj_id  BIGINT, IN _old_pid BIGINT) CASCADE;
  DROP FUNCTION IF EXISTS    do_move( _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT
                                   , IN _qty NUMERIC ) CASCADE;
  DROP FUNCTION IF EXISTS    do_move_objnum( _oid  BIGINT, _new_opid BIGINT) CASCADE;
  DROP FUNCTION IF EXISTS    do_move_objqtyi( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;
  DROP FUNCTION IF EXISTS    do_move_objqtyf( IN _oid  BIGINT, IN _old_opid BIGINT,  IN _new_opid BIGINT, IN _qty NUMERIC ) CASCADE;


-- вспомогательные функции
DROP FUNCTION IF EXISTS get_childs_cls(IN _cid BIGINT) CASCADE;

DROP FUNCTION IF EXISTS get_path_cls_info(_cid BIGINT,_cpid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls_arr_id(_cid BIGINT,_cpid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls_arr_title(_cid BIGINT,_cpid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls(_cid BIGINT,_cpid BIGINT) CASCADE;


DROP FUNCTION IF EXISTS get_path_obj_info(_oid BIGINT,_opid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_id(_oid BIGINT,_opid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_2id(_oid BIGINT,_opid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_2title(_oid BIGINT,_opid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj(_oid BIGINT,_opid BIGINT) CASCADE;

DROP FUNCTION IF EXISTS fn_array1_to_table(anyarray);
DROP FUNCTION IF EXISTS obj_arr_id_to_obj_info(IN anyarray);

DROP FUNCTION IF EXISTS fn_array2_to_table(IN anyarray);
DROP FUNCTION IF EXISTS tmppath_to_2id_info(IN TEXT,BIGINT );

DROP FUNCTION IF EXISTS ftr_bu_acls() CASCADE;
DROP FUNCTION IF EXISTS ftr_bu_any_obj() CASCADE;


---------------------------------------------------------------------------------------------------
-- создание основных элементов
---------------------------------------------------------------------------------------------------
REVOKE CREATE ON DATABASE wh3 FROM public CASCADE; -- запретить создавать схемы
REVOKE CREATE ON SCHEMA public FROM public CASCADE;-- запретить создавать в схемах
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON SEQUENCES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON TABLES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON FUNCTIONS FROM public;
SELECT whgrant_grouptouser('Admin','postgres');
-- расширения
CREATE EXTENSION IF NOT EXISTS pg_trgm;
-- домены
CREATE DOMAIN WHNAME AS VARCHAR
   -- CHECK (VALUE ~ '^([[:alnum:][:space:]''"!()*+,-.:;<=>^_|№])+$') 
   ;

CREATE DOMAIN TMPPATH AS VARCHAR
   COLLATE pg_catalog."C"
   CHECK (VALUE ~ '^{((((%*)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') ;

-- идентификаторы
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
-- перечень всех классов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS acls CASCADE;
CREATE TABLE acls ( 
  id       BIGINT   NOT NULL DEFAULT nextval('seq_cls_id') CHECK ( id>=0 )
  ,pid     BIGINT   NOT NULL DEFAULT 1
  ,title   WHNAME   NOT NULL
  ,note    TEXT              DEFAULT NULL
  ,kind    SMALLINT NOT NULL  CHECK ( kind BETWEEN 0 AND 3 )
  ,pkind   SMALLINT NOT NULL DEFAULT 0 CHECK ( pkind=0 )
  
  ,dobj    BIGINT            DEFAULT 1
  ,measure WHNAME            DEFAULT NULL
  --,path    ltree    NOT NULL 
  --,guipath VARCHAR  --NOT NULL

,CONSTRAINT pk_acls__id      PRIMARY KEY(id)
,CONSTRAINT pk_acls__id_kind UNIQUE(id,kind)
,CONSTRAINT fk_acls__id_pid  FOREIGN KEY (pid,pkind)
      REFERENCES acls (id,kind) MATCH FULL
      ON UPDATE RESTRICT ON DELETE RESTRICT

,CONSTRAINT ck_acls_root CHECK(  (id=0 AND pid=0)                 -- main root
                               OR(id=1 AND pid=0)                 -- Cls0 - class root 
                               OR(id>1 AND pid>0 AND id<>pid)   ) -- other
,CONSTRAINT ck_acls_mess CHECK(  (kind=0 AND measure IS NULL)
                               OR(kind>0 AND measure IS NOT NULL) ) --если нет единиц измерения класс должен быть абстрактным
,CONSTRAINT ck_acls_dobj CHECK(  (kind=0 AND dobj IS NULL)
                               OR(kind>0 AND measure IS NOT NULL) )
);
CREATE INDEX idx_acls__pid   ON acls(pid);
CREATE UNIQUE INDEX uidx_acls__title_vcharops ON acls(title varchar_pattern_ops);
CREATE INDEX idx_acls__title_tgm ON acls USING gin (title gin_trgm_ops);
--CREATE UNIQUE INDEX uidx_acls__title_bitmap ON acls(title);
--CREATE INDEX idx_acls__path  ON acls USING GIST(path);
--CREATE INDEX idx_acls__abstract_id ON acls(id) WHERE (kind>0);
--CREATE INDEX idx_acls__num_id      ON acls(id) WHERE (kind=1);
--CREATE INDEX idx_acls__qtyn_id     ON acls(id) WHERE (kind=2);
--CREATE INDEX idx_acls__qtyf_id     ON acls(id) WHERE (kind=3);
GRANT SELECT        ON TABLE acls  TO "Guest";
GRANT INSERT        ON TABLE acls  TO "TypeDesigner";
GRANT DELETE        ON TABLE acls  TO "Admin";
GRANT UPDATE(pid,title,note,dobj,measure) ON TABLE acls  TO "TypeDesigner";
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

INSERT INTO prop_kind (id, title) VALUES (0,  'text')
                                        ,(1,  'name')
                                        ,(2,  'ftTextArray')
                                        ,(100, 'long')
                                        ,(101, 'double')
                                        ,(200, 'TimeStamp')
                                        ,(201, 'Date')
                                        ,(202, 'Time')
                                        ,(300, 'link')
                                        ,(400, 'file')
                                        ,(500, 'ftJSON')
                                        ,(600, 'ftBool');
---------------------------------------------------------------------------------------------------
-- основная описания свойств(переменных) действий
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop CASCADE;
CREATE TABLE prop ( 
 id    BIGINT   NOT NULL DEFAULT nextval('seq_prop_id')
,title WHNAME   NOT NULL
,kind  SMALLINT NOT NULL DEFAULT 0 
,var   TEXT ARRAY        DEFAULT NULL
,var_strict BOOLEAN      DEFAULT FALSE
,CONSTRAINT pk_prop__id    PRIMARY KEY ( id )
,CONSTRAINT uk_prop__title UNIQUE ( title )
,CONSTRAINT fk_prop__kind   FOREIGN KEY ( kind )
    REFERENCES                 prop_kind( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
);
CREATE INDEX idx_prop__title_vpo ON prop(title varchar_pattern_ops);
CREATE INDEX idx_prop__title_tgm ON prop USING gin (title gin_trgm_ops);

GRANT SELECT        ON TABLE prop  TO "Guest";
GRANT INSERT        ON TABLE prop  TO "TypeDesigner";
GRANT DELETE        ON TABLE prop  TO "TypeDesigner";
GRANT UPDATE(title,kind,var,var_strict)
                    ON TABLE prop  TO "TypeDesigner";
---------------------------------------------------------------------------------------------------
-- таблица свойств классов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS prop_cls CASCADE;
CREATE TABLE prop_cls ( 
 id      BIGINT  NOT NULL DEFAULT nextval('seq_prop_cls_id')
,cls_id   BIGINT  NOT NULL
,cls_kind SMALLINT NOT NULL CHECK (cls_kind BETWEEN 1 AND 3)
,prop_id BIGINT NOT NULL
,val     TEXT
,CONSTRAINT pk_propcls               PRIMARY KEY ( id )
,CONSTRAINT uk_propcls__propid_clsid UNIQUE ( prop_id, cls_id )
,CONSTRAINT fk_propcls__propid       FOREIGN KEY ( prop_id )
    REFERENCES                              prop ( id )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_propcls__clsid        FOREIGN KEY ( cls_id,cls_kind )
    REFERENCES                           acls( id,kind )
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
,color  VARCHAR(64)     DEFAULT NULL
,script TEXT            DEFAULT NULL
,CONSTRAINT pk_act__id    PRIMARY KEY ( id  )
,CONSTRAINT uk_act__title UNIQUE ( title )
);
CREATE INDEX idx_act__title_vpo ON act(title varchar_pattern_ops);
CREATE INDEX idx_act__title_tgm ON act USING gin (title gin_trgm_ops);

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
    ,cls_id   BIGINT  NOT NULL
    ,cls_kind SMALLINT NOT NULL DEFAULT 1 CHECK (cls_kind BETWEEN 0 AND 3 )
    ,act_id   BIGINT  NOT NULL

,CONSTRAINT pk_refclsact__id    PRIMARY KEY ( id )
,CONSTRAINT uk_refclsact_clsid_actid UNIQUE (cls_id, act_id)
,CONSTRAINT fk_refclsact__clsid FOREIGN KEY (cls_id, cls_kind )
    REFERENCES                      acls ( id, kind )
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
    ,act_id  BIGINT NOT NULL
    ,prop_id BIGINT NOT NULL

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

  ,src_cls_id   BIGINT   NOT NULL 
  ,src_cls_kind SMALLINT NOT NULL DEFAULT 1 --CHECK ((src_cls_kind=1 AND src_cls_id>1)OR(src_cls_kind=0 AND src_cls_id=1))
  ,src_obj_id  BIGINT            DEFAULT NULL
  ,src_path    TMPPATH  NOT NULL DEFAULT '{%}'

  ,cls_id      BIGINT   NOT NULL 
  ,obj_id      BIGINT            DEFAULT NULL
  
  ,dst_cls_id  BIGINT   NOT NULL 
  ,dst_cls_kind SMALLINT NOT NULL DEFAULT 1 --CHECK ((dst_cls_kind=1 AND dst_cls_id>1)OR(dst_cls_kind=0 AND dst_cls_id=1))
  ,dst_obj_id  BIGINT            DEFAULT NULL
  ,dst_path    TMPPATH  NOT NULL DEFAULT '{%}'

,CONSTRAINT pk_permmove__id PRIMARY KEY ( id )   
,CONSTRAINT fk_permmove__acess_group FOREIGN KEY (access_group) 
    REFERENCES                               wh_role (rolname)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmove__srcclsid       FOREIGN KEY (src_cls_id,src_cls_kind)
    REFERENCES                               acls(id,kind)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
,CONSTRAINT fk_permmove__dstclsid       FOREIGN KEY (dst_cls_id,dst_cls_kind)
    REFERENCES                               acls(id,kind)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

--CONSTRAINT ck_perm_act_src_path
--    CHECK (src_path ~ '^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') 
-- CONSTRAINT ck_perm_act_dst_path
--    CHECK (dst_path ~ '^{((((%+)|({(%|[[:digit:]]+),(%|[[:digit:]]+)})),?)+)}$') 
);--INHERITS (perm);
CREATE INDEX idx_permmove__user      ON perm_move(access_group);
CREATE INDEX idx_permmove__srcclsobj ON perm_move(src_cls_id, src_obj_id);
CREATE INDEX idx_permmove__clsobj    ON perm_move(cls_id,     obj_id);
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

  ,act_id          BIGINT   NOT NULL 
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
CREATE INDEX idx_permact__user      ON perm_act(access_group);
CREATE INDEX idx_permact__srcclsobj ON perm_act(cls_id, obj_id);
CREATE INDEX idx_permact__actid     ON perm_act(act_id);
--CREATE INDEX perm_act_src_path_idx ON perm_act (src_path text_pattern_ops);
-- для кодировки "C" не надо отдельно делать индекс для текстовых операций
-- а для полей шаблонов путей в домене указана кодировка С
CREATE INDEX perm_act_src_path_idx ON perm_act (src_path);

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
,cls_id     BIGINT  NOT NULL 
,cls_kind   SMALLINT NOT NULL CHECK (cls_kind BETWEEN 1 AND 3 )
,move_logid BIGINT
,act_logid  BIGINT   UNIQUE
,prop       JSONB

,CONSTRAINT pk_objname__id               PRIMARY KEY(id)
,CONSTRAINT uk_objname__id_cid_ckind     UNIQUE (id, cls_id,cls_kind)
,CONSTRAINT uk_objname__title_cid        UNIQUE (title, cls_id)
,CONSTRAINT uk_objname__movelogid        UNIQUE (move_logid) 

,CONSTRAINT fk_obj__cls         FOREIGN KEY (cls_id,cls_kind)
    REFERENCES                  acls    ( id, kind )
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_objname__prop  ON obj_name USING gin ("prop") ;
CREATE INDEX idx_objname__title_vpo ON obj_name (title varchar_pattern_ops) ;
CREATE INDEX idx_objname__title_tgm ON obj_name USING gin (title gin_trgm_ops);
CREATE INDEX idx_objname__cid  ON obj_name (cls_id);
CREATE UNIQUE INDEX idx_objname__id_cid  ON obj_name(id,cls_id);


GRANT SELECT        ON TABLE obj_name  TO "Guest";
GRANT INSERT        ON TABLE obj_name  TO "ObjDesigner";
GRANT DELETE        ON TABLE obj_name  TO "ObjDesigner";
GRANT UPDATE ( title) ON TABLE obj_name   TO "ObjDesigner";
GRANT UPDATE ( move_logid, act_logid, prop) ON TABLE obj_name  TO "User";
---------------------------------------------------------------------------------------------------
-- детальные сведения объект номерной
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS obj_num CASCADE;
CREATE TABLE obj_num (
  id         BIGINT  NOT NULL  CHECK (  (id=0 AND pid=0) OR( id>0 AND id<>pid ))
 ,cls_id     BIGINT  NOT NULL 
 ,cls_kind   BIGINT  NOT NULL DEFAULT 1 CHECK (cls_kind=1)
 ,pid        BIGINT  NOT NULL DEFAULT 1 
     REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT

 ,CONSTRAINT pk_objnum__id          PRIMARY KEY(id)
 ,CONSTRAINT uk_objnum__id_cid      UNIQUE (id,cls_id)
 ,CONSTRAINT fk_objnum__idclsid     FOREIGN KEY (id,cls_id,cls_kind)
    REFERENCES                          obj_name(id,cls_id,cls_kind)
    MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE


);-- INHERITS (obj);
CREATE INDEX idx_objnum_pid ON obj_num (pid) ;

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
 ,cls_id     BIGINT  NOT NULL
 ,cls_kind   BIGINT  NOT NULL DEFAULT 2 CHECK (cls_kind=2)
 ,pid        BIGINT        NOT NULL CHECK(pid>0) DEFAULT 1 
      REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
 ,qty        NUMERIC(20,0) NOT NULL CHECK (qty>=0)
 ,CONSTRAINT uk_obj_qtyi__id_pid UNIQUE ( id, pid )   

 ,CONSTRAINT fk_objqtyi__id_cid_ckind  FOREIGN KEY (id,cls_id,cls_kind)
   REFERENCES                            obj_name(id,cls_id,cls_kind)
   MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_objqtyi_pid ON obj_qtyi (pid) ;
CREATE INDEX idx_objqtyi_id_cid ON obj_qtyi (id, cls_id);

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
 ,cls_id     BIGINT  NOT NULL 
 ,cls_kind   BIGINT  NOT NULL DEFAULT 3 CHECK (cls_kind=3)
 ,pid        BIGINT        NOT NULL CHECK(pid>0) DEFAULT 1 
      REFERENCES obj_num( id )       MATCH FULL ON UPDATE CASCADE ON DELETE SET DEFAULT
 ,qty        NUMERIC NOT NULL CHECK (qty>=0)
 ,CONSTRAINT uk_obj_qtyf__id_pid UNIQUE ( id, pid )   

  ,CONSTRAINT fk_objqtyf__id_cid_ckind FOREIGN KEY (id,cls_id,cls_kind)
   REFERENCES                             obj_name(id,cls_id,cls_kind)
   MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);
CREATE INDEX idx_objqtyf_pid ON obj_qtyf (pid) ;
CREATE INDEX idx_objqtyf_id_cid ON obj_qtyf (id, cls_id);

GRANT SELECT        ON TABLE obj_qtyf  TO "Guest";
GRANT INSERT        ON TABLE obj_qtyf  TO "User";
GRANT DELETE        ON TABLE obj_qtyf  TO "User";
GRANT UPDATE (pid, qty)
                    ON TABLE obj_qtyf  TO "User";


---------------------------------------------------------------------------------------------------
-- таблица общих сведений действий объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_main CASCADE;
CREATE TABLE log_main (
   id       BIGINT   NOT NULL DEFAULT nextval('seq_log_id') PRIMARY KEY
  ,timemark TIMESTAMPTZ NOT NULL DEFAULT now()
  ,username NAME NOT NULL DEFAULT CURRENT_USER REFERENCES wh_role(rolname) MATCH FULL ON UPDATE CASCADE ON DELETE RESTRICT
  ,src_path BIGINT[] 
  ,obj_id   BIGINT   NOT NULL REFERENCES obj_name( id ) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE 
);
CREATE INDEX idx_logmain__timemark ON log_main  (timemark DESC);
CREATE INDEX idx_logmain__src_cid  ON log_main  ((src_path[1][1]));
CREATE INDEX idx_logmain__src_oid  ON log_main  ((src_path[1][2]));
CREATE INDEX idx_logmain__oid      ON log_main  (obj_id);
GRANT SELECT        ON TABLE log_main  TO "Guest";
GRANT INSERT        ON TABLE log_main  TO "User";
GRANT DELETE        ON TABLE log_main  TO "User";
---------------------------------------------------------------------------------------------------
-- таблица логов действий номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_detail_act CASCADE;
CREATE TABLE log_detail_act (
   id      BIGINT   NOT NULL REFERENCES log_main(id)  MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE 
  ,act_id  BIGINT   NOT NULL REFERENCES act( id )     MATCH FULL ON UPDATE RESTRICT ON DELETE RESTRICT 
  ,prop    JSONB
  ,CONSTRAINT pk_logactdet__id UNIQUE (id)
);
CREATE INDEX idx_logactdet__aid   ON log_detail_act  (act_id);
GRANT SELECT        ON TABLE log_detail_act  TO "Guest";
GRANT INSERT        ON TABLE log_detail_act  TO "User";
GRANT DELETE        ON TABLE log_detail_act  TO "User";
---------------------------------------------------------------------------------------------------
-- таблица логов перемещения номерных объектов
---------------------------------------------------------------------------------------------------
DROP TABLE IF EXISTS log_detail_move CASCADE;
CREATE TABLE log_detail_move (
   id       BIGINT    NOT NULL REFERENCES log_main(id) MATCH FULL ON UPDATE RESTRICT ON DELETE CASCADE 
  ,dst_path BIGINT[]  
  ,qty      NUMERIC   NOT NULL 
  ,prop_lid BIGINT             REFERENCES log_detail_act(id) MATCH FULL ON UPDATE RESTRICT ON DELETE SET NULL 
  ,CONSTRAINT pk_logmovdet__id UNIQUE (id)
) ;
CREATE INDEX idx_logmovdet__prop_lid  ON log_detail_move (prop_lid);
CREATE INDEX idx_logmovdet__dst_cid   ON log_detail_move ((dst_path[1][1]));
CREATE INDEX idx_logmovdet__dst_oid   ON log_detail_move ((dst_path[1][2]));
GRANT SELECT        ON TABLE log_detail_move  TO "Guest";
GRANT INSERT        ON TABLE log_detail_move  TO "User";
GRANT DELETE        ON TABLE log_detail_move  TO "User";
-----------------------------------------------------------------------------------------------------------------------------
-- таблица избранных свойст объектов отображаемых при просмотре пользователем каталога объектов по типу или по местоположению
-----------------------------------------------------------------------------------------------------------------------------
--DROP TABLE IF EXISTS favorite_prop CASCADE;
CREATE TABLE favorite_prop(
    id           BIGINT NOT NULL DEFAULT nextval('favorite_prop_id_seq') UNIQUE
    ,user_label  NAME      NOT NULL DEFAULT CURRENT_USER 
    ,cls_id      BIGINT       NOT NULL
    ,act_id      BIGINT   NOT NULL
    ,prop_id     BIGINT   NOT NULL

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







---------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------
DROP VIEW IF EXISTS obj CASCADE;
CREATE VIEW obj AS
SELECT id, pid, title, cdif.cls_id, prop, qty, move_logid, act_logid,cdif.cls_kind FROM
(
SELECT id, pid,1::NUMERIC AS qty,cls_id,cls_kind FROM obj_num
UNION ALL
SELECT id, pid, qty,cls_id,cls_kind FROM obj_qtyi
UNION ALL
SELECT id, pid, qty,cls_id,cls_kind FROM obj_qtyf
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
            INSERT INTO ref_cls_act(cls_id, act_id,cls_kind) VALUES (NEW.cls_id, NEW.act_id,(SELECT kind FROM acls WHERE id=NEW.cls_id));
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
-- тригер создания объекта
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftg_ins_obj() CASCADE;
CREATE FUNCTION ftg_ins_obj() RETURNS TRIGGER AS $body$
DECLARE
  _kind SMALLINT;
BEGIN
  SELECT kind INTO _kind FROM acls WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
  IF NOT FOUND THEN
    RAISE EXCEPTION ' %: cls.id=% not present in cls table',TG_NAME, NEW.cls_id ;
  END IF;

  NEW.id := COALESCE(NEW.id,nextval('seq_obj_id'));

  PERFORM FROM obj WHERE id=NEW.id;-- проверяем существование количественного объета
  IF NOT FOUND THEN
    INSERT INTO obj_name(id, title, cls_id,cls_kind)VALUES(NEW.id, NEW.title, NEW.cls_id,_kind);
  END IF;
  
  CASE _kind
    WHEN 1 THEN INSERT INTO obj_num(id,cls_id, pid)VALUES (NEW.id,NEW.cls_id,NEW.pid);NEW.qty:=1;
    WHEN 2 THEN 
      IF (ceil(NEW.qty)<>NEW.qty)THEN
        RAISE EXCEPTION ' qty is not integer NEW.qty=%', NEW.qty;
      END IF;
      INSERT INTO obj_qtyi(id,cls_id, pid, qty) VALUES (NEW.id,NEW.cls_id,NEW.pid,NEW.qty);
    WHEN 3 THEN INSERT INTO obj_qtyf(id,cls_id, pid, qty) VALUES (NEW.id,NEW.cls_id,NEW.pid,NEW.qty);
    ELSE RAISE EXCEPTION ' %: wrong kind = %',TG_NAME , _kind ;
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
  SELECT kind INTO _kind FROM acls WHERE id=NEW.cls_id;-- _kind находим сами не пользуя NEW.kind
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

   RAISE DEBUG '%: NEW=% OLD=%',TG_NAME,NEW,OLD;

  CASE _kind
    WHEN 1 THEN 
      UPDATE obj_num  SET pid=NEW.pid WHERE id =  NEW.id;
    WHEN 2 THEN 
      IF (ceil(NEW.qty)<>NEW.qty)THEN
        RAISE EXCEPTION ' qty is not integer NEW.qty=%', NEW.qty;
      END IF;
      UPDATE obj_qtyi SET pid=NEW.pid, qty=NEW.qty WHERE id = NEW.id AND pid=OLD.pid;
    WHEN 3 THEN 
      RAISE DEBUG '%: UPDATE obj_qtyf SET pid=%, qty=% WHERE id = % AND pid=%',TG_NAME,NEW.pid,NEW.qty,NEW.id,OLD.pid;
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
  SELECT kind INTO _kind FROM acls WHERE id=OLD.cls_id;-- _kind находим сами не пользуя NEW.kind
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
-- Вставка базовых классов и объектов
---------------------------------------------------------------------------------------------------

INSERT INTO acls(id,pid,title,kind,dobj) VALUES (0,0,'nullClsRoot',0,NULL);
INSERT INTO acls(id,pid,title,kind,dobj) VALUES (1,0,'ClsRoot',0,NULL);
INSERT INTO acls(id,pid,title,kind,measure) VALUES (2,1,'RootNumType',1,'шт.');

INSERT INTO obj(id,pid,title,cls_id)VALUES (0,0,'nullNumRoot',2);
INSERT INTO obj(id,pid,title,cls_id)VALUES (1,0,'ObjRoot',2);


ALTER TABLE acls
  ADD CONSTRAINT cls_default_objid_fkey FOREIGN KEY (dobj)
      REFERENCES obj_num (id) MATCH FULL
      ON UPDATE RESTRICT ON DELETE SET DEFAULT;


COMMIT TRANSACTION;