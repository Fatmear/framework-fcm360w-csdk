#! /bin/bash

export Start_at=`date +%H:%M:%S`
export PROJECT_ROOT=$PWD
export curtitle="cmd:"$0
export ret="0"
export def_proj="FCM360W"

doneseg(){
	echo " "
	date +%Y/%m/%d 
	echo "START TIME: $Start_at"
	echo "END TIME: `date +%H:%M:%S`"
	exit $ret
}

helpinfo(){
	echo " "
	echo "Usage: $0 r/new Project Version [debug/release]"
	echo "       $0 clean"
	echo "       $0 h/-h"
	echo "Example:"
	echo "       $0 new $def_proj your_firmware_version"
	echo " "
	echo "Supported projects include but are not limited to:"
	echo "       FCM360W"
	echo " "
	echo If you have any question, please contact Quectel.
	echo " "
	echo " "
	exit $ret
}

cleanall(){
	if [ -d "$1" ]; then
		rm -rf $1 > /dev/null
		echo "clean \"$1\" done"
	fi
	if [ -d "$2" ]; then
		rm -rf $2 > /dev/null
		echo "clean \"$2\" done"
	fi
	if [ -d "$3" ]; then
		rm -rf $3 > /dev/null
		echo "clean \"$3\" done"
	fi
}

export build_oper=$1
if test "$build_oper" = ""; then
	oper=new
elif test "$build_oper" = "r"; then
	oper=r
elif test "$build_oper" = "new"; then
	oper=new
elif test "$build_oper" = "n"; then
	oper=new
elif test "$build_oper" = "clean"; then
	oper=clean
elif test "$build_oper" = "c"; then
	oper=clean
elif test "$build_oper" = "h"; then
	oper=h
elif test "$build_oper" = "-h"; then
	oper=h
elif test "$build_oper" = "help"; then
	oper=h
elif test "$build_oper" = "/h"; then
	oper=h
elif test "$build_oper" = "/?"; then
	oper=h
elif test "$build_oper" = "?"; then
	oper=h
else 
	echo " "
	echo "ERR: unknown build operation: $build_oper, should be r/n/new/clean/h/-h"
	echo " "
	export ret=1
	helpinfo	
fi

if test "$oper" = "h"; then
	helpinfo
fi

export ql_root_dir=$(pwd)
if test "$oper" = "clean"; then
	cleanall $ql_root_dir/ql_build $ql_root_dir/ql_out
	doneseg
fi

if test "$2" = ""; then
	echo " "
	echo "We need the Project to build the firmware..."
	echo " "
	export ret=1
	helpinfo
fi
export buildproj=$2
#if test "$buildproj" = ""; then
#	buildproj=$def_proj
#fi
buildproj=${buildproj^^}

if test "$3" = ""; then
	echo " "
	echo "We need the Version to build the firmware..."
	echo " "
	export ret=1
	helpinfo
fi

export buildver=$3
buildver=${buildver^^}
export ql_app_ver=${buildver}_APP

export ql_tool_dir=$ql_root_dir/ql_tools
export ql_gcc_name=nds32le-elf-mculib-v3s
if [ ! -d $ql_tool_dir/$ql_gcc_name ];then 
   tar -Jvxf $ql_tool_dir/$ql_gcc_name.txz -C $ql_tool_dir
fi

if test "$oper" = "new"; then
	cleanall $ql_root_dir/ql_build $ql_root_dir/ql_out
fi

export GCC_PATH=$ql_tool_dir/$ql_gcc_name/bin/

cd $ql_root_dir/ql_kernel/Boards/ecr6600/standalone/

#make all
./make_ota_diff.sh $3

# according to the build return to see success or not
if [ $? != 0 ]; then
	echo ""
	echo "xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx"
	echo "            Firmware building is ERROR!             "
	echo "xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx"
	echo ""
	export ret=1
	cd $ql_root_dir
	doneseg
fi

cd $ql_root_dir

if [ ! -d "ql_out" ]; then
    mkdir ql_out
fi

if [ ! -f "$ql_root_dir/ql_build/standalone/ECR6600F_standalone_allinone.bin" ]; then
	echo ""
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo "           Firmware generation is ERROR!          "
	echo xxxxxxxxxxxxxxxxxx   ERROR   xxxxxxxxxxxxxxxxxxxxxxx
	echo ""
	export ret=1
	cd $ql_root_dir
	doneseg
fi

if [ ! -d "ql_out/debug" ]; then
    mkdir ql_out/debug
fi
cp $ql_root_dir/ql_build/standalone/standalone.map $ql_root_dir/ql_out/debug/standalone.map
cp $ql_root_dir/ql_build/standalone/standalone.lst $ql_root_dir/ql_out/debug/standalone.lst
cp $ql_root_dir/ql_build/standalone/standalone.elf $ql_root_dir/ql_out/debug/standalone.elf
cp $ql_root_dir/ql_build/standalone/standalone.nm $ql_root_dir/ql_out/debug/standalone.nm
cp $ql_root_dir/ql_build/standalone/ECR6600F_standalone_allinone.bin $ql_root_dir/ql_out/ECR6600F_standalone_allinone.bin
cp $ql_root_dir/ql_build/standalone/ECR6600F_standalone_cpu_0x7000.bin $ql_root_dir/ql_out/ECR6600F_standalone_cpu_0x7000.bin

python3 $ql_tool_dir/codesize.py --map $ql_root_dir/ql_out/debug/standalone.map --outobj ql_out/debug/outobj.csv --outlib ql_out/debug/outlib.csv  --outsect ql_out/debug/outsect.csv

echo ""
echo "********************        PASS         ***********************"
echo "              Firmware package is ready for you.                "
echo "********************        PASS         ***********************"
echo ""

doneseg

