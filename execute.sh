#!/bin/bash

#compile programs
cd src
g++ -O3 -o TSP_GA_par TSP_GA_par.cpp -pthread -DFILEMATRIX
g++ -O3 -o TSP_GA_ff TSP_GA_ff.cpp -pthread -I /usr/local/fastflow/ -DFILEMATRIX
g++ -O3 -o TSP_GA_seq TSP_GA_seq.cpp -DFILEMATRIX

echo "Running sequential code"

file=../tests_tsplib95/u2152.tsp

seq=0
par1=0
par=0

for chromosome in 2048; do
	sum=0
	for i in {1..8}; do
    echo $i
  	time=$(./TSP_GA_seq 123 ${chromosome} 30 30 70 ${file})
  	sum="$( bc <<<"$sum + $time" )"
  done
  echo "results SEQ for: " ${file} ${chromosome} 
  seq=$(bc <<< "scale=2 ; $sum / 8")
  echo $seq
  echo ""
done

echo "End Seq."

echo ""

echo "Running parallel ST code"

blocks_sp=()
blocks_sc=()

for chromosome in 2048; do
  sum=0
  for i in {1..8}; do
    echo $i
    time=$(./TSP_GA_par 1 123 ${chromosome} 30 30 70 ${file}) 
    sum="$( bc <<<"$sum + $time" )"
  done
  echo "results PAR for: " 1 ${file} ${chromosome} 
  par1=$(bc <<< "scale=2 ; $sum / 8")
  speedup=$(bc <<< "scale=4 ; $seq / $par1")
  scalability=$(bc <<< "scale=4 ; $par1 / $par1")
  echo $par1
  blocks_sp+=("$speedup")
  blocks_sc+=("$scalability")
  echo ""
done

for chromosome in 2048; do
	for nw in 2 4 8 16 32 64 128; do
    echo $i
	  sum=0
		for i in {1..8}; do
      echo $i
  		time=$(./TSP_GA_par ${nw} 123 ${chromosome} 30 30 70 ${file}) 
  		sum="$( bc <<<"$sum + $time" )"
  	done
  	echo "results PAR for: " ${nw} ${file} ${chromosome} 
  	par=$(bc <<< "scale=2 ; $sum / 8")
    speedup=$(bc <<< "scale=4 ; $seq / $par")
    scalability=$(bc <<< "scale=4 ; $par1 / $par")
    echo $par
    blocks_sp+=("$speedup")
    blocks_sc+=("$scalability")
    echo ""
  done
done

echo "End parallel ST."

echo ""

echo "speedup=[${blocks_sp[@]}]"
echo "scalability=[${blocks_sc[@]}]"

echo ""

echo "Running parallel FastFlow code"

blocks_sp=()
blocks_sc=()

for chromosome in 2048; do
  sum=0
  for i in {1..8}; do
    echo $i
    time=$(./TSP_GA_ff 1 123 ${chromosome} 30 30 70 ${file}) 
    sum="$( bc <<<"$sum + $time" )"
  done
  echo "results PAR for: " 1 ${file} ${chromosome} 
  par1=$(bc <<< "scale=2 ; $sum / 8")
  speedup=$(bc <<< "scale=4 ; $seq / $par1")
  scalability=$(bc <<< "scale=4 ; $par1 / $par1")
  echo $par1
  blocks_sp+=("$speedup")
  blocks_sc+=("$scalability")
  echo ""
done

for chromosome in 2048; do
  for nw in 2 4 8 16 32 64 128; do
    sum=0
    for i in {1..8}; do
      echo $i
      time=$(./TSP_GA_ff ${nw} 123 ${chromosome} 30 30 70 ${file}) 
      sum="$( bc <<<"$sum + $time" )"
    done
    echo "results PAR for: " ${nw} ${file} ${chromosome} 
    par=$(bc <<< "scale=2 ; $sum / 8")
    speedup=$(bc <<< "scale=4 ; $seq / $par")
    scalability=$(bc <<< "scale=4 ; $par1 / $par")
    echo $par
    blocks_sp+=("$speedup")
    blocks_sc+=("$scalability")
    echo ""
  done
done

echo "End parallel FF."

echo ""

echo "speedup=[${blocks_sp[@]}]"
echo "scalability=[${blocks_sc[@]}]"