#include "stdafx.h"

void Cblowfish::set_key(const BYTE* key, int cb_key)
{
    int i, j;
    DWORD datal, datar;

    memcpy(m_p, p, sizeof(t_bf_p));
    memcpy(m_s, s, sizeof(t_bf_s));

    j = 0;
    for (i = 0; i < 18; i++)
    {
        int a = key[j++]; j %= cb_key;
        int b = key[j++]; j %= cb_key;
        int c = key[j++]; j %= cb_key;
        int d = key[j++]; j %= cb_key;
        m_p[i] ^= a << 24 | b << 16 | c << 8 | d;
    }

    datal = datar = 0;

    for (i = 0; i < 18;)
    {
        encipher(datal, datar);

        m_p[i++] = datal;
        m_p[i++] = datar;
    }

    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 256;)
        {
            encipher(datal, datar);

            m_s[i][j++] = datal;
            m_s[i][j++] = datar;
        }
    }
}

inline DWORD Cblowfish::S(DWORD x, int i) const
{
    return m_s[i][(x >> ((3 - i) << 3)) & 0xff];
}

inline DWORD Cblowfish::bf_f(DWORD x) const
{
    return ((S(x, 0) + S(x, 1)) ^ S(x, 2)) + S(x, 3);
}

inline void Cblowfish::ROUND(DWORD& a, DWORD b, int n) const
{
    a ^= bf_f(b) ^ m_p[n];
}

void Cblowfish::encipher(DWORD& xl, DWORD& xr) const
{
    DWORD Xl = xl;
    DWORD Xr = xr;

    Xl ^= m_p[0];
    ROUND (Xr, Xl, 1);  ROUND (Xl, Xr, 2);
    ROUND (Xr, Xl, 3);  ROUND (Xl, Xr, 4);
    ROUND (Xr, Xl, 5);  ROUND (Xl, Xr, 6);
    ROUND (Xr, Xl, 7);  ROUND (Xl, Xr, 8);
    ROUND (Xr, Xl, 9);  ROUND (Xl, Xr, 10);
    ROUND (Xr, Xl, 11); ROUND (Xl, Xr, 12);
    ROUND (Xr, Xl, 13); ROUND (Xl, Xr, 14);
    ROUND (Xr, Xl, 15); ROUND (Xl, Xr, 16);
    Xr ^= m_p[17];

    xr = Xl;
    xl = Xr;
}

void Cblowfish::decipher(DWORD& xl, DWORD& xr) const
{
    DWORD  Xl = xl;
    DWORD  Xr = xr;

    Xl ^= m_p[17];
    ROUND (Xr, Xl, 16);  ROUND (Xl, Xr, 15);
    ROUND (Xr, Xl, 14);  ROUND (Xl, Xr, 13);
    ROUND (Xr, Xl, 12);  ROUND (Xl, Xr, 11);
    ROUND (Xr, Xl, 10);  ROUND (Xl, Xr, 9);
    ROUND (Xr, Xl, 8);   ROUND (Xl, Xr, 7);
    ROUND (Xr, Xl, 6);   ROUND (Xl, Xr, 5);
    ROUND (Xr, Xl, 4);   ROUND (Xl, Xr, 3);
    ROUND (Xr, Xl, 2);   ROUND (Xl, Xr, 1);
    Xr ^= m_p[0];

    xl = Xr;
    xr = Xl;
}

static inline DWORD reverse(DWORD v)
{
    _asm
    {
        mov     eax, v
        xchg    al, ah
        rol     eax, 16
        xchg    al, ah
        mov     v, eax
    }
    return v;
}

void Cblowfish::encipher(const void* s, void* d, int size) const
{
    const DWORD* r = reinterpret_cast<const DWORD*>(s);
    DWORD* w = reinterpret_cast<DWORD*>(d);
    size >>= 3;
    while (size--)
    {
        DWORD a = reverse(*r++);
        DWORD b = reverse(*r++);
        encipher(a, b);
        *w++ = reverse(a);
        *w++ = reverse(b);
    }
}

void Cblowfish::decipher(const void* s, void* d, int size) const
{
    const DWORD* r = reinterpret_cast<const DWORD*>(s);
    DWORD* w = reinterpret_cast<DWORD*>(d);
    size >>= 3;
    while (size--)
    {
        DWORD a = reverse(*r++);
        DWORD b = reverse(*r++);
        decipher(a, b);
        *w++ = reverse(a);
        *w++ = reverse(b);
    }
}