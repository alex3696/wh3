CLS
ECHO OFF
CHCP 1251
rem schtasks /create /tn "��������� �����������" /tr "c:\db\backup.bat" /sc ��������� /st 06:00:00
SCHTASKS /Create /RU SYSTEM /SC DAILY /TN "��������� �����������" /TR "c:\db\backup.bat" /ST 06:00:00
IF NOT %ERRORLEVEL%==0 MSG * "������ ��� �������� ������ ���������� �����������."