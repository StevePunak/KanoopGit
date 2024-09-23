#ifndef GITASSETS_H
#define GITASSETS_H
#include <Kanoop/kanoopcommon.h>
#include "gitentities.h"

class GitAssets
{
public:
    static void registerAssets();

    enum AssetType
    {
        InvalidAsset = 0,

        Branch,
        CheckGreen,
        Copy,
        DashBlueOnWhite,
        DashWhiteOnGrey,
        Debug,
        File,
        Folder,
        LedRed,
        MinusRed,
        Pencil,
        PlusGreen,
        PlusWhiteOnGrey,
        Pull,
        Push,
        RenameFile,
        Stash,
        StashPop,
        WarningRed,
        WindowClose,
    };

private:
    class AssetToStringMap : public KANOOP::EnumToStringMap<AssetType>
    {
    public:
        AssetToStringMap()
        {
            insert(Branch,              "branch.png");
            insert(CheckGreen,          "check-green.png");
            insert(Copy,                "copy.png");
            insert(DashBlueOnWhite,     "dash-blue-on-white.png");
            insert(DashWhiteOnGrey,     "dash-white-on-grey.png");
            insert(Debug,               "debug.png");
            insert(File,                "file.png");
            insert(Folder,              "file.png");
            insert(LedRed,              "led-red.png");
            insert(MinusRed,            "x-white-on-red.png");
            insert(Pencil,              "pencil.png");
            insert(PlusGreen,           "plus-green.png");
            insert(PlusWhiteOnGrey,     "plus-white-on-grey.png");
            insert(Pull,                "push.png");
            insert(Push,                "pull.png");
            insert(RenameFile,          "rename-file.png");
            insert(Stash,               "stash.png");
            insert(StashPop,            "stash-pop.png");
            insert(WarningRed,          "warning-red.png");
            insert(WindowClose,         "window-close.png");
        }
    };

    static const AssetToStringMap _AssetToStringMap;

};

#endif // GITASSETS_H
