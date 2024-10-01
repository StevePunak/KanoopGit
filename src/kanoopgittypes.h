#ifndef KANOOPGITTYPES_H
#define KANOOPGITTYPES_H
#include <Kanoop/kanoopcommon.h>

enum GitRoles
{
    IndexEntryRole = KANOOP::UserRole + 1,

    AbsolutePathRole,
    BranchRole,
    CreateBranchRole,
    CommitRole,
    IsRepoHeadCommitRole,
    ObjectIdRole,
    ReferenceRole,
    RelativePathRole,
    RemoteNameRole,
    RepoPathRole,
    StashRole,
    StatusEntryRole,
    TreeChangeEntryRole,
};

enum ControlType
{
    UnknownControlType = 0,

    StagedFiles,
    UnstagedFiles,
};

enum ColHeaders
{
    CH_Name = 1,
    CH_BranchOrTag,
    CH_Graph,
    CH_Message,
    CH_Timestamp,
    CH_SHA,
    CH_Old,
    CH_New,
    CH_Tag,
    CH_Text,
    CH_File,
    CH_RecentRepos,
};

// These need to be explicitly called out and maintained with original values
enum FileTypes
{
    UnknownFileFtype    = 0,

    SshPrivateKey       = 1,
    SshPublicKey        = 2,
    CloneToDirectory    = 3,
    RepoDirectory       = 4,
};

#endif // KANOOPGITTYPES_H
