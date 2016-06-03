#! /bin/bash
clear

datas=(tsukuba)

datadir='./eval/data'
params='./include/params.h'
alg_exe='./a.out'
disp2pfm_exe='./eval/disp2pfm'
eval_exe='./eval/eval.out'
threshold='1'

if [ $# -gt 0 ]; then
	if [ $1 -gt 0 ]; then
		rm -f $alg_exe sift_match.o
		make
		$alg_exe
		exit 0
	fi
fi

if [ ! -f $eval_exe ]; then
	cd eval/
	g++ -o eval.out eval.cc `pkg-config opencv --libs --cflags`
	cd ..
fi

if [ ! -d $datadir ]; then
	datatar='./eval/data.tar.gz'
	if [ ! -f $datatar ]; then
		echo 'no data'
		exit 1
	fi
	tar -zxvf $datatar -C './eval/'
	clear
fi

rm -rf core
ulimit -c unlimited

for data in ${datas[@]}
do
	data=$datadir/$data
	
	# 图像信息文件
	iminfo=$data/iminfo.txt
	width=`awk -F= '/width/ {print $2}' $iminfo`
	height=`awk -F= '/height/ {print $2}' $iminfo`
	ndispar=`awk -F= '/ndispar/ {print $2}' $iminfo`
	
	rm -f $params
	echo '#ifndef STEREOV_PARAMS_H' > $params
	echo '#define STEREOV_PARAMS_H' >> $params
	echo "#define WIDTH $width" >> $params
	echo "#define HEIGHT $height" >> $params
	echo "#define DISPARNUM $ndispar" >> $params
	if [ $# -gt 0 ]; then
		echo "#define SIFTMATCH 1" >> $params
	fi
	echo "#endif" >> $params
	
	rm -f $alg_exe
	cmd_ret=`make`
	rm -f *.o
	if [ ! -f $alg_exe ]; then
		echo $cmd_ret
		exit 1
	fi
	
	rm -rf ret && mkdir ret && cp $data/imL.png ret/imL.png
	$alg_exe $data/imL.png $data/imR.png $data/dispar_ret.png ret/ret.png
	if [ $# -gt 0 ]; then
		exit 0
	fi
	$eval_exe $data/dispar_ret.png $data/dispar_truth.png $data/nonocc.png
	rm -f $data/dispar_ret.png
done