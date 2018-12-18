--BEGIN TRANSACTION;
--SET client_min_messages = 'notice';
DO $$
DECLARE 
  end_date DATE;
  begin_date DATE;
  sheduled_act_id BIGINT;
  col_name TEXT;
  previos_date DATE;
  begin_interval DATE;
  end_interval DATE;
  next_act DATE;
  qry TEXT;

  _curs refcursor;
  rec record;
  _ctid tid;

  obj_create TEXT;
  obj_insert TEXT;
BEGIN
  begin_date:=current_date;
  end_date:='2019-02-15';
  SELECT id INTO sheduled_act_id FROM act WHERE title='Калибровка';
  --end_date:='?Конец периода?DATE?2018.12.31 23:59:56?'::TIMESTAMP::DATE;
  --SELECT id INTO sheduled_act_id FROM act WHERE title='?Действие?STRING?Калибровка?';

  SELECT id INTO obj_create FROM prop WHERE title='Дата выпуска';
  SELECT id INTO obj_insert FROM prop WHERE title='Дата ввода в эксплуатацию';

  drop table IF EXISTS pg_temp.act_shedule;
  create temporary TABLE act_shedule
    AS(
      SELECT 
        --obj.id          AS oid
        --,obj.cls_id     AS cid
        get_path_cls(obj.cls_id,1)  AS "полный тип" 
        ,acls.title AS "тип"
        ,obj.title  AS "объект"
        ,period     AS "период"
        ,obj.dt_insert::DATE AS "Дата первой записи"
        ,prop->>obj_create AS "Дата выпуска"
        ,prop->>obj_insert AS "Дата ввода в эксплуатацию"
        ,(SELECT MAX(timemark)::DATE AS previos
                    FROM log_main 
                    WHERE log_main.act_id = sheduled_act_id --( SELECT id FROM act_info)
                    AND log_main.act_id<>0 
                    AND log_main.obj_id=obj.id
                    GROUP BY log_main.obj_id, log_main.act_id
        )previos
      FROM obj
      LEFT JOIN acls ON acls.id=obj.cls_id 
      INNER JOIN LATERAL(
        SELECT period
        FROM ref_cls_act 
        INNER JOIN (SELECT id FROM get_path_cls_info(obj.cls_id, 1)) ct ON ref_cls_act.cls_id = ct.id
        WHERE ref_cls_act.period IS NOT NULL
        AND ref_cls_act.act_id = sheduled_act_id --( SELECT id FROM act_info)
      ) period ON TRUE
      WHERE acls.kind=1 --AND period IS NOT NULL
      ORDER BY "полный тип" 
        ,(substring(obj.title, '^[0-9]+')::INT, obj.title ) ASC 
      --LIMIT 10
    );

  begin_interval:=begin_date;
  end_interval:=end_date;
  WHILE begin_interval < end_interval LOOP
    col_name:=quote_ident(to_char(begin_interval, 'month YYYY'));
    qry:='ALTER TABLE pg_temp.act_shedule ADD COLUMN '||col_name||' DATE;';
    RAISE NOTICE  '%',qry;
    EXECUTE qry;
/*
    EXECUTE 'UPDATE act_shedule SET '||col_name||' = previos+"период" WHERE 
      '''||begin_interval||'''::DATE-"период">previos';
      


    EXECUTE 'UPDATE act_shedule SET '||col_name||' = previos+"период" WHERE 
      (DATE_TRUNC(''month'', '''||begin_interval||'''::DATE), DATE_TRUNC(''month'', CURRENT_DATE)+ INTERVAL ''1 month''- INTERVAL ''1 day'') OVERLAPS
       (previos+"период", previos+"период")';
       */
    begin_interval:= begin_interval+INTERVAL '1 month';
  END LOOP;

  
  OPEN _curs FOR EXECUTE 'SELECT * FROM act_shedule ' FOR UPDATE ;
  LOOP
    FETCH NEXT FROM _curs INTO rec;
    EXIT WHEN rec IS NULL;
    previos_date:=rec.previos;
    begin_interval:=begin_date;
    end_interval:=DATE_TRUNC('month', begin_interval)+ INTERVAL '1 month'- INTERVAL '1 day';
    _ctid := rec.ctid;
    --RAISE NOTICE  '----------------';
        
    WHILE begin_interval < end_date LOOP
      --RAISE NOTICE  '%<>%',begin_interval,end_interval;
      col_name:=quote_ident(to_char(begin_interval, 'month YYYY'));
   
        IF (previos_date IS NULL OR begin_interval > previos_date+rec."период") THEN
          
          qry:=format('UPDATE act_shedule SET %s = %L::DATE WHERE ctid = $1 RETURNING ctid',col_name,begin_date);
          --RAISE NOTICE  'qry = %  rec.ctid=% ',qry,rec.ctid;
          EXECUTE qry USING _ctid INTO _ctid;
          --UPDATE act_shedule SET "january   2018" = begin_date WHERE CURRENT OF curs;
          
          previos_date:=begin_date;
        ELSE
          next_act := previos_date+rec."период";
          IF (begin_interval,end_interval) OVERLAPS(next_act,next_act) THEN
            --UPDATE act_shedule SET "january   2018" = next_act WHERE CURRENT OF curs;
            qry:=format('UPDATE act_shedule SET %s = %L::DATE WHERE ctid = $1 RETURNING ctid',col_name,next_act);
            --RAISE NOTICE  'qry = %  rec.ctid=% ',qry,rec.ctid;
            EXECUTE qry USING _ctid INTO _ctid;
            previos_date:=next_act;
          END IF;
        END IF;


        
      begin_interval:= end_interval   + INTERVAL '1 day';
      end_interval  := begin_interval + INTERVAL '1 month'-INTERVAL '1 day';
    END LOOP;
    
  

  END LOOP;
  CLOSE _curs;
  
  
 
END$$;  
SELECT ctid,* FROM act_shedule ;

--SELECT ctid, * FROM act_shedule WHERE ctid='(4294967295,0)'::tid
--UPDATE act_shedule SET "january   2019" = '2019-01-09'::DATE WHERE ctid = '(0,11)'::tid
--SELECT ctid, * FROM act_shedule LIMIT 100

--COMMIT TRANSACTION;














