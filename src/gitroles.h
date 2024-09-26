#ifndef GITROLES_H
#define GITROLES_H
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

#endif // GITROLES_H
