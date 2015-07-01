SELECT version();

SELECT 
--DISTINCT ON(dst_tid)
dst_tid,dst_tid_label
FROM move_destiation 
WHERE src_oid=3

ANALYZE VERBOSE typetree;
ANALYZE VERBOSE objtree;
ANALYZE VERBOSE moverules;


select geqo()

/*
DROP TYPE whobject;

CREATE TYPE whobject AS (
    fname	NAME,	
    ftype       NAME
);

*/




WITH RECURSIVE parents AS (
    SELECT 	0 AS ord,
		*, 
		ARRAY[t._oid] AS exist, 
		ARRAY[t._tid] AS types, 
		'/'||'('||t._tlabel||')'||t._olabel AS strpath,
		FALSE AS cycle,
		ARRAY[ROW(t._tlabel,t._olabel)] AS tst,
		ARRAY[t._tlabel,t._olabel] AS tst2
    FROM wobjects AS t 
	WHERE _oid = 55099 --[item.pid]
		UNION ALL
		SELECT 	ord-1,
			t.*,
			exist || t._oid,
			types || t._tid,
			'/'||'('||t._tlabel||')'||t._olabel||strpath AS strpath,
			t._oid = ANY(exist) ,
			tst || ROW(t._tlabel,t._olabel),
			tst2 || ARRAY[t._tlabel,t._olabel]
		FROM parents AS p, wobjects AS t 
			WHERE t._oid = p._pid AND NOT cycle 
)
SELECT  
--min(parents.id),
* 
FROM 	parents 
WHERE 
	NOT cycle 
--AND	_pid=0 AND _oid<>0
--	GROUP BY parents.id,parents.pid,parents.label,parents.tid,parents.fcomment,parents.exist,parents.path,parents.cycle
--LIMIT 1;



/*
CREATE OR REPLACE FUNCTION 'create'+@_name (IN _label VARCHAR,IN _pid  INTEGER DEFAULT=0,IN _id  INTEGER DEFAULT=0,  ) 
RETURNS SETOF @table_name AS
$body$
DECLARE
    tmp    	INTEGER;
    new_row 	@table_name%rowtype;

BEGIN
	IF _id=_pid		-- если id=pid, значит узел является корнем,
	   AND _id=0 		-- для нашего случая разрешаем только один корень id=pid=0
	THEN
		new_row.id = _id;
		new_row.id = _pid;
		new_row.id = _label;


		INSERT INTO @table_name(id,pid,label)   VALUES (_id,_pid,_label);
	ELSE
		-- делаем поиск нового идентификатора 
		LOOP
			SELECT id FROM INTO tmp @table_name WHERE id IN  (SELECT nextval('@sq_id_name') );
			EXIT WHEN tmp IS NULL;  --!!!ограничить число переборов на случай полного заполнения
		END LOOP;
	END IF;


	return



END;
$body$
LANGUAGE 'plpgsql';
*/



-- Дополнительные функции и запросы

-- Запрос показывающий родителей узла (только для PG8.4)
/**
--SELECT * FROM gettree_parents( 3 );
/*
WITH RECURSIVE parents AS (
    SELECT 	* , 
		ARRAY[t.id] AS exist, 
		FALSE AS cycle 
    FROM typetree AS t 
	WHERE id = 5 --[item.pid]
		UNION ALL
		SELECT 	t.*, 
			exist || t.id, 
			t.id = ANY(exist) 
		FROM parents AS p, typetree AS t 
			WHERE t.id = p.pid AND NOT cycle
)
SELECT * FROM parents WHERE NOT cycle ;--LIMIT [max_deep];
*/

-- процедура выборки показывающий родителей узла
/**
CREATE OR REPLACE FUNCTION @getparents_name (pid_in      INTEGER,deep        INTEGER DEFAULT NULL  ) 
RETURNS SETOF @table_name AS
$body$
DECLARE
    exist_ids   INTEGER[]   := ARRAY[0];    -- Для пустого массива плохо работает ALL
    pid_now     INTEGER     := pid_in;      -- Текущий pid
    deep_now    INTEGER     := deep;        -- Текущаа глубина
    item        @table_name;
BEGIN
    WHILE 
        pid_now IS NOT NULL AND 
        pid_now > 0 AND
        pid_now <> ALL (exist_ids) AND
        (deep_now IS NULL OR
        deep_now > 0)
    LOOP
        SELECT * INTO item 
            FROM @table_name 
            WHERE id = pid_now;
        IF item.id IS NULL THEN
            EXIT;
        END IF;
        RETURN NEXT item;
        pid_now := item.pid;
        exist_ids := exist_ids || item.id;
        IF deep_now IS NOT NULL THEN
            deep_now := deep_now - 1;
        END IF;
    END LOOP;
    RETURN;
END;
$body$
LANGUAGE 'plpgsql';
*/

-- Запрос выборки всех дочерних узлов (только для PG8.4)
/**
WITH RECURSIVE children AS (
    SELECT *, 
	   '/'||label           		AS path,
          -- pid || '/' || id          		AS id_path,
           ARRAY[id]                            AS exist,
           FALSE                                AS cycle
    FROM typetree
    WHERE pid = 1
    UNION ALL
        SELECT t.*, 
	        path || '/' || t.label,
            --   id_path || '.' || t.pid || '/' || t.id,
               exist || t.id, 
               t.id = ANY(exist)
        FROM children AS c, 
             typetree  AS t
        WHERE t.pid = c.id AND NOT cycle -- AND array_length(exist, 1) < 10
)
SELECT * FROM children  WHERE NOT cycle ;--ORDER BY path;-- LIMIT 1000;
*/

-- процедура выборки детей !!!!!! не работает так - надо пилить напильником !!!!!
/**
CREATE OR REPLACE FUNCTION @getchilds_name (pid_in      INTEGER) 
RETURNS SETOF @table_name AS
$body$
DECLARE
    item       @table_name;
BEGIN

 WITH RECURSIVE children AS (	SELECT *, '/'||label  AS path,ARRAY[id] AS exist,FALSE AS cycle
					FROM typetree    WHERE pid = pid_in
				UNION ALL
				SELECT t.*,path || '/' || t.label, exist || t.id, t.id = ANY(exist)
					FROM children AS c, typetree  AS t
					WHERE t.pid = c.id AND NOT cycle -- AND array_length(exist, 1) < 10
)
 SELECT id,pid,folder,folder,label INTO item FROM children ; --WHERE NOT cycle ;--ORDER BY path;-- LIMIT 1000;
 RETURN NEXT item;

END;
$body$
LANGUAGE 'plpgsql';
*/
-----------------------------------------------------------------------------------------------------------------------------




    















