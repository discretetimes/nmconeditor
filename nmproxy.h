#ifndef NMPROXY_H
#define NMPROXY_H

#include <QObject>
#include <QAbstractListModel>

#include <arpa/inet.h>

#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/ConnectionSettings>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Utils>
#include <NetworkManagerQt/Device>
#include <NetworkManagerQt/Ipv4Setting>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/WirelessSecuritySetting>
#include <NetworkManagerQt/WiredSetting>
#include <QCoreApplication>

#include <QDBusMetaType>
#include <QTextStream>
#include <QUuid>

#include <QDBusError>
#include <QDBusMetaType>
#include <QDBusPendingReply>

#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>

class NMProxy : public QObject
{
    Q_OBJECT
public:
    explicit NMProxy(QObject *parent = nullptr);

public slots:
    void listDevices();
    QStringList listWiredConnections();
    QString findDeviceByName(const QString &iface);
    void activateWiredConnection(const QString &connectionName, const QString &interfaceName);
    void deactivateWiredConnection(const QString &connectionName);
    void createAutoWiredConnection(const QString &name, const QString &interfaceName);
    void createWiredConnection(const QString &name, const QString &interfaceName);
    void deleteConnection(const QString &name);
    void modifyIpv4Setting(const QString &connectionName);
};

#endif // NMPROXY_H

