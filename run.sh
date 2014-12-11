source comp.sh
for alg in {"FCFS","BACKFILL","SPIRAL"}
do

echo "==========$alg========="
g++ -D $alg Batch.cpp Proc.h Proc.cpp Slot.h Slot.cpp Event.h Event.cpp  -o batch

for name in {"atlas 1000 8696",  "ctc 1000 308", "cea 1000 64", "das 1000 64", "inteldshort","nasa 1000 128" }
do

echo $name
rslt=$(./batch $name)
echo $rslt
echo "$rslt" >> "rslt.txt"
done
done
