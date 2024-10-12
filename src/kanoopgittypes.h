#ifndef KANOOPGITTYPES_H
#define KANOOPGITTYPES_H
#include <Kanoop/kanoopcommon.h>

enum GitRoles
{
    IndexEntryRole = KANOOP::UserRole + 1,

    AbsolutePathRole,
    BranchRole,
    CanonicalNameRole,
    CreateBranchRole,
    CommitRole,
    ControlTypeRole,
    IsRepoHeadCommitRole,
    IsCurrentBranchRole,
    IsLocalReferenceRole,
    IsRemoteReferenceRole,
    IsSubmoduleInitializedRole,
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
    LocalBranches,
    RemoteBranches,
    Submodules,
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

enum RefreshItem
{
    RefreshNone =                   0x0000,
    RefreshStatusEntries =          0x0001,
    RefreshFileSystemTree =         0x0002,
    RefreshCommitTable =            0x0004,
    RefreshLeftSidebar =            0x0008,

    RefreshAll = (RefreshStatusEntries | RefreshFileSystemTree | RefreshCommitTable | RefreshLeftSidebar),
};
Q_DECLARE_FLAGS(RefreshItems, RefreshItem)
Q_DECLARE_OPERATORS_FOR_FLAGS(RefreshItems)

#endif // KANOOPGITTYPES_H
