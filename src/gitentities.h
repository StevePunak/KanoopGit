#ifndef GITENTITIES_H
#define GITENTITIES_H
#include <Kanoop/kanoopcommon.h>

class GitEntities
{
public:
    static void registerEntityTypes();

    enum Type
    {
        InvalidEntity = 0,

        Commit,
        Credentials,
        File,
        FileLine,
        Folder,
        IndexEntry,
        PlaceHolder,
        Reference,
        Remote,
        Repository,
        Stash,
        Submodule,
        TitleItem,
        WorkInProgress,
    };

private:
    class EntityTypeToStringMap : public KANOOP::EnumToStringMap<Type>
    {
    public:
        EntityTypeToStringMap()
        {
            insert(Commit,          "Commit");
            insert(Credentials,     "Credentials");
            insert(File,            "File");
            insert(FileLine,        "FileLine");
            insert(Folder,          "Folder");
            insert(IndexEntry,      "IndexEntry");
            insert(PlaceHolder,     "PlaceHolder");
            insert(Reference,       "Reference");
            insert(Remote,          "Remote");
            insert(Repository,      "Repository");
            insert(Stash,           "Stash");
            insert(Submodule,       "Submodule");
            insert(TitleItem,       "TitleItem");
            insert(WorkInProgress,  "WorkInProgress");
        }
    };

    static const EntityTypeToStringMap _EntityTypeToStringMap;
};

#endif // GITENTITIES_H
