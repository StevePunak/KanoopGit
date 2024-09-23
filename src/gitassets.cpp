#include "gitassets.h"
#include "gitroles.h"
#include <Kanoop/gui/resources.h>
#include <Kanoop/entitymetadata.h>

const GitAssets::AssetToStringMap GitAssets::_AssetToStringMap;

void GitAssets::registerAssets()
{
    QList<AssetType> types = _AssetToStringMap.keys();
    for(AssetType type : types) {
        QString path = QString(":/assets/%1").arg(_AssetToStringMap.getString(type));
        Resources::registerImage(type, path);
    }
}
