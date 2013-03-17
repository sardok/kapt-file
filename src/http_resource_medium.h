#ifndef _HTTP_CONTENT_BUILDER_H
#define _HTTP_CONTENT_BUILDER_H
#include <QObject>
#include <QNetworkReply>

#include "resource_medium.h"

class Dist;
class QNetworkAccessManager;

class HttpResourceMedium: public ResourceMedium
{
  Q_OBJECT;

public:
  HttpResourceMedium(const QString &host, const QString &dist);
  ~HttpResourceMedium();
  void run(void);
		    
signals:
  void DistReady(Dist *);

private slots:
  void ReplyCallback(QNetworkReply *);
  void ErrorCallback(QNetworkReply::NetworkError);
    
private:
  quint32 _port;
  QNetworkAccessManager *_mng;
  Dist *_root;
};
#endif
