BEGIN TRANSACTION;

--SET client_min_messages = 'error';
--SET client_min_messages = 'debug';
SET client_min_messages = 'NOTICE';
--SHOW client_min_messages=OFF;

SELECT whgrant_grouptouser('TypeDesigner',  'postgres');
-------------------------------------------------------------------------------
-- удаляем все записи классы/свойства';
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

DROP FUNCTION IF EXISTS sgg_drop_temporary_items() CASCADE;
CREATE OR REPLACE FUNCTION sgg_drop_temporary_items()
 RETURNS VOID  AS $BODY$
DECLARE 

BEGIN
  DROP TABLE IF EXISTS __cls00 CASCADE;
  DROP TABLE IF EXISTS __cls01 CASCADE;
  DROP TABLE IF EXISTS __cls CASCADE;
  DROP TABLE IF EXISTS __obj CASCADE;
  DROP TABLE IF EXISTS __hist CASCADE;
  DROP TABLE IF EXISTS __departament CASCADE;
  DROP TABLE IF EXISTS __worker CASCADE;

  DROP FUNCTION IF EXISTS sgg_load_tables() CASCADE;
  
  DROP FUNCTION IF EXISTS sgg_sc_import_departament();
  DROP FUNCTION IF EXISTS sgg_sc_import_worker();

  DROP FUNCTION IF EXISTS sgg_import_cls();
  DROP FUNCTION IF EXISTS sgg_import_hist();

  DROP FUNCTION IF EXISTS iris_translit(p_string character varying);

END $BODY$ LANGUAGE plpgsql;

  SELECT sgg_drop_temporary_items();
-------------------------------------------------------------------------------
-- функция транслитерации';
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

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls00 CASCADE;
CREATE TABLE __cls00 (
  id bigint NOT NULL,
  title text NOT NULL
);

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls01 CASCADE;
CREATE TABLE __cls01 (
  id bigint NOT NULL,
  title text NOT NULL,
  pid bigint NOT NULL 
);

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __cls CASCADE;
CREATE TABLE __cls (
  id bigint NOT NULL,
  title text NOT NULL,
  period integer,
  pid bigint NOT NULL 
);

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __obj CASCADE;
CREATE TABLE __obj (
  obj_id bigint NOT NULL
  ,cls_id bigint NOT NULL
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

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __hist CASCADE;
CREATE TABLE __hist (
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

-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __departament CASCADE;
CREATE TABLE __departament (
  id             BIGINT NOT NULL
  ,title         TEXT
  ,note          TEXT
 );
 
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __worker CASCADE;
CREATE TABLE __worker (
  id      BIGINT NOT NULL
  ,fam    TEXT
  ,im     TEXT
  ,oth    TEXT
  ,dep_id BIGINT
 );

-------------------------------------------------------------------------------
--DO $$
DROP FUNCTION IF EXISTS sgg_load_tables() CASCADE;
CREATE OR REPLACE FUNCTION sgg_load_tables()
 RETURNS VOID  AS $BODY$

DECLARE 
  home_dir TEXT;
BEGIN

  
  RAISE NOTICE 'SERVER version %', version();
  
  IF(version() ILIKE '%linux%') THEN
    home_dir:='/home/alex/wh3_data/';
  ELSE
    home_dir:='c:\_SAV\tmp\';
  END IF;

  RAISE NOTICE 'Import data from directory %', home_dir;
  EXECUTE 'COPY __cls00       FROM '''||home_dir||'__cls00.csv''       WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __cls01       FROM '''||home_dir||'__cls01.csv''       WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __cls         FROM '''||home_dir||'__cls.csv''         WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __obj         FROM '''||home_dir||'__obj.csv''         WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __hist        FROM '''||home_dir||'__hist.csv''        WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';

  EXECUTE 'COPY __departament FROM '''||home_dir||'__departament.csv'' WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __worker      FROM '''||home_dir||'__worker.csv''      WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  
  RAISE NOTICE 'Processing __cls00';
  CREATE UNIQUE INDEX idxu__cls00_id ON __cls00 (id);
  CREATE UNIQUE INDEX idxu__cls00_title ON __cls00 (title);
  DELETE FROM __cls WHERE title ~~* '%нет данных%';

  RAISE NOTICE 'Processing __cls01';
  CREATE UNIQUE INDEX idxu__cls01_id ON __cls01 (id);
  CREATE UNIQUE INDEX idxu__cls01_title ON __cls01 (title);
  ALTER TABLE __cls01 ADD CONSTRAINT fk_cls01__cls00_id FOREIGN KEY (pid)
    REFERENCES __cls00 (id) MATCH FULL ON UPDATE RESTRICT ON DELETE RESTRICT;
  DELETE FROM __cls01 WHERE title ~~* '%нет данных%';
  
  RAISE NOTICE 'Processing __cls';
  ALTER TABLE __cls ADD CONSTRAINT fk_cls__cls01_id FOREIGN KEY (pid)
    REFERENCES __cls01 (id) MATCH FULL ON UPDATE RESTRICT ON DELETE RESTRICT;
  CREATE UNIQUE INDEX idxu__cls_id ON __cls(id);
  CREATE UNIQUE INDEX idxu__cls_title ON __cls(title);
  ALTER TABLE __cls ADD COLUMN wh3_cid bigint;
  DELETE FROM __cls00 WHERE title ~~* '%нет данных%';

  RAISE NOTICE 'Processing __obj';
  ALTER TABLE __obj ADD CONSTRAINT fk_obj_cid__cls_id FOREIGN KEY (cls_id)
    REFERENCES __cls (id) MATCH FULL ON UPDATE RESTRICT ON DELETE RESTRICT;
  ALTER TABLE __obj ADD COLUMN wh3_oid bigint;
  ALTER TABLE __obj ADD COLUMN wh3_cid bigint;
  CREATE INDEX idx__obj_oid ON __obj (obj_id);
  UPDATE __obj  SET pasport_path  = NULLIF(substring(to_json(pasport_path)::TEXT from '"~"%~""' FOR '~' ),'')
                   ,curr_cal_path = NULLIF(substring(to_json(curr_cal_path)::TEXT from '"~"%~""' FOR '~' ),'')
                   ,folder_path   = NULLIF(substring(to_json(folder_path)::TEXT from '"~"%~""' FOR '~' ),'')
                   ,note1   = NULLIF(substring(to_json(note1)::TEXT from '"~"%~""' FOR '~' ),'')
                   ,note2   = NULLIF(substring(to_json(note2)::TEXT from '"~"%~""' FOR '~' ),'')
                   ,arhived = CASE WHEN arhived='ИСТИНА' THEN 'TRUE' ELSE 'FALSE' END 
                   ;
  --UPDATE __obj  SET pasport_path= replace(pasport_path, '\\', '\\\\');
  --UPDATE __obj  SET curr_cal_path= replace(curr_cal_path, '\\', '\\\\');
  --UPDATE __obj  SET folder_path= replace(folder_path, '\\', '\\\\');
  --UPDATE __obj  SET note1= replace(note1, '\\', '\\\\');
  --UPDATE __obj  SET note2= replace(note2, '\\', '\\\\');
  --UPDATE __obj  SET note1= replace(note1, '"', '\\"');
  --UPDATE __obj  SET note2= replace(note2, '"', '\\"');
  --SELECT NULLIF(trim(both '"' from to_json('dgfdg dfg\dfg\d fgdgdfg'::TEXT)::TEXT),'');
  ALTER TABLE __obj ALTER COLUMN arhived TYPE BOOLEAN USING arhived::BOOLEAN;

  RAISE NOTICE 'Processing __hist';
  CREATE INDEX idx__hist_oid ON __hist (sc_oid);
  UPDATE __hist  SET husetime= NULLIF(REPLACE (REGEXP_REPLACE(husetime,'([^[:digit:],.$])','','g'),',','.') ,'')
                    ,hdepth=   NULLIF(REPLACE (REGEXP_REPLACE(hdepth,'([^[:digit:],.$])','','g'),',','.') ,'')
                    ,hpress=   NULLIF(REPLACE (REGEXP_REPLACE(hpress,'([^[:digit:],.$])','','g'),',','.') ,'')
                    ,htemp=    NULLIF(REPLACE (REGEXP_REPLACE(htemp,'([^[:digit:],.$])','','g'),',','.') ,'')
                    ,hresult   = NULLIF(substring(to_json(hresult)::TEXT from '"~"%~""' FOR '~' ),'');
  --UPDATE __hist  SET husetime= NULLIF(REPLACE (REGEXP_REPLACE(husetime,'([^[:digit:],.$])','','g'),',','.') ,'');
  --UPDATE __hist  SET hdepth=   NULLIF(REPLACE (REGEXP_REPLACE(hdepth,'([^[:digit:],.$])','','g'),',','.') ,'');
  --UPDATE __hist  SET hpress=   NULLIF(REPLACE (REGEXP_REPLACE(hpress,'([^[:digit:],.$])','','g'),',','.') ,'');
  --UPDATE __hist  SET htemp=    NULLIF(REPLACE (REGEXP_REPLACE(htemp,'([^[:digit:],.$])','','g'),',','.') ,'');
  --UPDATE __hist  SET hresult= replace(hresult, '\\', '\\\\');
  --UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\t', '\\t','g') ,'');
  --UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\r', '\\r','g') ,'');
  --UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\n', '\\n','g') ,'');
  --UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '\r\n', '\\r\\n','g') ,'');
  --UPDATE __hist  SET hresult= NULLIF(REGEXP_REPLACE(hresult, '"', '\"','g') ,'');

  ALTER TABLE __hist ALTER COLUMN husetime TYPE NUMERIC USING husetime::numeric;
  ALTER TABLE __hist ALTER COLUMN hdepth TYPE NUMERIC USING hdepth::numeric;
  ALTER TABLE __hist ALTER COLUMN hpress TYPE NUMERIC USING hpress::numeric;
  ALTER TABLE __hist ALTER COLUMN htemp TYPE NUMERIC USING htemp::numeric;

  RAISE NOTICE 'Processing __departament';
  DELETE FROM __departament WHERE title ~~* '%нет данных%';

  RAISE NOTICE 'Processing __worker';
  ALTER TABLE __worker ADD COLUMN wh3_oid bigint;
  ALTER TABLE __worker ADD COLUMN wh3_cid bigint;
  CREATE INDEX idx__worker_oid ON __worker (id);

  RAISE NOTICE 'переименовываем одинаковые типы';
  --UPDATE __cls00  SET title=title || '_0';
  UPDATE __cls01  SET title=title || '_1' WHERE title IN (SELECT title FROM __cls00);
  UPDATE __cls  SET title=title || '_2' WHERE title IN (SELECT title FROM __cls01) OR title IN (SELECT title FROM __cls00);

  --RAISE NOTICE 'удаляем из имён запрещённые символы';
  --UPDATE __cls00  SET title=replace(title, '/', '|');
  --UPDATE __cls01  SET title=replace(title, '/', '|');
  --UPDATE __cls  SET title=replace(title, '/', '|');
  --UPDATE __obj  SET title=replace(title, '/', '|');
  --UPDATE __departament  SET title=replace(title, '/', '|');

  --RAISE NOTICE 'проверяем имена';
  --ALTER TABLE __cls ALTER COLUMN title TYPE whname;
  --ALTER TABLE __obj ALTER COLUMN title TYPE whname;
  --ALTER TABLE __departament ALTER COLUMN title TYPE whname;

  RAISE NOTICE 'заполняем группы пользователей';
  PERFORM FROM wh_role WHERE rolname='Вед.инженер по ремонту ГО';
  IF NOT FOUND THEN
    INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Вед.инженер по ремонту ГО',false, false);
  ELSE
    UPDATE wh_role SET rolcanlogin=FALSE, rolcreaterole=FALSE WHERE rolname='Вед.инженер по ремонту ГО';
  END IF;

  PERFORM FROM wh_role WHERE rolname='Инженер по ремонту ГО';
  IF NOT FOUND THEN
    INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Инженер по ремонту ГО',false, false);
  ELSE
    UPDATE wh_role SET rolcanlogin=FALSE, rolcreaterole=FALSE WHERE rolname='Инженер по ремонту ГО';
  END IF;
  PERFORM FROM wh_role WHERE rolname='Инженер-метролог';
  IF NOT FOUND THEN
    INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Инженер-метролог',false, false);
  ELSE
    UPDATE wh_role SET rolcanlogin=FALSE, rolcreaterole=FALSE WHERE rolname='Инженер-метролог';
  END IF;
  PERFORM FROM wh_role WHERE rolname='Диспетчер ГО';
  IF NOT FOUND THEN
    INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Диспетчер ГО',false, false);
  ELSE
    UPDATE wh_role SET rolcanlogin=FALSE, rolcreaterole=FALSE WHERE rolname='Диспетчер ГО';
  END IF;
  PERFORM FROM wh_role WHERE rolname='Кладовщик';
  IF NOT FOUND THEN
    INSERT INTO wh_role (rolname, rolcanlogin, rolcreaterole) VALUES ('Кладовщик',false, false);
  ELSE
    UPDATE wh_role SET rolcanlogin=FALSE, rolcreaterole=FALSE WHERE rolname='Кладовщик';
  END IF;

--END$$;  
END $BODY$ LANGUAGE plpgsql;

SELECT sgg_load_tables();

-------------------------------------------------------------------------------
-- добавляем свойства
-- добавляем действия
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS sgg_add_prop_and_act() CASCADE;
CREATE OR REPLACE FUNCTION sgg_add_prop_and_act()
 RETURNS VOID  AS $BODY$
DECLARE 
 pid_service_cid BIGINT;
 pid_service_oid BIGINT;
 prid_calp BIGINT;
 prid_desc BIGINT;
 prid_repair_reason BIGINT;
 prid_remdesc BIGINT;
 prid_note BIGINT;
 prid_invn BIGINT;
 prid_pasp BIGINT;
 prid_calfile BIGINT;
 prid_objfolder BIGINT;
 prid_reldate BIGINT;
 prid_indate BIGINT;
 prid_usehours BIGINT;
 prid_depth BIGINT; 
 prid_press BIGINT; 
 prid_temp BIGINT; 
 pid_desc_profil BIGINT; 
 pid_desc_prover BIGINT; 
 pid_desc_kalibr BIGINT; 
 pid_desc_gis BIGINT; 
 pid_fm BIGINT; 
 pid_nm BIGINT; 
 pid_ot BIGINT; 

 aid_chmain BIGINT;
 aid_remont BIGINT;
 aid_proverka BIGINT;
 aid_prof BIGINT;
 aid_gis BIGINT;
 aid_calib BIGINT; 
 aid_change_desc BIGINT; 
 aid_change_note BIGINT; 
 aid_ch_worker_info BIGINT; 
 aid_ch_service_oid BIGINT; 

 cid_struct_unit BIGINT;
 cid_geo_equipment BIGINT;
 cid_company BIGINT;
 cid_department BIGINT;
 cid_sector_sc BIGINT;
 cid_personal BIGINT;
 cid_zakaz BIGINT;
 cid_zip BIGINT;

 oid_company_sgg BIGINT;
 oid_company_girs BIGINT;
 oid_company_cgg BIGINT;
 oid_department_sc BIGINT;
 oid_sector_sc_repair BIGINT;
 oid_sector_sc_repair_wait BIGINT;
 oid_sector_sc_metrology BIGINT;
 oid_sector_sc_pp BIGINT;
 oid_sector_sc_pv BIGINT;
 oid_sector_sc_warehouse BIGINT;
 oid_sector_sc_conservation BIGINT;
 oid_sector_sc_utilize BIGINT;
 oid_zakaz_go BIGINT;
 oid_zakaz_mtr BIGINT;

 idpath_sgg_sc TEXT;

BEGIN
  RAISE NOTICE 'Вставляем свойства';
  INSERT INTO prop(title, kind)VALUES('Идентификатор класса в Service', 100) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_service_cid;
  INSERT INTO prop(title, kind)VALUES('Идентификатор объекта в Service', 100) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_service_oid;
  INSERT INTO prop(title, kind)VALUES('Период калибровки(мес.)', 100) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_calp;
  INSERT INTO prop(title, kind)VALUES('Описание', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_desc;
  INSERT INTO prop(title, kind)VALUES('Примечание', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_note;
  INSERT INTO prop(title, kind)VALUES('Инвентарный номер', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_invn;
  INSERT INTO prop(title, kind)VALUES('Паспорт', 400) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_pasp;
  INSERT INTO prop(title, kind)VALUES('Файл калибровки', 400) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_calfile;
  INSERT INTO prop(title, kind)VALUES('Папка прибора', 300) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_objfolder;
  INSERT INTO prop(title, kind)VALUES('Дата выпуска', 201) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_reldate;
  INSERT INTO prop(title, kind)VALUES('Дата ввода в эксплуатацию', 201) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_indate;
  INSERT INTO prop(title, kind)VALUES('Наработка(ч.)', 101) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_usehours;
  INSERT INTO prop(title, kind)VALUES('Глубина(м.)', 101) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_depth;
  INSERT INTO prop(title, kind)VALUES('Давление(МПа)', 101) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_press;
  INSERT INTO prop(title, kind)VALUES('Температура(град.С)', 101) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_temp;
  INSERT INTO prop(title, kind, var)VALUES('Причина ремонта', 0,'{Небрежное отношение,Нарушение условий эксплуатации,Нарушение условий обслуживания,Компонентная база,Износ,Конструктивный недостаток,Брак при изготовлении}') ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_repair_reason;
  INSERT INTO prop(title, kind)VALUES('Описание ремонта', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO prid_remdesc;
  INSERT INTO prop(title, kind)VALUES('Описание профилактики', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_desc_profil;
  INSERT INTO prop(title, kind)VALUES('Описание проверки', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_desc_prover;
  INSERT INTO prop(title, kind)VALUES('Описание калибровки', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_desc_kalibr;
  INSERT INTO prop(title, kind)VALUES('Описание ГИС', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_desc_gis;
  INSERT INTO prop(title, kind)VALUES('Фамилия', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_fm;
  INSERT INTO prop(title, kind)VALUES('Имя', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_nm;
  INSERT INTO prop(title, kind)VALUES('Отчество', 0) ON CONFLICT (title) DO UPDATE SET kind = EXCLUDED.kind
    RETURNING id INTO pid_ot;

  RAISE NOTICE 'Вставляем действия + свойства действий';

  INSERT INTO act (title) VALUES ('Изменить Идентификатор объекта в Service') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_ch_service_oid;
  INSERT INTO ref_act_prop(act_id, prop_id) VALUES (aid_ch_service_oid, pid_service_oid) 
                                                  ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title) VALUES ('Изменить основные свойства') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_chmain;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_chmain, prid_note)
                                                 ,(aid_chmain, prid_invn)
                                                 ,(aid_chmain, prid_pasp)
                                                 ,(aid_chmain, prid_calfile)
                                                 ,(aid_chmain, prid_objfolder)
                                                 ,(aid_chmain, prid_reldate)
                                                 ,(aid_chmain, prid_indate)
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title,color) VALUES ('Ремонт','rgb(255, 128, 128)') 
    ON CONFLICT (title) DO UPDATE SET color=EXCLUDED.color 
    RETURNING id INTO aid_remont;
  INSERT INTO ref_act_prop(act_id, prop_id) VALUES (aid_remont, prid_repair_reason) 
                                                  ,(aid_remont, prid_remdesc) 
                                                  ,(aid_remont, prid_note) 
                                                  ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title,color) VALUES ('Проверка','rgb(220, 220, 220)')
    ON CONFLICT (title) DO UPDATE SET color=EXCLUDED.color 
    RETURNING id INTO aid_proverka;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_proverka, pid_desc_prover) 
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title,color) VALUES ('Профилактика','rgb(255, 255, 128)')
    ON CONFLICT (title) DO UPDATE SET color=EXCLUDED.color 
    RETURNING id INTO aid_prof;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_prof, pid_desc_profil)
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title,color) VALUES ('Калибровка','rgb(128, 220, 255)')
    ON CONFLICT (title) DO UPDATE SET color=EXCLUDED.color 
    RETURNING id INTO aid_calib;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_calib, pid_desc_kalibr)
                                                 ,(aid_calib, prid_calfile) 
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title,color) VALUES ('ГИС','rgb(220, 128, 0)')
    ON CONFLICT (title) DO UPDATE SET color=EXCLUDED.color 
    RETURNING id INTO aid_gis;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_gis, pid_desc_gis)
                                                 ,(aid_gis, prid_usehours)
                                                 ,(aid_gis, prid_depth)
                                                 ,(aid_gis, prid_press)
                                                 ,(aid_gis, prid_temp)
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title) VALUES ('Изменить описание') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_change_desc;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_change_desc, prid_desc) ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;

  INSERT INTO act (title) VALUES ('Изменить примечание') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_change_note;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_change_note, prid_note) ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;


  INSERT INTO act (title) VALUES ('Изменить сведения о работнике') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_ch_worker_info;
  INSERT INTO ref_act_prop(act_id, prop_id)VALUES (aid_ch_worker_info, pid_fm)
                                                 ,(aid_ch_worker_info, pid_nm)
                                                 ,(aid_ch_worker_info, pid_ot)
                                                 ON CONFLICT ON CONSTRAINT uk_refactprop__actid_propid DO NOTHING;


  RAISE NOTICE 'добавляем основные классы';
  INSERT INTO acls(pid,title,kind,dobj) VALUES (1,'Структурные подразделения',0,NULL) RETURNING id INTO cid_struct_unit;
  INSERT INTO acls(pid,title,kind,dobj) VALUES (1,'Геофизическое оборудование',0,NULL) RETURNING id INTO cid_geo_equipment;
  INSERT INTO acls(pid,title,kind,measure) VALUES (cid_struct_unit,'Предприятие',1,'ед.') RETURNING id INTO cid_company;
  INSERT INTO acls(pid,title,kind,measure) VALUES (cid_struct_unit,'Отдел',1,'ед.') RETURNING id INTO cid_department;
  INSERT INTO acls(pid,title,kind,measure) VALUES (cid_struct_unit,'Участок СЦ',1,'ед.') RETURNING id INTO cid_sector_sc;
  INSERT INTO acls(pid,title,kind,measure) VALUES (1,'Персонал',1,'чел.') RETURNING id INTO cid_personal;
  INSERT INTO acls(pid,title,kind,measure) VALUES (1,'Заказ',1,'шт.') RETURNING id INTO cid_zakaz; 
  INSERT INTO acls(pid,title,kind,dobj) VALUES (1,'ЗИП',0,NULL) RETURNING id INTO cid_zip ; 


  RAISE NOTICE 'добавляем основные объекты';
  INSERT INTO obj(title,cls_id,pid) VALUES ('Гирс-Сервис',cid_company, 1 )RETURNING id INTO oid_company_girs;
  INSERT INTO obj(title,cls_id,pid) VALUES ('ЦГГ',cid_company, 1 )RETURNING id INTO oid_company_cgg;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Севергазгеофизика',cid_company, 1 )RETURNING id INTO oid_company_sgg;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Сервисный центр',cid_department, oid_company_sgg )RETURNING id INTO oid_department_sc;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Ремонтный участок',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_repair;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Долгосрочный ремонт',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_repair_wait;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Метрология',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_metrology;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Пункт проката',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_pp;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Пункт выдачи',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_pv;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Склад',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_warehouse;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Консервация',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_conservation;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Утилизация',cid_sector_sc, oid_department_sc )RETURNING id INTO oid_sector_sc_utilize;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Заказ ГО',cid_zakaz, oid_company_sgg )RETURNING id INTO oid_zakaz_go;
  INSERT INTO obj(title,cls_id,pid) VALUES ('Заказ МТР',cid_zakaz, oid_company_sgg )RETURNING id INTO oid_zakaz_mtr;

  RAISE NOTICE 'разрешения действий';

  INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('TypeDesigner', 0, 1, NULL, aid_ch_service_oid );

  INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('TypeDesigner', 0, cid_personal, NULL, aid_ch_worker_info );

  INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('TypeDesigner', 0, cid_department, NULL, aid_change_desc );

  RAISE NOTICE 'добавляем ДЕЙСТВИЯ для всей категории геофизического оборудования';
  INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id, src_path)VALUES 
    ('TypeDesigner',          0, cid_geo_equipment, NULL, aid_chmain,   '{%}' )
   ,('Инженер по ремонту ГО', 0, cid_geo_equipment, NULL, aid_remont,   format('{{%s,%s},%%}',cid_sector_sc,oid_sector_sc_repair) )
   ,('Инженер по ремонту ГО', 0, cid_geo_equipment, NULL, aid_proverka, format('{{%s,%s},%%}',cid_sector_sc,oid_sector_sc_repair) ) 
   ,('Инженер по ремонту ГО', 0, cid_geo_equipment, NULL, aid_prof,     format('{{%s,%s},%%}',cid_sector_sc,oid_sector_sc_repair) ) 
   ,('Инженер-метролог',      0, cid_geo_equipment, NULL, aid_calib,    format('{{%s,%s},%%}',cid_sector_sc,oid_sector_sc_metrology) )
   ,('Диспетчер ГО',          0, cid_geo_equipment, NULL, aid_gis,      format('{{%s,%%},%%}',cid_personal) );


  ----------------
  -- Движение ГО
  ----------------
  RAISE NOTICE 'добавляем ПРАВИЛА ПЕРЕМЕЩЕНИЯ для ГО';
  idpath_sgg_sc := format('{{%s,%s},{%s,%s}%%}', cid_department, oid_department_sc,cid_company,oid_company_sgg);

  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Инженер по ремонту ГО', 0 ,cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_metrology, idpath_sgg_sc );
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ( 'Инженер по ремонту ГО', 0,cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc
    ,cid_sector_sc, oid_sector_sc_pp, idpath_sgg_sc );
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ( 'Инженер по ремонту ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc
    ,cid_sector_sc, oid_sector_sc_repair_wait, idpath_sgg_sc);
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Инженер по ремонту ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair_wait, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Ремонтный  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Консервация
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Вед.инженер по ремонту ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_conservation, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Консервация >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Вед.инженер по ремонту ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_conservation, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Инженер по ремонту ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_utilize, idpath_sgg_sc );
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Инженер-метролог', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_metrology, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Инженер-метролог', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_metrology, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_pp, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт проката  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Метрология
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_pp, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_metrology, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт проката  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_pp, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт проката  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт выдачи
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_pp, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_pv, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт выдачи  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_pv, idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Пункт выдачи  >>  /*[Персонал]*
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_pv, idpath_sgg_sc 
    ,cid_personal, NULL, '{%}' );
  -- /*[Персонал]*  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок СЦ]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES
    ( 'Диспетчер ГО', 0, cid_geo_equipment, NULL
    ,cid_personal, NULL, '{%}' 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Заказ]Заказ ГО  >>  /*[Предприятие]Гирс-Сервис
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_geo_equipment, NULL
    ,cid_zakaz,   oid_zakaz_go,     format('{{%s,%s}%%}',cid_company,oid_company_sgg)
    ,cid_company, oid_company_girs, '{%}' );
  -- /*[Предприятие]Гирс-Сервис  >>  /*[Предприятие]СГГ/[Заказ]Заказ ГО
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_geo_equipment, NULL
    ,cid_company, oid_company_girs, '{%}' 
    ,cid_zakaz,   oid_zakaz_go,       format('{{%s,%s}%%}',cid_company,oid_company_sgg) );
  -- /*[Предприятие]Гирс-Сервис  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_geo_equipment, NULL
    ,cid_company, oid_company_girs,          '{%}' 
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный  >>  /*[Предприятие]Гирс-Сервис
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_geo_equipment, NULL
    ,cid_sector_sc, oid_sector_sc_repair, idpath_sgg_sc
    ,cid_company, oid_company_girs,          '{%}' );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Утилизация
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_geo_equipment, NULL
    ,cid_sector_sc, NULL,  idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_utilize, idpath_sgg_sc );
  ----------------
  -- Движение МТР
  ----------------
  RAISE NOTICE 'добавляем ПРАВИЛА ПЕРЕМЕЩЕНИЯ для МТР';
  -- /*[Предприятие]СГГ/[Заказ]Заказ МТР  >>  /*[Предприятие]Гирс-Сервис
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_zakaz,   oid_zakaz_mtr,     format('{{%s,%s}%%}',cid_company,oid_company_sgg)
    ,cid_company, oid_company_girs, '{%}' );
  -- /*[Предприятие]Гирс-Сервис  >>  /*[Предприятие]СГГ/[Заказ]Заказ ГО
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_company, oid_company_girs, '{%}' 
    ,cid_zakaz,   oid_zakaz_go,       format('{{%s,%s}%%}',cid_company,oid_company_sgg) );
  -- /*[Предприятие]Гирс-Сервис  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_company, oid_company_girs,          '{%}' 
    ,cid_sector_sc, oid_sector_sc_warehouse, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный  >>  /*[Предприятие]Гирс-Сервис
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_sector_sc, oid_sector_sc_warehouse, idpath_sgg_sc
    ,cid_company, oid_company_girs,          '{%}' );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Склад  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_sector_sc, oid_sector_sc_warehouse, idpath_sgg_sc
    ,cid_sector_sc, oid_sector_sc_repair,    idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Склад
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_sector_sc, oid_sector_sc_repair,    idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_warehouse, idpath_sgg_sc );
  -- /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Ремонтный  >>  /*[Предприятие]СГГ/[Отдел]Сервисный центр/[Участок]Утилизация
  INSERT INTO perm_move( access_group, access_disabled, cls_id, obj_id, src_cls_id, src_obj_id, src_path ,dst_cls_id, dst_obj_id, dst_path)VALUES 
    ('Кладовщик', 0 ,cid_zip, NULL
    ,cid_sector_sc, oid_sector_sc_repair,  idpath_sgg_sc 
    ,cid_sector_sc, oid_sector_sc_utilize, idpath_sgg_sc );


END $BODY$ LANGUAGE plpgsql;

SELECT sgg_add_prop_and_act();

-------------------------------------------------------------------------------
-- импортируем отделы'
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

  pid_service_oid BIGINT;
  aid_ch_service_oid BIGINT;
BEGIN
  RAISE NOTICE 'Import departaments';

  SELECT id INTO pid_service_oid FROM prop WHERE title = 'Идентификатор объекта в Service';
  SELECT id INTO aid_ch_service_oid FROM act WHERE title='Изменить Идентификатор объекта в Service' ;

  INSERT INTO act (title) VALUES ('Изменить Идентификатор объекта в Service') ON CONFLICT (title) DO UPDATE SET title=EXCLUDED.title
    RETURNING id INTO aid_ch_service_oid;
  
  SELECT id INTO sgg_company_oid FROM obj WHERE title='Севергазгеофизика';
  SELECT id INTO department_cid FROM acls WHERE title='Отдел';
  SELECT id INTO _pid_desc FROM prop WHERE title='Описание';
  SELECT id INTO _aid_chfndep FROM act WHERE title='Изменить описание' ;

  FOR rec IN import_dep LOOP
    SELECT id INTO _oid FROM obj WHERE title=rec.title;
    RAISE DEBUG 'append departament % % ',_oid,rec;
    IF NOT FOUND THEN
      INSERT INTO obj(title,cls_id,pid) VALUES (rec.title,department_cid, sgg_company_oid ) RETURNING id INTO _oid;
    END IF;

    PERFORM lock_for_act(_oid, sgg_company_oid);
    _prop_val :=format('{"%s":"%s"}',pid_service_oid, rec.id );
    PERFORM do_act(_oid, aid_ch_service_oid, _prop_val);
    PERFORM lock_reset(_oid, sgg_company_oid);

    _prop_val := format('{"%s":"%s"}',_pid_desc, rec.note );
    PERFORM lock_for_act(_oid, sgg_company_oid);
    PERFORM do_act(_oid, _aid_chfndep, _prop_val);
    PERFORM lock_reset(_oid, sgg_company_oid);
    --INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cid, 1, _pid_fndep, rec.note);
  END LOOP;
  
RETURN;
END; 
$BODY$ LANGUAGE plpgsql;


SELECT sgg_sc_import_departament();
-------------------------------------------------------------------------------
-- импортируем работников'
------------------------------------------------------------------------------
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

  pid_service_oid BIGINT;
  aid_ch_service_oid BIGINT;

BEGIN
  RAISE NOTICE 'Import personnel';
  
  SELECT id INTO _cid_personal FROM acls WHERE title='Персонал';

  SELECT id INTO _pid_fam FROM prop WHERE title='Фамилия';
  SELECT id INTO _pid_nm FROM prop WHERE title='Имя';
  SELECT id INTO _pid_ot FROM prop WHERE title='Отчество';

  SELECT id INTO _aid_ch_worker_info FROM act WHERE title='Изменить сведения о работнике' ;

  SELECT id INTO pid_service_oid FROM prop WHERE title = 'Идентификатор объекта в Service';
  SELECT id INTO aid_ch_service_oid FROM act WHERE title='Изменить Идентификатор объекта в Service' ;
 

  FOR rec IN import_worker LOOP
    SELECT title INTO _dep_title FROM __departament WHERE id=rec.dep_id;
    SELECT id INTO _oid_dep FROM obj WHERE title=_dep_title;

    _user_title:= rec.fam || ' ' || substring(rec.im from 1 for 1) || '.' || substring(rec.oth from 1 for 1) || '.' ;
    --_user_title:= iris_translit(_user_title);
    _user_title_idx = 0;
    LOOP
      PERFORM FROM obj WHERE cls_id=_cid_personal AND title=_user_title;
      IF FOUND THEN
       _user_title:= rec.fam || ' ' || substring(rec.im from 1 for 1) || '.' || substring(rec.oth from 1 for 1) || '. ' ||_user_title_idx;
       _user_title_idx:=_user_title_idx+1;
      ELSE 
        EXIT;
      END IF;
    END LOOP;
    
    INSERT INTO obj(title,cls_id,pid) VALUES (_user_title,_cid_personal, _oid_dep ) RETURNING id INTO _oid;
    UPDATE __worker SET wh3_oid = _oid, wh3_cid=_cid_personal WHERE  id=rec.id;

    PERFORM lock_for_act(_oid, _oid_dep);
    _prop_val := format('{"%s":"%s"}',pid_service_oid, rec.id );
    PERFORM do_act(_oid, aid_ch_service_oid, _prop_val);
    PERFORM lock_reset(_oid, _oid_dep);


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
-- импорт классов из старой базы
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
  
  pid_service_cid  BIGINT;
  pid_cal_period   BIGINT;
  pid_note         BIGINT;
  pid_desc         BIGINT;

  _cls_id      BIGINT;
  

  _curr_oid      BIGINT;

  _curr_pid      BIGINT;

  cid_zakaz      BIGINT;

  oid_zakaz_go      BIGINT;
BEGIN
  RAISE NOTICE 'Import cls tree';

  SELECT id INTO pid_service_cid FROM prop WHERE title = 'Идентификатор класса в Service';
  SELECT id INTO pid_cal_period  FROM prop WHERE title = 'Период калибровки(мес.)';
  SELECT id INTO pid_desc        FROM prop WHERE title = 'Описание';
  
  SELECT id INTO _geo_equipment_id FROM acls WHERE title = 'Геофизическое оборудование';  

  RAISE NOTICE 'Import level 0';
  FOR rec IN import_cls00 LOOP
    --RAISE DEBUG 'ADD ABSTRACT CLS=% TO ROOT ',rec.title;
    INSERT INTO acls(pid,title,kind,dobj) VALUES (_geo_equipment_id,rec.title,0,NULL);
  END LOOP;

  RAISE NOTICE 'Import level 1';
  FOR rec IN import_cls01 LOOP
    SELECT title INTO _title FROM __cls00 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    --RAISE DEBUG 'ADD ABSTRACT CLS=% TO % (%)',rec.title,_title,rec;
    INSERT INTO acls(pid,title,kind,dobj) VALUES (_pid,rec.title,0,NULL) ;
  END LOOP;

  SELECT id INTO cid_zakaz     FROM acls WHERE title='Заказ';
  SELECT id INTO oid_zakaz_go  FROM obj WHERE title='Заказ ГО' ;

  RAISE NOTICE 'Import cls';
  FOR rec IN import_cls LOOP
    SELECT title INTO _title FROM __cls01 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    --RAISE DEBUG 'ADD NUMERIC CLS=% (period=%) TO % (%)',rec.title,rec.period,_title,rec;
    INSERT INTO acls(pid,title,kind,measure, dobj) VALUES (_pid,rec.title,1,'ед', oid_zakaz_go) RETURNING id INTO _cls_id ;
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, pid_service_cid, rec.id);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, pid_cal_period, rec.period);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, pid_desc, NULL);
  END LOOP;

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_import_cls();




--COMMIT TRANSACTION;


--BEGIN TRANSACTION;



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
  _oid_conservation BIGINT;
  _cid_conservation BIGINT;
  _oid_metr BIGINT;
  _cid_metr BIGINT;
  _oid_pv BIGINT;
  _cid_pv BIGINT;

  _aid_maininfo BIGINT;
  _aid_gis    BIGINT;
  _aid_chnote BIGINT;
  _aid_kalibr BIGINT;
  _aid_repair BIGINT;
  _aid_proverka BIGINT;
  _aid_profil BIGINT;
  _aid_ch_service_oid BIGINT;
  
  
  _pid_note        BIGINT;
  _pid_usehours    BIGINT;
  _pid_depth       BIGINT;
  _pid_press       BIGINT;
  _pid_temp        BIGINT;
  _pid_repairnote  BIGINT;
  _pid_desc_profil BIGINT;
  _pid_desc_prover BIGINT;
  _pid_desc_kalibr BIGINT;
  _pid_kalfile     BIGINT;
  _pid_invn      BIGINT;
  _pid_pasp      BIGINT;
  _pid_objfolder BIGINT;
  _pid_reldate   BIGINT;
  _pid_indate    BIGINT;
  _pid_service_oid BIGINT;
  _pid_desc_gis BIGINT;

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

  processing_obj BIGINT;
  processing_hist BIGINT;

BEGIN
  RAISE NOTICE 'Import obj and history ';
  RAISE NOTICE 'Objects=% History=%',(SELECT count(*) FROM __obj),(SELECT count(*) FROM __hist);
  processing_obj:=0;
  processing_hist:=0;

  
  SELECT id INTO _pid_note FROM prop WHERE title='Примечание';
  SELECT id INTO _pid_usehours FROM prop WHERE title='Наработка(ч.)';
  SELECT id INTO _pid_depth FROM prop WHERE title='Глубина(м.)';
  SELECT id INTO _pid_press FROM prop WHERE title='Давление(МПа)';
  SELECT id INTO _pid_temp FROM prop WHERE title='Температура(град.С)';
  SELECT id INTO _pid_repairnote FROM prop WHERE title='Описание ремонта';
  SELECT id INTO _pid_desc_profil FROM prop WHERE title='Описание профилактики';
  SELECT id INTO _pid_desc_prover FROM prop WHERE title='Описание проверки';
  SELECT id INTO _pid_desc_kalibr FROM prop WHERE title='Описание калибровки';
  SELECT id INTO _pid_desc_gis FROM prop WHERE title='Описание ГИС';
  SELECT id INTO _pid_kalfile FROM prop WHERE title='Файл калибровки';
  SELECT id INTO _pid_invn FROM prop WHERE title='Инвентарный номер' ;
  SELECT id INTO _pid_pasp FROM prop WHERE title='Паспорт' ;
  SELECT id INTO _pid_objfolder FROM prop WHERE title='Папка прибора' ;
  SELECT id INTO _pid_reldate FROM prop WHERE title='Дата выпуска' ;
  SELECT id INTO _pid_indate FROM prop WHERE title='Дата ввода в эксплуатацию' ;
  SELECT id INTO _pid_service_oid FROM prop WHERE title='Идентификатор объекта в Service' ;

  SELECT id,cls_id  INTO _oid_repair,_cid_repair FROM obj_name WHERE title='Ремонтный участок';
  SELECT id,cls_id  INTO _oid_drepair,_cid_drepair FROM obj_name WHERE title='Долгосрочный ремонт';
  SELECT id,cls_id  INTO _oid_conservation,_cid_conservation FROM obj_name WHERE title='Консервация';
  SELECT id,cls_id  INTO _oid_metr,_cid_metr FROM obj_name WHERE title='Метрология';
  SELECT id,cls_id  INTO _oid_pv,_cid_pv FROM obj_name WHERE title='Пункт выдачи';

  SELECT id INTO _aid_ch_service_oid FROM act WHERE title='Изменить Идентификатор объекта в Service' ;
  SELECT id INTO _aid_maininfo FROM ACT WHERE title='Изменить основные свойства' ;
  SELECT id INTO _aid_gis FROM act WHERE title='ГИС';
  SELECT id INTO _aid_chnote FROM act WHERE title='Изменить примечание';
  SELECT id INTO _aid_kalibr FROM act WHERE title='Калибровка';
  SELECT id INTO _aid_profil FROM act WHERE title='Профилактика';
  SELECT id INTO _aid_proverka FROM act WHERE title='Проверка';
  SELECT id INTO _aid_repair FROM act WHERE title='Ремонт';


  FOR impobj IN import_obj LOOP
    processing_obj:=processing_obj+1;
    IF ( processing_obj%300 = 0) THEN 
      RAISE NOTICE '% Processed objects % history %',now()::TIME,processing_obj,processing_hist;
    END IF ;
    -- создаём объект
    SELECT title INTO _cls_title FROM __cls WHERE id = impobj.cls_id;
    SELECT id INTO _cid_obj FROM acls WHERE title=_cls_title;
    RAISE DEBUG '_cid_obj = % sc_oid=% _cls_title=%',_cid_obj, impobj.obj_id,_cls_title;
    INSERT INTO obj(title,cls_id,pid) VALUES (impobj.title, _cid_obj, _oid_pv )RETURNING id INTO _oid_obj;
    UPDATE __obj SET wh3_oid = _oid_obj, wh3_cid=_cid_obj WHERE obj_id=impobj.obj_id;

    -- добавляем основные сведения 
    _src_path:=format('{{%s,%s},{2,1}}',_cid_pv,_oid_pv); -- помещаем в пункт выдачи
    _prop:= ('{}'::JSONB);
    _date:=NULL;
    _note:=NULL;
    _act_lid_previos:=NULL;
    _lid :=NULL;
    -- берём первую дату упоминания
    SELECT hinput_date INTO _date FROM __hist WHERE hinput_date IS NOT NULL AND sc_oid=impobj.obj_id ORDER BY hinput_date ASC LIMIT 1;
    _date:=COALESCE(_date,now()) - '1 week'::INTERVAL;--отматываем неделю назад на всякий случай
    RAISE DEBUG 'first date = % first_obj=%',_date,impobj;
    -- вводим идентификатор старой базы
    _prop:=_prop || format('{"%s":"%s"}',_pid_service_oid, impobj.obj_id )::JSONB;
    INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
    INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_ch_service_oid, _prop)  RETURNING id INTO _act_lid_previos;
    -- вводим основные сведения
    _prop:=_prop || format('{"%s":"%s"}', _pid_invn, impobj.invtitle)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_pasp, impobj.pasport_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_kalfile, impobj.curr_cal_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_objfolder, impobj.folder_path)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_reldate, impobj.release_date)::JSONB;
    _prop:=_prop || format('{"%s":"%s"}', _pid_indate, impobj.inservice_date)::JSONB;
    INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
    INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid,_aid_maininfo, _prop)  RETURNING id INTO _act_lid_previos;

    FOR rec IN import_log(impobj.obj_id) LOOP
      processing_hist:=processing_hist+1;
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
        _prop:=_prop || format('{"%s":"%s"}', _pid_desc_gis, _note)::JSONB;
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
                _prop:=_prop || format('{"%s":"%s"}',   _pid_desc_prover, _note )::JSONB;
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

      -- перемещаем в Пункт выдачи
      RAISE DEBUG 'MOVE TO PV';
      IF(rec.hto_pp_date IS NOT NULL) THEN
        _date:=COALESCE (rec.hto_pp_date,rec.hinput_date) +'07:00:00'::INTERVAL;
        _dst_path:=format('{{%s,%s},{2,1}}',_cid_pv,_oid_pv);
        INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
        INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
        _src_path:=_dst_path;
        UPDATE obj_num SET pid = _oid_pv WHERE id=_oid_obj;
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
    
    -- ставим последнее примечание
    _note:= COALESCE (impobj.note1,'') || ' ' || COALESCE (impobj.note2,'');
    _prop:=_prop || format('{"%s":"%s"}', _pid_note, _note)::JSONB;
    _date:=_date+'01:00:00'::INTERVAL;
    INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
    INSERT INTO log_detail_act(id, act_id, prop)     VALUES (_lid, _aid_maininfo, _prop)      RETURNING id INTO _act_lid_previos;
    UPDATE obj_name SET prop = _prop WHERE id=_oid_obj;

    -- Перемещаем в консервацию (архив)
    RAISE DEBUG 'MOVE TO CONSERVATION';
    IF(impobj.arhived IS TRUE) THEN
      _date:=_date+'09:00:00'::INTERVAL;
      _dst_path:=format('{{%s,%s},{2,1}}',_cid_conservation,_oid_conservation);
      INSERT INTO log_main(timemark, src_path, obj_id) VALUES (_date, _src_path, _oid_obj) RETURNING id INTO _lid;
      INSERT INTO log_detail_move(id, dst_path, qty, prop_lid) VALUES (_lid,  _dst_path, 1, _act_lid_previos);
      _src_path:=_dst_path;
      UPDATE obj_num SET pid = _oid_conservation WHERE id=_oid_obj;
    END IF;

  END LOOP; --FOR impobj IN import_obj LOOP

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_import_hist();


SELECT sgg_drop_temporary_items();
DROP FUNCTION IF EXISTS sgg_drop_temporary_items() CASCADE;


COMMIT TRANSACTION;

--VACUUM;
--ANALYZE;





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
