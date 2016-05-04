--SET client_min_messages = 'error';
SET client_min_messages = 'debug';

ALTER DOMAIN whname
    DROP CONSTRAINT IF EXISTS  whname_check;
--SHOW client_min_messages=OFF;
-------------------------------------------------------------------------------
--PRINT '';
--PRINT '- удаляем все записи классы/свойства';
--PRINT '';
-------------------------------------------------------------------------------
--SELECT whgrant_grouptouser('TypeDesigner',  'postgres');
-------------------------------------------------------------------------------
DROP TABLE IF EXISTS __mtr_group CASCADE;
CREATE TABLE __mtr_group
(
  id text NOT NULL,
  title text NOT NULL
);
COPY __mtr_group FROM 'c:\_SAV\tmp\__MTR_GROUP.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
--CREATE UNIQUE INDEX idxu__mtr_group_id ON __mtr_group (id);
--CREATE UNIQUE INDEX idxu__mtr_group_title ON __mtr_group (title);

-------------------------------------------------------------------------------
-- импортируем перечень мтр
-------------------------------------------------------------------------------

DROP TABLE IF EXISTS __mtr CASCADE;
CREATE TABLE __mtr
(
  id text
  ,mtr_id text
  ,title text
  ,description text
  ,mess text
  ,qty text
  ,price text
  ,summ text
  ,note text
);
COPY __mtr FROM 'c:\_SAV\tmp\__MTR_SC.csv'  WITH CSV HEADER DELIMITER ';' ENCODING 'WIN866' ;
DELETE FROM __mtr WHERE qty IS NULL OR qty='';
UPDATE __mtr SET mess=trim(both ' ' FROM mess);
UPDATE __mtr SET mess='шт.' WHERE mess IS NULL OR mess='';
UPDATE __mtr SET mess=mess||'.' WHERE mess NOT LIKE '%.';

UPDATE __mtr  SET qty=   NULLIF(REPLACE (REGEXP_REPLACE(qty,'([^[:digit:],.$])','','g'),',','.') ,'');
ALTER TABLE __mtr ALTER COLUMN  qty TYPE NUMERIC USING  qty::numeric;

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


ALTER TABLE __mtr DROP COLUMN id;
ALTER TABLE __mtr ADD COLUMN id SERIAL;
ALTER TABLE __mtr ADD COLUMN category_parent SMALLINT;




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

  cid_sta BIGINT;
  cid_previos BIGINT;
  sta_category1 TEXT;
  sta_category2 TEXT;
  sta_category3 TEXT;

  cid_sta_zayavka BIGINT;
  oid_zayavka2016 BIGINT;

  cid_curr BIGINT;
  cid_parent BIGINT;
  sta_title TEXT;
  sta_whtitle WHNAME;
  sta_title_idx INTEGER;
BEGIN
  --SELECT id FROM cls WHERE title = 'ЗИП';
  SELECT id INTO cid_sta FROM cls WHERE title = 'ЗИП';
  IF FOUND THEN 
    DELETE from acls WHERE id IN (SELECT _id FROM get_childs_cls(cid_sta));
    INSERT INTO cls(pid,title,kind) VALUES (1,'ЗИП',0) RETURNING id INTO cid_sta ; 
  END IF;
  RAISE DEBUG 'cid_sta %',cid_sta;


  FOR rec IN import_mtg_group LOOP
    sta_category1:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '#"%#".%.%' for '#');
    sta_category2:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '%.#"%#".%' for '#');
    sta_category3:=substring (REGEXP_REPLACE(rec.id,'([^[:digit:],.$])','','g') from '%.%.#"%#"' for '#');

    IF (0 < sta_category3::NUMERIC ) THEN
      SELECT id INTO cid_previos FROM cls WHERE title LIKE sta_category1||'.'||sta_category2||'.%';
    ELSEIF (0 < sta_category2::NUMERIC ) THEN
      SELECT id INTO cid_previos FROM cls WHERE title LIKE sta_category1||'.%';
    ELSE 
      cid_previos:=cid_sta;
    END IF;

    --RAISE DEBUG 'rec.id % -->%.%.%  cid_previos %',rec.id,sta_category1,sta_category2,sta_category3,cid_previos;
    
    INSERT INTO cls(pid,title,kind) VALUES (cid_previos,rec.id||' '||rec.title,0);
  END LOOP;

  FOR rec IN import_subcat LOOP
    SELECT id INTO cid_previos FROM cls WHERE title LIKE rec.mtr_id||'%';
    
    INSERT INTO cls(pid,title,kind) VALUES (cid_previos,rec.title,0);

  END LOOP;

  INSERT INTO cls(pid,title,kind,measure) VALUES (cid_sta,'Заказ',1,'шт.') RETURNING id INTO cid_sta_zayavka ; 
  INSERT INTO obj(title,cls_id,pid) VALUES ('заявка СЦ',cid_sta_zayavka, 1 ) RETURNING id INTO oid_zayavka2016;

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

  RAISE DEBUG 'prid_desc %',prid_desc;
  RAISE DEBUG 'prid_note %',prid_note;
  RAISE DEBUG 'prid_uname %',prid_uname;
  RAISE DEBUG 'prid_price %',prid_price;
  RAISE DEBUG 'prid_link %',prid_link;

  
  FOR rec IN import_mtr LOOP
    IF(rec.category_parent IS NOT NULL) THEN
      SELECT id INTO cid_parent FROM cls WHERE kind=0 AND title=rec.title;
    ELSEIF (rec.mtr_id IS NOT NULL AND rec.mtr_id<>'') THEN
      SELECT id INTO cid_parent FROM cls WHERE kind=0 AND title LIKE rec.mtr_id||'%';
    ELSE
      cid_parent:=cid_sta;
    END IF;

  
    IF(rec.category_parent IS NOT NULL) THEN
      sta_title:=rec.description;
    ELSE
      sta_title:=COALESCE(rec.title,'')||' '||COALESCE(rec.description,'');
    END IF; 
    --RAISE DEBUG 'sta_title %',sta_title;
    BEGIN
      --RAISE DEBUG 'sta_title %',sta_title::WHNAME; --SELECT sta_title::WHNAME;
      sta_whtitle:=sta_title;
    EXCEPTION
    WHEN SQLSTATE '23514' THEN --check_violation
        RAISE DEBUG 'sta_title = %',sta_title;
    END;

    sta_title_idx:=0;
    LOOP
      PERFORM FROM cls WHERE title=sta_title;
      IF FOUND THEN
       sta_title:= sta_title||'_'||sta_title_idx;
       sta_title_idx:=sta_title_idx+1;
      ELSE 
        EXIT;
      END IF;
    END LOOP;

    
    INSERT INTO cls(pid,title,kind,measure,dobj) VALUES (cid_parent,sta_title,rec.kind,rec.mess,oid_zayavka2016) RETURNING id INTO cid_curr ;
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_mtr_id, rec.mtr_id);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_desc, NULL);
    INSERT INTO prop_cls(cls_id, cls_kind, prop_id, val) VALUES (cid_curr , rec.kind, prid_uname, NULL);

    INSERT INTO obj(title,cls_id,pid,qty) VALUES ('заявка 2016', cid_curr, oid_zayavka2016,rec.qty );
    
  END LOOP;
  
  
END$$;









--DROP TABLE IF EXISTS __mtr_group CASCADE;