

#define COBS_ENCODE_DST_BUF_LEN_MAX(SRC_LEN)            ((SRC_LEN) + (((SRC_LEN) + 253u)/254u))
#define COBS_DECODE_DST_BUF_LEN_MAX(SRC_LEN)            (((SRC_LEN) == 0) ? 0u : ((SRC_LEN) - 1u))

/*
 * For in-place encoding, the source data must be offset in the buffer by
 * the following amount (or more).
 */
#define COBS_ENCODE_SRC_OFFSET(SRC_LEN)                 (((SRC_LEN) + 253u)/254u)


/*****************************************************************************
 * Typedefs
 ****************************************************************************/

typedef enum
{
    COBS_ENCODE_OK                  = 0x00,
    COBS_ENCODE_NULL_POINTER        = 0x01,
    COBS_ENCODE_OUT_BUFFER_OVERFLOW = 0x02
} cobs_encode_status;

typedef struct
{
    size_t              out_len;
    cobs_encode_status  status;
} cobs_encode_result;


typedef enum
{
    COBS_DECODE_OK                  = 0x00,
    COBS_DECODE_NULL_POINTER        = 0x01,
    COBS_DECODE_OUT_BUFFER_OVERFLOW = 0x02,
    COBS_DECODE_ZERO_BYTE_IN_INPUT  = 0x04,
    COBS_DECODE_INPUT_TOO_SHORT     = 0x08
} cobs_decode_status;

typedef struct
{
    size_t              out_len;
    cobs_decode_status  status;
} cobs_decode_result;




cobs_encode_result cobs_encode(char *dst_buf_ptr, size_t dst_buf_len, const char *src_ptr, size_t src_len)
{
    cobs_encode_result  result              = { 0, COBS_ENCODE_OK };
    const uint8_t *     src_read_ptr        = src_ptr;
    const uint8_t *     src_end_ptr         = src_read_ptr + src_len;
    uint8_t *           dst_buf_start_ptr   = dst_buf_ptr;
    uint8_t *           dst_buf_end_ptr     = dst_buf_start_ptr + dst_buf_len;
    uint8_t *           dst_code_write_ptr  = dst_buf_ptr;
    uint8_t *           dst_write_ptr       = dst_code_write_ptr + 1;
    uint8_t             src_byte            = 0;
    uint8_t             search_len          = 1;


    /* First, do a NULL pointer check and return immediately if it fails. */
    if ((dst_buf_ptr == NULL) || (src_ptr == NULL))
    {
        result.status = COBS_ENCODE_NULL_POINTER;
        return result;
    }

    if (src_len != 0)
    {
        /* Iterate over the source bytes */
        for (;;)
        {
            /* Check for running out of output buffer space */
            if (dst_write_ptr >= dst_buf_end_ptr)
            {
                result.status |= COBS_ENCODE_OUT_BUFFER_OVERFLOW;
                break;
            }

            src_byte = *src_read_ptr++;
            if (src_byte == 0)
            {
                /* We found a zero byte */
                *dst_code_write_ptr = search_len;
                dst_code_write_ptr = dst_write_ptr++;
                search_len = 1;
                if (src_read_ptr >= src_end_ptr)
                {
                    break;
                }
            }
            else
            {
                /* Copy the non-zero byte to the destination buffer */
                *dst_write_ptr++ = src_byte;
                search_len++;
                if (src_read_ptr >= src_end_ptr)
                {
                    break;
                }
                if (search_len == 0xFF)
                {
                    /* We have a long string of non-zero bytes, so we need
                     * to write out a length code of 0xFF. */
                    *dst_code_write_ptr = search_len;
                    dst_code_write_ptr = dst_write_ptr++;
                    search_len = 1;
                }
            }
        }
    }

    /* We've reached the end of the source data (or possibly run out of output buffer)
     * Finalise the remaining output. In particular, write the code (length) byte.
     * Update the pointer to calculate the final output length.
     */
    if (dst_code_write_ptr >= dst_buf_end_ptr)
    {
        /* We've run out of output buffer to write the code byte. */
        result.status |= COBS_ENCODE_OUT_BUFFER_OVERFLOW;
        dst_write_ptr = dst_buf_end_ptr;
    }
    else
    {
        /* Write the last code (length) byte. */
        *dst_code_write_ptr = search_len;
    }

    /* Calculate the output length, from the value of dst_code_write_ptr */
    result.out_len = dst_write_ptr - dst_buf_start_ptr;

    return result;
}


/* Decode a COBS byte string.
 *
 * dst_buf_ptr:    The buffer into which the result will be written
 * dst_buf_len:    Length of the buffer into which the result will be written
 * src_ptr:        The byte string to be decoded
 * src_len         Length of the byte string to be decoded
 *
 * returns:        A struct containing the success status of the decoding
 *                 operation and the length of the result (that was written to
 *                 dst_buf_ptr)
 */
size_t cobs_decode(const uint8_t *input, size_t length, uint8_t *output)
{
    size_t read_index = 0;
    size_t write_index = 0;
    uint8_t cod;
    int i;

    while(read_index < length)
    {
        cod = input[read_index];

        if(read_index + cod > length && cod != 1)
        {
            return 0;
        }

        read_index++;

        for(i = 1; i < cod; i++)
        {
            output[write_index++] = input[read_index++];
        }
        if(cod != 0xFF && read_index != length)
        {
            output[write_index++] = '\0';
        }
    }

    return write_index;
}