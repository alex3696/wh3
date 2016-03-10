CLS
ECHO OFF
CHCP 1251
rem schtasks /create /tn "Резервное копирование" /tr "c:\db\backup.bat" /sc ежедневно /st 06:00:00
SCHTASKS /Create /RU SYSTEM /SC DAILY /TN "Резервное копирование" /TR "c:\db\backup.bat" /ST 06:00:00
IF NOT %ERRORLEVEL%==0 MSG * "Ошибка при создании задачи резервного копирования."