BEGIN TRANSACTION;

--SET client_min_messages = 'error';
--SET client_min_messages = 'warning';
SET client_min_messages = 'notice';
--SET client_min_messages = 'debug';
--SHOW client_min_messages;
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS stat_get_obj_location(_oid BIGINT,_begin TIMESTAMP) CASCADE;
CREATE OR REPLACE FUNCTION stat_get_obj_location(_oid BIGINT,_begin TIMESTAMP) 
  RETURNS BIGINT AS
$BODY$ 
  SELECT CASE WHEN dst_path[1][2] IS NOT NULL THEN dst_path[1][2] ELSE src_path[1][2] END AS obj_parent_oid
      FROM log_main 
      LEFT JOIN log_detail_move USING (id)
      WHERE obj_id = _oid 
        AND timemark < _begin 
      ORDER BY timemark DESC
      FETCH FIRST 1 ROW ONLY --LIMIT 1
$BODY$
LANGUAGE sql STABLE COST 10;
GRANT EXECUTE ON FUNCTION stat_get_obj_location(_oid BIGINT,_begin TIMESTAMP) TO "Guest";
/**
SELECT * FROM stat_get_obj_location(3883,'2015.02.10 00:00+5');
SELECT * FROM stat_get_obj_location(3883,'2015.04.10 00:00+5');
SELECT * FROM stat_get_obj_location(3883,'2017.04.10 00:00+5');
*/
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------

DROP FUNCTION IF EXISTS stat_mov_obj_all(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) CASCADE;
CREATE OR REPLACE FUNCTION stat_mov_obj_all(_oid BIGINT, _begin TIMESTAMP, _end TIMESTAMP) 
 RETURNS TABLE( dst_time INTERVAL
               ,dst_oid  BIGINT
              ) AS $BODY$ 
DECLARE 
  _olog CURSOR(_start TIMESTAMP, _stop TIMESTAMP) IS
    SELECT log_dt, src_oid, log.dst_oid, mobj_id, mcls_id FROM log 
      WHERE mobj_id= _oid 
        AND act_id IS NULL
        AND log_dt>=_start AND log_dt<=_stop 
    ORDER BY log_dt ASC
    ;
  rec           RECORD;
  curr_dt       TIMESTAMP;
  curr_dst_oid  BIGINT;

  _location_cursor CURSOR IS
      SELECT CASE WHEN log.dst_oid IS NOT NULL THEN log.dst_oid ELSE log.src_oid END 
      FROM log WHERE mobj_id = _oid 
    AND log_dt < _begin  
    ORDER BY log_dt DESC 
    
    ;
BEGIN

-- пытаемся найти первое упоминание до диапазона 
--  SELECT log.act_id,log.dst_oid,log.src_oid INTO rec FROM log WHERE mobj_id = _oid 
--    AND log_dt < _begin  -- AND log_dt>'2000.01.01 00:00'
--    ORDER BY log_dt DESC 
--  LIMIT 1; ??? почемуто выполняется медленно если объект создан внутри диапазона !!ЕСЛИ ЕСТЬ ЛИМИТ!!

--    SELECT CASE WHEN log.dst_oid IS NOT NULL THEN log.dst_oid ELSE log.src_oid END 
--      INTO curr_dst_oid FROM log WHERE mobj_id = _oid 
--    AND log_dt < _begin  
--    ORDER BY log_dt DESC 
--    LIMIT 1 --FETCH FIRST 1 ROW ONLY
--    ; 

  -- перенос LIMIT в функцию работает так же 
  --SELECT stat_get_obj_location(_oid,_begin) INTO curr_dst_oid; 

  OPEN  _location_cursor;
  FETCH _location_cursor INTO curr_dst_oid;
  CLOSE _location_cursor;

  IF curr_dst_oid IS NOT NULL THEN 
    curr_dt :=_begin; 
    --RAISE NOTICE  'объект существовал до диапазона' ;
  ELSE
    -- ищем первое упоминание внутри диапазона
    SELECT src_oid,log_dt INTO rec FROM log 
      WHERE mobj_id= _oid 
        AND log_dt>=_begin AND log_dt<=_end
      ORDER BY log_dt ASC
      LIMIT 1;
    IF rec.log_dt IS NOT NULL AND rec.src_oid IS NOT NULL THEN 
      --RAISE NOTICE  'объект создан внутри диапазона' ;
      curr_dt :=rec.log_dt; 
      curr_dst_oid:= rec.src_oid;
    ELSE 
      --RAISE NOTICE  'внутри диапазона нет сведений - выход' ;
      RETURN;
    END IF;
  END IF;

  OPEN _olog(_begin,_end);
  LOOP
    FETCH _olog INTO rec;
    IF rec.log_dt IS NOT NULL THEN
      dst_time := rec.log_dt - curr_dt;
      dst_oid  := curr_dst_oid;
      RETURN NEXT;
      curr_dst_oid := rec.dst_oid;
      curr_dt      := rec.log_dt;
    ELSE
      dst_time := _end - curr_dt;
      dst_oid  := curr_dst_oid;
      RETURN NEXT;
      EXIT;
    END IF;
  END LOOP;
  CLOSE _olog;

  RETURN;
END; 
$BODY$ LANGUAGE plpgsql STABLE COST 50 ROWS 50;
GRANT EXECUTE ON FUNCTION stat_mov_obj_all(BIGINT,TIMESTAMP,TIMESTAMP) TO "Guest";
/**
SELECT * FROM log WHERE mobj_id= 3883 ORDER BY log_dt DESC;
SELECT * FROM stat_mov_obj_all (3883,'2015.01.01 00:00+5','2015.07.01 00:00+5');
*/
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------
COMMIT TRANSACTION;