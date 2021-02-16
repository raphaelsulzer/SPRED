#!/usr/bin/bash

echo "*****************************************************************"
echo "***********    Segment and collapse a mesh **********************"
echo "*****************************************************************"

data_folder="/user/dsalinas/home/Documents/Recherche/Code/data"
simplified_data_folder="/user/dsalinas/home/Documents/Recherche/Code/data/simplified/output"

build_folder="/user/dsalinas/home/Documents/CodeSVN/urban/build"

#./bin/Release/region_growing_detection $1                  $2      $3  $4 $5 $6 $7
#./bin/Release/region_growing_detection paris_big_50000.off test.seg 0 .12 10 20 .2
function run_segmentation {
	

#make -j -C ${build_folder}
echo ""
echo ""
echo "-----------------------------------------------------------------"
echo "Run segmentation"
echo "-----------------------------------------------------------------"
echo ""
cmd="${build_folder}/bin/Release/./region_growing_detection ${data_folder}/$1 ${simplified_data_folder}/$2 $3 $4 $5 $6 $7"
echo $cmd
$cmd
echo "-----------------------------------------------------------------"
}

#./bin/Release/urban_collapse $1               $2           $3               $4 $5  $6
#./bin/Release/urban_collapse lans_church.off lans_70.off segmented_mesh.seg .01 0.8 0.8
function run_collapse {
echo ""
echo ""
echo "-----------------------------------------------------------------"
echo "Run collapse"
echo "-----------------------------------------------------------------"
echo ""
cmd="${build_folder}/bin/Release/./urban_collapse 
	${data_folder}/$1 ${simplified_data_folder}/$2 
	${simplified_data_folder}/$3 
	$4 
	$5 "
echo $cmd
$cmd
echo "-----------------------------------------------------------------"
}

#new
# urban_collapse $1        $2 $3            $4         $5 
# urban_collapse input.off N detect.param reg.param contr.param

#old
# urban_collapse $1        $2 $3      $4    $5     $6         $7   $8           $9
# urban_collapse input.off N epsilon alpha area coplan_alpha dist abstr_weight bdry_weight
function urban_collapse {
#make -j -C ${build_folder}

#computed segmented namefile for instance for $1=lans.off
#we compute lans_$4_$5_$6_$7.seg
suffix=".off"
segmented_namefile=${1%$suffix}
segmented_namefile="${segmented_namefile}.seg"

output_namefile=${1%$suffix}
output_namefile="${output_namefile}_$2.off"

run_segmentation $1 ${segmented_namefile} $3 $4

run_collapse $1 ${output_namefile} ${segmented_namefile} $2 $5

echo ${segmented_namefile}
echo ${output_namefile}
}




#new
# urban_collapse $1        $2 $3            $4         $5 
# urban_collapse input.off k detect.param reg.param contr.param


# Given a mesh with k proxies detected, 
# collapse until the number of vertices drop below 
#    ratio * k
# For instance, try k=5
# urban_collapse $1        $2 $3      $4    $5     $6         $7   $8           $9
# urban_collapse input.off ratio epsilon alpha area coplan_alpha dist abstr_weight bdry_weight
function urban_collapse_with_proxy_ratio {
#make -j -C ${build_folder}

#computed segmented namefile for instance for $1=lans.off
#we compute lans_$4_$5_$6_$7.seg
suffix=".off"
segmented_namefile=${1%$suffix}
segmented_namefile="${segmented_namefile}.seg"


#if file not there
run_segmentation $1 ${segmented_namefile} $3 $4


#compute the number of collapses


##### Choice 1 ############################
# the number of collapses is the number of proxies times 
# a ratio which is $2
###########################################
# the number of proxy is the number of lines
#num_lines=($(cat ${simplified_data_folder}/${segmented_namefile} | wc -l))
#num_collapses="$(echo "$num_lines*$2" | bc)"
#echo $num_collapses

##### Choice 2 ############################
# the number of collapses is the number of 
# points of input.off ($1) times a ratio ($2)
# in this case $2 should be in )0,1(
###########################################
# the number of proxy is the number of lines
num_lines=($(sed -n '2p' ${data_folder}/${1} | awk '{ print $1 }'))
num_collapses="$(echo "$num_lines*$2" | bc)"
num_collapses=${num_collapses%.*} 
echo $num_collapses


output_namefile=${1%$suffix}
output_namefile="${output_namefile}_${num_collapses}.off"

run_collapse $1 ${output_namefile} ${segmented_namefile} $num_collapses $5

echo ${segmented_namefile}
echo ${output_namefile}
}






################################" main 
# ex :
# bash urban_collapse input.off N epsilon alpha area coplan_alpha dist abstr_weight bdry_weight
#   N ratio number of collapses
#   epsilon : tolerance paremeter for the region growing (currently not used)
#	alpha : max angle for the region growing in (0,90)
#	area : min area for the region growing in R+ (m)
#	coplan_alpha : max angle to merge two proxies in (0,90)
#	dist : max ratio dist to merge two proxies in R+
#   abstr_weight,bdry_weight : abstraction and bdry weight in (0,1)

#dans ce cas l'argument doit etre une politique et le second le fichier ou all
if [ $# -eq 5 ]; then
#urban_collapse $1 $2 $3 $4 $5 $6 $7 $8 $9
urban_collapse_with_proxy_ratio $1 $2 $3 $4 $5
exit 0 
else
	echo $#
	echo "usage bash $0 input.off N epsilon alpha area coplan_alpha dist abstr_weight bdry_weight\n
		N number of collapses \n
		epsilon : tolerance paremeter for the region growing (currently not used) \n
		alpha : max angle for the region growing in (0,90) \n
		area : min area for the region growing in R+ (m) \n
		coplan_alpha : max angle to merge two proxies in (0,90) \n
		dist : max ratio dist to merge two proxies in R+ \n
		abstr_weight,bdry_weight : abstraction and bdry weight in (0,1)\n"	
fi


