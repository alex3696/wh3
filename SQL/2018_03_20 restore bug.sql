CREATE EXTENSION postgres_fdw;
CREATE SERVER foreign_server
        FOREIGN DATA WRAPPER postgres_fdw
        OPTIONS (host '192.168.0.16', port '5432', dbname 'wh3');
CREATE USER MAPPING FOR postgres
        SERVER foreign_server
        OPTIONS (user 'postgres', password 'Ro6mecha');

IMPORT FOREIGN SCHEMA public
    FROM SERVER foreign_server INTO publicsgg;



-- проверка истории основной
SELECT public.acls.title, public.obj_name.title  
        ,publicsgg.log_main.*, public.log_main.*
FROM publicsgg.log_main
INNER JOIN public.log_main USING(id)
INNER JOIN public.obj_name ON public.obj_name.id=public.log_main.obj_id
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
     publicsgg.log_main.id<>public.log_main.id
  OR publicsgg.log_main.timemark<>public.log_main.timemark
  OR publicsgg.log_main.username<>public.log_main.username
  OR publicsgg.log_main.src_path<>public.log_main.src_path
  OR publicsgg.log_main.obj_id<>public.log_main.obj_id
  OR publicsgg.log_main.act_id<>public.log_main.act_id
  ;


-- проверка истории детали действий
SELECT public.acls.title,public.log_main.obj_id, public.obj_name.title  ,public.log_detail_act.*,publicsgg.log_detail_act.* 
FROM publicsgg.log_detail_act
INNER JOIN public.log_detail_act USING(id)
INNER JOIN public.log_main USING(id)
INNER JOIN public.obj_name ON public.obj_name.id=public.log_main.obj_id
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
(publicsgg.log_detail_act.prop IS NULL AND public.log_detail_act.prop IS NOT NULL)OR 
(publicsgg.log_detail_act.prop <> public.log_detail_act.prop)


-- проверка истории детали перемещений
SELECT public.acls.title, public.obj_name.title  
        ,publicsgg.log_detail_move.*, public.log_detail_move.*
FROM publicsgg.log_detail_move
INNER JOIN public.log_main USING(id)
INNER JOIN public.log_detail_move USING(id)
INNER JOIN public.obj_name ON public.obj_name.id=public.log_main.obj_id
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
     publicsgg.log_detail_move.id<>public.log_detail_move.id
  OR publicsgg.log_detail_move.dst_path<>public.log_detail_move.dst_path
  OR publicsgg.log_detail_move.qty<>public.log_detail_move.qty
  OR publicsgg.log_detail_move.prop_lid<>public.log_detail_move.prop_lid
  ;


-- проверка текущих свойств не равных тем, что были
SELECT public.acls.title, public.obj_name.title  
    , publicsgg.obj_name.* , public.obj_name.*
FROM publicsgg.obj_name
INNER JOIN public.obj_name USING(id)
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
  (publicsgg.obj_name.prop IS NULL     AND public.obj_name.prop IS NOT NULL)OR 
  (publicsgg.obj_name.prop IS NOT NULL AND public.obj_name.prop IS NULL)OR 
   publicsgg.obj_name.prop <> public.obj_name.prop
;

-- получение новых записей в лог таблице
drop table IF EXISTS new_log_main;
create temporary TABLE new_log_main  
    AS(
      SELECT * FROM publicsgg.log_main 
      WHERE timemark > '2018-03-18 00:00:00'::TIMESTAMP
      );
  SELECT * FROM new_log_main ;
  SELECT distinct ON(obj_id) obj_id FROM new_log_main ;

-- получение новых записей в лог таблице подробности ДЕЙСТВИЙ
drop table IF EXISTS new_detail_act;
create temporary TABLE new_detail_act  
    AS(
      SELECT publicsgg.log_detail_act.* FROM publicsgg.log_detail_act 
      INNER JOIN publicsgg.log_main USING (id)
      WHERE timemark > '2018-03-18 00:00:00'::TIMESTAMP
      AND act_id<>0
      );
  SELECT * FROM new_detail_act;





-- текущие свойства ПЕРЕСЕЧЕНИЕ перечнем изменённых

SELECT 
      public.acls.title, public.obj_name.title  
      , ((public.obj_name.prop - '104') = publicsgg.obj_name.prop)  AS eq
      , ( public.obj_name.prop->>'104' = publicsgg.obj_name.prop->>'124')  AS eq104_124
    , publicsgg.obj_name.* , public.obj_name.*
FROM publicsgg.obj_name
INNER JOIN public.obj_name USING(id)
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
     (publicsgg.obj_name.prop IS NULL     AND public.obj_name.prop IS NOT NULL)
  OR (publicsgg.obj_name.prop IS NOT NULL AND public.obj_name.prop IS NULL)
  OR  publicsgg.obj_name.prop <> public.obj_name.prop
  AND publicsgg.obj_name.id NOT IN (SELECT distinct ON(obj_id) obj_id FROM new_log_main)
ORDER BY public.obj_name.dt_update


--AND public.obj_name.id =4336


SELECT '{"104": 44 , "105":77}'::jsonb - '104'

659
98
561

стало


"{"101": "2661", "104": "исправен", "105": "", "106": "\\\\pfsgg.ru\\fs\\Метрология", "107": "\\\\pfsgg.ru\\fs\\Метрология", "108": "", "109": "", "110": "", "118": "Проверка работоспобности. Исправен. В ПП.", "124": "на Камчатку "}"
"{"101": "2661", "104": "на Камчатку ", "105": "", "106": "\\\\pfsgg.ru\\fs\\Метрология", "107": "\\\\pfsgg.ru\\fs\\Метрология", "108": "", "109": "", "110": "", "118": "Проверка работоспобности. Исправен. В ПП.", "124": "на Камчатку "}"


drop table IF EXISTS non_intersect_obj_name;
create temporary TABLE non_intersect_obj_name  
    AS(
SELECT public.obj_name.id
FROM publicsgg.obj_name
INNER JOIN public.obj_name USING(id)
INNER JOIN public.acls ON public.acls.id = public.obj_name.cls_id
WHERE 
     (publicsgg.obj_name.prop IS NULL     AND public.obj_name.prop IS NOT NULL)
  OR (publicsgg.obj_name.prop IS NOT NULL AND public.obj_name.prop IS NULL)
  OR  publicsgg.obj_name.prop <> public.obj_name.prop
AND 
publicsgg.obj_name.id NOT IN (SELECT distinct ON(obj_id) obj_id FROM new_log_main)
);
SELECT * FROM non_intersect_obj_name;





DROP FUNCTION IF EXISTS pg_temp.reconstruct() CASCADE;
CREATE OR REPLACE FUNCTION pg_temp.reconstruct() 
 RETURNS VOID
AS $BODY$ 
DECLARE
    _lid BIGINT;
  _sgg_prop JSONB;
  _sggbkp_prop JSONB;

  --получаем обьект 
  _cursor_obj CURSOR IS
    SELECT * FROM public.obj_name 
    --WHERE public.obj_name.id IN (SELECT * FROM non_intersect_obj_name)
    --WHERE public.obj_name.id = 4336
    ;

  --получаем историю изменений свойств и копируем в текущую базу
  _cursor_log CURSOR (_oid BIGINT) IS
    SELECT * FROM public.log_detail_act
    INNER JOIN public.log_main USING(id)
    WHERE public.log_main.obj_id = _oid
    --AND timemark < '2018-03-18 00:00:00'::TIMESTAMP 
    ORDER BY timemark ASC
  ;

  _cursor_lognew CURSOR (_oid BIGINT) IS
    SELECT * FROM new_log_main
    INNER JOIN new_detail_act USING(id)
    WHERE new_log_main.obj_id = _oid
    ORDER BY timemark ASC
  ;
BEGIN 
  FOR curr_obj IN _cursor_obj LOOP

    --RAISE NOTICE  'curr_obj.id = %',curr_obj.id;
    -- log old restore
    FOR _c_log IN _cursor_log(curr_obj.id) LOOP
    _sggbkp_prop:=_c_log.prop;
    SELECT prop INTO _sgg_prop FROM publicsgg.log_detail_act WHERE id = _c_log.id;
    --RAISE NOTICE  'lid = %',_c_log.id;
    --RAISE NOTICE  'bkp_prop=%',_sggbkp_prop;
    --RAISE NOTICE  'sgg_prop=%',_sgg_prop;
    IF _sgg_prop IS NULL OR _sggbkp_prop<>_sgg_prop THEN 
      UPDATE publicsgg.log_detail_act 
        SET prop=_sggbkp_prop
        WHERE id = _c_log.id;
    END IF;
    _lid:=_c_log.id;
    END LOOP; --FOR _c_log IN _cursor_log(curr_obj.id) LOOP 
    --RAISE NOTICE  'last bkp_prop=% ',_sggbkp_prop;

    FOR _new_log IN _cursor_lognew(curr_obj.id) LOOP

      _sggbkp_prop:= COALESCE(_sggbkp_prop,'{}'::JSONB) || _new_log.prop;
      UPDATE publicsgg.log_detail_act 
        SET prop=_sggbkp_prop
        WHERE id = _new_log.id;
      _lid:=_new_log.id;
    END LOOP;
    -- restore current
    UPDATE publicsgg.obj_name 
      SET prop=_sggbkp_prop , act_logid=_lid
      WHERE id = curr_obj.id;

    _sggbkp_prop:=NULL;
  END LOOP;
  

--RETURN '{"a":5 ,"b":6}'::JSONB;
END; 
$BODY$ LANGUAGE plpgsql VOLATILE COST 2000;
select pg_temp.reconstruct();









