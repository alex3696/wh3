
SET client_min_messages='debug1';


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
-------------------------------------------------------------------------------
-- поиск всех дочерних классов
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_childs_cls(IN _obj_id BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_childs_cls(IN _obj_id BIGINT)
 RETURNS TABLE(
  _id    BIGINT
  ,_title WHNAME
  ,_kind  SMALLINT
  ,_pid   BIGINT
  ,_note  TEXT
  ,_measure WHNAME
  ,_dobj  BIGINT
) AS $BODY$ 
BEGIN
RETURN QUERY(
    WITH RECURSIVE children AS (
    SELECT id,  title, kind, pid,  note, measure,dobj
           ,ARRAY[id]                            AS exist
           ,FALSE                                AS cycle
    FROM cls
    WHERE id = _obj_id
    UNION ALL
        SELECT t.id, t.title, t.kind, t.pid, t.note, t.measure, t.dobj
               ,exist || t.id 
               ,t.id = ANY(exist)
        FROM children AS c, 
             cls  AS t
        WHERE t.pid = c.id AND 
              NOT cycle 
              --AND array_length(exist, 1) < 1000 -- глубина дерева
)
SELECT id,  title, kind, pid,  note, measure,dobj
    FROM children WHERE NOT cycle --ORDER BY ord LIMIT 100;
    );
END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_childs_cls(BIGINT) TO "Guest";

SELECT * FROM get_childs_cls(101);
SELECT * FROM get_childs_cls(0);



-------------------------------------------------------------------------------
-- функция получения информации о наследовании класса
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS    get_path_cls_info(_cid BIGINT,_cpid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_info(_cid BIGINT,_cpid BIGINT DEFAULT 0)
 RETURNS TABLE( 
     id BIGINT 
   , pid BIGINT
   , measure WHNAME
   , kind SMALLINT
   , title WHNAME
   , note TEXT
   , dobj BIGINT
   , arr_id     BIGINT[]
   , arr_title  NAME[]
   , path       TEXT
   , CYCLE      BOOLEAN
   ) AS $BODY$ 
BEGIN
RETURN QUERY 
  WITH RECURSIVE parents AS 
    (SELECT c.id, c.pid, c.measure, c.kind, c.title, c.note, c.dobj
          , ARRAY[c.id] AS arr_id
          , ARRAY[c.title]::NAME[] AS arr_title
          , '/'||c.title AS path
          ,FALSE AS cycle
       FROM cls c
       WHERE --c.id=333 
        _cid IS NOT NULL AND _cid>0 AND c.id = _cid 
       UNION ALL
       SELECT n.id, n.pid, NULL::WHNAME , n.kind, n.title, n.note, NULL
            , p.arr_id     || ARRAY[n.id]::BIGINT[]
            , p.arr_title  || ARRAY[n.title]::NAME[]
            , '/'||n.title|| p.path
            , n.id = any (p.arr_id) AS CYCLE
         FROM parents AS p 
         LEFT JOIN acls n ON p.pid=n.id
         WHERE NOT p.cycle 
               AND n.id <> _cpid --AND n.id > 0
     )
     SELECT  * FROM parents WHERE NOT parents.CYCLE;

END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_path_cls_info(BIGINT,BIGINT) TO "Guest";
SELECT * FROM get_path_cls_info(150);
SELECT * FROM get_path_cls_info(150,0);
SELECT * FROM get_path_cls_info(150,1);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls_arr_id(BIGINT,BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_arr_id(_cid BIGINT, _cpid BIGINT DEFAULT 0)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_id FROM get_path_cls_info($1,$2) WHERE pid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls_arr_id(150);
SELECT * FROM get_path_cls_arr_id(150,0);
SELECT * FROM get_path_cls_arr_id(150,1);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls_arr_title(BIGINT,BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_arr_title(_cid BIGINT, _cpid BIGINT DEFAULT 0)
  RETURNS NAME[] AS
$BODY$ 
    SELECT arr_title FROM get_path_cls_info($1,$2) WHERE pid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls_arr_title(150);
SELECT * FROM get_path_cls_arr_title(150,1);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls(BIGINT,BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls(_cid BIGINT, _cpid BIGINT DEFAULT 0)
  RETURNS TEXT AS
$BODY$ 
    SELECT path FROM get_path_cls_info($1,$2) WHERE pid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls(150);
SELECT * FROM get_path_cls(150,1);







-------------------------------------------------------------------------------
-- функция получения информации о пути объектов, по идентификатору объекта
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_info(_oid BIGINT,_opid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_info(_oid BIGINT,_opid BIGINT DEFAULT 0)
 RETURNS TABLE(
     oid        BIGINT
    ,opid       BIGINT
    ,otitle     WHNAME
    ,cid        BIGINT
    ,ctitle     WHNAME

    ,arr_id     BIGINT[]
    ,arr_2id    BIGINT[]
    ,arr_2title NAME[]
    ,path       TEXT
    ,CYCLE      BOOLEAN
    ) AS $BODY$ 
BEGIN
RETURN QUERY 
    WITH RECURSIVE parents AS 
    (SELECT o.id, o.pid 
          , o.title
          , c.id, c.title
          , ARRAY[o.id]::BIGINT[] AS arr_id
          , ARRAY[ ARRAY[c.id,o.id]::BIGINT[] ]::BIGINT[] AS arr_2id
          , ARRAY[ ARRAY[c.title,o.title]::NAME[] ]::NAME[] AS arr_2title
          , '/['||c.title||']'||o.title AS path
          , FALSE AS CYCLE
        FROM obj AS o
        LEFT JOIN cls c ON c.id=o.cls_id
          WHERE _oid IS NOT NULL AND o.id = _oid AND o.id>0
     UNION ALL
     SELECT o.id, o.pid 
          , onm.title
          , c.id, c.title
          , p.arr_id     || ARRAY[o.id]::BIGINT[]
          , p.arr_2id    || ARRAY[c.id,o.id]::BIGINT[]
          , p.arr_2title || ARRAY[c.title,onm.title]::NAME[]
          , '/['||c.title||']'||onm.title|| p.path
          , o.id = any (p.arr_id) AS CYCLE
        FROM 
        parents AS p
        LEFT JOIN obj_num AS o ON o.id = p.pid
        LEFT JOIN obj_name AS onm ON onm.id = o.id
        LEFT JOIN acls c ON onm.cls_id=c.id AND c.kind BETWEEN 1 AND 3
        WHERE NOT p.CYCLE
              AND o.id <> _opid --AND o.id>0
        )
   SELECT * FROM parents WHERE NOT parents.CYCLE;

END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 1000 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_path_obj_info(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_info(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;

SELECT * FROM get_path_obj_info(105);
SELECT * FROM get_path_obj_info(105,0);
SELECT * FROM get_path_obj_info(105,1);


-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_id(BIGINT, BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_id(_oid bigint, _opid BIGINT DEFAULT 0)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_id FROM get_path_obj_info($1,$2) WHERE opid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_id(105);
SELECT * FROM get_path_obj_arr_id(105,1);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_2id(BIGINT, BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_2id(_oid bigint, _opid BIGINT DEFAULT 0)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_2id FROM get_path_obj_info($1,$2) WHERE opid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_2id(105);
SELECT * FROM get_path_obj_arr_2id(105,1);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_2title(BIGINT, BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_2title(_oid bigint, _opid BIGINT DEFAULT 0)
  RETURNS NAME[] AS
$BODY$ 
    SELECT arr_2title FROM get_path_obj_info($1,$2) WHERE opid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_2title(105);
SELECT * FROM get_path_obj_arr_2title(105,1);

-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj(BIGINT, BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj(_oid bigint, _opid BIGINT DEFAULT 0)
  RETURNS TEXT AS
$BODY$ 
    SELECT path FROM get_path_obj_info($1,$2) WHERE opid=$2;
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj(BIGINT,BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj(BIGINT,BIGINT) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj(105);
SELECT * FROM get_path_obj(105,1);



-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования одномерного массива идентификаторов класса в таблицу
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_array1_to_table(IN anyarray);
CREATE OR REPLACE FUNCTION fn_array1_to_table(IN anyarray)
  RETURNS TABLE(idx integer, id anyelement) 
  AS $BODY$ 
  SELECT row, $1[row] from generate_subscripts($1, 1) as row
$BODY$ LANGUAGE sql IMMUTABLE;
SELECT * FROM fn_array1_to_table('{101,102,103,104}'::int[]);


-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования одномерного массива объектоа в таблицу-путь
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS obj_arr_id_to_obj_info(IN anyarray);
CREATE OR REPLACE FUNCTION obj_arr_id_to_obj_info(IN idpath anyarray)
  RETURNS TABLE(
    arr_2title NAME[]
    ,path       TEXT
    ) AS $BODY$
DECLARE
    --rec      RECORD;
    --path     TEXT;
    --result   NAME [];
    get_obj_info CURSOR IS 
    SELECT cls.id AS cls_id
        , cls.title::NAME AS cls_title
        , obj.id AS obj_id
        , obj.title::NAME AS obj_title
        ,  arr.id
        FROM fn_array1_to_table(idpath) arr
        LEFT JOIN obj_name obj ON obj.id=arr.id
        LEFT JOIN acls cls ON cls.id=obj.cls_id 
        ORDER BY idx;
BEGIN
    path:='';
    FOR rec IN get_obj_info LOOP
        arr_2title := arr_2title || ARRAY[ ARRAY[rec.cls_title,rec.obj_title] ];
        path:=  '/['||COALESCE(rec.cls_title,'')||']'||COALESCE(rec.obj_title,'') || path;
    END LOOP;
    RETURN next;
    RETURN;
END;
$BODY$
  LANGUAGE plpgsql STABLE ;

SELECT * FROM obj_arr_id_to_obj_info('{101,102,103,104}'::BIGINT[]);



-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования одномерного массива идентификаторов класса в таблицу
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS fn_array2_to_table(IN anyarray);
CREATE OR REPLACE FUNCTION fn_array2_to_table(IN anyarray)
  RETURNS TABLE(idx integer, id1 anyelement, id2 anyelement) 
  AS $BODY$ 
  SELECT row, $1[row][1],$1[row][2] from generate_subscripts($1, 1) as row
$BODY$ LANGUAGE sql IMMUTABLE;

SELECT * FROM fn_array2_to_table('{{101,1011},{102,1022},{103,1033},{104,1044}}'::int[]);
SELECT * FROM fn_array2_to_table('{{%,%},{%,1022},{103,%},{104,1044}}'::NAME[]);



-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования шаблона пути массива 
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS tmppath_to_2id_info(IN TEXT,BIGINT );
CREATE OR REPLACE FUNCTION tmppath_to_2id_info(IN tpath TEXT,IN offset_qty BIGINT DEFAULT 0)
  RETURNS TABLE(
     arr_2title NAME[]
    ,arr_2id    BIGINT[]
    ,path       TEXT
    
    ) AS $BODY$
DECLARE
    _unpackpath CURSOR IS 
    SELECT cls.id AS cls_id
        , cls.title::NAME AS cls_title
        , obj.id AS obj_id
        , obj.title::NAME AS obj_title
        --,  arr.id1 ,  arr.id2
        FROM (
                SELECT ORDINALITY as ord,res[1] as id1,res[2] as id2
                FROM 
                     regexp_matches( substring($1 from '{(.+)}' ),  
                     '%+|{(%|[[:digit:]]+),(%|[[:digit:]]+)}','g') WITH ORDINALITY res

             ) arr
        LEFT JOIN obj_name obj ON obj.id= CASE WHEN arr.id2='%' THEN NULL ELSE arr.id2::BIGINT END
        LEFT JOIN acls cls ON cls.id= CASE 
                                            WHEN arr.id1 IS NULL OR arr.id1='%'
                                            THEN obj.cls_id
                                            ELSE arr.id1::BIGINT END
        ORDER BY arr.ord DESC OFFSET offset_qty;

BEGIN
    path:='';
    FOR rec IN _unpackpath LOOP
        arr_2title := ARRAY[ ARRAY[rec.cls_title,rec.obj_title] ] || arr_2title;
        arr_2id :=  ARRAY[ ARRAY[rec.cls_id,rec.obj_id]::BIGINT[] ]::BIGINT[] || arr_2id;
        path:=  path || '/['||COALESCE(rec.cls_title,'')||']'||COALESCE(rec.obj_title,'');
    END LOOP;
    RETURN next;
    RETURN;
END;
$BODY$
  LANGUAGE plpgsql STABLE ;


SELECT * FROM tmppath_to_2id_info('{%,{%,%},%%{%,106},{108,%}{111,122}}%');
SELECT * FROM tmppath_to_2id_info('{%%,%%%%}');




GRANT EXECUTE ON FUNCTION get_childs_cls(IN _cid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION get_path_cls_info(_cid BIGINT,_cpid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(_cid BIGINT,_cpid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(_cid BIGINT,_cpid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls(_cid BIGINT,_cpid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION get_path_obj_info(_oid BIGINT,_opid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(_oid BIGINT,_opid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(_oid BIGINT,_opid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(_oid BIGINT,_opid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj(_oid BIGINT,_opid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION fn_array1_to_table(IN anyarray) TO "Guest";
GRANT EXECUTE ON FUNCTION obj_arr_id_to_obj_info(IN anyarray) TO "User";
GRANT EXECUTE ON FUNCTION fn_array2_to_table(IN anyarray) TO "Guest";
GRANT EXECUTE ON FUNCTION tmppath_to_2id_info(IN TEXT,BIGINT) TO "Guest";


---------------------------------------------------------------------------------------------------
-- тригер для таблицы классов, запрещающий зацикливание
---------------------------------------------------------------------------------------------------
-- Узел не может подчиняться потомку == потомок узла не может быть его родителем ==
-- перемещение ветки OLD.pid -> NEW.pid
-- 0.1.2.3.4.5 перемещаем 4.5 в ветку 0.1 -> OLD.pid=3 -> NEW.pid=1
-- проверяем чтоб в родителях новой ветки отсутствовал id текущей ветки
DROP FUNCTION IF EXISTS ftr_bu_acls() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bu_acls()  RETURNS trigger AS
$body$
DECLARE
BEGIN
-- Если произошло изменение родителя узла
  IF NEW.pid IS NOT NULL AND (NEW.pid <> OLD.pid OR OLD.pid IS NULL) THEN
  -- Пытаемся найти в родителькой ветки нового родителя текущий узел
    PERFORM FROM get_path_cls_info(NEW.pid) WHERE id = OLD.id;
    IF FOUND THEN
      RAISE EXCEPTION ' %: can`t change pid - cycling error',TG_NAME;
    END IF;
  END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bu_acls BEFORE UPDATE ON acls FOR EACH ROW EXECUTE PROCEDURE ftr_bu_acls();
GRANT EXECUTE ON FUNCTION ftr_bu_acls() TO "TypeDesigner";
---------------------------------------------------------------------------------------------------
-- тригер для объектов, запрещающий зацикливание
---------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS ftr_bu_any_obj() CASCADE;
CREATE OR REPLACE FUNCTION ftr_bu_any_obj()  RETURNS trigger AS
$body$
DECLARE
BEGIN
-- Если произошло изменение родителя узла
  IF NEW.pid IS NOT NULL AND (NEW.pid <> OLD.pid OR OLD.pid IS NULL) THEN
  -- Пытаемся найти в родителькой ветки нового родителя текущий узел
    PERFORM FROM get_path_obj_info(NEW.pid) WHERE oid = OLD.id;
    IF FOUND THEN
      RAISE EXCEPTION ' %: can`t change pid - cycling error',TG_NAME;
    END IF;
  END IF;
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';

CREATE TRIGGER tr_bu_obj_num BEFORE UPDATE ON obj_num FOR EACH ROW EXECUTE PROCEDURE ftr_bu_any_obj();
CREATE TRIGGER tr_bu_obj_num BEFORE UPDATE ON obj_qtyi FOR EACH ROW EXECUTE PROCEDURE ftr_bu_any_obj();
CREATE TRIGGER tr_bu_obj_num BEFORE UPDATE ON obj_qtyf FOR EACH ROW EXECUTE PROCEDURE ftr_bu_any_obj();
GRANT EXECUTE ON FUNCTION ftr_bu_any_obj() TO "User";