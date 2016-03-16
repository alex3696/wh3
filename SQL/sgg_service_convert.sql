--SET client_min_messages='debug1';
--SHOW client_min_messages;


DROP TABLE IF EXISTS __cls00 CASCADE;
CREATE TABLE __cls00
(
  id bigint NOT NULL,
  title text NOT NULL
);
COPY __cls00 FROM 'c:\_SAV\tmp\__cls00.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
CREATE UNIQUE INDEX idxu__cls00_id ON __cls00 (id);
CREATE UNIQUE INDEX idxu__cls00_title ON __cls00 (title);


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
  ,use_hours text
  ,note1 text
  ,note2 text
  ,arhived text
  ,ts text
);
COPY __obj FROM 'c:\_SAV\tmp\__obj.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем ерунду';
PRINT '';
-------------------------------------------------------------------------------
DELETE FROM __cls WHERE title ~~* '%нет данных%';
DELETE FROM __cls01 WHERE title ~~* '%нет данных%';
DELETE FROM __cls00 WHERE title ~~* '%нет данных%';
-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем все записи классы/свойства';
PRINT '';
-------------------------------------------------------------------------------
DELETE FROM acls WHERE id > 99;
DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
-------------------------------------------------------------------------------
PRINT '';
PRINT '- удаляем из имён запрещённые символы';
PRINT '';
-------------------------------------------------------------------------------
UPDATE __cls00  SET title=replace(title, '/', '|');
UPDATE __cls01  SET title=replace(title, '/', '|');
UPDATE __cls  SET title=replace(title, '/', '|');
UPDATE __obj  SET title=replace(title, '/', '|');
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
-------------------------------------------------------------------------------
-- заполняем и настраиваем базовую структуру
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
SET @prid_press =INSERT INTO prop(title, kind)VALUES('Давление(ч.)', 101)RETURNING id;
SET @prid_temp =INSERT INTO prop(title, kind)VALUES('Температура(ч.)', 101)RETURNING id;

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
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_remont, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_remont, @prid_remdesc);

SET @act_id_proverka = INSERT INTO act (title) VALUES ('Проверка')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_proverka, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_proverka, @prid_usehours);

SET @act_id_prof =     INSERT INTO act (title) VALUES ('Профилактика')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_prof, @prid_note);

SET @act_id_calib =     INSERT INTO act (title) VALUES ('Калибровка')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_calib, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_calib, @prid_calfile);

SET @act_id_gis =      INSERT INTO act (title) VALUES ('ГИС')RETURNING id;
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_note);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_usehours);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_depth);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_press);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_gis, @prid_temp);


-------------------------------------------------------------------------------
PRINT '';
PRINT '- добавляем основные классы';
PRINT '';
-------------------------------------------------------------------------------
DECLARE @cid_personal;
SET @cid_personal = INSERT INTO cls(pid,title,kind,measure) VALUES (1,'Персонал',1,'чел.') RETURNING id;

DECLARE @struct_units_id, @geo_equipment_id;
SET @struct_units_id = INSERT INTO cls(pid,title,kind) VALUES (1,'Структурные подразделения',0) RETURNING id;
SET @geo_equipment_id = INSERT INTO cls(pid,title,kind) VALUES (1,'Геофизическое оборудование',0) RETURNING id;

DECLARE @company_id, @department_id, @department_area_id;
SET @company_id = INSERT INTO cls(pid,title,kind,measure) VALUES (@struct_units_id,'Предприятие',1,'ед.') RETURNING id;
SET @department_id = INSERT INTO cls(pid,title,kind,measure) VALUES (@struct_units_id,'Отдел',1,'ед.') RETURNING id;
SET @department_area_id = INSERT INTO cls(pid,title,kind,measure) VALUES (@struct_units_id,'Участок СЦ',1,'ед.') RETURNING id;

DECLARE @sgg_company_id;
SET @sgg_company_id = INSERT INTO obj(title,cls_id,pid) VALUES ('Севергазгеофизика',@company_id, 1 )RETURNING id;

DECLARE @sc_departament_id,@sc_dep_kontr_id,@sc_dep_bur_id,@sc_dep_gti_id;
SET @sc_departament_id = INSERT INTO obj(title,cls_id,pid) VALUES ('СЦ',@department_id, @sgg_company_id )RETURNING id;
SET @sc_dep_kontr_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ Контроль',@department_id, @sgg_company_id )RETURNING id;
SET @sc_dep_bur_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ Бурение',@department_id, @sgg_company_id )RETURNING id;
SET @sc_dep_gti_id = INSERT INTO obj(title,cls_id,pid) VALUES ('ПГЭ ГТИ',@department_id, @sgg_company_id )RETURNING id;


DECLARE @remont,@metrolog,@pp,@sklad,@konserv,@dremont,@spisano;
SET @remont = INSERT INTO obj(title,cls_id,pid) VALUES ('Ремонт',@department_area_id, @sc_departament_id )RETURNING id;
SET @metrolog = INSERT INTO obj(title,cls_id,pid) VALUES ('Метрология',@department_area_id, @sc_departament_id )RETURNING id;
SET @pp = INSERT INTO obj(title,cls_id,pid) VALUES ('Пункт проката',@department_area_id, @sc_departament_id )RETURNING id;
SET @sklad = INSERT INTO obj(title,cls_id,pid) VALUES ('Склад',@department_area_id, @sc_departament_id )RETURNING id;
SET @konserv = INSERT INTO obj(title,cls_id,pid) VALUES ('Консервация',@department_area_id, @sc_departament_id )RETURNING id;
SET @dremont = INSERT INTO obj(title,cls_id,pid) VALUES ('Долгосрочный ремонт',@department_area_id, @sc_departament_id )RETURNING id;
SET @spisano = INSERT INTO obj(title,cls_id,pid) VALUES ('Списано',@department_area_id, @sc_departament_id )RETURNING id;



-------------------------------------------------------------------------------
-- конвертер из старой базы
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS sgg_sc_import() CASCADE;
CREATE OR REPLACE FUNCTION sgg_sc_import()
 RETURNS VOID  AS
$BODY$
DECLARE
  

  import_cls00 CURSOR IS SELECT id, title FROM __cls00;
  import_cls01 CURSOR IS SELECT id, title, pid FROM __cls01;
  import_cls CURSOR IS SELECT id, title, period, pid FROM __cls;
  import_obj CURSOR IS 
    SELECT obj_id, cls_id, title, invtitle, pasport_path, next_cal_date, 
       preios_cal_date, curr_cal_path, previos_cal_path, user_id_1, 
       user_id_2, id_priznak, id_uchastok, id_gr_part, folder_path, 
       release_date, inservice_date, use_hours, note1, note2, arhived, ts
    FROM __obj ;

  _geo_equipment_id      BIGINT;

  _title      TEXT;
  _pid      BIGINT;
  

  
  _prop_cal_period_id INTEGER;
  _prop_note_id INTEGER;


  _cls_id      BIGINT;

  _prop_desc_id      BIGINT;
  

  _curr_oid      BIGINT;
  _curr_aid      BIGINT;


  _prid_note BIGINT;
  _prid_invn BIGINT;
  _prid_pasp BIGINT;
  _prid_calfile BIGINT;
  _prid_objfolder BIGINT;
  _prid_reldate BIGINT;
  _prid_indate BIGINT;
  _prid_usehours BIGINT;

  _prop_val JSONB;
BEGIN
  SELECT id INTO _prop_cal_period_id FROM prop WHERE title = 'Период калибровки(мес.)';
  SELECT id INTO _prop_desc_id FROM prop WHERE title = 'Описание';


  SELECT id INTO _geo_equipment_id FROM acls WHERE title = 'Геофизическое оборудование';  



--_act_cal_period_id : = INSERT INTO act (title) VALUES ('Изменить основные калибровки')RETURNING id;

  FOR rec IN import_cls00 LOOP
    RAISE DEBUG 'ADD ABSTRACT CLS=% TO ROOT ',rec.title;
    INSERT INTO cls(pid,title,kind) VALUES (_geo_equipment_id,rec.title,0);
  END LOOP;

  FOR rec IN import_cls01 LOOP
    SELECT title INTO _title FROM __cls00 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    RAISE DEBUG 'ADD ABSTRACT CLS=% TO % (%)',rec.title,_title,rec;
    INSERT INTO cls(pid,title,kind) VALUES (_pid,rec.title,0) ;
  END LOOP;

  SELECT id INTO _curr_aid FROM ACT WHERE title='Изменить основные свойства' ;
  
  FOR rec IN import_cls LOOP
    SELECT title INTO _title FROM __cls01 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    RAISE DEBUG 'ADD NUMERIC CLS=% (period=%) TO % (%)',rec.title,rec.period,_title,rec;
    INSERT INTO cls(pid,title,kind,measure) VALUES (_pid,rec.title,1,'ед') RETURNING id INTO _cls_id ;
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, _prop_cal_period_id, rec.period);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, _prop_desc_id, NULL);


    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
      VALUES ('TypeDesigner', 0, _cls_id, NULL, _curr_aid );

    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
      VALUES ('Инженер по ремонту ГО', 0, _cls_id, NULL, (SELECT id FROM ACT WHERE title='Ремонт' )
      ,(SELECT '{'||cls_id||','||id||',%}' FROM obj_name WHERE title='Ремонт'));
      
    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
      VALUES ('Инженер по ремонту ГО', 0, _cls_id, NULL, (SELECT id FROM ACT WHERE title='Проверка' )
      ,(SELECT '{'||cls_id||','||id||',%}' FROM obj_name WHERE title='Ремонт'));
      
    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
      VALUES ('Инженер по ремонту ГО', 0, _cls_id, NULL, (SELECT id FROM ACT WHERE title='Профилактика' )
      ,(SELECT '{'||cls_id||','||id||',%}' FROM obj_name WHERE title='Ремонт'));
      
    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
      VALUES ('Инженер-метролог', 0, _cls_id, NULL, (SELECT id FROM ACT WHERE title='Калибровка' )
      ,(SELECT '{'||cls_id||','||id||',%}' FROM obj_name WHERE title='Метрология'));
      
    INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id,src_path)
      VALUES ('Диспетчер ГО', 0, _cls_id, NULL, (SELECT id FROM ACT WHERE title='ГИС' )
      ,(SELECT '{'||cls_id||','||id||',%}' FROM obj_name WHERE title='Пункт проката'));

  END LOOP;

  SELECT id INTO _prid_note FROM prop WHERE title='Примечание' ;
  SELECT id INTO _prid_invn FROM prop WHERE title='Инвентарный номер' ;
  SELECT id INTO _prid_pasp FROM prop WHERE title='Паспорт' ;
  SELECT id INTO _prid_calfile FROM prop WHERE title='Файл калибровки' ;
  SELECT id INTO _prid_objfolder FROM prop WHERE title='Папка прибора' ;
  SELECT id INTO _prid_reldate FROM prop WHERE title='Дата выпуска' ;
  SELECT id INTO _prid_indate FROM prop WHERE title='Дата ввода в эксплуатацию' ;
  SELECT id INTO _prid_usehours FROM prop WHERE title='Наработка(ч.)' ;

  
  FOR rec IN import_obj LOOP
    SELECT title INTO _title FROM __cls WHERE id = rec.cls_id;
    SELECT id INTO _cls_id FROM acls WHERE title = _title;
    RAISE DEBUG 'ADD OBJECT [%]% ', _title,rec.title;
    INSERT INTO obj(title,cls_id,pid) VALUES (rec.title, _cls_id, 1 )RETURNING id INTO _curr_oid;


    rec.pasport_path := replace(rec.pasport_path, '\', '\\\\');
    rec.curr_cal_path := replace(rec.curr_cal_path, '\', '\\\\');
    rec.folder_path := replace(rec.folder_path, '\', '\\\\');

    rec.note1 := replace(rec.note1, '"', '\\"');
    rec.note2 := replace(rec.note2, '"', '\\"');

    RAISE DEBUG 'rec.pasport_path=%', rec.pasport_path;
    RAISE DEBUG 'rec.pasport_path=%', rec.pasport_path;

    _prop_val := format('{"%s":"%s","%s":"%s","%s":"%s","%s":"%s","%s":"%s","%s":"%s","%s":"%s","%s":%s}', 
      _prid_note, COALESCE(rec.note1,'')||' '||COALESCE(rec.note2,''),
      _prid_invn, rec.invtitle,
      _prid_pasp, replace(rec.pasport_path, '\', '\\'),
      _prid_calfile, replace(rec.curr_cal_path, '\', '\\'),
      _prid_objfolder, replace(rec.folder_path, '\', '\\'),
      _prid_reldate, rec.release_date,
      _prid_indate, rec.inservice_date,
      _prid_usehours, COALESCE(rec.use_hours,'0') );

    RAISE DEBUG 'DO ACT: oid=% aid=% prop=%', _curr_oid, _curr_aid, _prop_val;
    PERFORM lock_for_act(_curr_oid, 1);
    PERFORM do_act(_curr_oid, _curr_aid, _prop_val);
    PERFORM lock_reset(_curr_oid, 1);
    
  END LOOP;

RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_sc_import();


SELECT * FROM prop;

SELECT format('{"%s":"%s"}', '123','qwe')::JSONB;

















