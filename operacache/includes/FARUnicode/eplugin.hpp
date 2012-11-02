#ifndef __EPLUGIN_HPP
#define __EPLUGIN_HPP
/*
  EPLUGIN.HPP

  Ext Plugin API for FAR Manager 1.63

  Rev 20.06.2000
*/

#if !defined(__PLUGIN_HPP)
#include <plugin.hpp>
#endif

#if defined(__BORLANDC__) && (__BORLANDC <= 0x520)
  #pragma option -a1
#elif defined(__GNUC__) || (defined(__WATCOMC__) && (__WATCOMC__ < 1100)) || defined(__LCC__)
  #pragma pack(1)
  #if defined(__LCC__)
    #define _export __declspec(dllexport)
  #endif
#else
  #pragma pack(push,1)
  #if _MSC_VER
    #define _export
  #endif
#endif

// Последовательность цветов FAR Manager
// HKEY_CURRENT_USER\Software\Far\Colors\CurrentPalette
// по 1 байту на индекс
enum FarColorsCurrentPalette{
  MenuText,
  MenuSelectedText,
  MenuHighlight,
  MenuSelectedHighlight,
  MenuBox,
  MenuTitle,
  HMenuText,
  HMenuSelectedText,
  HMenuHighlight,
  HMenuSelectedHighlight,
  PanelText,
  PanelSelectedText,
  PanelHighlightText,
  PanelInfoText,
  PanelCursor,
  PanelSelectedCursor,
  PanelTitle,
  PanelSelectedTitle,
  PanelColumnTitle,
  PanelTotalInfo,
  PanelSelectedInfo,
  DialogText,
  DialogHighlightText,
  DialogBox,
  DialogBoxTitle,
  DialogHighlightBoxTitle,
  DialogEdit,
  DialogButton,
  DialogSelectedButton,
  DialogHighlightButton,
  DialogHighlightSelectedButton,
  DialogMenuText,
  DialogMenuSelectedText,
  DialogMenuHighlight,
  DialogMenuSelectedHighlight,
  WarningDialogText,
  WarningDialogHighlightText,
  WarningDialogBox,
  WarningDialogBoxTitle,
  WarningDialogHighlightBoxTitle,
  WarningDialogEdit,
  WarningDialogButton,
  WarningDialogSelectedButton,
  WarningDialogHighlightButton,
  WarningDialogHighlightSelectedButton,
  KeyBarNumber,
  KeyBarText,
  KeyBarBackGround,
  CommandLine,
  Clock,
  ViewerText,
  ViewerSelectedText,
  ViewerStatus,
  EditorText,
  EditorSelectedText,
  EditorStatus,
  HelpText,
  HelpHighlightText,
  HelpTopic,
  HelpSelectedTopic,
  HelpBox,
  HelpBoxTitle,
  PanelDragText,
  DialogEditUnChanged,
  PanelScrollBar,
  HelpScrollBar,
  PanelBox,
  PanelScreensNumber,
  DialogEditSelected,
  CommandLineSelected,
  ViewerArrows,
};

// Эта структура не является стандартной - это аналог структуры FarDialogItem.
// This structure is not standard - an analogue of structure FarDialogItem.
struct InitDialogItem
{
  int Type;
  int X1, Y1, X2, Y2;
  int Focus;
  INT_PTR Selected;
  unsigned int Flags;
  int DefaultButton;
  wchar_t *Data;
};

// Эта структура не является стандартной - это аналог структуры FarMenuItem.
// This structure is not standard - an analogue of structure FarMenuItem.
struct InitMenuItem
{
  wchar_t* Text;
  int Selected;
  int Checked;
  int Separator;
};

enum{
  stdbOk,
  stdbCancel,
  stdbAbort,
  stdbRetry,
  stdbIgnore,
  stdbYes,
  stdbNo
};

// "std" кнопки :-)
wchar_t *_stdButton[7]={
  L"Ok",
  L"Cancel",
  L"Abort",
  L"Retry",
  L"Ignore",
  L"Yes",
  L"No"
};

#if defined(__BORLANDC__)
#ifdef __cplusplus
extern "C"{
#endif

//void InitStdButtons(int mbuttons);
/*
int FarMessageBox(struct PluginStartupInfo *psi,
                  char *caption,
                  char *help,
                  UINT typeMsg,
                  char *fmtText,
                  ...);

int FarInputBox(struct PluginStartupInfo *psi,
                  char *caption,
                  char *help,
                  char *history,
                  char *Data,
                  WORD  DataLen,
                  char *fmtPrompt,
                  ...);
*/
void InitDialogItems(struct InitDialogItem *Init,
                     struct FarDialogItem *Item,
                     int ItemsNumber);

#ifdef __cplusplus
};
#endif
#endif


#if defined(__BORLANDC__) && (__BORLANDC <= 0x520)
  #pragma option -a.
#elif defined(__GNUC__) || (defined(__WATCOMC__) && (__WATCOMC__ < 1100)) || defined(__LCC__)
  #pragma pack()
#else
  #pragma pack(pop)
#endif

#endif /* __EPLUGIN_HPP */
