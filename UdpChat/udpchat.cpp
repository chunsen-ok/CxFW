﻿#include "udpchat.h"
#include <QUdpSocket>

#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkDatagram>

#include <QNetworkInterface>
#include <QNetworkAddressEntry>

constexpr quint16 PORT = 32214;

UdpChat::UdpChat(QObject *parent)
    : QObject(parent)
    , mSocket(new QUdpSocket(this))
{
    mSocket->bind(QHostAddress("127.0.0.1"), 32215);
    connect(mSocket, &QUdpSocket::readyRead,
            this, &UdpChat::onReadyRead);
}

UdpChat::~UdpChat()
{

}

void UdpChat::setHost(const QString &host)
{
    mSocket->disconnectFromHost();
    mHost.setAddress(host);
}

void UdpChat::setSubnet(const QString &net)
{
    QStringList netInfo = net.split("/");
    if (netInfo.size() > 0) {
        mSubnet = QHostAddress(netInfo.value(0));
    }
    if (netInfo.size() > 1) {
        mSubnetMask = netInfo.value(1).toInt();
    }
}

QStringList UdpChat::hostAddrs() const
{
    QStringList hosts;
    QHostInfo info = QHostInfo::fromName(QHostInfo::localHostName());
    for (const QHostAddress &addr: info.addresses()) {
        if (addr.isInSubnet(mSubnet, mSubnetMask)) {
            hosts.append(addr.toString());
        }
    }

    return hosts;
}

void UdpChat::sendMsg(const QString &peer, const QString &msg)
{
    QByteArray ba;
    ba.append(static_cast<char>(Msg::Hello));
    ba.append(msg.toUtf8());

    mSocket->writeDatagram(ba, QHostAddress(peer), PORT);
}

void UdpChat::sendBroadCast()
{
    QByteArray ba;
    ba.append(static_cast<char>(Msg::Search));
    ba.append(QString("Hello").toUtf8());
    mSocket->writeDatagram(ba, QHostAddress::Broadcast, PORT);
}

void UdpChat::onReadyRead()
{
    qDebug() << "OnReadyRead";
    while(mSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = mSocket->receiveDatagram();
        const QByteArray ba = datagram.data();
        if (ba.size() > 0) {
            const char msg = ba.at(0);
            const QString info = QString::fromUtf8(ba.right(ba.size()-1));
            emit msgReady(msg, QHostAddress(datagram.senderAddress().toIPv4Address()).toString(), PORT, info);
        }
    }
}

void UdpChat::onDisconnected()
{
    if (mHost.toString() != QString("127.0.0.1")) {
        qDebug() << "set host addr:" << mHost << ":" << 32215;
        mSocket->bind(mHost, 32214);
    }
}
