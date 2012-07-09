#pragma once

#include <QString>
#include <QStringList>

#include "TargetVersion.h"

#include "WebContents.h"

/**
 * @brief     Get plugin infos from the web.
 * @details   This class gets version and download infos from the plugin delivery php scripts.
 * @author    Andeas Schrell
 */

class WebInfos : public QObject {

        Q_OBJECT

    public:
        /** Creates an instance for the given plugin under the SDK version
         * @param identifier plugin identifier
         * @param sdkVersion the version of the ASP sdk (currently 8)
         */
        WebInfos(QString identifier, QString sdkVersion, QString installedVersion);

        /** Fetch the infos from the web database. */
        void fetch();

        /** Check for newer version
         * @returns true if there is a newer version than this one available.
         */
        bool isWebNewer();

        /** Name of the plugin
         * @returns the name of the plugin
         */
        QString name();

        /** Identifier of the plugin
         * @returns the identifier of the plugin
         */
        QString identifier();

        /** Version string of the installed plugin
         * @returns the installed version number of the plugin
         */
        QString installedVersion();

        /** Version string of the plugin download
         * @returns the web version number of the plugin
         */
        QString webVersion();

        /** Link of the plugin
         * @returns a link to the new version download
         */
        QString link();

        /** SDK version info
         * @returns the SDK version
         */
        QString sdk_version();

        public slots:

        /** This slot is called when the infos are available
         * @param str the resulting string we can parse
         */
        void infosArrived(QString str);

        signals:

        /** This signal is emitted when the values are available. */
        void ready();

    private:
        WebContents* m_wc; /**< our web contents */
        QString m_name; /**< holds the plugin name */
        QString m_identifier; /**< holds the plugin identifier */
        QString m_installedVersion; /** the installed version */
        QString m_webVersion; /**< holds the plugin version */
        QString m_link; /**< holds the link to the plugin download */
        QString m_sdk_version; /**< this is the SDK version */

};

