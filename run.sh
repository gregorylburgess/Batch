numjobs=3000;
for name in  "ctc.txt $numjobs 308" "cea.txt $numjobs 45824" "das.txt $numjobs 64" "nasa.txt $numjobs 128 test.txt $numjobs"
do
echo "==========$name=========="
echo "==========$name==========" >>"rslt.txt"

for alg in "FCFS" "BACKFILL" "SPIRAL"
do
echo "$alg"
echo "$alg" >> "rslt.txt"
g++ -D $alg -D SILENT -O4 Batch.cpp Proc.h Proc.cpp Slot.h Slot.cpp Event.h Event.cpp  -o batch
rslt=$(./batch test/$name)
echo $rslt
echo "$rslt" >> "rslt.txt"
done
done
