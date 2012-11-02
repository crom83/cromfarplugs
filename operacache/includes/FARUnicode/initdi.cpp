void InitDialogItems(struct InitDialogItem *Init,
                     struct FarDialogItem *Item,
                     int ItemsNumber)
{
  int i;
  for (i=0; i < ItemsNumber; i++)
  {
    Item[i].Type=Init[i].Type;
    Item[i].X1=Init[i].X1;
    Item[i].Y1=Init[i].Y1;
    Item[i].X2=Init[i].X2;
    Item[i].Y2=Init[i].Y2;
    Item[i].Focus=Init[i].Focus;
    Item[i].Selected=Init[i].Selected;
    Item[i].Flags=Init[i].Flags;
    Item[i].DefaultButton=Init[i].DefaultButton;

    if ((unsigned int)Init[i].Data < 2000)
      strcpy_s(Item[i].Data, 512, GetMsg((unsigned int)Init[i].Data));
    else
      strcpy_s(Item[i].Data, 512, Init[i].Data);
  }
}
