SET client_min_messages='debug1';
SHOW client_min_messages;


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


DROP TABLE IF EXISTS __cls;
CREATE TABLE __cls
(
  id bigint NOT NULL,
  title text NOT NULL,
  period integer,
  pid bigint NOT NULL REFERENCES __cls01(id)
);
COPY __cls FROM 'c:\_SAV\tmp\__cls.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;

DROP TABLE IF EXISTS __obj;
CREATE TABLE __obj
(
  obj_id bigint NOT NULL
  ,cls_id text NOT NULL
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

DELETE FROM __cls WHERE title ~~* '%нет данных%';
DELETE FROM __cls01 WHERE title ~~* '%нет данных%';
DELETE FROM __cls00 WHERE title ~~* '%нет данных%';

DELETE FROM acls WHERE id > 174;

UPDATE __cls00  SET title=title || '_0';
UPDATE __cls01  SET title=title || '_1';

UPDATE __cls00  SET title=replace(title, '/', '|');
UPDATE __cls01  SET title=replace(title, '/', '|');
UPDATE __cls  SET title=replace(title, '/', '|');

ALTER TABLE __cls ALTER COLUMN title TYPE whname;





-------------------------------------------------------------------------------
-- поиск всех дочерних классов
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
    FROM __obj;

  _title      TEXT;
  _pid      BIGINT;
  

  _prop_cal_period_id INTEGER;
  _act_cal_period_id  INTEGER;

  _cls_id      BIGINT;
BEGIN

  SELECT id INTO _prop_cal_period_id FROM prop WHERE title = 'Период калибровки(дни)';
  IF NOT FOUND THEN
    INSERT INTO prop(title, kind)VALUES('Период калибровки(дни)', 10)RETURNING id INTO _prop_cal_period_id;
  END IF;
  

--_act_cal_period_id : = INSERT INTO act (title) VALUES ('Изменить основные калибровки')RETURNING id;

  FOR rec IN import_cls00 LOOP
    RAISE DEBUG 'ADD ABSTRACT CLS=% TO ROOT ',rec.title;
    INSERT INTO cls(pid,title,kind) VALUES (1,rec.title,0);
  END LOOP;

  FOR rec IN import_cls01 LOOP
    SELECT title INTO _title FROM __cls00 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    RAISE DEBUG 'ADD ABSTRACT CLS=% TO % (%)',rec.title,_title,rec;
    INSERT INTO cls(pid,title,kind) VALUES (_pid,rec.title,0) ;
  END LOOP;

  FOR rec IN import_cls LOOP
    SELECT title INTO _title FROM __cls01 WHERE id = rec.pid;
    SELECT id INTO _pid FROM acls WHERE title = _title;
    RAISE DEBUG 'ADD NUMERIC CLS=% (period=%) TO % (%)',rec.title,rec.period,_title,rec;
    INSERT INTO cls(pid,title,kind,measure) VALUES (_pid,rec.title,1,'ед') RETURNING id INTO _cls_id ;
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (_cls_id , 1, _prop_cal_period_id, rec.period);

  END LOOP;


RETURN;
END; 
$BODY$ LANGUAGE plpgsql;

SELECT sgg_sc_import();
























