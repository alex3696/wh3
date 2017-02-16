/** Система безопасности построена как аналог ролей postgres
придобавлении пользователей в эти таблицы добавляются/изменяются пользователи в системных таблицах

/** 
0.2
Описание и порядок запуска скриптов для создания БД

-1 security.sql
	Создаём систему безопасности подобную встроенной в postgres
	--wh_role 	 	- таблица ролей
	--wh_auth_members 	- таблица членства ролей
	--Копируем пользователей из Postgres --> WH
	В ДАЛЬНЕЙШЕМ ИСПОЛЬЗУЕМ ВМЕСТО ВСТРОЕННОЙ В POSTGRES
	
-2 main.sql
	Основные таблицы и оьъекты проекта
	Создаём заново Guest,User,ObjetDesigner,TypeDesigner,Admin если их нет и меняем если есть

-2 test.sql
	- тесты системы безопасности
	- тесты основных объектов

*/	

-- убираем все привилегии "по умолчанию"	
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON SEQUENCES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON TABLES FROM public;
ALTER DEFAULT PRIVILEGES REVOKE ALL PRIVILEGES ON FUNCTIONS FROM public;

/** Будьте внимательны при создании пользователей с опциями 
CREATEROLE - они могут удалять и редактировать любые группы и пользователей несмотря то, к какой они группе относятся 
WITH ADMIN OPTION - позволяет передавать свои права другим пользователям, лучше не пользоваться ею - все права пусть раздаёт администратор создающий роли
HINT пользователя прошедьшего аутентификацию можно ограничить правами только одной роли SET ROLE TO "role name"

Рекомендации(настоятельные) по создании пользователей:
CREATE ROLE "Администратор" 	LOGIN NOSUPERUSER INHERIT NOCREATEDB   CREATEROLE IN ROLE "Admin"		ENCRYPTED PASSWORD '1';
CREATE ROLE "Дизайнер типов" 	LOGIN NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE IN ROLE "TypeDesigner"	ENCRYPTED PASSWORD '1';
CREATE ROLE "Дизайнер объектов" LOGIN NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE IN ROLE "ObjDesigner"		ENCRYPTED PASSWORD '1';
CREATE ROLE "Пользователь" 	LOGIN NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE IN ROLE "User"		ENCRYPTED PASSWORD '1';
CREATE ROLE "Гость" 		LOGIN NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE IN ROLE "Guest"		ENCRYPTED PASSWORD '1';

TODO Рекомендации(настоятельные) по создании групп: 
CREATE ROLE "Подгруппа1"      NOLOGIN NOSUPERUSER NOINHERIT NOCREATEDB NOCREATEROLE;

*/
------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '--- Уничтожаем все группы и пользователей в нашей системе';
DECLARE @role_table,@auth_table;
SET @role_table = SELECT table_name FROM information_schema.tables WHERE table_name= 'wh_role';
SET @auth_table = SELECT table_name FROM information_schema.tables WHERE table_name= 'wh_auth_members';

IF (LINES(@role_table)>0 )
BEGIN
	ALTER TABLE wh_role DISABLE TRIGGER ALL;
	DELETE FROM wh_role;
END

IF (LINES(@auth_table)>0 )
BEGIN
	ALTER TABLE wh_auth_members DISABLE TRIGGER ALL;
	DELETE FROM wh_auth_members;
END


PRINT '';
PRINT '--- Уничтожаем все таблицы безопасности';
DROP TABLE IF EXISTS  wh_role CASCADE;
DROP TABLE IF EXISTS wh_auth_members CASCADE;

DROP FUNCTION IF EXISTS ftr_bd_wh_auth_members();
DROP FUNCTION IF EXISTS ftr_bd_wh_role();
DROP FUNCTION IF EXISTS ftr_bi_wh_auth_members();
DROP FUNCTION IF EXISTS ftr_bi_wh_role();
DROP FUNCTION IF EXISTS ftr_bu_wh_auth_members();
DROP FUNCTION IF EXISTS ftr_bu_wh_role();

DROP FUNCTION IF EXISTS whGrant_GroupToUser(IN _role NAME,IN _user NAME) ;
------------------------------------------------------------------------------------------------------------




------------------------------------------------------------------------------------------------------------
--wh_role 	- таблица ролей
------------------------------------------------------------------------------------------------------------
CREATE TABLE wh_role(
	id		SERIAL 	NOT NULL,		-- идентификатор пользователя -- TODO нельзя изменять(trigger+GRANT???)
	rolname		NAME 	NOT NULL,		-- имя пользователя 	-- TODO нельзя изменять(trigger+GRANT???)
	rolcanlogin 	boolean NOT NULL DEFAULT false,
	rolcreaterole 	boolean NOT NULL DEFAULT false,
--	rolcreatedb 	boolean NOT NULL,
	rolconnlimit 	integer NOT NULL DEFAULT -1,
	rolpassword 	text		 DEFAULT NULL,
	rolvaliduntil 	timestamp with time zone DEFAULT NULL,
	rolcomment	text		 DEFAULT NULL,
	
	CONSTRAINT pk_id_wh_group 		PRIMARY KEY (id),	-- уникальность идентификатора групп\пользователей
	CONSTRAINT uk_groupname_wh_group 	UNIQUE (rolname),	-- обеспечиваем уникальные имена групп\пользователей

	CONSTRAINT ck_group_nopass  		CHECK ( (rolcanlogin=false AND rolpassword IS NULL AND NOT rolcreaterole) OR  rolcanlogin=true ) --группа не нуждается в пароле
,CONSTRAINT ck_role_rolname CHECK (rolname ~ '^([[:alnum:][:space:]!()*+,-.:;<=>^_|№])+$')
);

CREATE INDEX ON wh_role USING btree (id,rolname);
CREATE INDEX ON wh_role USING btree (rolname,id);
--CREATE INDEX ON wh_role USING btree (rolname);

------------------------------------------------------------------------------------------------------------
--wh_auth_members - таблица членства ролей
------------------------------------------------------------------------------------------------------------
CREATE TABLE wh_auth_members
(
	roleid 		INTEGER NOT NULL,
	member 		INTEGER NOT NULL,

	CONSTRAINT fk_roleid_wh_auth_members	FOREIGN KEY (roleid)  	REFERENCES wh_role (id) 	MATCH FULL ON DELETE CASCADE ON UPDATE CASCADE NOT DEFERRABLE,
	CONSTRAINT fk_member_wh_auth_members	FOREIGN KEY (member)  	REFERENCES wh_role (id) 	MATCH FULL ON DELETE CASCADE ON UPDATE CASCADE NOT DEFERRABLE,

	CONSTRAINT uk_auth_members_role_member 	UNIQUE (roleid, member)	
);
CREATE UNIQUE INDEX wh_auth_members_member_role_index	ON wh_auth_members USING btree  (member, roleid);
--CREATE UNIQUE INDEX wg_auth_members_role_member_index   ON wh_auth_members USING btree  (roleid, member);--замена на уникальность uk_auth_members_role_member

------------------------------------------------------------------------------------------------------------
--ftr_bi_wh_role - тиггер создающий пользователей при всавке в таблицу пользователей
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ftr_bi_wh_role()  RETURNS trigger AS
$body$
BEGIN
	LOOP
		EXIT WHEN (SELECT id FROM wh_role WHERE id=NEW.id) IS NULL;  --!!!ограничить число переборов на случай полного заполнения
		SELECT nextval('wh_role_id_seq') INTO NEW.id;
		RAISE NOTICE 'searching id : NEW.id=%',NEW.id; -- отменяем попытку переноса
	END LOOP;

	RAISE NOTICE USING MESSAGE ='CREATE ROLE '||quote_ident(NEW.rolname)||' NOSUPERUSER INHERIT NOCREATEDB '
			||CASE WHEN NEW.rolcanlogin 	THEN ' LOGIN' 		ELSE ' NOLOGIN' END
			||CASE WHEN NEW.rolcreaterole 	THEN ' CREATEROLE' 	ELSE ' NOCREATEROLE' END
			||' CONNECTION LIMIT '|| NEW.rolconnlimit
			||CASE WHEN NEW.rolpassword IS NULL THEN ' '		ELSE ' ENCRYPTED PASSWORD '||quote_literal(NEW.rolpassword) END
			||CASE WHEN NEW.rolvaliduntil IS NULL THEN ' '		ELSE ' VALID UNTIL '||quote_literal(NEW.rolvaliduntil)END;
			
	EXECUTE 'CREATE ROLE '||quote_ident(NEW.rolname)||' NOSUPERUSER INHERIT NOCREATEDB '
			||CASE WHEN NEW.rolcanlogin 		THEN ' LOGIN' 		ELSE ' NOLOGIN' END
			||CASE WHEN NEW.rolcreaterole 		THEN ' CREATEROLE' 	ELSE ' NOCREATEROLE' END
			||' CONNECTION LIMIT '|| NEW.rolconnlimit
			||CASE WHEN NEW.rolpassword IS NULL 	THEN ' '		ELSE ' ENCRYPTED PASSWORD '||quote_literal(NEW.rolpassword) END
			||CASE WHEN NEW.rolvaliduntil IS NULL 	THEN ' '		ELSE ' VALID UNTIL '||quote_literal(NEW.rolvaliduntil)END;

	IF(NEW.rolcomment IS NOT NULL)
	THEN
		EXECUTE 'COMMENT ON ROLE '||quote_ident(NEW.rolname)||' IS '||quote_literal(NEW.rolcomment);
		RAISE NOTICE USING MESSAGE ='COMMENT ON ROLE '||quote_ident(NEW.rolname)||' IS '||quote_literal(NEW.rolcomment);
	END IF;

	RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bi_wh_role BEFORE INSERT ON wh_role FOR EACH ROW EXECUTE PROCEDURE ftr_bi_wh_role();
------------------------------------------------------------------------------------------------------------
--ftr_bd_wh_role - тиггер удаляющий пользователей при удалении в таблице пользователей
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ftr_bd_wh_role()  RETURNS trigger AS
$body$
BEGIN
	--BEGIN
		EXECUTE 'DROP ROLE IF EXISTS '||quote_ident(OLD.rolname);
		RAISE NOTICE USING MESSAGE ='DROP ROLE IF EXISTS '||quote_ident(OLD.rolname);
	--EXCEPTION
	--	WHEN object_in_use THEN RAISE NOTICE 'can not delete role % - object_in_use',OLD.rolname;
	--	WHEN dependent_objects_still_exist THEN RAISE NOTICE 'can not delete role % - dependent_objects_still_exist',OLD.rolname;
	--END;	

RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bd_wh_role BEFORE DELETE ON wh_role FOR EACH ROW EXECUTE PROCEDURE ftr_bd_wh_role();
------------------------------------------------------------------------------------------------------------
--ftr_bu_wh_role - тиггер обновляющий пользователей pjstgres при изменении в таблице пользователей
------------------------------------------------------------------------------------------------------------
-- TODO тригер на обновление данных о пользователе
CREATE OR REPLACE FUNCTION ftr_bu_wh_role()  RETURNS trigger AS
$body$
BEGIN
	IF NEW.id<>OLD.id 
		THEN RAISE EXCEPTION 'Невозможно изменить пользователя %, смена id недопустима!',OLD.rolname;  
		END IF;
	IF NEW.rolcanlogin<>OLD.rolcanlogin
		THEN RAISE EXCEPTION 'Невозможно изменить пользователя %, смена типа недопустима(пользователь/группа) недопустима!',OLD.rolname;  
		END IF;


	IF NEW.rolcreaterole<>OLD.rolcreaterole
		THEN EXECUTE 'ALTER ROLE '||quote_ident(OLD.rolname)|| CASE WHEN NEW.rolcreaterole THEN ' CREATEROLE' 	ELSE ' NOCREATEROLE' END;
		RAISE NOTICE USING MESSAGE = 'ALTER ROLE '||quote_ident(OLD.rolname)|| CASE WHEN NEW.rolcreaterole THEN ' CREATEROLE' 	ELSE ' NOCREATEROLE' END;
		END IF;
	IF NEW.rolconnlimit<>OLD.rolconnlimit
		THEN EXECUTE 'ALTER ROLE '||quote_ident(OLD.rolname)||' CONNECTION LIMIT '||NEW.rolconnlimit;
		END IF;
  IF NEW.rolpassword<>OLD.rolpassword THEN 
    EXECUTE 'ALTER ROLE '||quote_ident(OLD.rolname)||' ENCRYPTED PASSWORD '||quote_literal(NEW.rolpassword);
  END IF;
	IF NEW.rolvaliduntil<>OLD.rolvaliduntil
		THEN EXECUTE 'ALTER ROLE '||quote_ident(OLD.rolname)||' VALID UNTIL '||quote_literal(rolvaliduntil);
		END IF;
	IF NEW.rolcomment<>OLD.rolcomment
		THEN EXECUTE 'COMMENT ON ROLE '||quote_ident(OLD.rolname)||' IS '||quote_literal(NEW.rolcomment);
		END IF;
	IF NEW.rolname<>OLD.rolname
		THEN EXECUTE 'ALTER ROLE '||quote_ident(OLD.rolname)||' RENAME TO '||quote_ident(NEW.rolname);
		RAISE NOTICE USING MESSAGE = 'ALTER ROLE '||quote_ident(OLD.rolname)||' RENAME TO '||quote_ident(NEW.rolname);
		END IF;
	

  RAISE DEBUG 'UPDATE wh_role RECORD (%)', NEW;

RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bu_wh_role BEFORE UPDATE ON wh_role FOR EACH ROW EXECUTE PROCEDURE ftr_bu_wh_role();


------------------------------------------------------------------------------------------------------------
--ftr_bi_auth_members - тиггер добавляющий пользователя в определённую группу
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ftr_bi_wh_auth_members()  RETURNS trigger AS
$body$
DECLARE
	_role VARCHAR;
	_user VARCHAR;
BEGIN
	SELECT rolname INTO _role FROM wh_role WHERE wh_role.id=NEW.roleid;
	SELECT rolname INTO _user FROM wh_role WHERE wh_role.id=NEW.member;

	IF _role IS NOT NULL AND _user IS NOT NULL
	THEN 
		EXECUTE 'GRANT '||quote_ident(_role)||' TO '||quote_ident(_user);
	END IF;

	IF _role='Admin' 
	THEN
		UPDATE wh_role SET rolcreaterole=true WHERE wh_role.id= NEW.member;
	END IF;
	
RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bi_wh_auth_members BEFORE INSERT ON wh_auth_members FOR EACH ROW EXECUTE PROCEDURE ftr_bi_wh_auth_members();
------------------------------------------------------------------------------------------------------------
--ftr_bd_wh_role - тиггер удаляющий пользователей  пользователя с определённуой группы
------------------------------------------------------------------------------------------------------------
--REVOKE SELECT ON TABLE pg_authid FROM "Guest";
--GRANT SELECT(rolcreaterole,rolname) ON TABLE pg_authid  TO "Guest";
--GRANT INSERT,UPDATE,DELETE ON TABLE pg_authid  TO "Admin";


CREATE OR REPLACE FUNCTION ftr_bd_wh_auth_members()  RETURNS trigger AS
$body$
DECLARE
	_role VARCHAR;
	_user VARCHAR;
	_pg_createrole boolean;
BEGIN
	SELECT rolname INTO _role FROM wh_role WHERE wh_role.id=OLD.roleid;
	SELECT rolname INTO _user FROM wh_role WHERE wh_role.id=OLD.member;
	IF _role IS NOT NULL AND _user IS NOT NULL
		THEN EXECUTE 'REVOKE '||quote_ident(_role)||' FROM '||quote_ident(_user);
	END IF;

	--если пользователь админ и хочет дать права админу кому-то ещё проверяем в PG его принадлежность к админам
	SELECT rolcreaterole INTO _pg_createrole FROM pg_authid WHERE rolname=CURRENT_USER;
	IF _role='Admin' AND _pg_createrole IS TRUE
	THEN
		UPDATE wh_role SET rolcreaterole=false WHERE wh_role.id= OLD.member;
	END IF;

	
	RETURN OLD;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bd_wh_auth_members BEFORE DELETE ON wh_auth_members FOR EACH ROW EXECUTE PROCEDURE ftr_bd_wh_auth_members();
------------------------------------------------------------------------------------------------------------
--ftr_bd_wh_role - тиггер-пустышка для обновления таблицы членства
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION ftr_bu_wh_auth_members()  RETURNS trigger AS
$body$
BEGIN
	--RAISE EXCEPTION 'Невозможно изменить данные в wh_auth_members только вставка и удаление';
	RETURN NEW;
END;
$body$
LANGUAGE 'plpgsql';
CREATE TRIGGER tr_bu_wh_auth_members BEFORE UPDATE ON wh_auth_members FOR EACH ROW EXECUTE PROCEDURE ftr_bu_wh_auth_members();

------------------------------------------------------------------------------------------------------------
-- Добавляем пользователя в группу
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION whGrant_GroupToUser(IN _role NAME,IN _user NAME) 
	RETURNS void AS 
$body$	
DECLARE
  _role_id INTEGER;
  _user_id INTEGER;
BEGIN
  --RAISE NOTICE USING MESSAGE = 'IN_role='||_role||'  IN_user='||_user;
  SELECT id INTO _role_id FROM wh_role WHERE wh_role.rolname=_role;
  SELECT id INTO _user_id FROM wh_role WHERE wh_role.rolname=_user;

  PERFORM FROM wh_auth_members WHERE roleid=_role_id AND member=_user_id;
  IF NOT FOUND THEN
    IF _role_id IS NOT NULL AND _user_id IS NOT NULL THEN
      RAISE NOTICE 'Add User %[%] to group %[%]',_user,_user_id,_role,_role_id;
      INSERT INTO wh_auth_members(roleid,member)VALUES(_role_id,_user_id);
    END IF;
  ELSE
    RAISE NOTICE 'User % already in group %',_user,_role;
  END IF;

END;
$body$
LANGUAGE 'plpgsql';
------------------------------------------------------------------------------------------------------------
--Отображаем группы(NOLOGIN) в которы состоит пользователь */
------------------------------------------------------------------------------------------------------------
-- DROP VIEW wh_membership;
CREATE OR REPLACE VIEW wh_membership AS 
	SELECT 	usrgroup.id  		AS groupid,
		usrgroup.rolname 	AS groupname,
		wh_role.id		AS userid,
		wh_role.rolname 	AS username
FROM wh_role
RIGHT JOIN wh_auth_members membership
 ON membership.member=wh_role.id 
LEFT JOIN wh_role usrgroup
 ON membership.roleid = usrgroup.id;
--WHERE pg_authid.rolcanlogin; -- TODO 
------------------------------------------------------------------------------------------------------------
--Отображаем пользователей(LOGIN) */
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE VIEW wh_user AS
SELECT 	wh_role.id 	AS userid,
	wh_role.rolname AS username,
	wh_role.rolconnlimit,
	wh_role.rolvaliduntil,
	wh_role.rolcomment,
	wh_role.rolcreaterole,
      CASE WHEN 
      (
        SELECT rol.rolname
        FROM pg_authid
        LEFT JOIN pg_auth_members member ON pg_authid.oid = member.member 
        LEFT JOIN pg_authid rol ON member.roleid = rol.oid 
        WHERE pg_authid.rolname = CURRENT_USER AND rol.rolname = 'Admin'
      )IS NOT NULL THEN rolpassword ELSE NULL END 
    AS rolpassword --NULL::TEXT AS rolpassword
FROM wh_role
WHERE wh_role.rolcanlogin;
------------------------------------------------------------------------------------------------------------
--Отображаем группы(NOLOGIN) */
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE VIEW wh_group AS
SELECT 	wh_role.id 	AS groupid,
	wh_role.rolname AS groupname,
	wh_role.rolconnlimit,
	wh_role.rolvaliduntil,
	wh_role.rolcomment
FROM wh_role
WHERE NOT wh_role.rolcanlogin;
/**
 WHERE 	groname<>'Admin'
 AND   	groname<>'TypeDesigner'
 AND   	groname<>'ObjDesigner'
 AND   	groname<>'User'
 AND   	groname<>'Guest';
*/

------------------------------------------------------------------------------------------------------------
-- Обновляем всех пользователей из системных таблиц Postgres --> WH
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION PGtoWH()
  RETURNS void AS
$body$	
DECLARE
	rowvar RECORD;
BEGIN	
	ALTER TABLE wh_role DISABLE TRIGGER ALL;
	ALTER TABLE wh_auth_members DISABLE TRIGGER ALL;

	-- Копируем всех недостающих пользователей из системных таблиц Postgres --> WH
	INSERT INTO wh_role(rolname) SELECT rolname FROM pg_authid WHERE NOT EXISTS (select rolname FROM wh_role WHERE pg_authid.rolname=wh_role.rolname );

	-- Копируем все членства групп
	FOR rowvar IN SELECT pgrole.rolname AS pggroup,pgmember.rolname AS pguser
		FROM pg_auth_members
		LEFT JOIN pg_authid pgrole 	ON pgrole.oid=pg_auth_members.roleid 
		LEFT JOIN pg_authid pgmember 	ON pgmember.oid=pg_auth_members.member 
	LOOP
		PERFORM whGrant_GroupToUser(rowvar.pggroup,rowvar.pguser);
	END LOOP;

	-- обновляем существующих пользователей из системных таблиц Postgres --> WH 
	FOR rowvar IN 	SELECT pg_authid.*,pg_authid.oid,pg_catalog.shobj_description(oid, 'pg_authid') AS rolcomment,wh_role.id AS wh_role_id
			FROM wh_role,pg_authid WHERE pg_authid.rolname=wh_role.rolname 
	LOOP
		RAISE DEBUG 'Обновляем пользователя "%" [%] ', rowvar.rolname,rowvar.wh_role_id;
		IF rowvar.rolcanlogin IS FALSE THEN 
			rowvar.rolpassword=NULL; 
			rowvar.rolcreaterole=FALSE;
		END IF;
		
		UPDATE wh_role SET 	--rolname		= rowvar.rolname,
					rolcanlogin	= rowvar.rolcanlogin,
					rolcreaterole	= rowvar.rolcreaterole,
					rolconnlimit	= rowvar.rolconnlimit,
					rolpassword	= rowvar.rolpassword,
					rolvaliduntil	= rowvar.rolvaliduntil,
					rolcomment	= rowvar.rolcomment 
					WHERE rolname = rowvar.rolname;
	END LOOP;

	ALTER TABLE wh_role ENABLE TRIGGER ALL;
	ALTER TABLE wh_auth_members ENABLE TRIGGER ALL;
	


END;
$body$
LANGUAGE 'plpgsql';

--Копируем пользователей из Postgres --> WH
SELECT PGtoWH();

DROP FUNCTION PGtoWH();

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '--- Создание основных групп и раздача разрешений этим группам из моих таблиц';
PRINT '';
------------------------------------------------------------------------------------------------------------
CREATE OR REPLACE FUNCTION UpdateWhGroup(IN _role NAME,IN _comment TEXT)
  RETURNS void AS
$body$	
DECLARE
	rowvar RECORD;
	cancreaterole BOOLEAN;
BEGIN	
	SELECT * INTO rowvar FROM wh_role WHERE rolname = _role;
	IF(rowvar.rolname IS NOT NULL)
	THEN
		RAISE DEBUG '-Role % exist in wh_role - UPDATING',rowvar.rolname; 
		EXECUTE ('ALTER ROLE "'||_role||'" NOLOGIN NOSUPERUSER INHERIT NOCREATEDB NOCREATEROLE');

		UPDATE wh_role SET 	rolcanlogin=false,
					rolcreaterole=false,
					rolconnlimit=-1,
					rolpassword=NULL,
					rolvaliduntil=NULL,
					rolcomment=_comment
				WHERE rolname = _role;
			
	ELSE
        PERFORM FROM pg_authid WHERE rolname=_role; 
        IF NOT FOUND THEN
          RAISE NOTICE '-Role don`t exist in wh_role INSERTING'; 
          INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole,rolcomment)
            VALUES(_role,false,false,_comment);
        ELSE
          UPDATE wh_role SET rolcanlogin=false,
                             rolcreaterole=false,
                             rolconnlimit=-1,
                             rolpassword=NULL,
                             rolvaliduntil=NULL,
                             rolcomment=_comment
                             WHERE rolname = _role;
        END IF;
    END IF;


	EXECUTE 'REVOKE ALL PRIVILEGES ON ALL TABLES IN SCHEMA PUBLIC FROM GROUP '||quote_ident(_role);
	EXECUTE 'REVOKE ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA PUBLIC  FROM GROUP '||quote_ident(_role);
	EXECUTE 'REVOKE ALL PRIVILEGES ON ALL FUNCTIONS IN SCHEMA PUBLIC  FROM GROUP '||quote_ident(_role);
	

	
END;
$body$
LANGUAGE 'plpgsql';


/** Группа "Guest" - гость, позволяет просматривать все типы, объекты и правила */
SELECT UpdateWhGroup('Guest','встроенная группа гостей, позволяет просматривать все типы, объекты и правила');

/** Группа пользователей, позволяет перемещать объекты*/
SELECT UpdateWhGroup('User','встроенная группа пользователей, позволяет перемещать объекты');
GRANT "Guest" TO "User";

/** Группа дизайнера объектов, позволяет создавать\удалять\редактировать объекты*/
SELECT UpdateWhGroup('ObjDesigner','встроенная группа дизайнеров объектов, позволяет создавать,удалять,редактировать объекты');
GRANT "User" TO "ObjDesigner";

/** Группа дизайнера типов, позволяет создавать\удалять\редактировать типы и правила*/
SELECT UpdateWhGroup('TypeDesigner','встроенная группа дизайнеров типов, позволяет создавать,удалять,редактировать типы и правила');
GRANT "ObjDesigner" TO "TypeDesigner";

/** Группа администраторов позволяет создавать группы и пользователей*/
SELECT UpdateWhGroup('Admin','встроенная группа администраторов позволяет создавать группы и пользователей');
GRANT "TypeDesigner" TO "Admin";

GRANT SELECT(id,rolname,rolcanlogin,rolcreaterole,rolconnlimit,rolvaliduntil,rolcomment) ON TABLE wh_role TO "Guest";
GRANT SELECT ON TABLE wh_auth_members  	TO "Guest";
GRANT SELECT ON TABLE wh_group  	TO "Guest";
GRANT SELECT ON TABLE wh_user  		TO "Guest";
GRANT SELECT ON TABLE wh_membership	TO "Guest";

--Позволяет добавлять,удалять,редактировать пользователей
GRANT SELECT, UPDATE, INSERT, DELETE ON TABLE wh_role         TO "Admin" WITH GRANT OPTION;
-- Позволяет добавлять,удалять пользователей в группы
GRANT INSERT,DELETE                  ON TABLE wh_auth_members TO "Admin";
-- Позволяет читать,инкрементировать текущее значение очереди
GRANT USAGE                          ON TABLE wh_role_id_seq  TO "Admin"; 
GRANT EXECUTE ON FUNCTION whgrant_grouptouser(name, name) TO "Admin" WITH GRANT OPTION;

DROP FUNCTION UpdateWhGroup(IN _role NAME,IN _comment TEXT);

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '+ Тесты функциональности системы безопасности';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole, rolconnlimit, rolpassword, rolvaliduntil)	VALUES('test1',true,true,10,'pass','11.11.30');
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole, rolconnlimit, rolpassword )			VALUES('test2',true,true,10,'pass');
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole, rolconnlimit  )					VALUES('test3',true,true,10);
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole  )						VALUES('test4',true,true);
INSERT INTO wh_role(rolname,rolcanlogin  )								VALUES('test5',true);
INSERT INTO wh_role(rolname)										VALUES('группа6');
SELECT whgrant_grouptouser('группа6','test1' );
UPDATE wh_role SET rolname='test11'				WHERE rolname = 'test1';
UPDATE wh_role SET rolname='test111',rolcreaterole=false	WHERE rolname = 'test11';
UPDATE wh_role SET rolname='test1'				WHERE rolname = 'test111';

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Тесты ограничений системы безопасности';
PRINT '';
------------------------------------------------------------------------------------------------------------
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole, rolconnlimit, rolpassword )			VALUES('ошибка: группа не должна иметь пароль',false,false,10,'pass');
INSERT INTO wh_role(rolname,rolcanlogin, rolcreaterole  )						VALUES('ошибка: группа не может создавать пользователей',false,true);
INSERT INTO wh_role(rolname)										VALUES('test2'); --ошибка уникальности имени роли
UPDATE wh_role SET rolcanlogin= false WHERE rolname = 'test5';

------------------------------------------------------------------------------------------------------------
PRINT '';
PRINT '- Удаление тестовых данных системы безопасности';
------------------------------------------------------------------------------------------------------------
DELETE FROM wh_role WHERE rolname = 'test1';
DELETE FROM wh_role WHERE rolname = 'test2';
DELETE FROM wh_role WHERE rolname = 'test3';
DELETE FROM wh_role WHERE rolname = 'test4';
DELETE FROM wh_role WHERE rolname = 'test5';
DELETE FROM wh_role WHERE rolname = 'группа6';






