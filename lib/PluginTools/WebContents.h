#pragma once

#include <QObject>
#include <QString>
#include <QtNetwork/QNetworkAccessManager>

#include "TargetVersion.h"

/**
 * @brief     Get infos from web sites.
 * @details   This class gets infos from web sites like a version number or download
 *            link and provides these infos for consumers.
 * @author    Andeas Schrell
 */

class WebContents : public QObject {

        Q_OBJECT

    public:

        /** Create a new instance. */
        WebContents();

        /** Fetch infos asynchronously.
         * This method fetches infos from the given url and informs the caller when the infos are ready.
         * @param url URL to load.
         */
        void fetch(QString url);

        public slots:

        /** Slot called then URL was loaded.
         * This slot is called from the QNetworkAccessManager when the page was loaded.
         */
        void replyFinished(QNetworkReply*);

        signals:

        /** Signal which is emitted when the result string is available.
         * Connect to this signal to get the resulting string from the web call.
         * @param str resulting QString.
         */
        void stringAvailable(QString str);

    private:

        QNetworkAccessManager* m_manager; /**< our Network Access manager */

};
