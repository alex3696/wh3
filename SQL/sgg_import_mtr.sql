BEGIN TRANSACTION;

--SET client_min_messages = 'error';
--SET client_min_messages = 'debug';
SET client_min_messages = 'NOTICE';
--SHOW client_min_messages=OFF;

--ALTER DOMAIN whname DROP CONSTRAINT IF EXISTS  whname_check;

--SELECT whgrant_grouptouser('TypeDesigner',  'postgres');

-------------------------------------------------------------------------------
-- импортируем корневой перечень мтр
-------------------------------------------------------------------------------

DROP TABLE IF EXISTS __mtr_group CASCADE;
CREATE TABLE __mtr_group
(
  id text NOT NULL,
  title text NOT NULL
);

--CREATE UNIQUE INDEX idxu__mtr_group_id ON __mtr_group (id);
--CREATE UNIQUE INDEX idxu__mtr_group_title ON __mtr_group (title);

-------------------------------------------------------------------------------
-- импортируем перечень мтр
-------------------------------------------------------------------------------

DROP TABLE IF EXISTS __mtr CASCADE;
CREATE TABLE __mtr
(
  mtr_id text
  ,title text
  ,description text
  ,mess text
  ,qty text
  ,price text
  ,equip_cat text
  ,dst_dep text
);

-------------------------------------------------------------------------------
-- загружаем данные из файлов
-------------------------------------------------------------------------------

DO $$
DECLARE 
  home_dir TEXT;
BEGIN
  RAISE NOTICE 'SERVER version %', version();
  
  IF(version() ILIKE '%linux%') THEN
    home_dir:='/home/alex/wh3_data/';
  ELSE
    home_dir:='c:\_SAV\tmp\';
  END IF;

  RAISE NOTICE 'DATA DIR %', home_dir;

  EXECUTE 'COPY __mtr_group FROM '''||home_dir||'__MTR_GROUP.csv'' WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';
  EXECUTE 'COPY __mtr       FROM '''||home_dir||'__MTR_SC2.csv''    WITH CSV HEADER DELIMITER '';'' ENCODING ''WIN866'' ';

END$$;  

-------------------------------------------------------------------------------
-- обрабатываем загруженые таблички
-------------------------------------------------------------------------------
DELETE FROM __mtr WHERE qty IS NULL OR qty='' ;
UPDATE __mtr  SET qty= NULLIF(REPLACE (REGEXP_REPLACE(qty,'([^[:digit:],.$])','','g'),',','.') ,'');
--SELECT * FROM __mtr WHERE qty IS NULL;
UPDATE __mtr  SET qty= 0 WHERE qty IS NULL;
ALTER TABLE __mtr ALTER COLUMN  qty TYPE NUMERIC USING qty::numeric ;
ALTER TABLE __mtr ALTER COLUMN  qty SET NOT NULL ;


UPDATE __mtr SET mess=trim(both ' ' FROM mess);
UPDATE __mtr SET mess='шт.' WHERE mess IS NULL OR mess='';
UPDATE __mtr SET mess=mess||'.' WHERE mess NOT LIKE '%.';

ALTER TABLE __mtr ADD COLUMN kind SMALLINT;

UPDATE __mtr SET kind=3 WHERE mess='л.';
UPDATE __mtr SET kind=2 WHERE mess='к-т.';
UPDATE __mtr SET kind=2 WHERE mess='шт.';
UPDATE __mtr SET kind=3 WHERE mess='м.';
UPDATE __mtr SET kind=3 WHERE mess='кг.';
UPDATE __mtr SET kind=3 WHERE mess='т.';
--SELECT * FROM __mtr WHERE mtr_id IS NULL OR mtr_id='';
--SELECT DISTINCT(mess)  FROM __mtr; 
--SELECT * FROM __mtr WHERE mess LIKE 'л%';


--ALTER TABLE __mtr DROP COLUMN id;
ALTER TABLE __mtr ADD COLUMN id SERIAL;
ALTER TABLE __mtr ADD COLUMN category_parent SMALLINT;


ALTER TABLE __mtr ALTER COLUMN  equip_cat TYPE NUMERIC USING equip_cat::numeric ;
ALTER TABLE __mtr ALTER COLUMN  equip_cat SET NOT NULL ;


-- если title одинаковый то добавится подкатегория
UPDATE __mtr SET (category_parent) =
    (
   SELECT mid FROM
   (
   SELECT (row_number() OVER (PARTITION BY m.title ORDER BY m.title)-1)  AS mid
   ,m.id
   FROM (SELECT count(title),title FROM __mtr GROUP BY title,mtr_id) d
     RIGHT JOIN __mtr m ON m.title=d.title 
     where d.count>1
   ) zxc
         WHERE zxc.id = __mtr.id );

--SELECT * FROM __mtr WHERE qty IS NULL OR qty='';

-------------------------------------------------------------------------------
-- импортируем категории мтр
-------------------------------------------------------------------------------
DO $$
DECLARE 
  import_mtg_group CURSOR IS SELECT id, title FROM __mtr_group;
  import_mtr CURSOR IS SELECT * FROM __mtr;

  import_subcat CURSOR IS SELECT *
      FROM (SELECT count(title),title,mtr_id  FROM __mtr GROUP BY title,mtr_id) d
      where d.count>1;
  
  prid_desc BIGINT;
  prid_uname BIGINT;
  prid_mtr_id BIGINT;
  prid_price BIGINT;
  prid_link BIGINT;
  prid_note BIGINT;
  prid_eqip_cat BIGINT;
  prid_dst_dep BIGINT;

  cid_sta BIGINT;
  cid_previos BIGINT;
  sta_category1 TEXT;
  sta_category2 TEXT;
  sta_category3 TEXT;

  cid_zakaz BIGINT;
  oid_zakaz_mtr BIGINT;

  cid_curr BIGINT;
  cid_parent BIGINT;
  sta_title TEXT;
  sta_whtitle WHNAME;
  sta_title_idx INTEGER;

  oid_company_sgg BIGINT;

  aid_change_note BIGINT; 
BEGIN
  RAISE NOTICE 'Start impotr MTR script';

  RAISE NOTICE 'Clear all MTR';
  --SELECT id FROM acls WHERE title = 'ЗИП';
  SELECT id INTO cid_sta FROM acls WHERE title = 'ЗИП';
  IF FOUND THEN 
    DELETE from acls WHERE id IN (SELECT _id FROM get_childs_cls(cid_sta)WHERE _id<>cid_sta);
  END IF;
  RAISE DEBUG 'cid_sta %',cid_sta;

  RAISE NOTICE 'Build main MTR tree';
  FOR rec IN import_mtg_group LOOP
    sta_category1:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '#"%#".%.%' for '#');
    sta_category2:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '%.#"%#".%' for '#');
    sta_category3:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '%.%.#"%#"' for '#');

    IF (0 < sta_category3::NUMERIC ) THEN
      SELECT id INTO cid_previos FROM acls WHERE title LIKE sta_category1||'.'||sta_category2||'.%';
    ELSEIF (0 < sta_category2::NUMERIC ) THEN
      SELECT id INTO cid_previos FROM acls WHERE title LIKE sta_category1||'.%';
    ELSE 
      cid_previos:=cid_sta;
    END IF;

    --RAISE DEBUG 'rec.id % -->%.%.%  cid_previos %',rec.id,sta_category1,sta_category2,sta_category3,cid_previos;
    
    INSERT INTO acls(pid,title,kind,dobj) VALUES (cid_previos,rec.id||' '||rec.title,0,NULL);
  END LOOP;

  RAISE NOTICE 'Load action';
  SELECT id INTO aid_change_note FROM act WHERE title='Изменить примечание';

  RAISE NOTICE 'Build custom MTR tree';
  FOR rec IN import_subcat LOOP
    SELECT id INTO cid_previos FROM acls WHERE title LIKE rec.mtr_id||'%';
    
    INSERT INTO acls(pid,title,kind,dobj) VALUES (cid_previos,rec.title,0,NULL);

  END LOOP;

  RAISE NOTICE 'Add order';
  SELECT id INTO cid_zakaz       FROM acls WHERE title='Заказ';
  SELECT id INTO oid_zakaz_mtr   FROM obj  WHERE title='Заказ МТР';
  --SELECT id INTO oid_company_sgg FROM obj  WHERE title='Севергазгеофизика';
  
  RAISE NOTICE 'Load id properties ';

  SELECT id INTO prid_desc FROM prop WHERE title = 'Описание';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Описание', 0) RETURNING id INTO prid_desc ; END IF;
  
  SELECT id INTO prid_uname FROM prop WHERE title = 'Обозначение';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Обозначение', 0) RETURNING id INTO prid_uname ; END IF;
  
  SELECT id INTO prid_mtr_id FROM prop WHERE title = 'Код УНГ МТР';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Код УНГ МТР', 0) RETURNING id INTO prid_mtr_id ; END IF;
  
  SELECT id INTO prid_price FROM prop WHERE title = 'Цена(руб.)';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Цена(руб.)', 101) RETURNING id INTO prid_price ; END IF;
  
  SELECT id INTO prid_link FROM prop WHERE title = 'Ссылка на поставщика';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Ссылка на поставщика', 0) RETURNING id INTO prid_link ; END IF;

  SELECT id INTO prid_note FROM prop WHERE title = 'Примечание';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Примечание', 0) RETURNING id INTO prid_note ; END IF;

  SELECT id INTO prid_eqip_cat FROM prop WHERE title = 'Категория ЗИП';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Категория ЗИП', 0) RETURNING id INTO prid_eqip_cat ; END IF;

  SELECT id INTO prid_dst_dep FROM prop WHERE title = 'Заказчик ЗИП';
  IF NOT FOUND THEN INSERT INTO prop(title, kind)VALUES('Заказчик ЗИП', 0) RETURNING id INTO prid_dst_dep ; END IF;


  RAISE DEBUG 'prid_desc %',prid_desc;
  RAISE DEBUG 'prid_note %',prid_note;
  RAISE DEBUG 'prid_uname %',prid_uname;
  RAISE DEBUG 'prid_price %',prid_price;
  RAISE DEBUG 'prid_link %',prid_link;


  RAISE NOTICE 'Import classes';
  FOR rec IN import_mtr LOOP
    IF(rec.category_parent IS NOT NULL) THEN
      SELECT id INTO cid_parent FROM acls WHERE kind=0 AND title=rec.title;
    ELSEIF (rec.mtr_id IS NOT NULL AND rec.mtr_id<>'') THEN
      SELECT id INTO cid_parent FROM acls WHERE kind=0 AND title LIKE rec.mtr_id||'%';
    ELSE
      cid_parent:=cid_sta;
    END IF;

    IF(rec.category_parent IS NOT NULL) THEN
      sta_title:=rec.description;
    ELSE
      sta_title:=COALESCE(rec.title,'')||' '||COALESCE(rec.description,'');
    END IF; 
    
    BEGIN
      RAISE DEBUG 'sta_title %',sta_title::WHNAME; --SELECT sta_title::WHNAME;
      sta_whtitle:=sta_title;
    EXCEPTION
    WHEN SQLSTATE '23514' THEN --check_violation
        RAISE DEBUG 'sta_title = %',sta_title;
    END;

    sta_title_idx:=0;
    LOOP
      PERFORM FROM acls WHERE title=sta_title;
      IF FOUND THEN
       sta_title:= sta_title||'_'||sta_title_idx;
       sta_title_idx:=sta_title_idx+1;
      ELSE 
        EXIT;
      END IF;
    END LOOP;

    SELECT id INTO cid_curr FROM acls WHERE title=sta_title;
    IF NOT FOUND THEN
      INSERT INTO acls(pid,title,kind,measure,dobj) VALUES (cid_parent,sta_title,rec.kind,rec.mess,oid_zakaz_mtr) 
        RETURNING id INTO cid_curr;
      INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_mtr_id, rec.mtr_id);
      INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_desc, COALESCE(rec.title,'')||' '||COALESCE(rec.description,''));
      INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_uname, NULL);

      RAISE DEBUG '  equip_cat=%  %',rec.equip_cat,CASE rec.equip_cat WHEN 0 THEN 'расход.' WHEN 1 THEN 'ЗИП' ELSE 'ОС до 40' END;

      INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_eqip_cat, CASE rec.equip_cat WHEN 0 THEN 'расход.' WHEN 1 THEN 'ЗИП' ELSE 'ОС до 40' END);
      INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_dst_dep, rec.dst_dep);

      INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
        VALUES ('Кладовщик', 0, cid_curr, NULL, aid_change_note );

    END IF;

    PERFORM FROM obj WHERE cls_id=cid_curr AND title='заявка 2016' AND pid=oid_zakaz_mtr;
    IF NOT FOUND THEN
      INSERT INTO obj(title,cls_id,pid,qty) VALUES ('заявка 2016', cid_curr, oid_zakaz_mtr,rec.qty );
    END IF;

    
  END LOOP;
  
  RAISE NOTICE 'End impotr MTR script';
END$$;


DROP TABLE IF EXISTS __mtr_group CASCADE;
DROP TABLE IF EXISTS __mtr CASCADE;

COMMIT TRANSACTION;
--VACUUM;
--ANALYZE;
