#include "gitentities.h"

#include <Kanoop/entitymetadata.h>

const GitEntities::EntityTypeToStringMap GitEntities::_EntityTypeToStringMap;

void GitEntities::registerEntityTypes()
{
    QList<Type> types = _EntityTypeToStringMap.keys();
    for(Type type : types) {
        EntityMetadata::registerMetadata(type, _EntityTypeToStringMap.getString(type));
    }
}
