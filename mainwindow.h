//  Copyright (c) Mitropoulos Dimosthenis
//  Email   mitropoulosdimosthenis@gmail.com
//  License   GPL3
//  Feedback and bug reports are appreciated

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>

#include <QString>
#include <QRegularExpression>

#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QIcon>

#include <QMainWindow>
#include <QMenu>
#include <QToolButton>
#include <QSplitter>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QScrollBar>

//  Used for centering the window to the screen
#include <QStyle>
#include <QDesktopWidget>

//  KDE specific libraries
#include <KUrlNavigator>
#include <KConfig>
#include <KConfigGroup>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void setLocationUrl(const QString &url);

private slots:
    //  Automatically generated slots from ui editor
    void on_lineEdit_Search_textChanged(const QString &arg1);
    void on_lineEdit_Replace_textChanged(const QString &arg1);
    void on_treeWidget_Expressions_itemSelectionChanged();
    void on_treeWidget_Results_itemSelectionChanged();

    //  Handwritten  slots
    void initializeToolbar();
    void initializeKUrlNavigator();
    void initializeTreeWidget_Expressions();
    void initializeStatusBar();
    void initializeWindowPosition();

    void toolbuttonGobackClicked();
    void toolbuttonGoforwardClicked();
    void toolbuttonGoupClicked();
    void toolbuttonHorizontalviewClicked();
    void toolbuttonVerticalviewClicked();
    void toolbuttonTreeviewClicked();
    void toolbuttonMatchfilesClicked();
    void toolbuttonMatchdirectoriesClicked();
    void toolbuttonExactmatchClicked();
    //void depthChosen();
    void toggleTreewidgetExpressions(bool);

    void uiProportionsChanged();

    void treewidgetExpressionsShowcontextmenu(const QPoint&);
    void urlChanged();
    void showResults();


    void onPushbuttonApplyClicked();
    void onSplitterSplittermoved(int pos, int index);



private:
    Ui::MainWindow *ui;
    KUrlNavigator *urlNavigator = nullptr;
    QToolButton *toolbuttonGoback = nullptr;
    QToolButton *toolbuttonGoforward = nullptr;
    QToolButton *toolbuttonGoup = nullptr;
    QToolButton *toolbuttonHorizontalview = nullptr;
    QToolButton *toolbuttonVerticalview = nullptr;
    QToolButton *toolbuttonTreeview = nullptr;
    QToolButton *toolbuttonExactmatch = nullptr;
    QToolButton *toolbuttonMatchfiles = nullptr;
    QToolButton *toolbuttonMatchdirectories = nullptr;
   // QToolButton *toolbuttonDepth = nullptr;
    QToolButton *toolbuttonControl = nullptr;

    QLabel *statuslabelDepth = nullptr;
    QLabel *statuslabelSelectioninfo = nullptr;
    QLabel *statuslabelNumberofmatches = nullptr;

    QUrl locationUrl;

    KConfig config;
    KConfigGroup configgroupOptions;
    KConfigGroup configgroupExpressions;

    bool treewidgetExpressionsModified = false;
    void savesubtreeExpressions(QTreeWidgetItem *item, const QString &key);
    void loadsubtreeExpressions(QTreeWidgetItem *item, const QString &key);

    QTreeWidgetItem *addFolder(QTreeWidgetItem *parent = nullptr, const QString& name = "New Folder");
    QTreeWidgetItem *addSearchReplaceExpression(QTreeWidgetItem *parent = nullptr, const QString& name = "New Name", const QString& search = "Search Expression", const QString& replace = "Replace Epression");

    static constexpr int folderType = 1001;
    static constexpr int documentType = 1002;
protected:
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent *event) override;

};

#endif // MAINWINDOW_H
