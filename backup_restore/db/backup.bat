REM ПРИМЕР СОЗДАНИЯ РЕЗЕРВНОЙ КОПИИ БАЗЫ ДАННЫХ POSTGRESQL
CLS
ECHO OFF
CHCP 1251

REM Установка переменных окружения
rem http://www.postgresql.org/docs/9.2/static/libpq-envars.html
SET PGBIN=c:\_SAV\prog\PostgreSQL\9.5\bin\
SET PGDATABASE=wh3
SET PGHOST=localhost
SET PGPORT=5432
SET PGUSER=postgres
SET PGPASSWORD=1

REM Смена диска и переход в папку из которой запущен bat-файл
%~d0
CD %~dp0

REM Формирование имени файла резервной копии и файла-отчета
SET DATETIME=%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2% %TIME:~0,2%-%TIME:~3,2%-%TIME:~6,2%
SET DUMPFILE=%PGDATABASE% %DATETIME%.backup
SET LOGFILE=%PGDATABASE% %DATETIME%.log
SET DUMPPATH="Backup\%DUMPFILE%"
SET LOGPATH="Backup\%LOGFILE%"

SET ROLEDUMPFILE=%PGDATABASE% %DATETIME% roles.sql
SET ROLEPATH="Backup\%ROLEDUMPFILE%"

SET DUMPALLFILE=%PGDATABASE% %DATETIME% all.sql
SET DUMPALLPATH="Backup\%DUMPALLFILE%"


call "%PGBIN%\pg_dumpall" --roles-only -f %ROLEPATH% 2>%LOGPATH%
call "%PGBIN%\pg_dumpall" -v -f %DUMPALLPATH% 2>%LOGPATH%

REM Создание резервной копии
IF NOT EXIST Backup MD Backup
CALL "%PGBIN%\pg_dump.exe" --format=custom --verbose --file=%DUMPPATH% 2>%LOGPATH%

REM Анализ кода завершения
IF NOT %ERRORLEVEL%==0 GOTO Error
GOTO Successfull

REM В случае ошибки удаляется поврежденная резервная копия и делается соответствующая запись в журнале
:Error
DEL %DUMPPATH%
MSG * "Ошибка при создании резервной копии базы данных. Смотрите backup.log."
ECHO %DATETIME% Ошибки при создании резервной копии базы данных %DUMPFILE%. Смотрите отчет %LOGFILE%. >> backup.log
GOTO End

REM В случае удачного резервного копирования просто делается запись в журнал
:Successfull
ECHO %DATETIME% Успешное создание резервной копии %DUMPFILE% >> backup.log
GOTO End

:End