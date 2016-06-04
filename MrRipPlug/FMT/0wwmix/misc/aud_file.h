BOOL aud_is_valid(unsigned char* data, long size, long fsize)
{
    short samplerate;
    long sizein;
    char compression;
    char flags;
    memcpy(&samplerate, data, 2);
    memcpy(&sizein, data+2, 4);
    memcpy(&flags, data+10, 1);
    memcpy(&compression, data+11, 1);
    if (samplerate < 8000 || samplerate > 48000 ||
        sizein != fsize - 12) return FALSE;

    switch (compression)
    {
    case 1:
        return flags == 0;
    case 0x63:
        return flags == 2;
    }
    return FALSE;
}