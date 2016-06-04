unit tpf;
interface
uses rip_api, SysUtils, ZLib, Classes, Windows;
type
 PDinArray = ^DinArray;
 DinArray = array [0..$fffffff div SizeOf(TRipFileInfo)] of TRipFileInfo;

 PDinArray2 = ^DinArray2;
 DinArray2 = array [0..$fffffff div SizeOf(TRipNfoFmt)] of TRipNfoFmt;
//Основная функция поиска файлов
//FileName - исходное имя файла,
//BaseString - буфер данных для поиска
//readed - размер этого буфера
//HeadOffset - смещение стартовой сигнатуры от начала ВЫДИРАЕМОГО файла (по умолчанию = 0)
//FileSize - размер выдираемого файла
//FileOffset - смещение выдираемого файла от начала исходного
//finfo - текстовые данные выдираемого файла
//WorkFileSize - размер исходного файла
//Find - указатель на сервисную функцию поиска данных в буфере
//FindInFile - указатель на сервисную функцию поиска данных в файле
function PackCheck(Fmt            : SmallInt;
                   const FileName : PChar;
                   BaseString     : PChar;
                   readed         : longint;
                   var HeadOffset : Longint;
                   var SomeOffset : Longint;
                   var FileSize   : Longint;
                   var FileOffset : Longint;
                   var finfo      : PRipFileInfo;
                   var FilesNumber: longint;
                   WorkFileSize   : longint;
                   func           : TRipServiceFunc): longint;stdcall;export;
function PackCheckFree(finfo      : PRipFileInfo;
                       FilesNumber: longint): WordBool;stdcall;export;

function GetFile(Fmt           : SmallInt;
                 const PackName: PChar;
                 const FileName: PChar;
                 const PathName: PChar;
                 StartOffset   : longint;
                 FileSize      : longint;
                 Silence       : WordBool;
                 func          : TRipServiceFunc): WordBool;stdcall;export;

function PluginInfo(var RipNfo: TRipNfo): WordBool; stdcall; export;
function PluginInfoFree(var RipNfo: TRipNfo): WordBool; stdcall; export;

var
//сигнатуры формата (начальная и конечная)
 sign: array [0..3] of char = (#$4B, #$56, #$53, #$00);
{$R-}
implementation
function PackCheck(Fmt            : SmallInt;
                   const FileName : PChar;
                   BaseString     : PChar;
                   readed         : longint;
                   var HeadOffset : Longint;
                   var SomeOffset : Longint;
                   var FileSize   : Longint;
                   var FileOffset : Longint;
                   var finfo      : PRipFileInfo;
                   var FilesNumber: longint;
                   WorkFileSize   : longint;
                   func           : TRipServiceFunc): longint;stdcall;
var
  fres, i, j, rd, FOffset: longint;
  offsets : longint;
  streams : longint;

  str        : pChar;
  T,     Offs: integer;
  info       : PDinArray;

begin
 fres := func.Find(sign, BaseString, 4, readed);
 if fres = -1 then
 begin
  result := -1;
  exit;
 end;

 FOffset := FileOffset + fres + 4;
 rd := func.ReadFromFile(FileName, FOffset, streams, 4);
 if rd < 4 then
 begin
  result := -1;
  exit;
 end;

 FOffset := FOffset + 4;
 rd := func.ReadFromFile(FileName, FOffset, offsets, 4);
 if rd < 4 then
 begin
  result := -1;
  exit;
 end;

 FOffset := FileOffset+fres+12; //Start of list
 Offs := offsets;
 GetMem(info, SizeOf(TRipFileInfo)*streams);
 ZeroMemory(info, SizeOf(TRipFileInfo)*streams);
 if info = nil then
 begin
  result := -1;
  exit;
 end;
 i := 0;
 repeat
  rd := func.ReadFromFile(FileName, FOffset, T, 4);
  if rd < 4 then
  begin
   result := -1;
   exit;
  end;
  FOffset := FOffset+rd;
  str := StrAlloc(T+1);
  try
   rd := func.ReadFromFile(FileName, FOffset, str^, T);
   if rd < T then
   begin
    result := -1;
    exit;
   end;
   FOffset := FOffset + rd+4;
   Str[T] := #0;

   rd := func.ReadFromFile(FileName, FOffset, T, 4);
   if rd < 4 then
   begin
    result := -1;
    exit;
   end;
   FOffset := FOffset + rd+rd;

   CharToOem(str, str);
   for j:=0 to StrLen(str)-1 do
   begin
    if str[j] in [':'] then str[j]:='_'; 
   end;

   info^[i].FullFileName := StrAlloc(StrLen(str)+1);
   StrCopy(info^[i].FullFileName, str);
   info^[i].Description := StrAlloc(StrLen('')+1);
   StrCopy(info^[i].Description, '');
   info^[i].StartOffset := Offs;
   info^[i].FileSize    := T;
   inc(i);
   Offs := Offs + T;
  finally
   StrDispose(str)
  end;
 until (FOffset >= offsets);
 FileOffset := FileOffset+fres;
 FileSize := FileOffset+Offs;
 finfo    := PRipFileInfo(info);
 FilesNumber := streams;
 result := FileOffset + FileSize;
end;

function PackCheckFree(finfo      : PRipFileInfo;
                       FilesNumber: longint): WordBool;stdcall;export;
var i: integer;
begin
 result := FALSE;
 for i := 0 to FilesNumber-1 do
 begin
  StrDispose(PDinArray(finfo)^[i].FullFileName);
  StrDispose(PDinArray(finfo)^[i].Description);  
 end;
 FreeMem(finfo, SizeOf(TRipFileInfo)*FilesNumber);
 result := TRUE;
end;

function GetFile(Fmt           : SmallInt;
                 const PackName: PChar;
                 const FileName: PChar;
                 const PathName: PChar;
                 StartOffset   : longint;
                 FileSize      : longint;
                 Silence       : WordBool;
                 func          : TRipServiceFunc): WordBool;stdcall;
const BufferSize = 1024*10;
var
  Z        : TCustomZlibStream;
  InStream : TFileStream;
  MemStream: TMemoryStream;
  OutStream: TFileStream;
  Count    : integer;
  Buffer   : array[0..BufferSize-1]of byte;
begin
 MemStream := TMemoryStream.Create;
 try
  InStream := TFileStream.Create(PackName, fmOpenRead or fmShareDenyRead);
  try
   InStream.Position := StartOffset;
   MemStream.CopyFrom(InStream, FileSize);
  finally
   InStream.Free;
  end;
  OutStream := TFileStream.Create(PathName+'\'+FileName, fmCreate or fmShareDenyRead);
  try
   MemStream.Position := 0;
   Z :=  TDecompressionStream.Create(MemStream);
   try
    while true do
    begin
     Count := Z.Read(Buffer, BufferSize);
     if Count <> 0 then OutStream.WriteBuffer(Buffer, Count) else break
    end;
   finally
    Z.Free
   end
  finally
   OutStream.Free;
  end;
 finally
  MemStream.Free;
 end;
 result := true;
end;

//информация о плагине
function PluginInfo(var RipNfo: TRipNfo): WordBool; stdcall;
var
 info       : PDinArray2;
begin
  GetMem(info, SizeOf(TRipNfoFmt));
  if info = nil then
  begin
   result := false;
   exit;
  end;
  info^[0].OptionString := StrAlloc(StrLen('TPF archive')+1);
  StrCopy(info^[0].OptionString, 'TPF archive');
  info^[0].RegPath := StrAlloc(StrLen('enableTPF')+1);
  StrCopy(info^[0].RegPath, 'enableTPF');
  info^[0].MType := RIP_INFO_TYPE_PACK;

 RipNfo.KnownFormats := PRipNfoFmt(info);
 RipNfo.nKnownFormats:= 1;
 RipNfo.Copyright := StrAlloc(StrLen('(c) Kubyshev Vladimir, 2003')+1);
 StrCopy(RipNfo.Copyright, '(c) Kubyshev Vladimir, 2003');
 result := true;
end;

function PluginInfoFree(var RipNfo: TRipNfo): WordBool; stdcall;
var i: integer;
begin
 result := FALSE;
 for i := 0 to RipNfo.nKnownFormats-1 do
 begin
  StrDispose(PDinArray2(RipNfo.KnownFormats)^[i].RegPath);
  StrDispose(PDinArray2(RipNfo.KnownFormats)^[i].OptionString);
 end;
 FreeMem(RipNfo.KnownFormats, SizeOf(TRipNfoFmt)*RipNfo.nKnownFormats);
 StrDispose(RipNfo.Copyright);
 result := TRUE;
end;

end.
