del 1.log
psql --dbname=wh3 --username=postgres --set ON_ERROR_STOP=1 --set AUTOCOMMIT=off -L 1.log -f %1