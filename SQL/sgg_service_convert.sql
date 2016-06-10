-------------------------------------------------------------------------------
PRINT '';
PRINT '- функция транслитерации';
PRINT '';
------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION iris_translit(p_string character varying)
  RETURNS character varying AS
$BODY$
--Транслитерация
--Отличие от ГОСТ 7.79-2000
--ё = e, а не yo
--ы = y`, а не y'
select 
replace(
replace(
replace(
replace(
replace(
replace(
replace(
replace(
replace(
translate(lower($1), 
'абвгдеёзийклмнопрстуфхць', 'abvgdeezijklmnoprstufхc`'),
'ж', 'zh'),
'ч', 'ch'),
'ш', 'sh'),
'щ', 'shh'),
'ъ', '``'),
'ы', 'y`'),
'э', 'e`'),
'ю', 'yu'),
'я', 'ya');
$BODY$
  LANGUAGE sql IMMUTABLE
  COST 100;
--SET client_min_messages='debug1';
--SHOW client_min_messages;

SET client_min_messages = 'error';
--SHOW client_min_messages=OFF;
-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем все записи классы/свойства';
PRINT '';
-------------------------------------------------------------------------------
ALTER TABLE IF EXISTS log_main DISABLE TRIGGER ALL;
TRUNCATE log_main CASCADE;
TRUNCATE log_detail_act CASCADE;
TRUNCATE log_detail_move CASCADE;
ALTER TABLE IF EXISTS log_main ENABLE TRIGGER ALL;
DELETE FROM acls WHERE id > 99;
DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
DELETE FROM perm_act CASCADE;
DELETE FROM perm_move CASCADE;

SELECT whgrant_grouptouser('TypeDesigner',  'postgres');
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls00 CASCADE;
CREATE TABLE __cls00
(
  id bigint NOT NULL,
  title text NOT NULL
);
COPY __cls00 FROM 'c:\_SAV\tmp\__cls00.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
CREATE UNIQUE INDEX idxu__cls00_id ON __cls00 (id);
CREATE UNIQUE INDEX idxu__cls00_title ON __cls00 (title);

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls01 CASCADE;
CREATE TABLE __cls01
(
  id bigint NOT NULL,
  title text NOT NULL,
  pid bigint NOT NULL REFERENCES __cls00(id)
);
COPY __cls01 FROM 'c:\_SAV\tmp\__cls01.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
CREATE UNIQUE INDEX idxu__cls01_id ON __cls01 (id);
CREATE UNIQUE INDEX idxu__cls01_title ON __cls01 (title);

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls CASCADE;
CREATE TABLE __cls
(
  id bigint NOT NULL,
  title text NOT NULL,
  period integer,
  pid bigint NOT NULL REFERENCES __cls01(id)
);
COPY __cls FROM 'c:\_SAV\tmp\__cls.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
CREATE UNIQUE INDEX idxu__cls_id ON __cls(id);
CREATE UNIQUE INDEX idxu__cls_title ON __cls(title);
ALTER TABLE __cls ADD COLUMN wh3_cid bigint;
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __obj CASCADE;
CREATE TABLE __obj
(
  obj_id bigint NOT NULL
  ,cls_id bigint NOT NULL REFERENCES __cls(id)
  ,title text NOT NULL
  ,invtitle text
  ,pasport_path text
  ,next_cal_date text
  ,preios_cal_date text
  ,curr_cal_path text
  ,previos_cal_path text
  ,user_id_1 INTEGER
  ,user_id_2 INTEGER
  ,id_priznak INTEGER
  ,id_uchastok INTEGER
  ,id_gr_part INTEGER
  ,folder_path text
  ,release_date text
  ,inservice_date text
  ,use_hours NUMERIC
  ,note1 text
  ,note2 text
  ,arhived text
  ,ts text
);
COPY __obj FROM 'c:\_SAV\tmp\__obj.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
ALTER TABLE __obj ADD COLUMN wh3_oid bigint;
ALTER TABLE __obj ADD COLUMN wh3_cid bigint;
CREATE INDEX idx__obj_oid ON __obj (obj_id);

UPDATE __obj  SET pasport_path= replace(pasport_path, '\\', '\\\\');
UPDATE __obj  SET curr_cal_path= replace(curr_cal_path, '\\', '\\\\');
UPDATE __obj  SET folder_path= replace(folder_path, '\\', '\\\\');
UPDATE __obj  SET note1= replace(note1, '\\', '\\\\');
UPDATE __obj  SET note2= replace(note2, '\\', '\\\\');
UPDATE __obj  SET note1= replace(note1, '"', '\\"');
UPDATE __obj  SET note2= replace(note2, '"', '\\"');


-------------------------------------------------------------------------------
PRINT '';
PRINT '- история операций';
PRINT '';
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __hist CASCADE;
CREATE TABLE __hist
(
  hid          BIGINT NOT NULL
  ,sc_oid      BIGINT NOT NULL

  ,hinput_date  DATE
  ,hfrom_user   BIGINT

  ,hrepair_date DATE
  ,hrepair_user BIGINT

  ,hmetrdate    DATE
  ,hmetr_user   BIGINT

  ,hto_pp_date  DATE
  ,hfrom_pp_date DATE

  ,hto_user_from_pp BIGINT

  ,husetime    TEXT
  ,hresult     TEXT 

  ,hdrepair_date DATE
  ,hdepth     TEXT
  ,hpress     TEXT
  ,htemp      TEXT
);
CREATE INDEX idx__hist_oid ON __hist (sc_oid);

COPY __hist FROM 'c:\_SAV\tmp\__hist.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;

UPDATE __hist  SET husetime= NULLIF(REPLACE (REGEXP_REPLACE(husetime,'([^[:digit:],.$])','','g'),',','.') ,'');
UPDATE __hist  SET hdepth=   NULLIF(REPLACE (REGEXP_REPLACE(hdepth,'([^[:digit:],.$])','','g'),',','.') ,'');
UPDATE __hist  SET hpress=   NULLIF(REPLACE (REGEXP_REPLACE(hpress,'([^[:digit:],.$])','','g'),',','.') ,'');
UPDATE __hist  SET htemp=    NULLIF(REPLACE (REGEXP_REPLACE(htemp,'([^[:digit:],.$])','','g'),',','.') ,'');
UPDATE __hist  SET hresult= replace(hresult, '\\', '\\\\');
UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\t', '\\t','g') ,'');
UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\r', '\\r','g') ,'');
UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\n', '\\n','g') ,'');
UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\r\n', '\\r\\n','g') ,'');
UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '"', '\"','g') ,'');

ALTER TABLE __hist ALTER COLUMN husetime TYPE NUMERIC USING husetime::numeric;
ALTER TABLE __hist ALTER COLUMN hdepth TYPE NUMERIC USING hdepth::numeric;
ALTER TABLE __hist ALTER COLUMN hpress TYPE NUMERIC USING hpress::numeric;
ALTER TABLE __hist ALTER COLUMN htemp TYPE NUMERIC USING htemp::numeric;

-------------------------------------------------------------------------------
PRINT '';
PRINT '- загрузка отделов';
PRINT '';
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __departament CASCADE;
CREATE TABLE __departament
(
  id             BIGINT NOT NULL
  ,title         TEXT
  ,note          TEXT
 );
 COPY __departament FROM 'c:\_SAV\tmp\__departament.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
-------------------------------------------------------------------------------
PRINT '';
PRINT '- загрузка пользователей';
PRINT '';
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __worker CASCADE;
CREATE TABLE __worker
(
  id      BIGINT NOT NULL
  ,fam    TEXT
  ,im     TEXT
  ,oth    TEXT
  ,dep_id BIGINT
 );
 COPY __worker FROM 'c:\_SAV\tmp\__worker.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
ALTER TABLE __worker ADD COLUMN wh3_oid bigint;
ALTER TABLE __worker ADD COLUMN wh3_cid bigint;
CREATE INDEX idx__worker_oid ON __worker (id);

-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем ерунду';
PRINT '';
-------------------------------------------------------------------------------
DELETE FROM __cls WHERE title ~~* '%нет данных%';
DELETE FROM __cls01 WHERE title ~~* '%нет данных%';
DELETE FROM __cls00 WHERE title ~~* '%нет данных%';
DELETE FROM __departament WHERE title ~~* '%нет данных%';
-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем из имён запрещённые символы';
PRINT '';
-------------------------------------------------------------------------------
UPDATE __cls00  SET title=replace(title, '/', '|');
UPDATE __cls01  SET title=replace(title, '/', '|');
UPDATE __cls  SET title=replace(title, '/', '|');
UPDATE __obj  SET title=replace(title, '/', '|');
UPDATE __departament  SET title=replace(title, '/', '|');
-------------------------------------------------------------------------------
PRINT '';
PRINT '- переименовываем одинаковые типы';
PRINT '';
-------------------------------------------------------------------------------
--UPDATE __cls00  SET title=title || '_0';
UPDATE __cls01  SET title=title || '_1' WHERE title IN (SELECT title FROM __cls00);
UPDATE __cls  SET title=title || '_2' WHERE title IN (SELECT title FROM __cls01) OR title IN (SELECT title FROM __cls00);
-------------------------------------------------------------------------------
PRINT '';
PRINT '- проверяем имена';
PRINT '';
-------------------------------------------------------------------------------
ALTER TABLE __cls ALTER COLUMN title TYPE whname;
ALTER TABLE __obj ALTER COLUMN title TYPE whname;
ALTER TABLE __departament ALTER COLUMN title TYPE whname;
-------------------------------------------------------------------------------
-- заполняем группы пользователей
-------------------------------------------------------------------------------
INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Инженер по ремонту ГО',false, false);
INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Инженер-метролог',false, false);
INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Диспетчер ГО',false, false);
-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем свойства';
PRINT '';
-------------------------------------------------------------------------------
DECLARE @prid_calp,@prid_desc,@prid_remdesc,@prid_note,@prid_invn,@prid_pasp,@prid_calfile,@prid_objfolder;
DECLARE @prid_reldate,@prid_indate,@prid_usehours,@prid_depth,@prid_press,@prid_temp;
SET @prid_calp = INSERT INTO prop(title, kind)VALUES('Период калибровки(мес.)', 100)RETURNING id;
SET @prid_desc =INSERT INTO prop(title, kind)VALUES('Описание', 0)RETURNING id;

SET @prid_remdesc =INSERT INTO prop(title, kind)VALUES('Описание ремонта', 0)RETURNING id;
SET @prid_note =INSERT INTO prop(title, kind)VALUES('Примечание', 0)RETURNING id;
SET @prid_invn =INSERT INTO prop(title, kind)VALUES('Инвентарный номер', 0)RETURNING id;
SET @prid_pasp =INSERT INTO prop(title, kind)VALUES('Паспорт', 300)RETURNING id;
SET @prid_calfile =INSERT INTO prop(title, kind)VALUES('Файл калибровки', 300)RETURNING id;
SET @prid_objfolder =INSERT INTO prop(title, kind)VALUES('Папка прибора', 300)RETURNING id;
SET @prid_reldate =INSERT INTO prop(title, kind)VALUES('Дата выпуска', 201)RETURNING id;
SET @prid_indate =INSERT INTO prop(title, kind)VALUES('Дата ввода в эксплуатацию', 201)RETURNING id;
SET @prid_usehours =INSERT INTO prop(title, kind)VALUES('Наработка(ч.)', 101)RETURNING id;

SET @prid_depth =INSERT INTO prop(title, kind)VALUES('Глубина(м.)', 101)RETURNING id;
SET @prid_press =INSERT INTO prop(title, kind)VALUES('Давление(МПа)', 101)RETURNING id;
SET @prid_temp =INSERT INTO prop(title, kind)VALUES('Температура(град.С)', 101)RETURNING id;


DECLARE @pid_desc_profil,@pid_desc_kalibr;
SET @pid_desc_profil =INSERT INTO prop(title, kind)VALUES('Описание проверки|профилактики', 0)RETURNING id;
SET @pid_desc_kalibr =INSERT INTO prop(title, kind)VALUES('Описание калибровки', 0)RETURNING id;

-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем действия';
PRINT '';
-------------------------------------------------------------------------------
DECLARE @act_id_chmain,@act_id_remont,@act_id_proverka,@act_id_prof,@act_id_gis,@act_id_calib;
SET @act_id_chmain =   INSERT INTO act (title) VALUES ('Изменить основные свойства')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_invn);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_pasp);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_calfile);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_objfolder);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_reldate);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_indate);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_chmain, @prid_usehours);

SET @act_id_remont =   INSERT INTO act (title) VALUES ('Ремонт')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_remont, @prid_remdesc);

SET @act_id_proverka = INSERT INTO act (title) VALUES ('Проверка')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_proverka, @pid_desc_profil);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_proverka, @prid_usehours);

SET @act_id_prof =     INSERT INTO act (title) VALUES ('Профилактика')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_prof, @pid_desc_profil);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_prof, @prid_usehours);

SET @act_id_calib =     INSERT INTO act (title) VALUES ('Калибровка')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_calib, @pid_desc_kalibr);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_calib, @prid_calfile);

SET @act_id_gis =      INSERT INTO act (title) VALUES ('ГИС')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_usehours);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_depth);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_press);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_temp);

DECLARE @aid_chfndep;
SET @aid_chfndep = INSERT INTO act (title) VALUES ('Изменить описание')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@aid_chfndep, @prid_desc);

DECLARE @aid_chnote;
SET @aid_chnote = INSERT INTO act (title) VALUES ('Изменить примечание')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@aid_chnote, @prid_note);


-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем основные классы';
PRINT '';
-------------------------------------------------------------------------------

DECLARE @struct_units_id, @geo_equipment_id;
SET @struct_units_id = INSERT INTO acls(pid,title,kind,dobj) VALUES (1,'Структурные подразделения',0,NULL) RETURNING id;
SET @geo_equipment_id = INSERT INTO acls(pid,title,kind,dobj) VALUES (1,'Геофизическое оборудование',0,NULL) RETURNING id;

DECLARE @company_id, @department_id, @department_area_id;
SET @company_id = INSERT INTO acls(pid,title,kind,measure) VALUES (@struct_units_id,'Предприятие',1,'ед.') RETURNING id;
SET @department_id = INSERT INTO acls(pid,title,kind,measure) VALUES (@struct_units_id,'Отдел',1,'ед.') RETURNING id;
SET @department_area_id = INSERT INTO acls(pid,title,kind,measure) VALUES (@struct_units_id,'Участок СЦ',1,'ед.') RETURNING id;

DECLARE @sgg_company_id;
SET @sgg_company_id = INSERT INTO obj(title,cls_id,pid) VALUES ('Севергазгеофизика',@company_id, 1 )RETURNING id;

-------------------------------------------------------------------------------
PRINT '';
PRINT '- импортируем отделы';
PRINT '';
------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS sgg_sc_import_departament() CASCADE;
CREATE OR REPLACE FUNCTION sgg_sc_import_departament()
 RETURNS VOID  AS
$BODY$
DECLARE
  import_dep CURSOR IS SELECT id, title,note FROM __departament;
  sgg_company_oid BIGINT;
  department_cid BIGINT;
  _oid BIGINT;
  _pid_desc BIGINT;
  _aid_chfndep BIGINT;
  _prop_val JSONB;
BEGIN
  SELECT id INTO sgg_company_oid FROM obj WHERE title='Севергазгеофизика';
  SELECT id INTO department_cid FROM acls WHERE title='Отдел';
  SELECT id INTO _pid_desc FROM prop WHERE title='Описание';
  SELECT id INTO _aid_chfndep FROM act WHERE title='Изменить описание' ;

  

  FOR rec IN import_dep LOOP
    INSERT INTO obj(title,cls_id,pid) VALUES (rec.title,department_cid, sgg_company_oid ) RETURNING id INTO _oid;
    _prop_val := format('{"%s":"%s"}',_pid_desc, rec.note );
    PERFORM lock_for_act(_oid, sgg_company_oid);
    PERFORM do_act(_oid, _aid_chfndep, _prop_val);
    PERFORM lock_reset(_oid, sgg_company_oid);
    --INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cid, 1, _pid_fndep, rec.note);
  END LOOP;
  
RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
  VALUES ('TypeDesigner', 0, @department_id, NULL, @aid_chfndep );

SELECT sgg_sc_import_departament();
-------------------------------------------------------------------------------
PRINT '';
PRINT '- импортируем работников';
PRINT '';
------------------------------------------------------------------------------
DECLARE @cid_personal;
SET @cid_personal = INSERT INTO acls(pid,title,kind,measure) VALUES (1,'Персонал',1,'чел.') RETURNING id;

DECLARE @pid_fam,@pid_nm,@pid_ot;
SET @pid_fam = INSERT INTO prop(title, kind)VALUES('Фамилия', 0)RETURNING id;
SET @pid_nm = INSERT INTO prop(title, kind)VALUES('Имя', 0)RETURNING id;
SET @pid_ot = INSERT INTO prop(title, kind)VALUES('Отчество', 0)RETURNING id;

SET @aid_ch_worker_info =   INSERT INTO act (title) VALUES ('Изменить сведения о работнике')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@aid_ch_worker_info, @pid_fam);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@aid_ch_worker_info, @pid_nm);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@aid_ch_worker_info, @pid_ot);

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
  VALUES ('TypeDesigner', 0, @cid_personal, NULL, @aid_ch_worker_info );


DROP FUNCTION IF EXISTS sgg_sc_import_worker() CASCADE;
CREATE OR REPLACE FUNCTION sgg_sc_import_worker()
 RETURNS VOID  AS
$BODY$
DECLARE
  import_worker CURSOR IS SELECT id, fam, im, oth, dep_id FROM __worker;
  _cid_personal BIGINT;
  _dep_title TEXT;
  _oid_dep BIGINT;
  _user_title TEXT;
  _user_title_idx INTEGER;
  
  _oid BIGINT;
  

_pid_fam BIGINT;
_pid_nm BIGINT;
_pid_ot BIGINT;

  _aid_ch_worker_info BIGINT;
  _prop_val JSONB;
BEGIN
  SELECT id INTO _cid_personal FROM acls WHERE title='Персонал';

  SELECT id INTO _pid_fam FROM prop WHERE title='Фамилия';
  SELECT id INTO _pid_nm FROM prop WHERE title='Имя';
  SELECT id INTO _pid_ot FROM prop WHERE title='Отчество';

  SELECT id INTO _aid_ch_worker_info FROM act WHERE title='Изменить сведения о работнике' ;

  

  FOR rec IN import_worker LOOP
    SELECT title INTO _dep_title FROM __departament WHERE id=rec.dep_id;
    SELECT id INTO _oid_dep FROM obj WHERE title=_dep_title;

    _user_title:= substring(rec.im from 1 for 1) || '.' || rec.fam;
    --_user_title:= iris_translit(_user_title);
    _user_title_idx = 0;
    LOOP
      PERFORM FROM obj WHERE cls_id=_cid_personal AND title=_user_title;
      IF FOUND THEN
       _user_title:= substring(rec.im from 1 for 1) ||_user_title_idx ||'.' || rec.fam;
       _user_title_idx:=_user_title_idx+1;
      ELSE 
        EXIT;
      END IF;
    END LOOP;
    
    INSERT INTO obj(title,cls_id,pid) VALUES (_user_title,_cid_personal, _oid_dep ) RETURNING id INTO _oid;
    UPDATE __worker SET wh3_oid = _oid, wh3_cid=_cid_personal WHERE  id=rec.id;

    _prop_val := format('{"%s":"%s","%s":"%s","%s":"%s"}',_pid_fam,rec.fam, _pid_nm, rec.im,_pid_ot, rec.oth );
    PERFORM lock_for_act(_oid, _oid_dep);
    PERFORM do_act(_oid, _aid_ch_worker_info, _prop_val);
    PERFORM lock_reset(_oid, _oid_dep);
    
  END LOOP;
  
RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_sc_import_worker();
-------------------------------------------------------------------------------
-- добавляем участки СЦ
-------------------------------------------------------------------------------
DECLARE @sc_departament_id;
SET @sc_departament_id = SELECT id FROM obj WHERE title='Сервисный центр';
--DECLARE @sc_departament_id,@sc_dep_kontr_id,@sc_dep_bur_id,@sc_dep_gti_id;
--SET @sc_departament_id = INSERT INTO obj(title,cls_id,pid) VALUES ('СЦ',@department_id, @sgg_company_id )RETURNING id;
--SET @sc_dep_kontr_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ Контроль',@department_id, @sgg_company_id )RETURNING id;
--SET @sc_dep_bur_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ Бурение',@department_id, @sgg_company_id )RETURNING id;
--SET @sc_dep_gti_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ ГТИ',@department_id, @sgg_company_id )RETURNING id;


DECLARE @remont,@metrolog,@pp,@sklad,@konserv,@dremont,@spisano;
SET @remont = INSERT INTO obj(title,cls_id,pid) VALUES ('Ремонтный участок',@department_area_id, @sc_departament_id )RETURNING id;
SET @metrolog = INSERT INTO obj(title,cls_id,pid) VALUES ('Метрология',@department_area_id, @sc_departament_id )RETURNING id;
SET @pp = INSERT INTO obj(title,cls_id,pid) VALUES ('Пункт проката',@department_area_id, @sc_departament_id )RETURNING id;
SET @sklad = INSERT INTO obj(title,cls_id,pid) VALUES ('Склад',@department_area_id, @sc_departament_id )RETURNING id;
SET @konserv = INSERT INTO obj(title,cls_id,pid) VALUES ('Консервация',@department_area_id, @sc_departament_id )RETURNING id;
SET @dremont = INSERT INTO obj(title,cls_id,pid) VALUES ('Долгосрочный ремонт',@department_area_id, @sc_departament_id )RETURNING id;
SET @spisano = INSERT INTO obj(title,cls_id,pid) VALUES ('Списано',@department_area_id, @sc_departament_id )RETURNING id;


-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем действия для всей категории импортированых классов';
PRINT '';
------------------------------------------------------------------------------

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
  VALUES ('TypeDesigner', 0, @geo_equipment_id, NULL, @act_id_chmain );

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
  VALUES ('Инженер по ремонту ГО', 0, @geo_equipment_id, NULL, @act_id_remont, format('{{%s,%s},%%}',@department_area_id,@remont) );

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
  VALUES ('Инженер по ремонту ГО', 0, @geo_equipment_id, NULL, @act_id_proverka, format('{{%s,%s},%%}',@department_area_id,@remont) ); 

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
  VALUES ('Инженер по ремонту ГО', 0, @geo_equipment_id, NULL, @act_id_prof, format('{{%s,%s},%%}',@department_area_id,@remont) ); 

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
  VALUES ('Инженер-метролог', 0, @geo_equipment_id, NULL, @act_id_calib, format('{{%s,%s},%%}',@department_area_id,@metrolog) );

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
  VALUES ('Диспетчер ГО', 0, @geo_equipment_id, NULL, @act_id_gis, format('{{%s,%s},%%}',@department_area_id,@pp) ); 



-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем перемещения для всей категории импортированых классов';
PRINT '';
------------------------------------------------------------------------------

INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер по ремонту ГО', 0
                       ,@geo_equipment_id, NULL                   -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @remont, '{%}'       -- from [remont]%
                       ,@department_area_id, @metrolog, '{%}'     -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер по ремонту ГО', 0
                       ,@geo_equipment_id, NULL                   -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @remont, '{%}'       -- from [remont]%
                       ,@department_area_id, @pp, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер по ремонту ГО', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @remont, '{%}'       -- from [remont]%
                       ,@department_area_id, @dremont, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер по ремонту ГО', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @dremont, '{%}'       -- from [remont]%
                       ,@department_area_id, @remont, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер-метролог', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @metrolog, '{%}'       -- from [remont]%
                       ,@department_area_id, @remont, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Инженер-метролог', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @metrolog, '{%}'       -- from [remont]%
                       ,@department_area_id, @pp, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Диспетчер ГО', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @pp, '{%}'       -- from [remont]%
                       ,@department_area_id, @metrolog, '{%}'           -- to   [СРК2М]% 
                      );
INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'Диспетчер ГО', 0
                       ,@geo_equipment_id, NULL       -- what [ФЭУ-geo_equipment]%
                       ,@department_area_id, @pp, '{%}'       -- from [remont]%
                       ,@department_area_id, @remont, '{%}'           -- to   [СРК2М]% 
                      );



-------------------------------------------------------------------------------
-- конвертер из старой базы
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS sgg_import_cls() CASCADE;
CREATE OR REPLACE FUNCTION sgg_import_cls()
 RETURNS VOID  AS
$BODY$
DECLARE
  

  import_cls00 CURSOR IS SELECT id, title FROM __cls00;
  import_cls01 CURSOR IS SELECT id, title, pid FROM __cls01;
  import_cls CURSOR IS SELECT id, title, period, pid FROM __cls;
  

  _geo_equipment_id      BIGINT;

  _title      TEXT;
  _pid      BIGINT;
  

  
  _prop_cal_period_id INTEGER;
  _prop_note_id INTEGER;


  _cls_id      BIGINT;

  _prop_desc_id      BIGINT;
  

  _curr_oid      BIGINT;
  _curr_aid      BIGINT;

  _curr_pid      BIGINT;

BEGIN
  SELECT id INTO _prop_cal_period_id FROM prop WHERE title = 'Период калибровки(мес.)';
  SELECT id INTO _prop_desc_id FROM prop WHERE title = 'Описание';
  SELECT id INTO _geo_equipment_id FROM acls WHERE title = 'Геофизическое оборудование';  

  FOR rec IN import_cls00 LOOP
    --RAISE DEBUG 'ADD ABSTRACT CLS=% TO ROOT ',rec.title;
    INSERT INTO acls(pid,title,kind,dobj) VALUES (_geo_equipment_id,rec.title,0,NULL);
  END LOOP;

  FOR rec IN import_cls01 LOOP
    SELECT title INTO _title FROM __cls00 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    --RAISE DEBUG 'ADD ABSTRACT CLS=% TO % (%)',rec.title,_title,rec;
    INSERT INTO acls(pid,title,kind,dobj) VALUES (_pid,rec.title,0,NULL) ;
  END LOOP;

  SELECT id INTO _curr_aid FROM ACT WHERE title='Изменить основные свойства' ;
  
  FOR rec IN import_cls LOOP
    SELECT title INTO _title FROM __cls01 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    --RAISE DEBUG 'ADD NUMERIC CLS=% (period=%) TO % (%)',rec.title,rec.period,_title,rec;
    INSERT INTO acls(pid,title,kind,measure) VALUES (_pid,rec.title,1,'ед') RETURNING id INTO _cls_id ;
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, _prop_cal_period_id, rec.period);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, _prop_desc_id, NULL);
  END LOOP;

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_import_cls();











-------------------------------------------------------------------------------
-- конвертер из старой базы
-------------------------------------------------------------------------------
--SET client_min_messages='debug1';
--SHOW client_min_messages;
DROP FUNCTION IF EXISTS sgg_import_hist() CASCADE;
CREATE OR REPLACE FUNCTION sgg_import_hist()
 RETURNS VOID  AS
$BODY$
DECLARE
  _oid_repair BIGINT;
  _cid_repair BIGINT;
  _oid_drepair BIGINT;
  _cid_drepair BIGINT;
  _oid_metr BIGINT;
  _cid_metr BIGINT;
  _oid_pp BIGINT;
  _cid_pp BIGINT;

  _aid_maininfo BIGINT;
  _aid_gis    BIGINT;
  _aid_chnote BIGINT;
  _aid_kalibr BIGINT;
  _aid_repair BIGINT;
  _aid_proverka BIGINT;
  _aid_profil BIGINT;
  
  
  _pid_note        BIGINT;
  _pid_usehours    BIGINT;
  _pid_depth       BIGINT;
  _pid_press       BIGINT;
  _pid_temp        BIGINT;
  _pid_repairnote  BIGINT;
  _pid_desc_profil BIGINT;
  _pid_desc_kalibr BIGINT;
  _pid_kalfile     BIGINT;
  _pid_invn      BIGINT;
  _pid_pasp      BIGINT;
  _pid_objfolder BIGINT;
  _pid_reldate   BIGINT;
  _pid_indate    BIGINT;

  

  _src_path BIGINT[];
  _dst_path BIGINT[];

  _oid_obj BIGINT;
  _cid_obj BIGINT;
  
  _oid_user BIGINT;
  _cid_user BIGINT;
  
  _note TEXT;

  _act_lid_previos BIGINT;

  _prop JSONB;
  _lid BIGINT;
  _date TIMESTAMP;

  import_obj CURSOR IS SELECT * FROM __obj;
  import_log CURSOR(_sc_oid_curr BIGINT) IS 
    SELECT * FROM __hist WHERE sc_oid = _sc_oid_curr ORDER BY hinput_date ASC;


 _cls_title TEXT;
BEGIN
  SELECT id INTO _pid_note FROM prop WHERE title='Примечание';
  SELECT id INTO _pid_usehours FROM prop WHERE title='Наработка(ч.)';
  SELECT id INTO _pid_depth FROM prop WHERE title='Глубина(м.)';
  SELECT id INTO _pid_press FROM prop WHERE title='Давление(МПа)';
  SELECT id INTO _pid_temp FROM prop WHERE title='Температура(град.С)';
  SELECT id INTO _pid_repairnote FROM prop WHERE title='Описание ремонта';
  SELECT id INTO _pid_desc_profil FROM prop WHERE title='Описание проверки|профилактики';
  SELECT id INTO _pid_desc_kalibr FROM prop WHERE title='Описание калибровки';
  SELECT id INTO _pid_kalfile FROM prop WHERE title='Файл калибровки';
  SELECT id INTO _pid_invn FROM prop WHERE title='Инвентарный номер' ;
  SELECT id INTO _pid_pasp FROM prop WHERE title='Паспорт' ;
  SELECT id INTO _pid_objfolder FROM prop WHERE title='Папка прибора' ;
  SELECT id INTO _pid_reldate FROM prop WHERE title='Дата выпуска' ;
  SELECT id INTO _pid_indate FROM prop WHERE title='Дата ввода в эксплуатацию' ;

  SELECT id,cls_id  INTO _oid_repair,_cid_repair FROM obj_name WHERE title='Ремонтный участок';
  SELECT id,cls_id  INTO _oid_drepair,_cid_drepair FROM obj_name WHERE title='Долгосрочный ремонт';
  SELECT id,cls_id  INTO _oid_metr,_cid_metr FROM obj_name WHERE title='Метрология';
  SELECT id,cls_id  INTO _oid_pp,_cid_pp FROM obj_name WHERE title='Пункт проката';

  SELECT id INTO _aid_maininfo FROM ACT WHERE title='Изменить основные свойства' ;
  SELECT id INTO _aid_gis FROM act WHERE title='ГИС';
  SELECT id INTO _aid_chnote FROM act WHERE title='Изменить примечание';
  SELECT id INTO _aid_kalibr FROM act WHERE title='Калибровка';
  SELECT id INTO _aid_profil FROM act WHERE title='Профилактика';
  SELECT id INTO _aid_proverka FROM act WHERE title='Проверка';
  SELECT id INTO _aid_repair FROM act WHERE title='Ремонт';


  FOR impobj IN import_obj LOOP
    -- создаём объект
    SELECT title INTO _cls_title FROM __cls WHERE id = impobj.cls_id;
    SELECT id INTO _cid_obj FROM acls WHERE title=_cls_title;
    RAISE DEBUG '_cid_obj = % sc_oid=% _cls_title=%',_cid_obj, impobj.obj_id,_cls_title;
    INSERT INTO obj(title,cls_id,pid) VALUES (impobj.title, _cid_obj, _oid_pp )RETURNING id INTO _oid_obj;
    UPDATE __obj SET wh3_oid = _oid_obj, wh3_cid=_cid_obj WHERE obj_id=impobj.obj_id;
    -- добавляем основные сведения 
    _src_path:=format('{{%s,%s},{2,1}}',_cid_pp,_oid_pp); -- помещаем в пункт проката
    _prop:= ('{}'::JSONB);
    _date:=NULL;
    _note:=NULL;
    _act_lid_previos:=NULL;
    _lid :=NULL;
    -- берём первую дату упоминания
    SELECT hinput_date INTO _date FROM __hist WHERE hinput_date IS NOT NULL AND sc_oid=impobj.obj_id ORDER BY hinput_date ASC LIMIT 1;
    _date:=COALESCE(_date,now()) - '1 week'::INTERVAL;--отматываем неделю назад на всякий случай
    RAISE DEBUG 'first date = %',_date;
    _prop:=_prop || format('{"%s":"%s"}', _pid_invn, impobj.invtitle)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_pasp, impobj.pasport_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_kalfile, impobj.curr_cal_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_objfolder, impobj.folder_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_reldate, impobj.release_date)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_indate, impobj.inservice_date)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_usehours, 0)::JSONB;
    INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
    INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_maininfo, _prop)  RETURNING id INTO _act_lid_previos;

    
    FOR rec IN import_log(impobj.obj_id) LOOP
      -- текущее местоположение
      SELECT wh3_oid,wh3_cid INTO _oid_user,_cid_user FROM __worker WHERE  id=rec.hfrom_user;
      IF _cid_user IS NOT NULL AND _oid_user IS NOT NULL THEN
        _src_path:=format('{{%s,%s},{2,1}}',_cid_user,_oid_user);
      END IF;
      -- импортируем ГИС/Изменить примечание
      RAISE DEBUG 'GIS/CH_DESC';
      _note:=substring(rec.hresult from '%входной_контроль#"%#"ремонт_прибора%' for '#');
      _note:=trim(both from _note);
      _note:=NULLIF(_note,'');
      RAISE DEBUG 'REC % ', rec;
      _date:=rec.hinput_date;
      IF(_note IS NOT NULL AND _note ILIKE '%ГИС%')OR(rec.hdepth IS NOT NULL AND rec.hdepth>0 ) THEN
        _prop:=_prop || format('{"%s":"%s"}', _pid_note, _note)::JSONB;
        _prop:=_prop || format('{"%s":%s}',   _pid_usehours, COALESCE(rec.husetime,0) )::JSONB;
        _prop:=_prop || format('{"%s":%s}',   _pid_depth, COALESCE(rec.hdepth,0) )::JSONB;
        _prop:=_prop || format('{"%s":%s}',   _pid_press, COALESCE(rec.hpress,0) )::JSONB;
        _prop:=_prop || format('{"%s":%s}',   _pid_temp,  COALESCE(rec.htemp,0) )::JSONB;
        --RAISE DEBUG 'PROP % ', _prop;
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid, _aid_gis, _prop)      RETURNING id INTO _act_lid_previos;
      ELSE
        --ставим примечание
        _prop:=_prop || format('{"%s":"%s"}',   _pid_note, _note )::JSONB;
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_chnote, _prop)    RETURNING id INTO _act_lid_previos;
      END IF;
    -- импортируем ремонт/ профилактику / проверку
      RAISE DEBUG 'REPAIR';
      -- если есть дата ремонта то перемещаем в Ремонтный участок
      IF(rec.hrepair_date IS NOT NULL ) THEN 
        _date:=COALESCE (rec.hrepair_date,rec.hinput_date) +'02:00:00'::INTERVAL;
        _dst_path:=format('{{%s,%s},{2,1}}',_cid_repair,_oid_repair);
        INSERT INTO log_main(timemark, src_path, obj_id)        VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_move(id, dst_path, qty, prop_lid)VALUES (_lid,  _dst_path, 1, _act_lid_previos);
        _src_path:=_dst_path;
        UPDATE obj_num SET pid = _oid_repair WHERE id=_oid_obj;
      
        _note:=substring(rec.hresult from '%ремонт_прибора#"%#"калибровка_прибора%' for '#');
        _note:=trim(both from _note);
        _note:=NULLIF(_note,'');
        SELECT wh3_oid,wh3_cid INTO _oid_user,_cid_user FROM __worker WHERE id=rec.hrepair_user;
        -- если есть дата долгосрочного ремонта то перемещаем в долгосрочный и изменяем примечание
        IF(rec.hdrepair_date IS NOT NULL ) THEN 
          _date:=COALESCE (rec.hdrepair_date,rec.hinput_date) +'02:30:00'::INTERVAL;
          _dst_path:=format('{{%s,%s},{2,1}}',_cid_drepair,_oid_drepair);
          INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
          INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
          _src_path:=_dst_path;
          UPDATE obj_num SET pid = _oid_drepair WHERE id=_oid_obj;
          --ставим примечание
          _prop:=_prop || format('{"%s":"%s"}',   _pid_note, _note )::JSONB;
          INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
          INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_chnote, _prop)    RETURNING id INTO _act_lid_previos;
        ELSE
          -- если есть ремонтник или описание - выполняем профилактику/проверку/ремонт/
          IF(rec.hrepair_user<>-1 OR _note IS  NOT NULL ) THEN 
            _date:=COALESCE (rec.hrepair_date,rec.hinput_date) +'03:00:00'::INTERVAL;
            INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
            CASE
              WHEN _note ILIKE '%профилакт%' AND length(_note)<60 THEN
                _prop:=_prop || format('{"%s":"%s"}',   _pid_desc_profil, _note )::JSONB;
                INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_profil, _prop)RETURNING id INTO _act_lid_previos;
              WHEN _note ILIKE '%провер%' AND length(_note)<60 THEN
                _prop:=_prop || format('{"%s":"%s"}',   _pid_desc_profil, _note )::JSONB;
                INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_proverka, _prop)RETURNING id INTO _act_lid_previos;
              ELSE
                _prop:=_prop || format('{"%s":"%s"}',   _pid_repairnote, _note )::JSONB;
                INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_repair, _prop)RETURNING id INTO _act_lid_previos;
            END CASE;
          END IF; -- IF(rec.hrepair_user<>-1 OR _note IS  NOT NULL ) THEN 
        END iF; -- IF(rec.hdrepair_date IS NOT NULL ) THEN 
      END IF; -- IF(rec.hrepair_date IS NOT NULL ) THEN 

      -- импортируем калибровку
      RAISE DEBUG 'METROLOGY';
      -- если есть дата калибровки то перемещаем в метрологию
      IF(rec.hmetrdate IS NOT NULL ) THEN
        _date:=COALESCE (rec.hmetrdate,rec.hinput_date) +'04:00:00'::INTERVAL;
        _dst_path:=format('{{%s,%s},{2,1}}',_cid_metr,_oid_metr);
        INSERT INTO log_main(timemark, src_path, obj_id)         VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
        _src_path:=_dst_path;
        UPDATE obj_num SET pid = _oid_metr WHERE id=_oid_obj;
      
        _note:=substring(rec.hresult from '%калибровка_прибора#"%#"пункт_проката%' for '#');
        _note:=trim(both from _note);
        _note:=NULLIF(_note,'');
        -- если есть метролог или описание - выполняем калибровку
        IF(rec.hmetr_user<>-1 OR _note IS NOT NULL ) THEN 
          _date:=COALESCE (rec.hmetrdate,rec.hinput_date) +'05:00:00'::INTERVAL;
          _prop:=_prop || format('{"%s":"%s"}',   _pid_desc_kalibr, _note )::JSONB;
          INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
          INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_kalibr, _prop)RETURNING id INTO _act_lid_previos;
        END IF; -- IF(rec.hmetr_user<>-1 OR _note IS NOT NULL ) THEN 
      END IF; -- IF(rec.hmetrdate IS NOT NULL ) THEN

      --ставим примечание ПП
      RAISE DEBUG 'PP NOTE';
      _note:=substring(rec.hresult from '%пункт_проката#"%#"%' for '#');
      _note:=trim(both from _note);
      _note:=NULLIF(_note,'');
      IF(_note IS  NOT NULL ) THEN
        _date:=COALESCE (rec.hto_pp_date,rec.hinput_date) +'06:00:00'::INTERVAL;
        _prop:=_prop || format('{"%s":"%s"}',   _pid_note, _note )::JSONB;
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_chnote, _prop)RETURNING id INTO _act_lid_previos;
      END IF;

      -- перемещаем в ПП
      RAISE DEBUG 'MOVE TO PP';
      IF(rec.hto_pp_date IS NOT NULL) THEN
        _date:=COALESCE (rec.hto_pp_date,rec.hinput_date) +'07:00:00'::INTERVAL;
        _dst_path:=format('{{%s,%s},{2,1}}',_cid_pp,_oid_pp);
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
        _src_path:=_dst_path;
        UPDATE obj_num SET pid = _oid_pp WHERE id=_oid_obj;
      END IF;

      -- Выдаём из ПП
      RAISE DEBUG 'MOVE TO USER';
      SELECT wh3_oid,wh3_cid INTO _oid_user,_cid_user FROM __worker WHERE  id=rec.hto_user_from_pp;
      IF FOUND THEN
        _date:=COALESCE (rec.hfrom_pp_date,rec.hinput_date) +'08:00:00'::INTERVAL;
        _dst_path:=format('{{%s,%s},{2,1}}',_cid_user,_oid_user);
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
        _src_path:=_dst_path;
        UPDATE obj_num SET pid = _oid_user WHERE id=_oid_obj;
      END IF;
    END LOOP; -- FOR rec IN import_log(_oid_obj) LOOP
    

    _prop:=_prop || format('{"%s":"%s"}', _pid_note, _note)::JSONB;
    _prop:=_prop || format('{"%s":%s}',   _pid_usehours, COALESCE(impobj.use_hours,0) )::JSONB;
    _date:=_date+'01:00:00'::INTERVAL;
    INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
    INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid, _aid_maininfo, _prop)      RETURNING id INTO _act_lid_previos;
    UPDATE obj_name SET prop = _prop WHERE id=_oid_obj;


  END LOOP; --FOR impobj IN import_obj LOOP

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_import_hist();

COMMIT;

--SELECT COALESCE(NULL,now()) 

--SELECT '{"10":34,"11":"fsdfsd"}'::JSONB || '{"10":"qwe123"}'
--SELECT '{}'::JSONB || '{"10":"qwe123"}' || '{"11":"asd123"}'
--SELECT '{}'::JSONB || '{"10":"qwe123"}' || format('{"%s":"%s"}',11,'asd123')::JSONB

--SELECT ('2015.01.26'::DATE +'06:00:00'::INTERVAL)::DATE

--SELECT ('2015.01.26'::TIMESTAMP - '1 week'::INTERVAL)

--UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\\', '\\\\','g') ,'');
--SELECT hresult FROM __hist  WHERE hresult ILIKE '%Н=1500 м. р-р 1,22%'
--SELECT * FROM __hist  WHERE h_drepair_date IS NOT NULL LIMIT 100
--SELECT * FROM __hist  WHERE hrepair_user::INTEGER=-1 LIMIT 100
--SELECT * FROM __hist  WHERE hrepair_date IS NULL  AND hrepair_user::INTEGER=-1  LIMIT 10000


/*

SELECT log_id,log_dt,log_user,act_title,mcls_title,mobj_title,qty
      ,(SELECT path FROM tmppath_to_2id_info(src_ipath::TEXT, 1))
      ,(SELECT path FROM tmppath_to_2id_info(dst_ipath::TEXT, 1))
      ,prop,act_color
      ,log_dt::timestamptz::date AS log_date
      ,mcls_id,mobj_id 
      FROM (SELECT * FROM LOG)hist ORDER BY log_dt DESC LIMIT 24 OFFSET 0

SET enable_seqscan = ON;
SELECT *
,(SELECT path FROM tmppath_to_2id_info(src_ipath::TEXT,1)) --AS src_path
,(SELECT path FROM tmppath_to_2id_info(dst_ipath::TEXT,1)) --AS dst_path
 FROM (
  SELECT * FROM log ORDER BY log_dt DESC LIMIT 24 OFFSET 0) hist;

SELECT * FROM log WHERE act_title ~~* '%пере%' LIMIT 24 OFFSET 0


SELECT * FROM log_move ORDER BY timemark DESC LIMIT 24 OFFSET 10



DROP FUNCTION IF EXISTS iris_translit(p_string character varying) CASCADE;
DROP FUNCTION IF EXISTS sgg_sc_import() CASCADE;
DROP TABLE IF EXISTS __departament CASCADE;
DROP TABLE IF EXISTS __worker CASCADE;

DROP TABLE IF EXISTS __cls00 CASCADE;
DROP TABLE IF EXISTS __cls01 CASCADE;
DROP TABLE IF EXISTS __cls CASCADE;
DROP TABLE IF EXISTS __obj CASCADE;
DROP TABLE IF EXISTS __hist CASCADE;


*/