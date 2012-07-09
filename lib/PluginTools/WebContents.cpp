#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDebug>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "WebContents.h"

WebContents::WebContents() {
    m_manager = new QNetworkAccessManager(this);

connect(m_manager, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(replyFinished(QNetworkReply*)));

}

void WebContents::fetch(QString url) {
    m_manager->get(QNetworkRequest(QUrl(url)));
}

void WebContents::replyFinished(QNetworkReply* pReply) {

    QByteArray data = pReply->readAll();
    QString str(data);
    emit stringAvailable(str);

}
