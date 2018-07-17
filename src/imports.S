	.set noreorder

#include "pspstub.s"

	STUB_START	"UtilsForUser",0x40010011,0x00010005
	STUB_FUNC	0x920F104A,sceKernelIcacheInvalidateAll
	STUB_END

