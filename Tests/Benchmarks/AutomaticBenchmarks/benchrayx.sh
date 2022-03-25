# Benchmark script to test RAY-X Execution Time.
# To run use chmod or bash or sh

# HELP
if [ "$1" == "-h" ]; then
  echo "Benchmark script to test RAY-X Execution time."
  echo "Usage: `basename $0` filename.rml number_of_trials"
  echo "	-h Print this message"
  echo "args:"
  echo "filename.rml: RML File name, make sure that it's in TerminalApp directory."
  echo "number_of_trials: Loop number of RAY-X, the bigger the more precise."
  
  exit 0
fi

#SQRT MATH FUNC	
function _sqrt(){
	local x=$(echo "sqrt($1)" | bc)
	return $x
}

#BENCHMARK
RAYX=./../../../build/bin/TerminalApp 

echo "Running RAY-X Benchmark..."
if [ -f "$RAYX" ]; then

# if file exist the it will be printed 
echo "RAY-X Terminal APP found."
else

# is it is not exist then it will be printed
echo "RAY-X Terminal APP not found, make sure already built!"
exit 1
fi

echo "=============================================="

file="output"
words=()
total_time=0
total_time_rms=0
trials=$2

#Loop
for (( i=0; i<= trials;i++ ));do
	#Run RAYX .. Change path here if needed
	$RAYX -b -i ./../../../build/bin/$1 > output 
	
	#TREAT OUTPUT
	while read -r line; do
		if [[ $line == *"Benchmark:"* ]]; then	
		echo -e "$line"
		number=`echo $line | awk '{print $(NF-1)}'`
		total_time=$((total_time + number))
		total_time_rms=$((total_time_rms + number ** 2 ))
		fi
	done <$file
done

#Summary
echo "=============================================="
echo "Average execution time: " $((total_time / (trials+1))) " ms"

total_time_rms=$((total_time_rms / (trials+1)))
echo "RMS: " $(echo "sqrt($total_time_rms)" | bc) " ms"

rm output


