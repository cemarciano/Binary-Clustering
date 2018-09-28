typedef double data_t;

#define K 3							// Number of divisions
#define CORES 1 					// Number of CPUs
#define WARP 2            			// Multiplier to stddev when dividing space
#define PERC_MIN 0.15				// Minimum percentage in the interval [0, 1] of data to grab from a cluster
#define PERC_MULT 1.0				// Multiplier in the interval [0, 2] for how much a cluster will yield, where 1 is default
#define TAKE_AT_LEAST 1				// Minimum number of regisers to take from each cluster
