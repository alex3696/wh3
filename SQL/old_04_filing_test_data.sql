/* Заполнение тестовыми данными */

DECLARE @abstract_cls,@real_cls, @real_obj;
DECLARE @cur_abstract_id;


SET @abstract_cls= 1000;


DECLARE @base_id;


DELETE FROM t_cls WHERE id>200;
DELETE FROM t_cls WHERE label LIKE 'TEST%';


SET @base_id = INSERT INTO t_cls(label, type, pid,measurename)VALUES ('TEST', 0, 1, NULL) RETURNING id;

DECLARE @rnd_type;    
        set @rnd_type = INTEGER(1,3 );



WHILE (@abstract_cls > 0)
BEGIN
    SET @cur_abstract_id = INSERT INTO t_cls(label, type, pid,measurename)    
        VALUES ('TEST_A_@abstract_cls', 0, @base_id, NULL) RETURNING id;

    SET @real_cls=10;
    WHILE (@real_cls>0)
    BEGIN
        DECLARE @new_type;
        SET @new_type=CAST (@rnd_type AS INTEGER);


        IF (@new_type = 1)
        BEGIN
           INSERT INTO t_cls(label, type, pid)VALUES ('TEST_@abstract_cls @real_cls', @new_type, @cur_abstract_id);
        END
        ELSE
        BEGIN
           INSERT INTO t_cls(label, type, pid,measurename)    
                VALUES ('TEST_@abstract_cls @real_cls', @new_type, @cur_abstract_id, 'some');
        END

        SET @real_cls=@real_cls-1;
    END
    

    SET @abstract_cls=@abstract_cls-1;
END
VACUUM FULL ANALYSE ;

SET @abstract_cls =11000;

WHILE (@abstract_cls >0 )
BEGIN
    DROP TABLE IF EXISTS t_log_@abstract_cls CASCADE;
    DROP TABLE IF EXISTS t_state_@abstract_cls CASCADE;
        
    SET @abstract_cls=@abstract_cls-1;
END








/*папок-10, в каждую папку кладём по 100 типов - итого в таблице "typetree" 1000+10 записей*/
DECLARE @max_folder_qty, @type_per_folder,	@cur_folder_qty,	@cur_folder_id;
SET @max_folder_qty=10;
SET @type_per_folder=100;
SET @cur_folder_qty=0;
SET @cur_folder_id=0;

-- общее количество типов 10*100 = 1000
DECLARE @max_type_qty,@cur_type_qty,	@cur_type_id;
SET @max_type_qty = @max_folder_qty * @type_per_folder;
SET @cur_type_qty   = 0;
SET @cur_type_id   = 0;

-- по 1000 объектов на тип 
DECLARE @obj_per_type;
SET @obj_per_type = 100;

--общее количество объектов в таблице "objtree"  10*100*100 = 100 0000
DECLARE @max_obj_qty,	@cur_obj_qty,	@cur_obj_id;
SET @max_obj_qty = @max_type_qty * @obj_per_type;
SET @cur_obj_qty    = 0;
SET @cur_obj_id    = 0;

DECLARE @total_obj,@obj_pid;

DECLARE @type_seq,@obj_seq;

DECLARE @a1,@a2,@a3,@a4,@a5,@a6;



WHILE (@cur_folder_qty<@max_folder_qty)			-- пока текущее каоличество папок меньше заданного
BEGIN
	-- создаём папку в корне
	SET @cur_folder_id = INSERT INTO t_cls(pid,label)VALUES(0,'folder_@cur_folder_qty' ) RETURNING id;
	--PRINT @cur_folder_id;

	IF (LINES(@cur_folder_id) > 0)
	BEGIN
		SET @cur_type_qty=0;
		WHILE (@cur_type_qty<@type_per_folder)
		BEGIN
			SET @type_seq = @cur_folder_qty*@type_per_folder+@cur_type_qty;
			SET @cur_type_id = INSERT INTO t_cls(pid,label,tid)VALUES(@cur_folder_id,'type_@type_seq',1) RETURNING id;
			SET @cur_obj_qty=0;
			

			--инициализируем генераторы случайных чисел
			SET @obj_pid=INTEGER(0, CAST (@total_obj AS INTEGER) , 1) ;

			SET @a2=INTEGER(0,CAST (@type_seq AS INTEGER) , 1,CAST (@type_seq AS INTEGER)/3);
			SET @a3=INTEGER(0,CAST (@type_seq AS INTEGER) , 1,CAST (@type_seq AS INTEGER)/5);
			SET @a4=INTEGER(0,CAST (@cur_obj_qty AS INTEGER),1);
			SET @a5=INTEGER(0,CAST (@total_obj AS INTEGER),1,CAST (@total_obj AS INTEGER)/7);
			SET @a6=INTEGER(0,CAST (@total_obj AS INTEGER),1,CAST (@total_obj AS INTEGER)/3);

			

			WHILE (@cur_obj_qty<@obj_per_type)
			BEGIN	
				SET @total_obj=@type_seq*@obj_per_type+@cur_obj_qty;
				
				INSERT INTO objtree(pid,label,tid)VALUES( @obj_pid ,'object @type_seq @cur_obj_qty',@cur_type_id);
				PRINT @type_seq;
				SET @a1=CAST (@cur_type_id AS INTEGER);
				INSERT INTO moverules(label,tid,from_tid,to_tid,oid,oid_from,oid_to)VALUES('random rule @cur_obj_qty',@a1,@a2,@a3,@a4,@a5,@a6);
				
				SET @cur_obj_qty=@cur_obj_qty+1;
			END
			
			SET @cur_type_qty=@cur_type_qty+1;
		END
	END
	

	SET @cur_folder_qty=@cur_folder_qty+1;		-- инкрементируем количество папок
END


DECLARE @qty,@r1,@r2,@r3;
SET @qty=0;

	SET @r1=INTEGER(0,45000,1,1234);
	SET @r2=INTEGER(0,45000,1,23456);
	SET @r3=INTEGER(0,45000,1,34567);

WHILE (@qty<45000)
BEGIN

	--PRINT CAST (@r1 AS STRING)+'   '+CAST(@r2 AS STRING)+'   '+CAST(@r3 AS STRING)+'   ';
	
	INSERT INTO thistory(foid, fsrcoid, fdstoid)  VALUES ( @r1,@r2 ,@r3); 
	SET @qty=@qty+1;
END

VACUUM FULL ANALYSE ;



