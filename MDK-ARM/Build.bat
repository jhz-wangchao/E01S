@Echo off
::���б�����KEIL�İ�װ·��
set KEIL_DIR=C:\Keil_v5
::���б����ǹ����ļ�������
set ProjectName=E01SMB
::���б���Ϊ����������·��
set OutputDir=%ProjectName%

::������������Ķ�
Echo ��ʼ����%ProjectName%���̣�������Keil����
%KEIL_DIR%\UV4\UV4 -b %ProjectName%.uvprojx -o output.txt
Echo ������ɣ���������%ProjectName%.bin
%KEIL_DIR%\ARM\ARMCC\bin\fromelf --bin --output %ProjectName%.bin .\%OutputDir%\%ProjectName%.axf

set KEIL_DIR=
set ProjectName=
set OutputDir=
::������������ͣ
Echo ��ɣ���������˳�
pause
