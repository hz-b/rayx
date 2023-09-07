#!/usr/bin/env bash
# Takes a command and creates an SVG of a flame graph using perf and utilities
set -e
set -x

if [ $# -lt 2 ]; then
	echo Usage:
	echo $(basename "$0") COMMAND SVG_OUT
	exit 1
fi

if [ ! -d FlameGraph ]; then
	echo Please clone the FlameGraph repo so it is available at "FlameGraph"
	echo See: https://www.brendangregg.com/FlameGraphs/cpuflamegraphs.html
	exit 1
fi

cmd="$1"
svg_out="$2"

$cmd &
pid=$!
perf record -g -p $pid -o $svg_out.perf

perf script -i $svg_out.perf | FlameGraph/stackcollapse-perf.pl > $svg_out.perf-folded
FlameGraph/flamegraph.pl $svg_out.perf-folded > $svg_out

rm $svg_out.perf
rm $svg_out.perf-folded
