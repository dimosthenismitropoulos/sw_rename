//  Copyright (c) Mitropoulos Dimosthenis
//  Email   mitropoulosdimosthenis@gmail.com
//  License   GPL3
//  Feedback and bug reports are appreciated

#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) , config("SwRenamerc")
{
    ui->setupUi(this);


    configgroupOptions = config.group( "Options" );
    configgroupExpressions = config.group( "Expressions" );

    //  Default locationUrl
    //  TODO: have it change through options
    locationUrl.setUrl("file://" + QDir::currentPath());

    initializeKUrlNavigator();
    initializeToolbar();
    initializeTreeWidget_Expressions();
    initializeStatusBar();
    initializeWindowPosition();

    showResults();
}


MainWindow::~MainWindow()
{

    //  Save Regular Expressions
    if (treewidgetExpressionsModified){
        //  Clean up config
        config.deleteGroup("Expressions");

        for (int i =0; i < ui->treeWidget_Expressions->topLevelItemCount(); i++){
            savesubtreeExpressions(ui->treeWidget_Expressions->topLevelItem(i), QString::number(i));
        }
    }

    //configgroupOptions.writeEntry("Depth", toolbuttonDepth->text().remove("&"));
    configgroupOptions.writeEntry( "HorizontalView", toolbuttonHorizontalview->isChecked() );
    configgroupOptions.writeEntry( "VerticalView", toolbuttonVerticalview->isChecked() );
    configgroupOptions.writeEntry( "TreeView", toolbuttonTreeview->isChecked() );
    configgroupOptions.writeEntry( "MatchFiles", toolbuttonMatchfiles->isChecked() );
    configgroupOptions.writeEntry( "MatchDirectories", toolbuttonMatchdirectories->isChecked() );
    configgroupOptions.writeEntry( "ExactMatch", toolbuttonExactmatch->isChecked() );
    if (ui->splitter->count()>0){
        QSplitterHandle *splitter_handle = ui->splitter->handle(0);
        configgroupOptions.writeEntry("ExpressionsViewer", splitter_handle->isEnabled());
    }


    //  We don't need to delete QObjects with parents explicitly but we need to delete ui
    delete ui;
}

void MainWindow::savesubtreeExpressions(QTreeWidgetItem *item, const QString &key){
    QStringList expression;
    expression << item->text(0) << item->text(1) << item->text(2);
    configgroupExpressions.writeXdgListEntry(key,expression);
    for (int i=0; i < item->childCount(); i++){
        savesubtreeExpressions(item->child(i), key + "." + QString::number(i));
    }
}
void MainWindow::loadsubtreeExpressions(QTreeWidgetItem *item, const QString &key){
    QStringList expression = configgroupExpressions.readXdgListEntry(key);
    QTreeWidgetItem *treeWidgetItem_New;
    if (expression.size() ==3){
        if(expression.at(1) == "" && expression.at(2) == ""){
            treeWidgetItem_New = addFolder(item, expression.at(0).toLocal8Bit().constData());
        }else{
           treeWidgetItem_New = addSearchReplaceExpression(item, expression.at(0).toLocal8Bit().constData(),
                                      expression.at(1).toLocal8Bit().constData(), expression.at(2).toLocal8Bit().constData());
        }

        int i=0;
        QString newKey = key + "." + QString::number(i);
        while ( !configgroupExpressions.readXdgListEntry(newKey).empty()){
            loadsubtreeExpressions(treeWidgetItem_New, newKey);
            i++;
            newKey = key + "." + QString::number(i);
        }
    }    
}
void MainWindow::initializeKUrlNavigator(){
    urlNavigator = new KUrlNavigator(this);
    ui->layout_Url->addWidget(urlNavigator);
    urlNavigator->setLocationUrl(locationUrl);
    urlNavigator->setShowFullPath(true);
    urlNavigator->show();
    QObject::connect(urlNavigator, &KUrlNavigator::urlChanged, this, &MainWindow::urlChanged);
}
void MainWindow::initializeToolbar(){

    bool configentryHorizontalview = configgroupOptions.readEntry( "HorizontalView", bool() );
    bool configentryVerticalview = configgroupOptions.readEntry( "VerticalView", bool() );
    bool configentryTreeview = configgroupOptions.readEntry( "TreeView", bool() );
    bool configentryMatchfiles = configgroupOptions.readEntry( "MatchFiles", bool() );
    bool configentryMatchdirectories = configgroupOptions.readEntry( "MatchDirectories", bool() );
    bool configentryExactmatch = configgroupOptions.readEntry( "ExactMatch", bool() );
    //QString configEntry_Depth = configgroupOptions.readEntry( "Depth", QString() );
    bool configentryTreewidgetExpressions = configgroupOptions.readEntry("ExpressionsViewer", bool());


    //  GoBack GoForward GoTop
    toolbuttonGoback = new QToolButton(this);
    toolbuttonGoback->setText("Go Back");
    toolbuttonGoback->setToolTip("Go back");
    toolbuttonGoback->setIcon(QIcon::fromTheme("go-previous"));
    toolbuttonGoback->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->mainToolBar->addWidget(toolbuttonGoback);
    QObject::connect(toolbuttonGoback, &QToolButton::clicked, this, &MainWindow::toolbuttonGobackClicked);

    toolbuttonGoforward = new QToolButton(this);
    toolbuttonGoforward->setText("Go Forward");
    toolbuttonGoforward->setToolTip("Go forward");
    toolbuttonGoforward->setIcon(QIcon::fromTheme("go-next"));
    toolbuttonGoforward->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->mainToolBar->addWidget(toolbuttonGoforward);
    QObject::connect(toolbuttonGoforward, &QToolButton::clicked, this, &MainWindow::toolbuttonGoforwardClicked);

    toolbuttonGoup = new QToolButton(this);
    toolbuttonGoup->setText("Go Up");
    toolbuttonGoup->setToolTip("Go up");
    toolbuttonGoup->setIcon(QIcon::fromTheme("go-up"));
    toolbuttonGoup->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->mainToolBar->addWidget(toolbuttonGoup);
    QObject::connect(toolbuttonGoup, &QToolButton::clicked, this, &MainWindow::toolbuttonGoupClicked);

    //  Add Seperator
    ui->mainToolBar->addSeparator();

    //  Horizontal Vertical & Tree View
    toolbuttonHorizontalview = new QToolButton(this);
    toolbuttonHorizontalview->setText("Horizontal View");
    toolbuttonHorizontalview->setToolTip("Horizontal view mode");
    toolbuttonHorizontalview->setIcon(QIcon::fromTheme("view-list-icons"));
    //toolButton_horizontalView->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonHorizontalview->setCheckable(true);
    if (configentryHorizontalview){
       toolbuttonHorizontalview->setChecked(true);
    }else{
        toolbuttonHorizontalview->setChecked(false);
    }
    ui->mainToolBar->addWidget(toolbuttonHorizontalview);
    QObject::connect(toolbuttonHorizontalview, &QToolButton::clicked, this, &MainWindow::toolbuttonHorizontalviewClicked);

    toolbuttonVerticalview= new QToolButton(this);
    toolbuttonVerticalview->setText("Vertical View");
    toolbuttonVerticalview->setToolTip("Vertical view mode");
    toolbuttonVerticalview->setIcon(QIcon::fromTheme("view-list-details"));
    //toolButton_verticalView->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonVerticalview->setCheckable(true);
    if (configentryVerticalview){
       toolbuttonVerticalview->setChecked(true);
    }else{
        toolbuttonVerticalview->setChecked(false);
    }
    ui->mainToolBar->addWidget(toolbuttonVerticalview);
    QObject::connect(toolbuttonVerticalview, &QToolButton::clicked, this, &MainWindow::toolbuttonVerticalviewClicked);

    toolbuttonTreeview = new QToolButton(this);
    toolbuttonTreeview->setText("Tree View");
    toolbuttonTreeview->setToolTip("Tree view mode");
    toolbuttonTreeview->setIcon(QIcon::fromTheme("view-list-tree"));
    //toolButton_treeView->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonTreeview->setCheckable(true);
    toolbuttonTreeview->setVisible(false);  //  TODO
    if (configentryTreeview){
       toolbuttonTreeview->setChecked(true);
    }else{
        toolbuttonTreeview->setChecked(false);
    }
    ui->mainToolBar->addWidget(toolbuttonTreeview);
    QObject::connect(toolbuttonTreeview, &QToolButton::clicked, this, &MainWindow::toolbuttonTreeviewClicked);

    //  If it's first time opening the program set default view to horizontal view
    if (!(toolbuttonHorizontalview->isChecked() || toolbuttonVerticalview->isChecked() || toolbuttonTreeview->isChecked())){
        toolbuttonHorizontalview->setChecked(true);
    }

    ui->mainToolBar->addSeparator();

    //  Match Files | Directories
    toolbuttonMatchfiles= new QToolButton(this);
    toolbuttonMatchfiles->setText("Files");
    toolbuttonMatchfiles->setToolTip("Match files");
    toolbuttonMatchfiles->setIcon(QIcon::fromTheme("document-new"));
    toolbuttonMatchfiles->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonMatchfiles->setCheckable(true);
    toolbuttonMatchfiles->setChecked(true);    // True by default
    if (!configentryMatchfiles){
        toolbuttonMatchfiles->setChecked(false);
    }
    ui->mainToolBar->addWidget(toolbuttonMatchfiles);
    QObject::connect(toolbuttonMatchfiles, &QToolButton::clicked, this, &MainWindow::toolbuttonMatchfilesClicked);

    toolbuttonMatchdirectories= new QToolButton(this);
    toolbuttonMatchdirectories->setText("Directories");
    toolbuttonMatchdirectories->setToolTip("Match directories");
    toolbuttonMatchdirectories->setIcon(QIcon::fromTheme("folder"));
    toolbuttonMatchdirectories->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonMatchdirectories->setCheckable(true);
    toolbuttonMatchdirectories->setChecked(true);  // True by default
    if (!configentryMatchdirectories){
        toolbuttonMatchdirectories->setChecked(false);
    }
    ui->mainToolBar->addWidget(toolbuttonMatchdirectories);
    QObject::connect(toolbuttonMatchdirectories, &QToolButton::clicked, this, &MainWindow::toolbuttonMatchdirectoriesClicked);

    //  Exact match
    toolbuttonExactmatch= new QToolButton(this);
    toolbuttonExactmatch->setText("Exact");
    toolbuttonExactmatch->setToolTip("Match whole name");
    toolbuttonExactmatch->setIcon(QIcon::fromTheme("zoom-select-x"));
    toolbuttonExactmatch->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbuttonExactmatch->setCheckable(true);
    toolbuttonExactmatch->setChecked(configentryExactmatch);
    ui->mainToolBar->addWidget(toolbuttonExactmatch);
    QObject::connect(toolbuttonExactmatch, &QToolButton::clicked, this, &MainWindow::toolbuttonExactmatchClicked);

    //  Depth
//    QStringList depthOptions = {"Surface", "Level 1", "Level 2", "Level 3", "Recursive"};
//    toolbuttonDepth = new QToolButton(this);
//    if (depthOptions.contains(configEntry_Depth)){
//        toolbuttonDepth->setText(configEntry_Depth);
//    }else{
//        toolbuttonDepth->setText(depthOptions.at(0));
//    }
//    toolbuttonDepth->setToolTip("Recursion depth");
//    toolbuttonDepth->setIcon(QIcon::fromTheme("dialog-layers"));
//    toolbuttonDepth->setPopupMode(QToolButton::InstantPopup);
//    toolbuttonDepth->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
//   // toolButton_Depth->setToolButtonStyle((ui->mainToolBar->toolButtonStyle()));
//    ui->mainToolBar->addWidget(toolbuttonDepth);

    //QMenu* menu_depth = new QMenu(toolbuttonDepth);

//    for (int i = 0; i < depthOptions.length(); i++){
//        QAction *action = new QAction(nullptr);
//        action->setText(depthOptions.at(i));
//        menu_depth->addAction(action);
//        QObject::connect(action, SIGNAL(triggered(bool)),
//            this, SLOT(depthChosen()));
//    }
//    QObject::connect(menu_depth, &QMenu::aboutToHide, this, &MainWindow::showResults);
//    toolbuttonDepth->setMenu(menu_depth);

    //  Control
    toolbuttonControl= new QToolButton(this);
    toolbuttonControl->setText("Control");
    //toolButton_Control->setToolTip("");
    toolbuttonControl->setIcon(QIcon::fromTheme("preferences-other"));
    toolbuttonControl->setPopupMode(QToolButton::InstantPopup);
    toolbuttonControl->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //toolButton_Control->setCheckable(false);
    //toolButton_Control->setChecked(true);  // True by default
    ui->mainToolBar->addWidget(toolbuttonControl);
    //QObject::connect(toolButton_Control, &QToolButton::clicked, this, &MainWindow::toolButton_matchDirectories_clicked);

    auto menu_control = new QMenu(toolbuttonControl);
    auto action_toggleTreeWidgetExpressions = new QAction(menu_control);
    action_toggleTreeWidgetExpressions->setText("Expression Viewer");
    action_toggleTreeWidgetExpressions->setCheckable(true);
    menu_control->addAction(action_toggleTreeWidgetExpressions);
    QObject::connect(action_toggleTreeWidgetExpressions, SIGNAL(triggered(bool)), this, SLOT(toggleTreewidgetExpressions(bool)));
    toggleTreewidgetExpressions(configentryTreewidgetExpressions);
    action_toggleTreeWidgetExpressions->setChecked(configentryTreewidgetExpressions);
    toolbuttonControl->setMenu(menu_control);

}
void MainWindow::initializeTreeWidget_Expressions(){
    ui->treeWidget_Expressions->setColumnCount(3);
    ui->treeWidget_Expressions->setAcceptDrops(true);
    ui->treeWidget_Expressions->setDragEnabled(true);
    ui->treeWidget_Expressions->setDragDropMode(QAbstractItemView::InternalMove);
    ui->treeWidget_Expressions->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //ui->treeWidget_Expressions->setSortingEnabled(true);
    //ui->treeWidget_Expressions->setHeaderHidden(true);
    //ui->treeWidget_Expressions->setRootIsDecorated(false);

    QStringList treeWidget_ExpressionsHeaders;
    treeWidget_ExpressionsHeaders << "Name" << "Search" << "Replace";
    ui->treeWidget_Expressions->setHeaderLabels(treeWidget_ExpressionsHeaders);

    int i=0;
    QString key = QString::number(i);
    while ( !configgroupExpressions.readXdgListEntry(key).empty()){
        loadsubtreeExpressions(nullptr, key);
        i++;
        key = QString::number(i);
    }

    ui->treeWidget_Expressions->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_Expressions, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(treewidgetExpressionsShowcontextmenu(const QPoint&)));

}
void MainWindow::initializeStatusBar(){
    //statusLabel_Depth = new QLabel(this);
    statuslabelNumberofmatches = new QLabel(this);
    statuslabelNumberofmatches->setAlignment(Qt::AlignRight);
    //statusLabel_Depth->setText("Depth 0");
    ui->layout_Status->addWidget(statuslabelNumberofmatches);
    //ui->statusBar->addWidget(statusLabel_NumberOfMatches);
    //ui->statusBar->addPermanentWidget(statusLabel_Depth);
}
void MainWindow::initializeWindowPosition(){
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        )
    );
}

void MainWindow::resizeEvent(QResizeEvent* event){
    QMainWindow::resizeEvent(event);
    uiProportionsChanged();

}
void MainWindow::showEvent(QShowEvent *event){
    Q_UNUSED(event)
    static bool firstTimeShown = true;
    if (firstTimeShown){
        uiProportionsChanged();
    }else{
        firstTimeShown = false;
    }

}
void MainWindow::onSplitterSplittermoved(int pos, int index)
{
    Q_UNUSED(pos)
    Q_UNUSED(index)
    uiProportionsChanged();
}
void MainWindow::uiProportionsChanged(){
    //int scrollbarWidth2 = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);
   // int scrollbarWidth = ui->treeWidget_Results->verticalScrollBar()->width();

    ui->treeWidget_Results->setColumnWidth(0, ui->treeWidget_Results->width()/2);
    ui->treeWidget_Results->setColumnWidth(1, ui->treeWidget_Results->width()/2);
    /*
    if (ui->treeWidget_Results->horizontalScrollBar()->visibleRegion().isEmpty()){
        ui->treeWidget_Results->setColumnWidth(0, ui->treeWidget_Results->width()/2);
        ui->treeWidget_Results->setColumnWidth(1, ui->treeWidget_Results->width()/2);
    }
    else{
        ui->treeWidget_Results->setColumnWidth(0, (ui->treeWidget_Results->width() - 2*scrollbarWidth)/2);
        ui->treeWidget_Results->setColumnWidth(1, (ui->treeWidget_Results->width() - 2*scrollbarWidth)/2);
    }
*/


    ui->treeWidget_Expressions->setColumnWidth(0,ui->treeWidget_Expressions->width()/2);
    ui->treeWidget_Expressions->setColumnWidth(1,ui->treeWidget_Expressions->width()/4);
    ui->treeWidget_Expressions->setColumnWidth(2,ui->treeWidget_Expressions->width()/4);
}

void MainWindow::toolbuttonHorizontalviewClicked(){
    toolbuttonHorizontalview->setChecked(true);
    toolbuttonVerticalview->setChecked(false);
    toolbuttonTreeview->setChecked(false);

    showResults();
}
void MainWindow::toolbuttonVerticalviewClicked(){
    toolbuttonHorizontalview->setChecked(false);
    toolbuttonVerticalview->setChecked(true);
    toolbuttonTreeview->setChecked(false);

    showResults();
}
void MainWindow::toolbuttonTreeviewClicked(){
    toolbuttonHorizontalview->setChecked(false);
    toolbuttonVerticalview->setChecked(false);
    toolbuttonTreeview->setChecked(true);

    showResults();
}
void MainWindow::toolbuttonMatchfilesClicked(){
   // bool configEntry_MatchFiles = configgroupOptions.readEntry( "MatchFiles", bool() );
    showResults();
}
void MainWindow::toolbuttonMatchdirectoriesClicked(){
 //   bool configEntry_MatchDirectories = configgroupOptions.readEntry( "MatchDirectories", bool() );
    showResults();

}
void MainWindow::toolbuttonExactmatchClicked(){
  //  bool configEntry_ExactMatch = configgroupOptions.readEntry( "ExactMatch", bool() );
    showResults();
}



//void MainWindow::depthChosen(){
////    QAction *action = (QAction *) QObject::sender();
////    toolbuttonDepth->setText(action->text());

//    //QMenu *menu_depth = (QMenu *)action->parent();


//}
void MainWindow::toggleTreewidgetExpressions(bool state){
    if (state){
        QList<int> sizes = {1,2};
        ui->splitter->setSizes(sizes);
        for (int i = 0; i < ui->splitter->count(); i++)
        {
            QSplitterHandle *splitter_handle = ui->splitter->handle(i);
            splitter_handle->setEnabled(true);
        }
    }else{
        QList<int> sizes = {0,2};
        ui->splitter->setSizes(sizes);
        for (int i = 0; i < ui->splitter->count(); i++)
        {
            QSplitterHandle *splitter_handle = ui->splitter->handle(i);
            splitter_handle->setEnabled(false);
        }
    }


}

void MainWindow::setLocationUrl(const QString &url){
    QUrl test(url);
    if (test.isValid()){
        locationUrl.setUrl(url);
        //  QUrlNavigator won't work as expected if we don't have proper sceme in our url
        if (locationUrl.scheme()==""){
            locationUrl.setScheme("file");
        }
        urlNavigator->setLocationUrl(locationUrl);
        showResults();
    }
}
void MainWindow::urlChanged(){
    setLocationUrl(urlNavigator->locationUrl().toString());
}

void MainWindow::showResults(){
    QString search = ui->lineEdit_Search->text();
    QString replace = ui->lineEdit_Replace->text();
    QRegularExpression searchRegEx(search);

    //  If regex not valid
    if (!searchRegEx.isValid()){
        return;
    }

    //  We support only local filesystems for now
    QUrl locationUrl_copy = QUrl(locationUrl);
    if (locationUrl_copy.scheme() == "file"){
        locationUrl_copy.setScheme("");
    }else{
        return;
    }

    //  Exact match?
    bool exactMatch = toolbuttonExactmatch->isChecked();
    if (exactMatch){
        searchRegEx.setPattern("\\A(?:" + search + ")\\z");
    }

    //  Files?
    bool matchFiles = toolbuttonMatchfiles->isChecked();
    //  Directories
    bool matchDirectories = toolbuttonMatchdirectories->isChecked();


    //  Current Path
    QString directoryPath = locationUrl_copy.toString();
    QDir directory(directoryPath);

    //  Prepare the treeWidget that will show our results
    ui->treeWidget_Results->clear();
    if (toolbuttonHorizontalview->isChecked()){
        ui->treeWidget_Results->setColumnCount(2);
        QStringList treeWidget_ResultsHeaders;
        treeWidget_ResultsHeaders << "Before" << "After";
        ui->treeWidget_Results->setHeaderLabels(treeWidget_ResultsHeaders);
        ui->treeWidget_Results->setSortingEnabled(true);
        ui->treeWidget_Results->setHeaderHidden(false);
        ui->treeWidget_Results->setRootIsDecorated(false);
        ui->treeWidget_Results->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }else if(toolbuttonVerticalview->isChecked()){
        ui->treeWidget_Results->setColumnCount(1);
        ui->treeWidget_Results->setHeaderLabel("");
        ui->treeWidget_Results->setSortingEnabled(true);
        ui->treeWidget_Results->setHeaderHidden(true);
        ui->treeWidget_Results->setRootIsDecorated(false);
        ui->treeWidget_Results->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }


    int numOfItemsMatched=0;

    QList<QString> items = directory.entryList();
    for (auto & item : items){
        //  We need to exclude these special directories
        if (item != "." && item != ".."){

            QString itemPath = directory.path() + "/" + item;
            QFileInfo fileInfo(itemPath);

            if (fileInfo.isFile() && !matchFiles){
                continue;
            }
            if (fileInfo.isDir() && !matchDirectories){
                continue;
            }

            if (searchRegEx.match(item).hasMatch() || search == ""){
                QString cpy = item;
                if (search != ""){
                    cpy.replace(searchRegEx,replace);
                }

                numOfItemsMatched++;

                if (toolbuttonHorizontalview->isChecked()){
                    QTreeWidgetItem *treeWidgetItem;
                    if (fileInfo.isFile()){
                        treeWidgetItem = new QTreeWidgetItem(ui->treeWidget_Results, documentType);
                        treeWidgetItem->setIcon(0,QIcon::fromTheme("document-new"));
                    }else if(fileInfo.isDir()){
                        treeWidgetItem = new QTreeWidgetItem(ui->treeWidget_Results, folderType);
                        treeWidgetItem->setIcon(0,QIcon::fromTheme("folder"));
                    }
                    else{
                        continue;
                    }
                    treeWidgetItem->setText(0,item);
                    treeWidgetItem->setText(1,cpy);
                }else if(toolbuttonVerticalview->isChecked()){
                    QTreeWidgetItem *treeWidgetItem_Before;
                    QTreeWidgetItem *treeWidgetItem_After;

                    if (fileInfo.isFile()){
                        treeWidgetItem_Before = new QTreeWidgetItem(ui->treeWidget_Results, documentType);
                        treeWidgetItem_Before->setIcon(0,QIcon::fromTheme("document-new"));
                        treeWidgetItem_After = new QTreeWidgetItem(ui->treeWidget_Results, documentType);
                        treeWidgetItem_After->setIcon(0,QIcon::fromTheme("document-new"));
                    }else if(fileInfo.isDir()){
                        treeWidgetItem_Before = new QTreeWidgetItem(ui->treeWidget_Results, folderType);
                        treeWidgetItem_Before->setIcon(0,QIcon::fromTheme("folder"));
                        treeWidgetItem_After = new QTreeWidgetItem(ui->treeWidget_Results, folderType);
                        treeWidgetItem_After->setIcon(0,QIcon::fromTheme("folder"));
                    }else{
                        continue;
                    }
                    treeWidgetItem_Before->setText(0,item);
                    treeWidgetItem_After->setText(0,item);
                }
            }
        }
    }

    if (search != ""){
        if (numOfItemsMatched ==1){
            statuslabelNumberofmatches->setText(QStringLiteral("%1 match").arg(numOfItemsMatched));
        }
        else if (numOfItemsMatched >1){
            statuslabelNumberofmatches->setText(QStringLiteral("%1 matches").arg(numOfItemsMatched));
        }else{
            statuslabelNumberofmatches->setText("No matches");
        }
    }else{
        statuslabelNumberofmatches->setText("No expression to match against");
    }

    uiProportionsChanged();
}
void MainWindow::on_lineEdit_Search_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    showResults();
}
void MainWindow::on_lineEdit_Replace_textChanged(const QString &arg1)
{
     Q_UNUSED(arg1)
     showResults();
}

void MainWindow::toolbuttonGobackClicked()
{
        urlNavigator->goBack();
}
void MainWindow::toolbuttonGoforwardClicked()
{
    urlNavigator->goForward();
}
void MainWindow::toolbuttonGoupClicked()
{
    urlNavigator->goUp();
}

void MainWindow::treewidgetExpressionsShowcontextmenu(const QPoint& arg1){


       //   For other widgets
       //QPoint globalPosition = ui->treeWidget_Expressions->mapToGlobal(arg1);
       //   for QAbstractScrollArea and derived classes

    QMenu menu;
    if (ui->treeWidget_Expressions->indexAt(arg1).isValid() && !ui->treeWidget_Expressions->selectedItems().isEmpty()){

        if (ui->treeWidget_Expressions->selectedItems().at(0)->type() == folderType){
            menu.addAction("Add Folder");
            menu.addAction("Add Search/Replace Expression");
            menu.addSeparator();
            menu.addAction("Delete Folder");

            QPoint globalPosition = ui->treeWidget_Expressions->viewport()->mapToGlobal(arg1);
            QAction* selectedAction = menu.exec(globalPosition);
            if (selectedAction)
            {
                if (selectedAction->text() == "Add Folder"){
                    this->addFolder(ui->treeWidget_Expressions->selectedItems().at(0));

                }
                else if(selectedAction->text() == "Add Search/Replace Expression"){
                    this->addSearchReplaceExpression(ui->treeWidget_Expressions->selectedItems().at(0));
                }
                else if(selectedAction->text() == "Delete Folder"){
                    delete ui->treeWidget_Expressions->selectedItems().at(0);

                }
            }
        }
        else if (ui->treeWidget_Expressions->selectedItems().at(0)->type() == documentType){
            menu.addAction("Delete Expression");

            QPoint globalPosition = ui->treeWidget_Expressions->viewport()->mapToGlobal(arg1);
            QAction* selectedAction = menu.exec(globalPosition);
            if (selectedAction)
            {
                if (selectedAction->text() == "Delete Expression"){
                    delete ui->treeWidget_Expressions->selectedItems().at(0);
                }
            }
        }
        else{}

    }else{
        menu.addAction("Add Folder");
        menu.addAction("Add Search/Replace Expression");

        QPoint globalPosition = ui->treeWidget_Expressions->viewport()->mapToGlobal(arg1);
        QAction* selectedAction = menu.exec(globalPosition);
        if (selectedAction)
        {
            if (selectedAction->text() == "Add Folder"){
                this->addFolder(nullptr);

            }
            else if(selectedAction->text() == "Add Search/Replace Expression"){
                this->addSearchReplaceExpression(nullptr);
            }
        }

    }

}
QTreeWidgetItem* MainWindow::addFolder(QTreeWidgetItem *parent, const QString& name){
    QTreeWidgetItem *item;

    if (parent != nullptr){
        item = new QTreeWidgetItem(parent,folderType);
    }else{
        item = new QTreeWidgetItem(ui->treeWidget_Expressions,folderType);
    }

    item->setIcon(0,QIcon::fromTheme("folder"));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(0,name);

    treewidgetExpressionsModified = true;
    return item;
}
QTreeWidgetItem* MainWindow::addSearchReplaceExpression(QTreeWidgetItem *parent, const QString& name , const QString& search, const QString& replace){
    QTreeWidgetItem *item;

    if (parent != nullptr){
        item = new QTreeWidgetItem(parent,documentType);
    }else{
        item = new QTreeWidgetItem(ui->treeWidget_Expressions,documentType);
    }
    item->setIcon(0,QIcon::fromTheme("document-new"));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setText(0,name);
    item->setText(1,search);
    item->setText(2,replace);
    treewidgetExpressionsModified = true;
    return item;
}

void MainWindow::on_treeWidget_Expressions_itemSelectionChanged()
{
    ui->lineEdit_Search->setText(ui->treeWidget_Expressions->selectedItems().at(0)->text(1));
    ui->lineEdit_Replace->setText(ui->treeWidget_Expressions->selectedItems().at(0)->text(2));
}
void MainWindow::onPushbuttonApplyClicked()
{
    QString search = ui->lineEdit_Search->text();
    QString replace = ui->lineEdit_Replace->text();
    QRegularExpression searchRegEx(search);

    //  If regex not valid return
    if (!searchRegEx.isValid()){
        return;
    }

    //  We support only local filesystems for now
    QUrl locationUrl_copy = QUrl(locationUrl);
    if (locationUrl_copy.scheme() == "file"){
        locationUrl_copy.setScheme("");
    }else{
        return;
    }

    //  Exact match?
    bool exactMatch = toolbuttonExactmatch->isChecked();
    if (exactMatch){
        searchRegEx.setPattern("\\A(?:" + search + ")\\z");
    }

    //  Files?
    bool matchFiles = toolbuttonMatchfiles->isChecked();
    //  Directories
    bool matchDirectories = toolbuttonMatchdirectories->isChecked();


    //  Current Path
    QString directoryPath = locationUrl_copy.toString();
    QDir directory(directoryPath);


    QList<QString>::iterator i;
    QList<QString> items = directory.entryList();
    for (i = items.begin(); i != items.end(); i++){
        //  We need to exclude these special directories
        if (*i != "." && *i != ".."){

            QString itemPath = directory.path() + "/" + (*i);
            QFileInfo fileInfo(itemPath);

            if (fileInfo.isFile() && !matchFiles){
                continue;
            }
            if (fileInfo.isDir() && !matchDirectories){
                continue;
            }
            if (searchRegEx.match(*i).hasMatch()){
                QString cpy = *i;
                if (search != ""){
                    cpy.replace(searchRegEx,replace);
                }
                directory.rename(*i,cpy);
                showResults();
            }
        }
    }

    ui->lineEdit_Replace->setText("");
    ui->lineEdit_Replace->setText("");
}



void MainWindow::on_treeWidget_Results_itemSelectionChanged()
{
    if (!ui->treeWidget_Results->selectedItems().isEmpty()){
        QString path = locationUrl.path();
        QDir directory(path);

        QFileInfo fileInfo(directory,ui->treeWidget_Results->selectedItems().at(0)->text(0));
        if (fileInfo.isDir()){
            setLocationUrl(fileInfo.filePath());
        }

    }
}
