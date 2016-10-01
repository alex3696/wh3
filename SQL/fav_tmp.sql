BEGIN TRANSACTION;

--SET client_min_messages = 'error';
--SET client_min_messages = 'debug';
SET client_min_messages = 'NOTICE';
--SHOW client_min_messages=OFF;

DROP SEQUENCE IF EXISTS seq_fav_id CASCADE;

DROP TABLE IF EXISTS fav CASCADE;
DROP TABLE IF EXISTS fav_filter_by_cls_id CASCADE;
DROP TABLE IF EXISTS fav_filter_by_parent_obj_id CASCADE;
DROP TABLE IF EXISTS fav_view_prop_cls CASCADE;
DROP TABLE IF EXISTS fav_view_prop_act_common CASCADE;
DROP TABLE IF EXISTS fav_view_prop_act_usr CASCADE;



CREATE SEQUENCE seq_fav_id INCREMENT 1 MINVALUE 0 NO MAXVALUE START 100;


CREATE TABLE fav
(
  id    bigint NOT NULL DEFAULT nextval('seq_fav_id'::regclass)
 ,title name   NOT NULL 
 ,usr   name   NOT NULL DEFAULT "current_user"()

 ,grp   SMALLINT NOT NULL DEFAULT 1 -- 0:без группировки 1:группировка по типу 2:группировка по месту

  ,CONSTRAINT pk_fav__id PRIMARY KEY (id)
  /*
  cls_id integer NOT NULL,
  act_id integer NOT NULL,
  prop_id integer NOT NULL,
  
  CONSTRAINT fk_favoriteprop__ref_act_prop FOREIGN KEY (act_id, prop_id)
      REFERENCES public.ref_act_prop (act_id, prop_id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_favoriteprop__ref_class_act FOREIGN KEY (cls_id, act_id)
      REFERENCES public.ref_cls_act (cls_id, act_id) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT fk_favoriteprop__user_label FOREIGN KEY (user_label)
      REFERENCES public.wh_role (rolname) MATCH FULL
      ON UPDATE CASCADE ON DELETE CASCADE,
  CONSTRAINT uk_favoriteprop__id UNIQUE (user_label, cls_id, act_id, prop_id)
  */
);

--CREATE TABLE favorite_filter_by_connected_act

CREATE TABLE fav_filter_by_cls_id(
  fid BIGINT NOT NULL --REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,cid BIGINT NOT NULL --REFERENCES acls (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 ,CONSTRAINT fk_ffcid__fid__fav
    FOREIGN KEY     (fid)
    REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,CONSTRAINT fk_ffcid__cid__cls
    FOREIGN KEY     (cid)
    REFERENCES acls (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE fav_filter_by_parent_obj_id(
  fid BIGINT NOT NULL --REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,oid BIGINT NOT NULL --REFERENCES acls (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 ,CONSTRAINT fk_ffoid__fid__fav
    FOREIGN KEY     (fid)
    REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,CONSTRAINT fk_ffoid__oid__cls
    FOREIGN KEY     (oid)
    REFERENCES obj_num (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE fav_view_prop_cls(
  fid BIGINT NOT NULL --REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,pid BIGINT NOT NULL --REFERENCES acls (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 ,CONSTRAINT fk_fv_prop_cls__fid__fav
    FOREIGN KEY     (fid)
    REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
 ,CONSTRAINT fk_fv_prop_cls__pid__prop
    FOREIGN KEY     (pid)
    REFERENCES prop (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
    
);


CREATE TABLE fav_view_prop_act_common(
  fid BIGINT NOT NULL
-- ,cid BIGINT NOT NULL
 ,aid BIGINT NOT NULL
 ,show_datetime BOOLEAN NOT NULL DEFAULT FALSE
 ,show_user     BOOLEAN NOT NULL DEFAULT FALSE
 ,show_src_path BOOLEAN NOT NULL DEFAULT FALSE
 ,show_qty      BOOLEAN NOT NULL DEFAULT FALSE

 ,CONSTRAINT pk_fav_view_prop_act_common PRIMARY KEY (fid,aid)
  
 ,CONSTRAINT fk_fv_prop_actcomm__fid__fav
    FOREIGN KEY     (fid)
    REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 ,CONSTRAINT fk_fv_prop_actcomm__aid__act
    FOREIGN KEY (aid) REFERENCES 
            act (id)  MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

-- ,CONSTRAINT fk_fv_prop_actcomm__cid_aid__ref_cls_act
--    FOREIGN KEY (cid,    aid)    REFERENCES 
--    ref_cls_act (cls_id, act_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 
);


CREATE TABLE fav_view_prop_act_usr(
  fid BIGINT NOT NULL
-- ,cid BIGINT NOT NULL
 ,aid BIGINT NOT NULL
 ,pid BIGINT NOT NULL

 ,CONSTRAINT pk_fv_prop_actusr PRIMARY KEY (fid,aid,pid) 

 ,CONSTRAINT fk_fv_prop_actusr__fid__fav
    FOREIGN KEY     (fid)
    REFERENCES fav (id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

-- ,CONSTRAINT fk_fv_prop_actusr__cid_aid__ref_cls_act
--    FOREIGN KEY (cid,    aid)    REFERENCES 
--    ref_cls_act (cls_id, act_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE

 ,CONSTRAINT fk_fv_prop_actusr__aid_pid__ref_act_prop
    FOREIGN KEY  (aid,    pid)     REFERENCES 
    ref_act_prop (act_id, prop_id) MATCH FULL ON UPDATE CASCADE ON DELETE CASCADE
);



/**

CREATE TABLE favorite_view_prop_act_common
CREATE TABLE favorite_view_prop_act_usr
CREATE TABLE favorite_view_prop_compute


*/










COMMIT TRANSACTION;