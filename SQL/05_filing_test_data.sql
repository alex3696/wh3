---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Заполняем тестовыми данными';
PRINT '';
---------------------------------------------------------------------------------------------------



DELETE FROM prop CASCADE;
DELETE FROM act CASCADE;
DELETE FROM cls WHERE title='TestCls';

DECLARE @cls_id_1,@cls_id_2,@cls_id_3,@cls_id_4,@cls_id_5,@cls_id_6,@cls_id_7,@cls_id_8;
SET @cls_id_1 = INSERT INTO cls(pid,title,kind,measure) VALUES (1,'TestCls',1,'coco') RETURNING id;
SET @cls_id_2 = INSERT INTO cls(pid,title,kind) VALUES (1,'Приборы',0) RETURNING id;
SET @cls_id_3 = INSERT INTO cls(pid,title,kind) VALUES (1,'ЗИП',0) RETURNING id;
SET @cls_id_4 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_2,'СРК2М',1,'ед.') RETURNING id;
SET @cls_id_5 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_2,'ДИНА-К4-89',1,'ед.') RETURNING id;
SET @cls_id_6 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'ФЭУ-102',2,'шт.') RETURNING id;
SET @cls_id_7 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'упл.кольцо 105-115-58',2,'шт.') RETURNING id;
SET @cls_id_8 = INSERT INTO cls(pid,title,kind,measure) VALUES (@cls_id_3,'Спирт',3,'л.') RETURNING id;

DECLARE @oid_1,@oid_2,@oid_3,@oid_4,@oid_5,@oid_6,@oid_7,@oid_8,@oid_9,@oid_10,@oid_11,@oid_12,@oid_13;
SET @oid_1 = INSERT INTO obj(title,cls_id,pid) VALUES ('01',@cls_id_4, 1 )RETURNING id;
SET @oid_2 = INSERT INTO obj(title,cls_id,pid) VALUES ('02',@cls_id_4, 1 )RETURNING id;
SET @oid_3 = INSERT INTO obj(title,cls_id,pid) VALUES ('03',@cls_id_4, 1 )RETURNING id;
SET @oid_4 = INSERT INTO obj(title,cls_id,pid) VALUES ('01',@cls_id_5, 1 )RETURNING id;

SET @oid_5 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_6, 1, 9)RETURNING id;
SET @oid_6 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_7, 1, 50)RETURNING id;
SET @oid_7 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_8, 1 , 3.3)RETURNING id;
SET @oid_8 = INSERT INTO obj(title,cls_id,pid,qty) VALUES ('партия 1',@cls_id_8, 1 , 3.1)RETURNING id;

SET @oid_9 = INSERT INTO obj(title,cls_id,pid) VALUES ('1',@cls_id_1, 1 )RETURNING id;
SET @oid_10 = INSERT INTO obj(title,cls_id,pid) VALUES ('2',@cls_id_1, 1 )RETURNING id;


DECLARE @prop_id_1,@prop_id_2,@prop_id_3,@prop_id_4;
SET @prop_id_1 = INSERT INTO prop(title, kind)VALUES ('Наработка(ч.)', 10)RETURNING id;
SET @prop_id_2 = INSERT INTO prop(title, kind)VALUES ('Счёт ГК', 10)RETURNING id;
SET @prop_id_3 = INSERT INTO prop(title, kind)VALUES ('Комментарий', 0)RETURNING id;
SET @prop_id_4 = INSERT INTO prop(title, kind)VALUES ('Описание ремонта', 0)RETURNING id;

DECLARE @act_id_1,@act_id_2,@act_id_3,@act_id_4;
SET @act_id_1 = INSERT INTO act (title) VALUES ('Ремонт')RETURNING id;
SET @act_id_2 = INSERT INTO act (title) VALUES ('Проверка')RETURNING id;
SET @act_id_3 = INSERT INTO act (title) VALUES ('Профилактика')RETURNING id;
SET @act_id_4 = INSERT INTO act (title) VALUES ('ГИС')RETURNING id;

-- 'Ремонт' --Наработка+Счёт ГК+Комментарий+Описание ремонта
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_3);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_1, @prop_id_4);
-- 'Проверка' --Наработка+Счёт ГК+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_2, @prop_id_3);
-- 'Профилактика' --Наработка+Счёт ГК+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_2);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_3, @prop_id_3);
-- 'ГИС' --Наработка+Комментарий
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_4, @prop_id_1);
INSERT INTO ref_act_prop(act_id, prop_id)VALUES (@act_id_4, @prop_id_3);


INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_1);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_2);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_3);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_4, NULL, @act_id_4);


INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_6, NULL, @act_id_2);

INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_5, NULL, @act_id_1);
INSERT INTO perm_act(access_group, access_disabled, cls_id, obj_id, act_id)
    VALUES ('User', 0, @cls_id_5, NULL, @act_id_3);


INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id, src_path
                      ,dst_cls_id, dst_obj_id, dst_path)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL        -- whar [ФЭУ-102]%
                       ,2, NULL, '{%}'          -- from [ROOT]%
                       , @cls_id_4, NULL, '{%}' -- to   [СРК2М]% 
                      );

INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id
                      ,dst_cls_id, dst_obj_id)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL        -- whar [ФЭУ-102]%
                       ,2, NULL                -- from [ROOT]%
                       , @cls_id_5, NULL       -- to   [ДИНА-К4-89]% 
                      );

INSERT INTO perm_move( access_group, access_disabled
                      ,cls_id, obj_id
                      ,src_cls_id, src_obj_id
                      ,dst_cls_id, dst_obj_id)VALUES 
                     ( 'User', 0
                       ,@cls_id_6, NULL       -- whar [ФЭУ-102]%
                       ,@cls_id_5, NULL       -- from [ДИНА-К4-89]% 
                       ,2, NULL             -- to   [ROOT]%
                       
                      );



/*
delete from cls_abstr WHERE id>5000;
delete from cls_num WHERE id>5000;
delete from cls_qtyi WHERE id>5000;
delete from cls_qtyf WHERE id>5000;

delete from obj WHERE id>5000;

*/


---------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Вставка тестовых свойств';
PRINT '';
---------------------------------------------------------------------------------------------------
/*
DECLARE @prop_qty ;
SET @prop_qty= 10;
WHILE (@prop_qty > 0)
BEGIN
  INSERT INTO prop(title, kind)VALUES ('TestProp@prop_qty', 0);
  SET @prop_qty=@prop_qty-1;
END
*/


---------------------------------------------------------------------------------------------------
DECLARE @clstitle, @qty, @pid, @cnumid, @cqtyiid, @cqtyfid , @oqtyiid, @oqtyfid, @opid ;
SET @opid= 1;
SET @pid= 1;
SET @qty= 10;
--SET @qty= 10;

WHILE (@qty > 0)
BEGIN

  SET @pid =     INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsa@qty',0,NULL)RETURNING id;
  SET @pid = CAST (@pid AS INTEGER);
  SET @cnumid =  INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsn@qty',1,'ед')RETURNING id;
  SET @cqtyiid = INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsqi@qty',2,'шт')RETURNING id;
  SET @cqtyfid = INSERT INTO cls(pid,title,kind,measure) VALUES (@pid,'clsqf@qty',3, 'mes.')RETURNING id;

  SET @cnumid = CAST (@cnumid AS INTEGER);
  SET @cqtyiid = CAST (@cqtyiid AS INTEGER);
  SET @cqtyfid = CAST (@cqtyfid AS INTEGER);

  SET @opid= INSERT INTO obj(title,cls_id,pid) VALUES ('objnum@cnumid',   @cnumid,@opid)RETURNING id;
  INSERT INTO obj(title,cls_id,pid,qty) VALUES ('objqtyi@cqtyiid', @cqtyiid, @opid, 2);
  INSERT INTO obj(title,cls_id,pid,qty) VALUES ('objqtyi@cqtyfid', @cqtyfid, @opid, 3);

/**
  SET @oqtyiid = INSERT INTO obj_names_qtyi(title,cls_id,qty) VALUES ('objqtyi@cqtyiid', @cqtyiid)     RETURNING id;
  SET @oqtyfid = INSERT INTO obj_names_qtyf(title,cls_id) VALUES ('objqtyf@cqtyfid', @cqtyfid)     RETURNING id;

  INSERT INTO obj_details_qtyi(objqty_id, pid, qty) VALUES (@oqtyiid, @opid, 1);
  INSERT INTO obj_details_qtyf(objqty_id, pid, qty) VALUES (@oqtyfid, @opid, 1);
*/


  SET @qty=@qty-1;
END
VACUUM FULL ANALYSE ;








------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения количественных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,10); /*div to NULL*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*div to 10*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,1); SELECT move_object(103,104,1,100,5); /*mov to 15*/ SELECT lock_reset(103,104,1);
--SELECT * FROM lock_for_move(103,104,100); SELECT move_object(103,104,100,1,20); /*mov to NULL*/ SELECT lock_reset(103,104,100);
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты перемещения номерных объектов';
PRINT '';
------------------------------------------------------------------------------------------------------------
--SELECT * FROM lock_for_move(104,102,1);
--SELECT move_object(104,102,1,103,1); -- mov objnum
--SELECT lock_reset(104,102,1);

--SELECT * FROM lock_for_move(104,102,103);
--SELECT move_object(104,102,103,1,1); -- mov objnum
--SELECT lock_reset(104,102,103);













/*
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

*/


