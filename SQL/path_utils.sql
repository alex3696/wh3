
SET client_min_messages='debug1';


DROP FUNCTION IF EXISTS get_childs_cls(IN _cid BIGINT) CASCADE;

DROP FUNCTION IF EXISTS get_path_cls_info(IN _cid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls_arr_id(IN _cid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls_arr_title(IN _cid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_cls(IN _cid BIGINT) CASCADE;


DROP FUNCTION IF EXISTS get_path_obj_info(IN _oid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_id(IN _oid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_2id(IN _oid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj_arr_2title(IN _oid BIGINT) CASCADE;
DROP FUNCTION IF EXISTS get_path_obj(IN _oid BIGINT) CASCADE;

DROP FUNCTION IF EXISTS fn_array1_to_table(anyarray);
DROP FUNCTION IF EXISTS obj_arr_id_to_obj_info(IN anyarray);

DROP FUNCTION IF EXISTS fn_array2_to_table(IN anyarray);
DROP FUNCTION IF EXISTS tmparr_to_2id_info(IN TEXT);
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
              NOT cycle AND
              array_length(exist, 1) < 1000
)
SELECT id,  title, kind, pid,  note, measure,dobj
    FROM children WHERE NOT cycle --ORDER BY ord LIMIT 100;
    );
END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 1000 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_childs_cls(BIGINT) TO "Guest";

SELECT * FROM get_childs_cls(105);



-------------------------------------------------------------------------------
-- функция получения информации о наследовании класса
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls_info(IN _cid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_info(IN _cid BIGINT)
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
        _cid IS NOT NULL AND c.id = _cid AND _cid>1
       UNION ALL
       SELECT n.id, n.pid, NULL::WHNAME , n.kind, n.title, n.note, NULL
            , p.arr_id     || ARRAY[n.id]::BIGINT[]
            , p.arr_title  || ARRAY[n.title]::NAME[]
            , '/'||n.title|| p.path
            , n.id = any (p.arr_id) AS CYCLE
         FROM parents AS p 
         LEFT JOIN acls n ON p.pid=n.id
         WHERE NOT p.cycle 
               AND n.id > 1
     )
     SELECT  * FROM parents WHERE NOT parents.CYCLE;

END; 
$BODY$ LANGUAGE plpgsql STABLE  COST 100 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_path_cls_info(BIGINT) TO "Guest";
SELECT * FROM get_path_cls_info(150);
SELECT * FROM get_path_cls_info(105);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls_arr_id(IN _cid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_arr_id(_cid bigint)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_id FROM get_path_cls_info($1) WHERE pid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls_arr_id(150);
SELECT * FROM get_path_cls_arr_id(2167);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls_arr_title(IN _cid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls_arr_title(_cid bigint)
  RETURNS NAME[] AS
$BODY$ 
    SELECT arr_title FROM get_path_cls_info($1) WHERE pid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls_arr_title(150);
SELECT * FROM get_path_cls_arr_title(2167);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_cls(IN _cid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_cls(_cid bigint)
  RETURNS TEXT AS
$BODY$ 
    SELECT path FROM get_path_cls_info($1) WHERE pid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_cls(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_cls(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_cls(150);
SELECT * FROM get_path_cls(2167);







-------------------------------------------------------------------------------
-- функция получения информации о пути объектов, по идентификатору объекта
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_info(IN _oid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_info(IN _oid BIGINT)
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
          WHERE o.id = _oid
          AND o.id>1
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
        WHERE o.id>1 AND NOT p.CYCLE
        )
   SELECT * FROM parents WHERE NOT parents.CYCLE;

END; 
$BODY$ LANGUAGE plpgsql VOLATILE  COST 1000 ROWS 1000;
GRANT EXECUTE ON FUNCTION get_path_obj_info(BIGINT) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_info(BIGINT) TO "Admin" WITH GRANT OPTION;

SELECT * FROM get_path_obj_info(105);


-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_id(IN _oid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_id(_oid bigint)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_id FROM get_path_obj_info($1) WHERE opid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_id(550);
SELECT * FROM get_path_obj_arr_id(2167);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_2id(IN _oid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_2id(_oid bigint)
  RETURNS BIGINT[] AS
$BODY$ 
    SELECT arr_2id FROM get_path_obj_info($1) WHERE opid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_2id(150);
SELECT * FROM get_path_obj_arr_2id(2167);
-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj_arr_2title(IN _oid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj_arr_2title(_oid bigint)
  RETURNS NAME[] AS
$BODY$ 
    SELECT arr_2title FROM get_path_obj_info($1) WHERE opid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj_arr_2title(104);
SELECT * FROM get_path_obj_arr_2title(2167);

-------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS get_path_obj(IN _oid BIGINT) CASCADE;
CREATE OR REPLACE FUNCTION get_path_obj(_oid bigint)
  RETURNS TEXT AS
$BODY$ 
    SELECT path FROM get_path_obj_info($1) WHERE opid=1
$BODY$
  LANGUAGE sql STABLE COST 100;
GRANT EXECUTE ON FUNCTION get_path_obj(bigint) TO "Guest";
GRANT EXECUTE ON FUNCTION get_path_obj(bigint) TO "Admin" WITH GRANT OPTION;
SELECT * FROM get_path_obj(100);
SELECT * FROM get_path_obj(2167);



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
--SELECT * FROM fn_array2_to_table('{{101,1011},{102,1022},{103,1033},{104,1044}}'::int[]);
--SELECT * FROM fn_array2_to_table('{{%,%},{%,1022},{103,%},{104,1044}}'::NAME[]);



-----------------------------------------------------------------------------------------------------------------------------    
-- функция преобразования шаблона пути массива 
-----------------------------------------------------------------------------------------------------------------------------
DROP FUNCTION IF EXISTS tmppath_to_2id_info(IN TEXT);
CREATE OR REPLACE FUNCTION tmppath_to_2id_info(IN tpath TEXT)
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
        ORDER BY arr.ord;

BEGIN
    path:='';
    FOR rec IN _unpackpath LOOP
        arr_2title := arr_2title || ARRAY[ ARRAY[rec.cls_title,rec.obj_title] ];
        arr_2id := arr_2id || ARRAY[ ARRAY[rec.cls_id,rec.obj_id]::BIGINT[] ]::BIGINT[];
        path:=  '/['||COALESCE(rec.cls_title,'')||']'||COALESCE(rec.obj_title,'') || path;
    END LOOP;
    RETURN next;
    RETURN;
END;
$BODY$
  LANGUAGE plpgsql STABLE ;


SELECT * FROM tmppath_to_2id_info('{%,{%,%},%%{%,106},{108,%}{111,122}}%');








SELECT * FROM tmppath_to_2id_info('{%,%}');




GRANT EXECUTE ON FUNCTION get_childs_cls(IN _cid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION get_path_cls_info(IN _cid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_id(IN _cid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls_arr_title(IN _cid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_cls(IN _cid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION get_path_obj_info(IN _oid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_id(IN _oid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2id(IN _oid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj_arr_2title(IN _oid BIGINT) TO "User";
GRANT EXECUTE ON FUNCTION get_path_obj(IN _oid BIGINT) TO "User";

GRANT EXECUTE ON FUNCTION fn_array1_to_table(IN anyarray) TO "Guest";
GRANT EXECUTE ON FUNCTION obj_arr_id_to_obj_info(IN anyarray) TO "User";
GRANT EXECUTE ON FUNCTION fn_array2_to_table(IN anyarray) TO "Guest";
GRANT EXECUTE ON FUNCTION tmppath_to_2id_info(IN TEXT) TO "Guest";


