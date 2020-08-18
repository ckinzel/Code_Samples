// This function copies 'bits' number of bits to a destination location in memory
// If the number of bits does not fit evenly into a number of bytes, the left
// over is truncated. If the memory locations overlap or exceed bounds, there
// is unexpected behavior.

void* bitcpy(void* dest, const void* src, size_t bits) {

	// typecast the void pointers into character arrays
	char* cast_dest = (char*)dest;
	char* cast_src = (char*)src;

	// elementwise assignment
	for (int i = 0; i < bits / 8; i++) {

		cast_dest[i] = cast_src[i];
	}

	// return the new address
	return cast_dest;
}