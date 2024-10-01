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
        File,
        FileLine,
        Folder,
        IndexEntry,
        Reference,
        PlaceHolder,
        Repository,
        Credentials,
        Stash,
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
            insert(Reference,       "Reference");
            insert(PlaceHolder,     "PlaceHolder");
            insert(Repository,      "Repository");
            insert(Stash,           "Stash");
            insert(WorkInProgress,  "WorkInProgress");
        }
    };

    static const EntityTypeToStringMap _EntityTypeToStringMap;
};

#endif // GITENTITIES_H
