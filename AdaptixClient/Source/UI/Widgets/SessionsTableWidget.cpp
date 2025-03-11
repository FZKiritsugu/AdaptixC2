#include <UI/Widgets/SessionsTableWidget.h>
#include <UI/Widgets/AdaptixWidget.h>
#include <Client/Requestor.h>
#include <MainAdaptix.h>

SessionsTableWidget::SessionsTableWidget( QWidget* w )
{
    this->mainWidget = w;
    this->createUI();

    connect( tableWidget, &QTableWidget::doubleClicked,              this, &SessionsTableWidget::handleTableDoubleClicked );
    connect( tableWidget, &QTableWidget::customContextMenuRequested, this, &SessionsTableWidget::handleSessionsTableMenu );
    connect( tableWidget, &QTableWidget::itemSelectionChanged,       this, [this](){tableWidget->setFocus();} );
}

SessionsTableWidget::~SessionsTableWidget() = default;

void SessionsTableWidget::createUI()
{
    titleAgentID   = new QTableWidgetItem( "Agent ID" );
    titleAgentType = new QTableWidgetItem( "Agent Type" );
    titleExternal  = new QTableWidgetItem( "External" );
    titleListener  = new QTableWidgetItem( "Listener" );
    titleInternal  = new QTableWidgetItem( "Internal" );
    titleDomain    = new QTableWidgetItem( "Domain" );
    titleComputer  = new QTableWidgetItem( "Computer" );
    titleUser      = new QTableWidgetItem( "User" );
    titleOs        = new QTableWidgetItem( "OS" );
    titleProcess   = new QTableWidgetItem( "Process" );
    titleProcessId = new QTableWidgetItem( "PID" );
    titleThreadId  = new QTableWidgetItem( "TID" );
    titleTag       = new QTableWidgetItem( "Tags" );
    titleLast      = new QTableWidgetItem( "Last" );
    titleSleep     = new QTableWidgetItem( "Sleep" );

    tableWidget = new QTableWidget( this );
    tableWidget->setColumnCount( ColumnCount );
    tableWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    tableWidget->setAutoFillBackground( false );
    tableWidget->setShowGrid( false );
    tableWidget->setSortingEnabled( true );
    tableWidget->setWordWrap( false );
    tableWidget->setCornerButtonEnabled( false );
    tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
    tableWidget->setFocusPolicy( Qt::NoFocus );
    tableWidget->setAlternatingRowColors( true );
    tableWidget->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    tableWidget->horizontalHeader()->setCascadingSectionResizes( true );
    tableWidget->horizontalHeader()->setHighlightSections( false );
    tableWidget->verticalHeader()->setVisible( false );

    tableWidget->setHorizontalHeaderItem( ColumnAgentID,   titleAgentID );
    tableWidget->setHorizontalHeaderItem( ColumnAgentType, titleAgentType );
    tableWidget->setHorizontalHeaderItem( ColumnListener,  titleListener );
    tableWidget->setHorizontalHeaderItem( ColumnExternal,  titleExternal );
    tableWidget->setHorizontalHeaderItem( ColumnInternal,  titleInternal );
    tableWidget->setHorizontalHeaderItem( ColumnDomain,    titleDomain );
    tableWidget->setHorizontalHeaderItem( ColumnComputer,  titleComputer );
    tableWidget->setHorizontalHeaderItem( ColumnUser,      titleUser );
    tableWidget->setHorizontalHeaderItem( ColumnOs,        titleOs );
    tableWidget->setHorizontalHeaderItem( ColumnProcess,   titleProcess );
    tableWidget->setHorizontalHeaderItem( ColumnProcessId, titleProcessId );
    tableWidget->setHorizontalHeaderItem( ColumnThreadId,  titleThreadId );
    tableWidget->setHorizontalHeaderItem( ColumnTags,      titleTag );
    tableWidget->setHorizontalHeaderItem( ColumnLast,      titleLast );
    tableWidget->setHorizontalHeaderItem( ColumnSleep,     titleSleep );

    for(int i = 0; i < 15; i++) {
        if (GlobalClient->settings->data.SessionsTableColumns[i] == false)
            tableWidget->hideColumn(i);
    }

    mainGridLayout = new QGridLayout( this );
    mainGridLayout->setContentsMargins( 0, 0,  0, 0);
    mainGridLayout->addWidget( tableWidget, 0, 0, 1, 1);
}

void SessionsTableWidget::Clear() const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );

    for (auto agentId : adaptixWidget->Agents.keys()) {
        Agent* agent = adaptixWidget->Agents[agentId];
        adaptixWidget->Agents.remove(agentId);
        delete agent->Console;
        delete agent->FileBrowser;
        delete agent;
    }

    for (int index = tableWidget->rowCount(); index > 0; index-- )
        tableWidget->removeRow(index -1 );
}

void SessionsTableWidget::AddAgentItem( Agent* newAgent ) const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    if ( adaptixWidget->Agents.contains(newAgent->data.Id) )
        return;

    adaptixWidget->Agents[ newAgent->data.Id ] = newAgent;

    if( tableWidget->rowCount() < 1 )
        tableWidget->setRowCount( 1 );
    else
        tableWidget->setRowCount( tableWidget->rowCount() + 1 );

    bool isSortingEnabled = tableWidget->isSortingEnabled();
    tableWidget->setSortingEnabled( false );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnAgentID,   newAgent->item_Id );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnAgentType, newAgent->item_Type );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnListener,  newAgent->item_Listener );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnExternal,  newAgent->item_External );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnInternal,  newAgent->item_Internal );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnDomain,    newAgent->item_Domain );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnComputer,  newAgent->item_Computer );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnUser,      newAgent->item_Username );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnOs,        newAgent->item_Os );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnProcess,   newAgent->item_Process );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnProcessId, newAgent->item_Pid );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnThreadId,  newAgent->item_Tid );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnTags,      newAgent->item_Tags );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnLast,      newAgent->item_Last );
    tableWidget->setItem( tableWidget->rowCount() - 1, ColumnSleep,     newAgent->item_Sleep );
    tableWidget->setSortingEnabled( isSortingEnabled );

    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnAgentID,   QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnAgentType, QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnListener,  QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnExternal,  QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnInternal,  QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnProcessId, QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnThreadId,  QHeaderView::ResizeToContents );
    tableWidget->horizontalHeader()->setSectionResizeMode( ColumnSleep,     QHeaderView::ResizeToContents );
}

void SessionsTableWidget::RemoveAgentItem(const QString &agentId) const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    Agent* agent = adaptixWidget->Agents[agentId];
    adaptixWidget->Agents.remove(agentId);
    delete agent->Console;
    delete agent->FileBrowser;
    delete agent;

    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if (agentId == tableWidget->item( rowIndex, ColumnAgentID )->text()) {
            tableWidget->removeRow(rowIndex);
            break;
        }
    }
}



/// SLOTS

void SessionsTableWidget::handleTableDoubleClicked(const QModelIndex &index) const
{
    QString AgentId = tableWidget->item(index.row(),0)->text();

    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    adaptixWidget->LoadConsoleUI(AgentId);
}

void SessionsTableWidget::handleSessionsTableMenu(const QPoint &pos)
{
    if ( ! tableWidget->itemAt(pos) )
        return;

    auto ctxMenu = QMenu();

    auto agentSep1 = new QAction();
    agentSep1->setSeparator(true);
    auto agentSep2 = new QAction();
    agentSep2->setSeparator(true);

    /// AGENT MENU

    auto agentMenu = new QMenu("Agent", &ctxMenu);
    agentMenu->addAction("Tasks", this, &SessionsTableWidget::actionTasksBrowserOpen);
    agentMenu->addAction(agentSep1);
    agentMenu->addAction("File Browser",    this, &SessionsTableWidget::actionFileBrowserOpen);
    agentMenu->addAction("Process Browser", this, &SessionsTableWidget::actionProcessBrowserOpen);
    agentMenu->addAction(agentSep2);
    agentMenu->addAction("Exit", this, &SessionsTableWidget::actionAgentExit);

    /// ITEM MENU

    auto itemMenu = new QMenu("Item", &ctxMenu);
    itemMenu->addAction("Mark as Active",   this, &SessionsTableWidget::actionMarkActive);
    itemMenu->addAction("Mark as Inactive", this, &SessionsTableWidget::actionMarkInctive);
    itemMenu->addSeparator();
    itemMenu->addAction("Set items color", this, &SessionsTableWidget::actionItemColor);
    itemMenu->addAction("Set text color",  this, &SessionsTableWidget::actionTextColor);
    itemMenu->addAction("Reset color",     this, &SessionsTableWidget::actionColorReset);
    itemMenu->addSeparator();
    itemMenu->addAction( "Hide on client", this, &SessionsTableWidget::actionItemHide);

    auto ctxSep1 = new QAction();
    ctxSep1->setSeparator(true);
    auto ctxSep2 = new QAction();
    ctxSep2->setSeparator(true);

    ctxMenu.addAction( "Console", this, &SessionsTableWidget::actionConsoleOpen);
    ctxMenu.addAction(ctxSep1);
    ctxMenu.addMenu(agentMenu);
    ctxMenu.addMenu(itemMenu);
    ctxMenu.addAction(ctxSep2);
    ctxMenu.addAction( "Set tag", this, &SessionsTableWidget::actionItemTag);
    ctxMenu.addAction( "Remove from server", this, &SessionsTableWidget::actionAgentRemove);

    ctxMenu.exec(tableWidget->horizontalHeader()->viewport()->mapToGlobal(pos));
}

void SessionsTableWidget::actionConsoleOpen() const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            adaptixWidget->LoadConsoleUI(agentId);
        }
    }
}

void SessionsTableWidget::actionTasksBrowserOpen() const
{
    QString agentId = tableWidget->item( tableWidget->currentRow(), ColumnAgentID )->text();
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    adaptixWidget->TasksTab->SetAgentFilter(agentId);
    adaptixWidget->SetTasksUI();
}

void SessionsTableWidget::actionFileBrowserOpen() const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            adaptixWidget->LoadFileBrowserUI(agentId);
        }
    }
}

void SessionsTableWidget::actionProcessBrowserOpen() const
{
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            adaptixWidget->LoadProcessBrowserUI(agentId);
        }
    }
}

void SessionsTableWidget::actionAgentExit() const
{
    QStringList listId;

    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString message = QString();
    bool ok = false;
    bool result = HttpReqAgentExit(listId, *(adaptixWidget->GetProfile()), &message, &ok);
    if( !result ) {
        MessageError("JWT error");
        return;
    }
}

void SessionsTableWidget::actionMarkActive() const
{
    QStringList listId;
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString message = QString();
    bool ok = false;
    bool result = HttpReqAgentSetMark(listId, "", *(adaptixWidget->GetProfile()), &message, &ok);
    if( !result ) {
        MessageError("JWT error");
        return;
    }
}

void SessionsTableWidget::actionMarkInctive() const
{
    QStringList listId;
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString message = QString();
    bool ok = false;
    bool result = HttpReqAgentSetMark(listId, "Inactive", *(adaptixWidget->GetProfile()), &message, &ok);
    if( !result ) {
        MessageError("JWT error");
        return;
    }
}

void SessionsTableWidget::actionItemColor() const
{
    QStringList listId;
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QColor itemColor = QColorDialog::getColor(Qt::white, nullptr, "Select items color");
    if (itemColor.isValid()) {
        QString itemColorHex = itemColor.name();
        QString message = QString();
        bool ok = false;
        bool result = HttpReqAgentSetColor(listId, itemColorHex, "", false, *(adaptixWidget->GetProfile()), &message, &ok);
        if( !result ) {
            MessageError("JWT error");
            return;
        }
    }
}

void SessionsTableWidget::actionTextColor() const
{
    QStringList listId;
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QColor textColor = QColorDialog::getColor(Qt::white, nullptr, "Select text color");
    if (textColor.isValid()) {
        QString textColorHex = textColor.name();
        QString message = QString();
        bool ok = false;
        bool result = HttpReqAgentSetColor(listId, "",  textColorHex, false, *(adaptixWidget->GetProfile()), &message, &ok);
        if( !result ) {
            MessageError("JWT error");
            return;
        }
    }
}

void SessionsTableWidget::actionColorReset() const
{
    QStringList listId;
    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString message = QString();
    bool ok = false;
    bool result = HttpReqAgentSetColor(listId, "",  "", true, *(adaptixWidget->GetProfile()), &message, &ok);
    if( !result ) {
        MessageError("JWT error");
        return;
    }
}

void SessionsTableWidget::actionAgentRemove()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Confirmation",
                                      "Are you sure you want to delete all information about the selected agents from the server?\n\n"
                                      "If you want to hide the record, simply choose: 'Item -> Hide on Client'.",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    QStringList listId;

    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString message = QString();
    bool ok = false;
    bool result = HttpReqAgentRemove(listId, *(adaptixWidget->GetProfile()), &message, &ok);
    if( !result ) {
        MessageError("JWT error");
        return;
    }
}

void SessionsTableWidget::actionItemTag() const
{
    QStringList listId;

    auto adaptixWidget = qobject_cast<AdaptixWidget*>( mainWidget );
    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    if(listId.empty())
        return;

    QString tag = "";
    if(listId.size() == 1) {
        tag = tableWidget->item( tableWidget->currentRow(), ColumnTags )->text();
    }

    bool inputOk;
    QString newTag = QInputDialog::getText(nullptr, "Set tags", "New tag", QLineEdit::Normal,tag, &inputOk);
    if ( inputOk ) {
        QString message = QString();
        bool ok = false;
        bool result = HttpReqAgentSetTag(listId, newTag, *(adaptixWidget->GetProfile()), &message, &ok);
        if( !result ) {
            MessageError("JWT error");
            return;
        }
    }
}

void SessionsTableWidget::actionItemHide() const
{
    QList<QString> listId;

    for( int rowIndex = 0 ; rowIndex < tableWidget->rowCount() ; rowIndex++ ) {
        if ( tableWidget->item(rowIndex, 0)->isSelected() ) {
            auto agentId = tableWidget->item( rowIndex, ColumnAgentID )->text();
            listId.append(agentId);
        }
    }

    for (auto id : listId)
        this->RemoveAgentItem(id);
}
