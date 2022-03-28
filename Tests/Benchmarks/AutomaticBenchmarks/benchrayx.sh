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
total_time_sum=0
total_time_sumSq=0
trace_time_sum=0
trace_time_sumSq=0
trials=$2

#Loop
for (( i=0; i<= trials-1;i++ ));do
	#Run RAYX .. Change path here if needed
	$RAYX -b -i ./../../../build/bin/$1 > output 
	
	#TREAT OUTPUT
	while read -r line; do
		if [[ $line == *"Benchmark:"* ]]; then	
		echo -e "$line"
		number=`echo $line | awk '{print $(NF-1)}'`
		total_time_sum=$((total_time_sum + number))
		total_time_sumSq=$((total_time_sumSq + number ** 2 ))
		fi
		if [[ $line == *"Got Rays. Run-time:"* ]]; then	
		echo -e "$line"
		numberstr=`echo $line | awk '{print $(NF-1)}'`
		#echo "${numberstr}"
		number=${numberstr%.*}
		#echo "${number}"
		trace_time_sum=$((trace_time_sum + number))
		trace_time_sumSq=$((trace_time_sumSq + number ** 2 ))
		fi
	done <$file
done

#Summary
# Derivation of mean and standard deviation

echo "***************************************"

total_time_mean=$((total_time_sum / trials))
total_time_var=$(((total_time_sumSq - (total_time_sum * total_time_sum) / trials) / (trials-1)))
total_time_sd=`echo "scale=4; sqrt($total_time_var)" | bc`

trace_time_mean=$((trace_time_sum / trials))
trace_time_var=$(((trace_time_sumSq - (trace_time_sum * trace_time_sum) / trials) / (trials-1)))
trace_time_sd=`echo "scale=4; sqrt($trace_time_var)" | bc`


echo "=============================================="
echo "Average total execution time: " ${total_time_mean} +/-  ${total_time_sd}" ms"
echo "Average total tracing   time: " ${trace_time_mean} +/-  ${trace_time_sd}" ms"

rm output


