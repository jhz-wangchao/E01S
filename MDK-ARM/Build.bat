@Echo off
::下列变量是KEIL的安装路径
set KEIL_DIR=C:\Keil_v5
::下列变量是工程文件的名称
set ProjectName=E01SMB
::下列变量为编译物的相对路径
set OutputDir=%ProjectName%

::以下内容请勿改动
Echo 开始编译%ProjectName%工程，即将打开Keil环境
%KEIL_DIR%\UV4\UV4 -b %ProjectName%.uvprojx -o output.txt
Echo 编译完成，正在生成%ProjectName%.bin
%KEIL_DIR%\ARM\ARMCC\bin\fromelf --bin --output %ProjectName%.bin .\%OutputDir%\%ProjectName%.axf

set KEIL_DIR=
set ProjectName=
set OutputDir=
::编译结束后的暂停
Echo 完成！按任意键退出
pause
