工具：trstool
用途：烧录
版本号：1.3.3
1.进入目录code/standalone/freertos/scripts/bin/trstool，替换掉旧版本trstool
（通过命令行输入：./trstool version 来查看工具版本）
2.执行命令，开始烧录
sudo ./trstool flash -d /dev/ttyUSB0 -f ../../../build/standalone/ECR6600F_standalone_allinone.bin