#include "ToolData.h"

#include "PluginHub.h"
#include "PluginImageSettings.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"

#include <QDebug>
#include <QString>
#include <QPair>


ToolData::ToolData()
    : version(2), ownerId(-1), groupId(-1), owner(""), group(""), enabledIds()
{
}

ToolData::ToolData(const ToolData &toolData) {
    this->version = 2;
    this->owner = toolData.owner;
    this->ownerId = toolData.ownerId;
    this->group = toolData.group;
    this->groupId = toolData.groupId;
    this->v1_enabledIds = toolData.v1_enabledIds;
    if (toolData.version < 2) {
        QListIterator<int> i(toolData.v1_enabledIds);
        while (i.hasNext()) {
            this->addEnabledId(i.next());
        }
    }
    if (toolData.version > 1) {
        this->enabledIds = toolData.enabledIds;
    }
}

QList<PluginDependency*> ToolData::prerequisites(const PluginImageSettings &options, const PluginPipeSettings &settings) const {
    Q_UNUSED(options);
    Q_UNUSED(settings);
    QList<PluginDependency*> list;
    return list;
}

PluginData::Status ToolData::run(const PluginImageSettings &options, const PluginPipeSettings &settings)
{
	Q_UNUSED(options);
    Q_UNUSED(settings);

    qDebug() << "ToolData: run" << owner << "with id =" << ownerId << "data =" << this;

	return Complete;
}

bool ToolData::requiresDisk() const
{
	return false;
}

void ToolData::addEnabledId(int id, QString shortName, QString longName, QString hint) {
    Option *option = new Option(id, shortName, longName, hint);
    enabledIds.append(*option);
    v1_enabledIds.append(id);
}
