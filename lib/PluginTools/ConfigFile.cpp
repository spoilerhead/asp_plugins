#include "ConfigFile.h"

#include <QString>
#include <QMap>
#include <QTextStream>
#include <QDebug>
#include <QFile>

using namespace std;

ConfigFile::ConfigFile(QString fileName) {
    m_config.clear();
    m_auto = false;
    m_file = fileName;
    readFile();
}

ConfigFile::~ConfigFile() {
}

void ConfigFile::writeFile() {
    QFile file(m_file);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QMapIterator < QString, QString > i(m_config);
    while (i.hasNext()) {
        i.next();
        stream << i.key() << "=" << i.value() << endl;
    }
    file.close();
}

void ConfigFile::readFile() {
    QFile file(m_file);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString line;
    while (0 != (line = stream.readLine())) {
        QString key = line.mid(0, line.indexOf("="));
        QString val = line.mid(line.indexOf("=") + 1);
        m_config.insert(key, val);
    }
    file.close();
}

void ConfigFile::setValue(QString key, QString value) {
    m_config.insert(key, value);
    if (m_auto) writeFile();
}

QString ConfigFile::getValue(QString key) {
    QMap<QString, QString>::iterator i = m_config.find(key);
    if (i != m_config.end()) {
        return i.value();
    } else {
        return 0;
    }
}

void ConfigFile::autoWrite(bool autosave) {
    m_auto = autosave;
}
