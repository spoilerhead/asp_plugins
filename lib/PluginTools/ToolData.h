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
#include <QPair>

class Option {
public:
    Option(int id, QString shortName, QString longName, QString hint) {
        this->id = id;
        this->shortName = shortName;
        this->longName = longName;
        this->hint = hint;
    }
    int id;
    QString shortName;
    QString longName;
    QString hint;
};

class ToolData : public PluginData
{

public:
    ToolData();
    ToolData(const ToolData &);

    QList<PluginDependency*> prerequisites(const PluginImageSettings &options, const PluginPipeSettings &settings) const;
    Status run(const PluginImageSettings &options, const PluginPipeSettings &settings);
    bool requiresDisk() const;
    QString filterName() const { return ""; }

    int version;

    int ownerId;
    int groupId;
    QString owner;
    QString group;

    QList<int> v1_enabledIds;

    QList<Option> enabledIds;

    void addEnabledId(int id, QString shortName = "", QString longName = "", QString hint = "");

};

#endif
