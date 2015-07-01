




 






------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Заполняем дерево ';
PRINT '';
/** making struct */
PRINT '	root0';
PRINT '		A';
PRINT '			C';
PRINT '				obj1_in_c';
PRINT '				obj2_in_c';
PRINT '			D';
PRINT '		B';
PRINT '			obj1_in_b';
PRINT '			obj2_in_b';


------------------------------------------------------------------------------------------------------------




INSERT INTO typetree(pid,label)   VALUES (0,'A');--id = 2
INSERT INTO typetree(pid,label)   VALUES (0,'B');--id = 3

INSERT INTO typetree(pid,label)   VALUES (2,'C');--id = 4
INSERT INTO typetree(pid,label)   VALUES (2,'D');--id = 5

INSERT INTO typetree(id,pid,label,tid)   VALUES (5,4,'склад',1);--6 этот узел не может иметь дочерних узлов,id подменится тригером
INSERT INTO typetree(id,pid,label,tid)   VALUES (6,3,'ВИКИЗ',1);--7 этот узел не может иметь дочерних узлов,id подменится тригером


INSERT INTO objtree(pid, label, tid)   VALUES (0, 'склад 1', 6);-- id = 1
INSERT INTO objtree(pid, label, tid)   VALUES (0, 'склад 2', 6);-- id = 2

INSERT INTO objtree(pid, label, tid)   VALUES (1, 'ВИКИЗ №1', 7);-- 3
INSERT INTO objtree(pid, label, tid)   VALUES (2, 'ВИКИЗ №2', 7);-- 4
INSERT INTO objtree(pid, label, tid)   VALUES (2, 'ВИКИЗ №3', 7);-- 5


--INSERT INTO moverules(label, tid, from_tid, to_tid)   VALUES ('перетаскивание ВИКИЗ по СКЛАДАМ', 7, 6, 6);


-- попробуем сделать все варианты правил для перемещения ВИКИЗ 1(tid=7 id=3)из склада1(tid=6 id=1) в склад2 (tid=6 id=2)
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(*) с СКЛАД(*) на СКЛАД(*)', 7,6,6, 0,0,0);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(*) с СКЛАД(*) на СКЛАД(2)', 7,6,6, 0,0,2);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(*) с СКЛАД(1) на СКЛАД(*)', 7,6,6, 0,1,0);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(*) с СКЛАД(1) на СКЛАД(2)', 7,6,6, 0,1,2);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(1) с СКЛАД(*) на СКЛАД(*)', 7,6,6, 3,0,0);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(1) с СКЛАД(*) на СКЛАД(2)', 7,6,6, 3,0,2);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(1) с СКЛАД(1) на СКЛАД(*)', 7,6,6, 3,1,0);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('ВИКИЗ(1) с СКЛАД(1) на СКЛАД(2)', 7,6,6, 3,1,2);
INSERT INTO moverules(tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES (7,0,6,4,0,0);


INSERT INTO wh_role(rolname,rolcanlogin, rolcomment)	VALUES('тестовая группа',false,'тестовая группа');
SELECT whgrant_grouptorule('тестовая группа','ВИКИЗ(*) с СКЛАД(*) на СКЛАД(*)');
SELECT whgrant_grouptorule('тестовая группа','ВИКИЗ(*) с СКЛАД(*) на СКЛАД(2)');
SELECT whgrant_grouptorule('User','ВИКИЗ(*) с СКЛАД(*) на СКЛАД(*)');

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT 'ERR: -- проверка ограничений вставки ';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO typetree(id,pid,label,tid)   VALUES (-2,2,'obj2_in_b',1);-- ошибка, добавление узла с отрицательным id
INSERT INTO typetree(id,pid,label)   VALUES (30,30,'err');	-- ошибка, добавление второго корня
INSERT INTO typetree(id,pid,label)   VALUES (7,6,'Err'); 	-- ошибка, проверка вставки узел, который не может иметь дочерних узлов
INSERT INTO typetree(id,pid,label)   VALUES (7,100,'Err'); 	-- ошибка, проверка FK - пытаемся вставить ребёнка в несуществующий узел

INSERT INTO objtree(id,pid,label)   	VALUES (30,30,'ошибка, добавление второго корня');	
INSERT INTO objtree(id,pid,label,tid)   VALUES (-2,0,'ошибка, добавление узла с отрицательным id',2);
INSERT INTO objtree(id,pid,label)   	VALUES (7,100,'ошибка, проверка FK - пытаемся вставить ребёнка в несуществующий узел'); 	
INSERT INTO objtree(pid, label, tid)   VALUES (0, 'ВИКИЗ №1', 7);-- ошибка, проверка CK пытаемся вставить ребёнка узел где уже есть узел с таким именем

INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('перетаскивание ВИКИЗ №1 с склада 1 в склад 2', 7,6,6, NULL,1,2);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('перетаскивание ВИКИЗ №1 с склада 1 в склад 2', 7,0,6, 3,1,2);
INSERT INTO moverules(label, tid, from_tid, to_tid, oid, oid_from, oid_to)   VALUES ('перетаскивание ВИКИЗ №1 с склада 1 в склад 2', 7,6,0, 3,1,2);

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT 'ERR: -- проверка ограничений изменения';
PRINT '';
------------------------------------------------------------------------------------------------------------
UPDATE typetree SET id=15 WHERE id=2;			-- ошибка, нельзя изменить идентификатор узла
UPDATE typetree SET label='A' WHERE id=3;			-- ошибка, в папке не может быть одинаковых узлов
UPDATE typetree SET pid=5 WHERE id=2;			-- ошибка, нельзя переместить узел в один из своих детей
UPDATE typetree SET tid=3 WHERE id=3;			-- ошибка, нельзя менять тип

UPDATE objtree SET id=2 WHERE id=1;			-- ошибка, нельзя изменить идентификатор узла
UPDATE objtree SET tid=2 WHERE id=1;			-- ошибка, нельзя изменить тип узла
UPDATE objtree SET label='qqqq', pid=0 WHERE id=3;	-- Нельзя делать переименование и перенос одновременно
UPDATE objtree SET pid=3 WHERE id=1;			-- ошибка, нельзя переместить узел в один из своих детей

UPDATE objtree SET pid=2 WHERE id=3;			-- ошибка, нельзя переместить не имея разрешающего правила
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT 'ERR: -- -- проверка ограничений удаления';
PRINT '';
------------------------------------------------------------------------------------------------------------
DELETE FROM typetree WHERE id=0;				-- ошибка, корень нельзя удалять
DELETE FROM objtree WHERE id=0;				-- ошибка, корень нельзя удалять





