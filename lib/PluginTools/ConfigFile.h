#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QString>
#include <QMap>
#include <QFile>

using namespace std;

class ConfigFile {

    public:

        /** Constructor for the configuration file class.
         * Creates a new instance of a configuration. If the file exists, it is
         * opened and read.
         * @param filename the pathname of the configuration file.
         */
        ConfigFile(QString filename);

        /** Destruktor.
         */
        ~ConfigFile();

        /** Write the file.
         * Write the configuration to the file.
         */
        void writeFile();

        /** Read the file.
         * Read the configuration from the file.
         */
        void readFile();

        /** Set value.
         * Set a configuration value in the configuration.
         * @param key the key to write the value in the configuration.
         * @param value the value for that key.
         */
        void setValue(QString key, QString value);

        /** Get value.
         * Get a configuration value from the configuration.
         * @param key the key to read the value from the configuration.
         */
        QString getValue(QString key);

        /** Automatic file writing on settings change.
         * @param autosave Setting this to true enables atomatic file
         * writing on every settings change.
         */
        void autoWrite(bool autosave);

    private:

        QMap<QString, QString> m_config; /**< the map to hold the key value pairs */
        bool m_auto; /**< autoSave enabled? */
        QString m_file; /**< the file to read and write */

};

#endif // CONFIGFILE_H
