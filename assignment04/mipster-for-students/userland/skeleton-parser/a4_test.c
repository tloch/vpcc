int* data;
int* cdata;
int* minmax;
int* cminmax;
int max;
int min;
int len;
int i;

int main () {
	int j;
	int *k;

	data 	= malloc(10 * 4);
	cdata 	= data;  			// cursor to start of data
	minmax 	= malloc(2 * 4);
	cminmax = minmax;  			// cursor to start of minmax

	max 	= -2147483648;		// init w/ smallest 32-bit 2 complement integer
	min  	=  2147483647;		// init w/ largest  32-bit 2 complement integer

	len 	= 10;				// size of data array
	i 		= 0;

	*cdata 	= 12;				// init data with some numbers
	cdata = cdata + 1;
	*cdata 	= -12;
	cdata = cdata + 1;
	*cdata 	= 100;
	cdata = cdata + 1;
	*cdata 	= 42;
	cdata = cdata + 1;
	*cdata 	= -1024;
	cdata = cdata + 1;
	*cdata 	= 1234567890;
	cdata = cdata + 1;
	*cdata 	= -1234567890;
	cdata = cdata + 1;
	*cdata 	= -0;
	cdata = cdata + 1;
	*cdata 	= 0;
	cdata = cdata + 1;
	*cdata 	= -1;

	cdata = data; 				// reset cursor to data

	while (i < len) {
		if (*cdata < min) {
			min = *cdata;
		}
		if (*cdata > max) {
			max = *cdata;
		}
		cdata = cdata + 1;		// move cursor by 1 element (4 bytes)
		i = i + 1;
	}

	*cminmax = min;  			// store min. value at 0th position in minmax
	cminmax = cminmax + 1;
	*cminmax = max;				// store max. value at 1st position in minmax

	// print the results in minmax to stdout
	// add a return 0 if necessary
}

