VirtualFAT::VirtualFAT()
{
    Root = (SVFIP)my_malloc(sizeof SVFI);
    Root->FileName      = my_strdup("\\");
    Root->Description   = my_strdup("<dir>");
    Root->Plugin        = my_strdup("");
    Root->IsDirectory   = TRUE;
    Root->Fmt           = -1;
    Root->IsExtension   = FALSE;
    Root->Offset        = 0;
    Root->LevelUp       = NULL;
    Root->PackedSize    = 0;
    Root->UnpackedSize  = 0;
    Root->UserData.Size = 0;
    Root->UserData.Data = NULL;
}

VirtualFAT::~VirtualFAT()
{
    if (Root) {
        DelItem(Root);
    }
}

void VirtualFAT::ReInitFAT()
{
    if (Root) {
        DelItem(Root);
    }

    Root = (SVFIP)my_malloc(sizeof SVFI);
    Root->FileName      = my_strdup("\\");
    Root->Description   = my_strdup("<dir>");
    Root->Plugin        = my_strdup("");
    Root->IsDirectory   = TRUE;
    Root->Fmt           = -1;
    Root->IsExtension   = FALSE;
    Root->Offset        = 0;
    Root->LevelUp       = NULL;
    Root->PackedSize    = 0;
    Root->UnpackedSize  = 0;
    Root->UserData.Size = 0;
    Root->UserData.Data = NULL;
}

SVFIP VirtualFAT::GetDir(SVFIP Item, char* DirName)
{
    if (!Item->IsDirectory) return NULL;

    if (lstrlen(DirName)==0) return Root;

    if (lstrcmp(DirName, "..")==0) return Item->LevelUp;

    for (int i=0; i<Item->Items.size();i++){
        if (lstrcmp(DirName, Item->Items[i]->FileName)==0)
            return Item->Items[i];
    }
    return NULL;
}

SVFIP VirtualFAT::GetPath(SVFIP Item, char* PathName)
{
    SVFIP D;
    char *p, *s;

    if (!Item) D = Root;
    else D = Item;

    if (lstrlen(PathName)==0) return Root;

    p = my_strdup(PathName);
    char *fn = ExtractName(PathName);
    while (TRUE){
        char * t = ExtractRoot(p);
        
        D = GetDir(D, t);
        
        if (!D) {
            my_free(t);
            my_free(p);
            my_free(fn);
            return NULL;
        }
        s = my_substr(p, lstrlen(t)+1, lstrlen(p));                
        if (lstrlen(s)==0) {        
            my_free(t);
            my_free(p);
            my_free(s);
            my_free(fn);
            return D;
        }        
        my_free(p);
        my_free(t);        
        p = s;
    }
    my_free(p);
    my_free(fn);
    
    return NULL;
}

SVFIP VirtualFAT::InsertItem(SVFIP Dir, SVFIP Item)
{
    if (Dir) {        
        Item->LevelUp = Dir;
        Dir->Items.push_back(Item);        
    } else {
        Item->LevelUp = Root;
        Root->Items.push_back(Item);
    }
    
    return Item;
}

SVFIP VirtualFAT::InsertItem(const char* Dir, SVFIP Item)
{
    SVFIP D;
    SVFIP T;
    SVFIP I;
    char *p, *s;

    D = Root;

    p = my_strdup(Dir);    

    char *fn = ExtractName(Dir);
    while (TRUE){        

        char * t = ExtractRoot(p);
        
        T = GetDir(D, t);

        if (!T) {
            I = (SVFIP)my_malloc(sizeof(SVFI));
            I->FileName      = my_strdup(t);
            I->Description   = my_strdup("<dir>");
            I->Plugin        = my_strdup("");
            I->IsDirectory   = TRUE;
            I->Fmt           = -1;
            I->IsExtension   = FALSE;
            I->Offset        = 0;
            I->LevelUp       = D;
            I->PackedSize    = 0;
            I->UnpackedSize  = 0;
            I->UserData.Data = NULL;
            I->UserData.Size = 0;

            D = InsertItem(D, I);            
        } else D = T;

        s = my_substr(p, lstrlen(t)+1, lstrlen(p));                
        
        if (lstrlen(s)==0){
            my_free(t);
            my_free(p);
            my_free(s);
            my_free(fn);
            
            if (Item->Fmt == FMT_SPEC)
                return D;
            D = InsertItem(D, Item);
            return D;
        }
        my_free(p);
        my_free(t);
        p = s;        
    }
    my_free(p);
    my_free(fn);

    return NULL;
}

BOOL VirtualFAT::DelItem(SVFIP &Item)
{    
    if (Item->IsDirectory){
        for (int i = 0; i < Item->Items.size(); i++)
            DelItem(Item->Items[i]);        
    
        Item->Items.empty();
    }

    my_free(Item->Description);
    my_free(Item->FileName);
    my_free(Item->Plugin);
    if (Item->UserData.Data)
        my_free(Item->UserData.Data);
    my_free(Item);
    Item = NULL;    
        
    return TRUE;
}