#include <stddef.h>
#include <main.h>

typedef enum	
{
	 Unused       = 0x00,   /* Unused (framing character placeholder) */
	 DiffZero     = 0x01,   /* Range 0x01 - 0xCE:                     */
	 DiffZeroMax  = 0xCF,   /* n-1 explicit characters plus a zero    */
	 Diff         = 0xD0,   /* 207 explicit characters, no added zero */
	 Resume       = 0xD1,   /* Unused (resume preempted packet)       */
	 Reserved     = 0xD2,   /* Unused (reserved for future use)       */
	 RunZero      = 0xD3,   /* Range 0xD3 - 0xDF:                     */
	 RunZeroMax   = 0xDF,   /* 3-15 zeroes                            */
	 Diff2Zero    = 0xE0,   /* Range 0xE0 - 0xFE:                     */
	 Diff2ZeroMax = 0xFE,   /* 0-30 explicit characters plus 2 zeroes */
	 Error        = 0xFF    /* Unused (PPP LCP renegotiation)         */
} StuffingCode;

#define isDiff2Zero(X) (((X) & 0xE0) == (Diff2Zero & 0xE0))
#define isRunZero(X)   (((X) & 0xF0) == (RunZero   & 0xF0))
#define FinishBlock(X) \ (*code_ptr = Tx(X), code_ptr = dst++, code = DiffZero)


static uint8_t Rx(uint8_t x) { return(x == 0 ? 0x7E : x); }

/*
* UnStuffData decodes "srclength" bytes of data from the buffer
* "ptr", writing the output to "dst". If the decoded data does not
* fit within "dstlength" bytes or any other error occurs, then
* UnStuffData returns NULL.
*/



static uint8_t *UnStuffData(const uint8_t *ptr, unsigned int srclength, uint8_t *dst, unsigned int dstlength)
{
	const unsigned char *end   = ptr + srclength;
	const unsigned char *limit = dst + dstlength;
	while (ptr < end)
	{
		int z, c = Rx(*ptr++);
		if (c == Error || c == Resume || c == Reserved) 
			return(NULL);
		else if (c == Diff)       
		{ 
			z = 0;
			c--;
		}
		else if (isRunZero(c))
    { 
		z = c & 0xF;
		c = 0;     
		}
		else if (isDiff2Zero(c))  
		{ 
			z = 2;
			c &= 0x1F; 
		}
		else                      
		{
			z = 1;
			c--;
		}

		while (--c >= 0 && dst < limit)
			*dst++ = Rx(*ptr++);
		while (--z >= 0 && dst < limit)
			*dst++ = 0;
	}
	if (dst < limit) 
		return(dst-1);
	else
		return(NULL);
}