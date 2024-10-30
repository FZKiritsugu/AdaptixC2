#ifndef ADAPTIXCLIENT_ADAPTIXWIDGET_H
#define ADAPTIXCLIENT_ADAPTIXWIDGET_H

#include <main.h>
#include <Client/WebSocketWorker.h>
#include <UI/Dialogs/DialogSyncPacket.h>
#include <UI/Widgets/LogsWidget.h>
#include <UI/Widgets/ListenersWidget.h>
#include <UI/Widgets/SessionsTableWidget.h>
#include <Client/WidgetBuilder.h>
#include <Agent/Agent.h>

class SessionsTableWidget;

class AdaptixWidget : public QWidget
{
Q_OBJECT
    QGridLayout*    mainGridLayout    = nullptr;
    QHBoxLayout*    topHLayout        = nullptr;
    QPushButton*    listenersButton   = nullptr;
    QPushButton*    logsButton        = nullptr;
    QPushButton*    sessionsButton    = nullptr;
    QPushButton*    graphButton       = nullptr;
    QPushButton*    targetsButton     = nullptr;
    QPushButton*    jobsButton        = nullptr;
    QPushButton*    proxyButton       = nullptr;
    QPushButton*    downloadsButton   = nullptr;
    QPushButton*    credsButton       = nullptr;
    QPushButton*    screensButton     = nullptr;
    QPushButton*    keysButton        = nullptr;
    QPushButton*    reconnectButton   = nullptr;
    QFrame*         line_1            = nullptr;
    QFrame*         line_2            = nullptr;
    QFrame*         line_3            = nullptr;
    QFrame*         line_4            = nullptr;
    QSplitter*      mainVSplitter     = nullptr;
    QTabWidget*     mainTabWidget     = nullptr;
    QSpacerItem*    horizontalSpacer1 = nullptr;
    QStackedWidget* mainStackedWidget = nullptr;

    AuthProfile       profile;
    DialogSyncPacket* dialogSyncPacket = nullptr;

    void createUI();
    bool isValidSyncPacket(QJsonObject jsonObj);
    void processSyncPacket(QJsonObject jsonObj);

public:
    QThread*             ChannelThread     = nullptr;
    WebSocketWorker*     ChannelWsWorker   = nullptr;
    LogsWidget*          LogsTab           = nullptr;
    ListenersWidget*     ListenersTab      = nullptr;
    SessionsTableWidget* SessionsTablePage = nullptr;

    QMap<QString, WidgetBuilder*> RegisterAgents;
    QMap<QString, WidgetBuilder*> RegisterListeners;
    QMap<QString, QStringList>    LinkListenerAgent;
    QVector<ListenerData>         Listeners;
    QMap<QString, Agent*>         Agents;

    explicit AdaptixWidget(AuthProfile authProfile);
    ~AdaptixWidget();

    AuthProfile GetProfile();
    void        AddTab(QWidget* tab, QString title, QString icon = "" );
    void        RemoveTab(int index);

public slots:
    void ChannelClose();
    void DataHandler(const QByteArray& data);

    void LoadLogsUI();
    void LoadListenersUI();
};

#endif //ADAPTIXCLIENT_ADAPTIXWIDGET_H