#include <QString>
#include <QStringList>
#include <QDebug>

#include "WebInfos.h"

#include "WebContents.h"

WebInfos::WebInfos(QString identifier, QString sdkVersion, QString installedVersion) {
    m_wc = new WebContents;

    connect(m_wc, SIGNAL(stringAvailable(QString)), SLOT(infosArrived(QString)));

    m_identifier = identifier;
    m_sdk_version = sdkVersion;
    m_installedVersion = installedVersion;

}

void WebInfos::fetch() {
    m_wc->fetch("http://schrell.de/AfterShotPro/WebTools/PlugInfo.php?sdk=" + m_sdk_version + "&id=" + m_identifier);
}

void WebInfos::infosArrived(QString str) {
    QRegExp re("(\\s|')");
    str.remove(re);
    QStringList webFields = str.split(";");
    if (webFields.size() < 4) return;
    m_name = webFields.at(0);
    m_webVersion = webFields.at(2);
    m_link = webFields.at(3);
    emit ready();
}

bool WebInfos::isWebNewer() {
    QStringList web = m_webVersion.split(".");
    QStringList me = m_installedVersion.split(".");
    bool ok = true;
    int webMa = web.at(0).toInt(&ok);
    if (!ok) return false;
    int webMi = web.at(1).toInt(&ok);
    if (!ok) return false;
    int webFi = web.at(2).toInt(&ok);
    if (!ok) return false;
    int meMa = me.at(0).toInt(&ok);
    if (!ok) return false;
    int meMi = me.at(1).toInt(&ok);
    if (!ok) return false;
    int meFi = me.at(2).toInt(&ok);
    if (!ok) return false;
    qDebug() << "WebInfos: version check" << m_webVersion << TARGET_VERSION_STRING << meMa << webMa << meMi << webMi << meFi
        << webFi;
    if (meMa > webMa) return false;
    if ((meMa == webMa) && (meMi > webMi)) return false;
    if ((meMa == webMa) && (meMi == webMi) && (meFi > webFi)) return false;
    if ((meMa == webMa) && (meMi == webMi) && (meFi == webFi)) return false;
    return true;
}

QString WebInfos::name() {
    return m_name;
}

QString WebInfos::identifier() {
    return m_identifier;
}

QString WebInfos::installedVersion() {
    return m_installedVersion;
}

QString WebInfos::webVersion() {
    return m_webVersion;
}

QString WebInfos::sdk_version() {
    return m_sdk_version;
}

QString WebInfos::link() {
    return m_link;
}
