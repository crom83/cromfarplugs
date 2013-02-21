#include "StdAfx.h"
#include "MyStr.h"

int _httoi(const TCHAR *value)
{
	struct CHexMap
	{
		TCHAR chr;
		int value;
	};
	const int HexMapL = 16;
	CHexMap HexMap[HexMapL] =
	{
		{'0', 0}, {'1', 1},
		{'2', 2}, {'3', 3},
		{'4', 4}, {'5', 5},
		{'6', 6}, {'7', 7},
		{'8', 8}, {'9', 9},
		{'A', 10}, {'B', 11},
		{'C', 12}, {'D', 13},
		{'E', 14}, {'F', 15}
	};
	CString buf = value;
	buf.MakeUpper();
	TCHAR *mstr = (TCHAR*)(LPCTSTR)buf;
	TCHAR *s = mstr;
	int result = 0;
	if (*s == '0' && *(s + 1) == 'X') s += 2;
	bool firsttime = true;
	while (*s != '\0')
	{
		bool found = false;
		for (int i = 0; i < HexMapL; i++)
		{
			if (*s == HexMap[i].chr)
			{
				if (!firsttime) result <<= 4;
				result |= HexMap[i].value;
				found = true;
				break;
			}
		}
		if (!found) break;
		s++;
		firsttime = false;
	}
	return result;
}

CString ExtractName(CString Path, bool bRev)
{
	TCHAR slash = bRev?_T('/'):_T('\\');

  if (Path.IsEmpty())
    return _T("");

	if (Path.Right(1) == slash)
		Path = Path.Left(Path.GetLength()-1);

  int pos = Path.ReverseFind(slash);

  if (pos != -1)
    return Path.Right(Path.GetLength()-pos-1);
  return Path;
}
CString ExtractPath( CString Path, bool bRev)
{
	TCHAR slash = bRev?_T('/'):_T('\\');

  if (Path.IsEmpty())
    return _T("");

	if (Path.Right(1) == slash)
		Path = Path.Left(Path.GetLength()-1);

  int pos = Path.ReverseFind(slash);
  if (pos != -1)
    return Path.Left(pos);
  return _T("");
}
CString ExtractExt( CString Path )
{
  if (Path.IsEmpty())
    return _T("");

  if (Path.Right(1) == _T('\\'))
    Path = Path.Left(Path.GetLength()-1);

  int pos = Path.ReverseFind(_T('.'));
  if (pos != -1)
    return Path.Right(Path.GetLength()-pos-1);
  return Path;
}
bool TestExt( CString ext, CString extList )
{
  ext.MakeLower();
  ext.Insert(0, _T("*."));
  ext += _T(";");
  return extList.Find(ext) != -1;
}
void Tokenize(CString str,  strvec& tokens, const CString& sep /*= " "*/, bool bLeaveSep)
{
  str.TrimLeft();
  str.TrimRight();
  int i = 0;
  int j = 0;
  tokens.RemoveAll();
	int add = bLeaveSep?1:sep.GetLength();
  while (j != -1)
  {
    j = str.Find(sep, i);
    CString mid;
    if (j != -1)  mid = str.Mid(i, j-i);
    else          mid = str.Mid(i);
    mid.TrimLeft();
    mid.TrimRight();
    if (!mid.IsEmpty()) tokens.Add(mid);
    i = j + add;
  }
}

void RegExTokenize(CString str, CString regex, strvec& tokens)
{
	tokens.RemoveAll();

	HANDLE hRegex = NULL;
	
	if (fInfo.RegExpControl(NULL, RECTL_CREATE, 0, (void *)&hRegex))
	{
		if (fInfo.RegExpControl(hRegex, RECTL_COMPILE, 0, (void *)_C(regex)))
		{
			int brackets=fInfo.RegExpControl(hRegex, RECTL_BRACKETSCOUNT, 0, 0);

			RegExpMatch * match = new RegExpMatch[brackets];
			RegExpSearch search = 
			{
				_C(str),
				0,
				str.GetLength(),
				match,
				brackets,
				0
			};

			if (fInfo.RegExpControl(hRegex, RECTL_SEARCHEX, 0, (void *)&search))
			{
				for (int i = 1; i < brackets; i++)
					tokens.Add(str.Mid(match[i].start, match[i].end - match[i].start));
			}
			delete [] match;
		}
		fInfo.RegExpControl(hRegex, RECTL_FREE, 0, 0);
	}
}

void TokensToParams( strvec& tokens, strmap& params )
{
  params.RemoveAll();
  int from = 0;
  if(tokens[0].Left(1) != _T("-"))
  {
    params.Add(_T("filename"), Unquote(tokens[0]));
    from++;
  }
  for (int i = from; i < tokens.GetSize(); i++)
  {
    CString token = tokens[i];
    if (token.Left(1) == _T("-")) token.Delete(0);
    int j = token.Find(_T(":"));
    if (j == -1)
    {
      CString opt = token;
      opt.TrimLeft();
      opt.TrimRight();
      opt.MakeLower();
      params.Add(opt, _T(""));
    } else {
      CString opt = token.Left(j);
      opt.TrimLeft();
      opt.TrimRight();
      opt.MakeLower();
      CString param = token.Mid(j+1);
      param.TrimLeft();
      param.TrimRight();
      param.MakeLower();
      params.Add(opt, Unquote(param));
    }
  }
}
CString & Unquote( CString &str )
{
  if (!str.IsEmpty() && (str.Left(1)  == _T("\""))) str.Delete(0);
  if (!str.IsEmpty() && (str.Right(1) == _T("\""))) str.Delete(str.GetLength()-1);
  return str;
}
CString GetParam( strmap& params, CString opt  )
{
  int i = params.FindKey(opt);
  if (i == -1)
    return "";

  return params.GetValueAt(i);
}
bool ExistsParam( strmap& params, CString opt )
{
  int i = params.FindKey(opt);
  if (i == -1)
    return false;

  return true;
}
bool UnicodeToutf8(const wchar_t* wcs, size_t length, byte *outbuf, size_t outlength)
{
	//WideCharToMultiByte(CP_UTF8, 0, wcs, length, (LPSTR)outbuf, outlength, NULL, NULL);
	const wchar_t *pc = wcs;
	const wchar_t *end = pc + length;
	UINT  num_errors = 0;
	int pos = 0;
	for(unsigned int c = *pc; pc < end ; c = *(++pc)) 
	{
		if (c < (1 << 7)) 
		{
			outbuf[pos++] = byte(c);
		} 
		else if (c < (1 << 11)) 
		{
			outbuf[pos++] = byte((c >> 6) | 0xc0);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		} 
		else if (c < (1 << 16)) 
		{
			outbuf[pos++] = byte((c >> 12) | 0xe0);
			outbuf[pos++] = byte(((c >> 6) & 0x3f) | 0x80);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		} 
		else if (c < (1 << 21)) 
		{
			outbuf[pos++] = byte((c >> 18) | 0xf0);
			outbuf[pos++] = byte(((c >> 12) & 0x3f) | 0x80);
			outbuf[pos++] = byte(((c >> 6) & 0x3f) | 0x80);
			outbuf[pos++] = byte((c & 0x3f) | 0x80);
		}
		else 
			++num_errors;
	}
	return num_errors == 0;
}
int utf8ToUnicode ( const char* lpMultiByteStr, WCHAR* lpWideCharStr, int cmbChars )
{
  const unsigned char*    pmb = (unsigned char  *)lpMultiByteStr;
  unsigned short*         pwc = (unsigned short *)lpWideCharStr;
  const unsigned char*    pmbe;
  int  cwChars = 0;

  if ( cmbChars >= 0 )  pmbe = pmb + cmbChars;
  else                  pmbe = NULL;

  while ( (pmbe == NULL) || (pmb < pmbe) )
  {
    char            mb = *pmb++;
    unsigned int    cc = 0;
    unsigned int    wc;

    while ( (cc < 7) && (mb & (1 << (7 - cc)))) cc++;
    if ( cc == 1 || cc > 6 ) continue; // illegal character combination for UTF-8
    if ( cc == 0 ) wc = mb;
    else {
      wc = (mb & ((1 << (7 - cc)) - 1)) << ((cc - 1) * 6);
      while ( --cc > 0 )
      {
        if ( pmb == pmbe ) return cwChars;// reached end of the buffer
        mb = *pmb++;
        if ( ((mb >> 6) & 0x03) != 2 ) return cwChars;// not part of multibyte character
        wc |= (mb & 0x3F) << ((cc - 1) * 6);
      }
    }

    if ( wc & 0xFFFF0000 )  wc = L'?';
    *pwc++ = wc;
    cwChars++;
    if ( wc == L'\0' )  return cwChars;
  }

  return cwChars;
}
void DelEndSlash( CString & str, bool bRev )
{
	TCHAR slash = bRev?_T('/'):_T('\\');
	if (str.Right(1) == slash) str.Delete(str.GetLength() - 1);
}
void AddEndSlash( CString & str, bool bRev )
{
	TCHAR slash = bRev?_T('/'):_T('\\');
  if			(str.IsEmpty()) str = slash;
  else if (str.Right(1) != slash) str += slash;
}
void AddBeginSlash( CString & str )
{
	if			(str.IsEmpty()) str = _T("/");
	else if (str.Left(1) != _T("/")) str.Insert(0, _T("/"));
}
int __cdecl my_wcstombsz(char* mbstr, const wchar_t* wcstr, size_t count, UINT CodePage)
{
  if (count == 0 && mbstr != NULL)
    return 0;

  int result = ::WideCharToMultiByte(CodePage, 0, wcstr, -1, mbstr, (int)count, NULL, NULL);
  ATLASSERT(mbstr == NULL || result <= (int)count);
  if (result > 0)
    mbstr[result - 1] = 0;
  return result;
}
int __cdecl my_mbstowcsz(wchar_t* wcstr, const char* mbstr, size_t count, UINT CodePage)
{
  if (count == 0 && wcstr != NULL)
    return 0;

  int result = ::MultiByteToWideChar(CodePage, 0, mbstr, -1, wcstr, (int)count);
  ATLASSERT(wcstr == NULL || result <= (int)count);
  if (result > 0)
    wcstr[result - 1] = 0;
  return result;
}
char * getAnsiString( LPCTSTR str )
{
  int len = lstrlen(str);
#ifdef _UNICODE
  wchar_t * wc = (wchar_t *)str;
  char * ansi = (char*)my_malloc(len + 1);
  my_wcstombsz(ansi, wc, len);
  return ansi;
#else
  #error "TODO!!!"
#endif
}
char * getOemString( const CString& val )
{
  LPCTSTR str = val;
  int len = val.GetLength();
#ifdef _UNICODE
  wchar_t * wc = (wchar_t *)(LPCTSTR)val;
  char * ansi = (char*)my_malloc(len + 1);
  my_wcstombsz(ansi, wc, len, CP_ACP);
  ::AnsiToOem(ansi, ansi);
  return ansi;
#else
  #error "TODO!!!"
#endif
}
void strcpyC( char * dst, const CString& src, bool Ansi )
{
  char * data;
  if (Ansi)
    data = getAnsiString(src);
  else
    data = getOemString(src);
  lstrcpyA(dst, data);
  my_free(data);
}

CString WtoUTF8( LPCTSTR str )
{
	int len = lstrlen(str) * 2;
	byte * buf = (byte *)my_malloc(len);
	UnicodeToutf8(str, lstrlen(str), buf, len);
	CString sP = (char*)buf;
	my_free(buf);
	return sP;
}

CString UTF8toW( LPCTSTR str )
{
	CString s;
	char * ansi = getAnsiString(str);
	int len = (lstrlenA(ansi) + 1) * 2;
	utf8ToUnicode(ansi, s.GetBuffer(len), len);
	my_free(ansi);
	s.ReleaseBuffer();
	return s;
}