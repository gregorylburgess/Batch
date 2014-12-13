numjobs=8000
runs=20

for name in  "ctc.txt" "cea.txt" "das.txt" "sdsc.txt" "nasa.txt"
do
echo "==========$name=========="
echo "==========$name==========" >>"rslt.txt"
let ts=0
let as=0
let ms=0
let trt=0
let np=0

for alg in "FCFS" "BACKFILL" "SPIRAL" "EASY" "SPT" "LPT"
do
echo "$alg"
g++ -D $alg -D SILENT -O4 Batch.cpp Proc.h Proc.cpp Slot.h Slot.cpp Event.h Event.cpp  -o batch
rslt=$(./batch test/$name $numjobs)
echo $rslt
arrIN=(${rslt// / })
#get stuff

for x in $arrIN
do
	echo "> [$x]"
done
np=${arrIN[0]}
ts=${arrIN[3]}
as=${arrIN[6]}
ms=${arrIN[9]}
trt=${arrIN[13]}

out="$np,$alg,$ts,$as,$ms,$trt"
echo "$out"
echo "$out" >> "rslt.txt"
done

#Random 20x
alg="RANDOM"
echo "$alg"
echo "$alg" >> "rslt.txt"
g++ -D $alg -D SILENT -O4 Batch.cpp Proc.h Proc.cpp Slot.h Slot.cpp Event.h Event.cpp  -o batch
#stats for random
let ts=0
let as=0
let ms=0
let trt=0

for j in `seq $runs`
do
#run random
echo $j
rslt=$(./batch test/$name $numjobs)
arrIN=$(echo $rslt | tr ":" "\n")
#sum stuff
ts=`echo $ts+${arrIN[3]} | bc -l`
as=`echo $as+${arrIN[6]} | bc -l`
ms=`echo $ms+${arrIN[9]} | bc -l`
trt=`echo $trt+${arrIN[13]} | bc -l`
done

# average
ts=`echo $ts/$runs | bc -l`
as=`echo $as/$runs | bc -l`
ms=`echo $ms/$runs | bc -l`
trt=`echo $trt/$runs | bc -l`

out=",$np$alg,$ts,$as,$ms,$trt"
echo "$out"
echo "$out" >> "rslt.txt"

done
