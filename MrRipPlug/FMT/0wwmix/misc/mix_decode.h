#if !defined(AFX_MIX_DECODE_H__8D085A21_D71B_11D3_B604_0000B4936994__INCLUDED_)
#define AFX_MIX_DECODE_H__8D085A21_D71B_11D3_B604_0000B4936994__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const int mix_checksum = 0x00010000;
const int mix_encrypted = 0x00020000;

void get_blowfish_key(const BYTE* s, BYTE* d);

#endif // !defined(AFX_MIX_DECODE_H__8D085A21_D71B_11D3_B604_0000B4936994__INCLUDED_)
