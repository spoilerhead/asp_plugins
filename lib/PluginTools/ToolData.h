#ifndef TOOL_DATA_H
#define TOOL_DATA_H

#include "PluginData.h"
#include "PluginHub.h"
#include "PluginImageSettings.h"
#include "PluginPipeSettings.h"
#include "PluginTile.h"
#include "PluginImage.h"

#include <QString>
#include <QList>

class ToolData : public PluginData
{

public:
    ToolData(PluginHub *hub);

    QList<PluginDependency*> prerequisites(const PluginImageSettings &options, const PluginPipeSettings &settings) const;
    Status run(const PluginImageSettings &options, const PluginPipeSettings &settings);
    bool requiresDisk() const;
    QString filterName() const { return ""; }

    int version;

    int ownerId;
    int groupId;
    QString owner;
    QString group;

    QList<int> enabledIds;

    void addEnabledId(int id);

private:
	PluginHub *m_hub;

};

#endif
