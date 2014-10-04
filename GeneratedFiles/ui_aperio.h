/********************************************************************************
** Form generated from reading UI file 'aperio.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APERIO_H
#define UI_APERIO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_aperioClass
{
public:
    QAction *actionAbout;
    QAction *actionToggle;
    QAction *actionOpen;
    QAction *actionFullScreen;
    QAction *actionExit;
    QWidget *centralWidget;
    QPushButton *btnSearch;
    QLineEdit *txtSearchText;
    QPushButton *btnColor;
    QWidget *mainWidget;
    QSlider *shininessSlider;
    QPushButton *btnLight;
    QSlider *darknessSlider;
    QSlider *opacitySlider;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QListWidget *listWidget;
    QWidget *tab_2;
    QCheckBox *chkToroid;
    QSlider *phiSlider;
    QSlider *thetaSlider;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *lblThickness;
    QSlider *thicknessSlider;
    QWidget *tab;
    QCheckBox *chkCap;
    QCheckBox *chkDepthPeel;
    QCheckBox *chkWiggle;
    QLabel *label;
    QSlider *hingeSlider;
    QLineEdit *txtHingeAmount;
    QLabel *label_3;
    QLabel *label_4;
    QSlider *explodeSlider;
    QLineEdit *txtExplodeAmount;
    QSlider *slideSlider;
    QPushButton *btnHinge;
    QPushButton *btnExplode;
    QPushButton *btnSlide;
    QPushButton *btnHide;
    QPushButton *btnGlass;
    QPushButton *btnSlice;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *aperioClass)
    {
        if (aperioClass->objectName().isEmpty())
            aperioClass->setObjectName(QStringLiteral("aperioClass"));
        aperioClass->resize(881, 590);
        aperioClass->setStyleSheet(QLatin1String("QWidget{\n"
"	color: #dddddd;\n"
"	font: 10pt \"Lucida Sans\";\n"
"	selection-background-color: #FEE728;\n"
"}\n"
"\n"
"#centralWidget, QLineEdit, QSlider, QDial {\n"
"background: #111111;\n"
"}\n"
"QMenuBar {\n"
"	background-color:#3C3C3C;\n"
"	color: #ffffff;\n"
"	selection-color: #000000;\n"
"\n"
"	font: 10pt \"Lucida Sans\";\n"
"	padding: 5px;\n"
"}\n"
"QMenuBar::item:selected { /* when selected using mouse or keyboard */\n"
"	background: #FEE728;\n"
"	color: #444444;\n"
" }\n"
"\n"
"QStatusBar {\n"
"	background-color: #111111;\n"
"	color: #000;\n"
"	padding: 5px;\n"
"}\n"
"\n"
"QMenu  {\n"
"\n"
"	font: 10pt \"Lucida Sans\";\n"
"	/*	gridline-color: rgb(167, 207, 255);*/\n"
"	selection-color: #444444;\n"
"	color: #ffffff;\n"
"	background-color: #222222;\n"
"\n"
"/*	border: ;/*#565048;*/\n"
"\n"
"	border-left: 1px solid #aaaaaa;\n"
"	border-right: 1px solid #aaaaaa;\n"
"	border-bottom: 1px solid #aaaaaa;\n"
"}\n"
"\n"
"\n"
"#mainWidget {\n"
"	border: 2px solid #D0592A;\n"
"}\n"
"\n"
"QPushButton {\n"
"\n"
"b"
                        "ackground: rgba(86,80,72,150);\n"
"\n"
"border-radius: 4px;\n"
"color: #ffffff;\n"
"\n"
"padding: 5px;\n"
"\n"
"/*background: rgba(85, 120, 10, 150);*/\n"
"border: none;/*1px solid #808080;*/\n"
"/*border: 1px solid black;*/\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"/*background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #bbb);*/\n"
"background: #82766A;/* #6A8B0F;*/\n"
"\n"
"\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"/*background: qradialgradient(cx: 0.4, cy: -0.1,\n"
"fx: 0.4, fy: -0.1,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #ddd);*/\n"
"background: #474037;\n"
"}\n"
"\n"
"QCheckBox, QRadioButton {	\n"
" \n"
"	border-color: rgb(255, 255, 255);\n"
"	alternate-background-color: rgb(120, 255, 96);\n"
"}\n"
"\n"
"/** tabs ***/\n"
"#tab, #tab_2, #tab_3, QListView  {\n"
"	background: rgba(0, 0, 0, 0);\n"
"	color: #dddddd;\n"
"}\n"
"\n"
"QListView::indicator:checked\n"
"{\n"
"/*background-color: #988C84;\n"
"border: 1px solid #222222;*/\n"
"	background-image: url"
                        "(:/aperio/checked_list.png);\n"
"}\n"
"\n"
"QListView::indicator:unchecked\n"
"{\n"
"/*background: none;\n"
"border: 1px solid #222222;*/\n"
"	background-image: url(:/aperio/unchecked_list.png);\n"
"}\n"
"\n"
" QTabWidget::pane { /* The tab widget frame */\n"
"	border: none;\n"
"	background: rgba(57, 60, 46, 255);\n"
" }\n"
"\n"
"QTabBar::tab {\n"
"\n"
"color: #dddddd;\n"
"border-top-left-radius: 10px;\n"
"border-bottom-right-radius: 10px;\n"
"border-top-right-radius: 10px;\n"
"\n"
"margin-right: 2px;\n"
"\n"
"/*background: qradialgradient(cx: 0.3, cy: -0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #888);*/\n"
"\n"
"background: #524A44;\n"
"\n"
"     min-width: 8ex;\n"
"     /*padding: 5px;*/\n"
"	/*margin-right: 5px;*/\n"
"\n"
"	padding-right: 20px;\n"
"	padding-left: 20px;\n"
"	padding-top: 5px;\n"
"	padding-bottom: 5px;\n"
"}\n"
"\n"
"QTabBar::tab:hover\n"
"{\n"
"background: #CF7C2B;\n"
"color: #ffffff;\n"
"}\n"
"QTabBar::tab:selected  {\n"
"/*background: qradialgradient(cx: 0.3, cy: -"
                        "0.4,\n"
"fx: 0.3, fy: -0.4,\n"
"radius: 1.35, stop: 0 #fff, stop: 1 #ddd);*/\n"
"\n"
"background: rgba(208, 89, 42, 200);\n"
"\n"
"color: #ffffff;\n"
" }\n"
"\n"
"\n"
"QLineEdit {\n"
"color: black;\n"
"selection-color: black;\n"
"background: white;   \n"
"border: none;                     \n"
"} \n"
"\n"
" QCheckBox::indicator\n"
"  {\n"
"	width: 13px;\n"
"	height: 13px;\n"
"  }\n"
" QCheckBox::indicator:checked\n"
"  {\n"
"	background-image: url(:/aperio/checked_list.png);\n"
"/*	 border-left: 2px solid blue;*/\n"
"	/*image: url(:/additive/checked.gif);*/\n"
"  }\n"
"  QCheckBox::indicator:unchecked\n"
"  {\n"
"	background-image: url(:/aperio/unchecked_list.png);\n"
"/*	 border: 2px solid white;*/\n"
"	/*image: url(:/additive/unchecked.gif);*/\n"
"  }\n"
"\n"
"\n"
"\n"
"\n"
"\n"
"QScrollBar:vertical  {\n"
"\n"
"     background: #988C84;\n"
"     width: 15px;\n"
"     margin: 20px 0 20px 0;\n"
"/*	border-top: 1px solid white;\n"
"	border-bottom: 1px solid white;*/\n"
"\n"
" }\n"
" QScrollBar::handle:vertical  "
                        "{\n"
"     background: #392C23;\n"
"	min-height: 10px;\n"
"\n"
" }\n"
" QScrollBar::add-line:vertical  {\n"
"\n"
"     background: #7A6354;\n"
"     height: 20px;\n"
"     subcontrol-position: bottom;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" \n"
" QScrollBar::add-line:vertical:hover, QScrollBar::sub-line:vertical:hover   {\n"
"	background: #B09888;\n"
"\n"
"}\n"
"\n"
" QScrollBar::sub-line:vertical  {\n"
"/*	border: 2px solid #ffffff;*/\n"
"\n"
"     background: #7A6354;\n"
"     height: 20px;\n"
"     subcontrol-position: top;\n"
"     subcontrol-origin: margin;\n"
" }\n"
" QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical  {\n"
"     width: 3px;\n"
"     height: 4px;\n"
"     background: #392C23;\n"
" }\n"
"\n"
" QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical  {\n"
"     background: none;\n"
"}\n"
"\n"
"QSlider::handle:horizontal, QSlider::handle:vertical {\n"
" 	background-color: qlineargradient(spread:pad, x1:0.5, y1:1, x2:0.5, y2:0, stop:0 #FA7401, stop:1 #E39B51);\n"
"\n"
""
                        "background-color: rgba(86,80,72,200); /*#565048;*/\n"
"\n"
"	height: 1px;\n"
"	max-height: 1px;\n"
"/*	border-radius: 2px;*/\n"
"\n"
"/*	border: 1px solid #777777;*/\n"
"	border: 1px solid #524A44;\n"
"\n"
"\n"
"}\n"
"\n"
"QSlider::handle:horizontal:hover, QSlider::handle:vertical:hover {\n"
"background: #82766A;\n"
"\n"
"}"));
        actionAbout = new QAction(aperioClass);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionToggle = new QAction(aperioClass);
        actionToggle->setObjectName(QStringLiteral("actionToggle"));
        actionOpen = new QAction(aperioClass);
        actionOpen->setObjectName(QStringLiteral("actionOpen"));
        actionFullScreen = new QAction(aperioClass);
        actionFullScreen->setObjectName(QStringLiteral("actionFullScreen"));
        actionExit = new QAction(aperioClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        centralWidget = new QWidget(aperioClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        btnSearch = new QPushButton(centralWidget);
        btnSearch->setObjectName(QStringLiteral("btnSearch"));
        btnSearch->setEnabled(true);
        btnSearch->setGeometry(QRect(150, 90, 81, 20));
        txtSearchText = new QLineEdit(centralWidget);
        txtSearchText->setObjectName(QStringLiteral("txtSearchText"));
        txtSearchText->setEnabled(true);
        txtSearchText->setGeometry(QRect(20, 90, 121, 20));
        btnColor = new QPushButton(centralWidget);
        btnColor->setObjectName(QStringLiteral("btnColor"));
        btnColor->setEnabled(true);
        btnColor->setGeometry(QRect(150, 120, 81, 20));
        mainWidget = new QWidget(centralWidget);
        mainWidget->setObjectName(QStringLiteral("mainWidget"));
        mainWidget->setGeometry(QRect(20, 160, 841, 361));
        mainWidget->setCursor(QCursor(Qt::ArrowCursor));
        mainWidget->setMouseTracking(true);
        shininessSlider = new QSlider(centralWidget);
        shininessSlider->setObjectName(QStringLiteral("shininessSlider"));
        shininessSlider->setEnabled(true);
        shininessSlider->setGeometry(QRect(250, 50, 22, 91));
        shininessSlider->setMaximum(256);
        shininessSlider->setValue(47);
        shininessSlider->setOrientation(Qt::Vertical);
        shininessSlider->setInvertedAppearance(false);
        btnLight = new QPushButton(centralWidget);
        btnLight->setObjectName(QStringLiteral("btnLight"));
        btnLight->setEnabled(true);
        btnLight->setGeometry(QRect(20, 30, 100, 28));
        QIcon icon;
        icon.addFile(QStringLiteral(":/aperio/flashlight2.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnLight->setIcon(icon);
        btnLight->setIconSize(QSize(22, 22));
        darknessSlider = new QSlider(centralWidget);
        darknessSlider->setObjectName(QStringLiteral("darknessSlider"));
        darknessSlider->setEnabled(true);
        darknessSlider->setGeometry(QRect(280, 50, 22, 91));
        darknessSlider->setMaximum(96);
        darknessSlider->setValue(0);
        darknessSlider->setOrientation(Qt::Vertical);
        darknessSlider->setInvertedAppearance(false);
        opacitySlider = new QSlider(centralWidget);
        opacitySlider->setObjectName(QStringLiteral("opacitySlider"));
        opacitySlider->setEnabled(true);
        opacitySlider->setGeometry(QRect(139, 30, 91, 22));
        opacitySlider->setMaximum(100);
        opacitySlider->setValue(50);
        opacitySlider->setOrientation(Qt::Horizontal);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setEnabled(true);
        tabWidget->setGeometry(QRect(310, 10, 291, 131));
        QFont font;
        font.setFamily(QStringLiteral("Lucida Sans"));
        font.setPointSize(10);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        font.setStrikeOut(false);
        font.setKerning(true);
        tabWidget->setFont(font);
        tabWidget->setAutoFillBackground(false);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setTabShape(QTabWidget::Rounded);
        tabWidget->setElideMode(Qt::ElideNone);
        tabWidget->setDocumentMode(false);
        tabWidget->setMovable(true);
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        listWidget = new QListWidget(tab_3);
        listWidget->setObjectName(QStringLiteral("listWidget"));
        listWidget->setEnabled(true);
        listWidget->setGeometry(QRect(0, 0, 299, 100));
        QPalette palette;
        QBrush brush(QColor(221, 221, 221, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        QBrush brush1(QColor(0, 0, 0, 0));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Button, brush1);
        palette.setBrush(QPalette::Active, QPalette::Text, brush);
        palette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
        QBrush brush2(QColor(0, 0, 0, 255));
        brush2.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Active, QPalette::Base, brush2);
        palette.setBrush(QPalette::Active, QPalette::Window, brush1);
        QBrush brush3(QColor(254, 231, 40, 255));
        brush3.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Highlight, brush3);
        QBrush brush4(QColor(74, 77, 63, 255));
        brush4.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Button, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
        palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
        QBrush brush5(QColor(0, 0, 0, 255));
        brush5.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush5);
        palette.setBrush(QPalette::Inactive, QPalette::Window, brush1);
        palette.setBrush(QPalette::Inactive, QPalette::Highlight, brush3);
        palette.setBrush(QPalette::Inactive, QPalette::AlternateBase, brush4);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Disabled, QPalette::Button, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Text, brush);
        palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::NoBrush);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush6);
        palette.setBrush(QPalette::Disabled, QPalette::Window, brush1);
        palette.setBrush(QPalette::Disabled, QPalette::Highlight, brush3);
        palette.setBrush(QPalette::Disabled, QPalette::AlternateBase, brush4);
        listWidget->setPalette(palette);
        listWidget->setFocusPolicy(Qt::NoFocus);
        listWidget->setFrameShape(QFrame::NoFrame);
        listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        listWidget->setDragDropMode(QAbstractItemView::InternalMove);
        listWidget->setAlternatingRowColors(true);
        tabWidget->addTab(tab_3, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        chkToroid = new QCheckBox(tab_2);
        chkToroid->setObjectName(QStringLiteral("chkToroid"));
        chkToroid->setGeometry(QRect(20, 80, 81, 17));
        chkToroid->setAutoFillBackground(false);
        phiSlider = new QSlider(tab_2);
        phiSlider->setObjectName(QStringLiteral("phiSlider"));
        phiSlider->setGeometry(QRect(160, 20, 111, 22));
        phiSlider->setMinimum(1);
        phiSlider->setMaximum(100);
        phiSlider->setValue(2);
        phiSlider->setOrientation(Qt::Horizontal);
        phiSlider->setInvertedAppearance(false);
        phiSlider->setInvertedControls(false);
        phiSlider->setTickPosition(QSlider::NoTicks);
        thetaSlider = new QSlider(tab_2);
        thetaSlider->setObjectName(QStringLiteral("thetaSlider"));
        thetaSlider->setGeometry(QRect(160, 50, 111, 22));
        thetaSlider->setMinimum(3);
        thetaSlider->setMaximum(100);
        thetaSlider->setValue(3);
        thetaSlider->setOrientation(Qt::Horizontal);
        thetaSlider->setTickPosition(QSlider::NoTicks);
        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(20, 20, 101, 16));
        label_8 = new QLabel(tab_2);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(20, 50, 121, 16));
        lblThickness = new QLabel(tab_2);
        lblThickness->setObjectName(QStringLiteral("lblThickness"));
        lblThickness->setGeometry(QRect(110, 80, 121, 16));
        thicknessSlider = new QSlider(tab_2);
        thicknessSlider->setObjectName(QStringLiteral("thicknessSlider"));
        thicknessSlider->setEnabled(true);
        thicknessSlider->setGeometry(QRect(180, 80, 91, 16));
        thicknessSlider->setMinimum(1);
        thicknessSlider->setMaximum(100);
        thicknessSlider->setValue(40);
        thicknessSlider->setOrientation(Qt::Horizontal);
        thicknessSlider->setInvertedAppearance(false);
        thicknessSlider->setInvertedControls(false);
        thicknessSlider->setTickPosition(QSlider::NoTicks);
        tabWidget->addTab(tab_2, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        chkCap = new QCheckBox(tab);
        chkCap->setObjectName(QStringLiteral("chkCap"));
        chkCap->setGeometry(QRect(10, 20, 261, 20));
        chkCap->setChecked(true);
        chkDepthPeel = new QCheckBox(tab);
        chkDepthPeel->setObjectName(QStringLiteral("chkDepthPeel"));
        chkDepthPeel->setEnabled(true);
        chkDepthPeel->setGeometry(QRect(10, 50, 261, 20));
        chkDepthPeel->setChecked(false);
        chkWiggle = new QCheckBox(tab);
        chkWiggle->setObjectName(QStringLiteral("chkWiggle"));
        chkWiggle->setEnabled(true);
        chkWiggle->setGeometry(QRect(10, 80, 261, 20));
        chkWiggle->setChecked(true);
        tabWidget->addTab(tab, QString());
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(140, 20, 46, 13));
        hingeSlider = new QSlider(centralWidget);
        hingeSlider->setObjectName(QStringLiteral("hingeSlider"));
        hingeSlider->setEnabled(true);
        hingeSlider->setGeometry(QRect(660, 20, 101, 22));
        hingeSlider->setMaximum(100);
        hingeSlider->setValue(0);
        hingeSlider->setOrientation(Qt::Horizontal);
        hingeSlider->setTickPosition(QSlider::NoTicks);
        txtHingeAmount = new QLineEdit(centralWidget);
        txtHingeAmount->setObjectName(QStringLiteral("txtHingeAmount"));
        txtHingeAmount->setEnabled(true);
        txtHingeAmount->setGeometry(QRect(770, 20, 41, 20));
        txtHingeAmount->setLayoutDirection(Qt::LeftToRight);
        txtHingeAmount->setAlignment(Qt::AlignCenter);
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(250, 10, 21, 21));
        label_3->setPixmap(QPixmap(QString::fromUtf8(":/aperio/shiny.png")));
        label_3->setScaledContents(true);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(280, 10, 21, 21));
        label_4->setPixmap(QPixmap(QString::fromUtf8(":/aperio/rough.png")));
        label_4->setScaledContents(true);
        explodeSlider = new QSlider(centralWidget);
        explodeSlider->setObjectName(QStringLiteral("explodeSlider"));
        explodeSlider->setEnabled(true);
        explodeSlider->setGeometry(QRect(660, 70, 101, 22));
        explodeSlider->setMaximum(100);
        explodeSlider->setValue(0);
        explodeSlider->setOrientation(Qt::Horizontal);
        explodeSlider->setTickPosition(QSlider::NoTicks);
        txtExplodeAmount = new QLineEdit(centralWidget);
        txtExplodeAmount->setObjectName(QStringLiteral("txtExplodeAmount"));
        txtExplodeAmount->setEnabled(true);
        txtExplodeAmount->setGeometry(QRect(770, 70, 41, 20));
        txtExplodeAmount->setLayoutDirection(Qt::LeftToRight);
        txtExplodeAmount->setAlignment(Qt::AlignCenter);
        slideSlider = new QSlider(centralWidget);
        slideSlider->setObjectName(QStringLiteral("slideSlider"));
        slideSlider->setEnabled(true);
        slideSlider->setGeometry(QRect(660, 110, 101, 22));
        slideSlider->setMaximum(100);
        slideSlider->setValue(0);
        slideSlider->setOrientation(Qt::Horizontal);
        slideSlider->setTickPosition(QSlider::NoTicks);
        btnHinge = new QPushButton(centralWidget);
        btnHinge->setObjectName(QStringLiteral("btnHinge"));
        btnHinge->setEnabled(true);
        btnHinge->setGeometry(QRect(610, 10, 41, 41));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/aperio/hinge.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnHinge->setIcon(icon1);
        btnHinge->setIconSize(QSize(38, 38));
        btnExplode = new QPushButton(centralWidget);
        btnExplode->setObjectName(QStringLiteral("btnExplode"));
        btnExplode->setEnabled(true);
        btnExplode->setGeometry(QRect(610, 55, 41, 41));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/aperio/explode.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExplode->setIcon(icon2);
        btnExplode->setIconSize(QSize(37, 37));
        btnSlide = new QPushButton(centralWidget);
        btnSlide->setObjectName(QStringLiteral("btnSlide"));
        btnSlide->setEnabled(true);
        btnSlide->setGeometry(QRect(610, 100, 41, 41));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/aperio/sliding.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSlide->setIcon(icon3);
        btnSlide->setIconSize(QSize(34, 34));
        btnHide = new QPushButton(centralWidget);
        btnHide->setObjectName(QStringLiteral("btnHide"));
        btnHide->setEnabled(true);
        btnHide->setGeometry(QRect(20, 120, 121, 21));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/aperio/hide.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnHide->setIcon(icon4);
        btnHide->setIconSize(QSize(25, 20));
        btnGlass = new QPushButton(centralWidget);
        btnGlass->setObjectName(QStringLiteral("btnGlass"));
        btnGlass->setEnabled(true);
        btnGlass->setGeometry(QRect(820, 12, 41, 41));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/aperio/glass2.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGlass->setIcon(icon5);
        btnGlass->setIconSize(QSize(30, 30));
        btnSlice = new QPushButton(centralWidget);
        btnSlice->setObjectName(QStringLiteral("btnSlice"));
        btnSlice->setEnabled(true);
        btnSlice->setGeometry(QRect(770, 110, 41, 41));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/aperio/slice.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSlice->setIcon(icon6);
        btnSlice->setIconSize(QSize(50, 50));
        aperioClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(aperioClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 881, 31));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        aperioClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(aperioClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        aperioClass->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionToggle);
        menuFile->addAction(actionOpen);
        menuFile->addAction(actionFullScreen);
        menuFile->addSeparator();
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(aperioClass);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(aperioClass);
    } // setupUi

    void retranslateUi(QMainWindow *aperioClass)
    {
        aperioClass->setWindowTitle(QApplication::translate("aperioClass", "Aperio", 0));
#ifndef QT_NO_TOOLTIP
        aperioClass->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        actionAbout->setText(QApplication::translate("aperioClass", "About", 0));
        actionToggle->setText(QApplication::translate("aperioClass", "Toggle Preview", 0));
        actionToggle->setShortcut(QApplication::translate("aperioClass", "P", 0));
        actionOpen->setText(QApplication::translate("aperioClass", "Open", 0));
        actionOpen->setShortcut(QApplication::translate("aperioClass", "Ctrl+O", 0));
        actionFullScreen->setText(QApplication::translate("aperioClass", "FullScreen", 0));
        actionFullScreen->setShortcut(QApplication::translate("aperioClass", "Alt+Return", 0));
        actionExit->setText(QApplication::translate("aperioClass", "Exit", 0));
        btnSearch->setText(QApplication::translate("aperioClass", "Search", 0));
        txtSearchText->setPlaceholderText(QString());
        btnColor->setText(QApplication::translate("aperioClass", "Color", 0));
#ifndef QT_NO_TOOLTIP
        shininessSlider->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnLight->setToolTip(QApplication::translate("aperioClass", "Light (Forward-Scattered Translucency)", 0));
#endif // QT_NO_TOOLTIP
        btnLight->setText(QApplication::translate("aperioClass", "On", 0));
#ifndef QT_NO_TOOLTIP
        darknessSlider->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        opacitySlider->setToolTip(QApplication::translate("aperioClass", "Opacity", 0));
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("aperioClass", "Groups", 0));
        chkToroid->setText(QApplication::translate("aperioClass", "Toroidal", 0));
#ifndef QT_NO_TOOLTIP
        phiSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        thetaSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        label_7->setText(QApplication::translate("aperioClass", "Phi Roundness", 0));
        label_8->setText(QApplication::translate("aperioClass", "Theta Roundness", 0));
        lblThickness->setText(QApplication::translate("aperioClass", "Thickness", 0));
#ifndef QT_NO_TOOLTIP
        thicknessSlider->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("aperioClass", "Superquad", 0));
        chkCap->setText(QApplication::translate("aperioClass", "Cap Hole in Previewer", 0));
        chkDepthPeel->setText(QApplication::translate("aperioClass", "Depth Peeling for Translucency", 0));
        chkWiggle->setText(QApplication::translate("aperioClass", "Wiggle Selected Mesh", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("aperioClass", "Options", 0));
        label->setText(QApplication::translate("aperioClass", "Opacity:", 0));
#ifndef QT_NO_TOOLTIP
        hingeSlider->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        txtHingeAmount->setToolTip(QApplication::translate("aperioClass", "Max Hinge amount (degrees)", 0));
#endif // QT_NO_TOOLTIP
        txtHingeAmount->setText(QApplication::translate("aperioClass", "170", 0));
        txtHingeAmount->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        label_3->setToolTip(QApplication::translate("aperioClass", "Shininess", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QString());
#ifndef QT_NO_TOOLTIP
        label_4->setToolTip(QApplication::translate("aperioClass", "Roughness", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QString());
#ifndef QT_NO_TOOLTIP
        explodeSlider->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        txtExplodeAmount->setToolTip(QApplication::translate("aperioClass", "Max Explosion Amount (units)", 0));
#endif // QT_NO_TOOLTIP
        txtExplodeAmount->setText(QApplication::translate("aperioClass", "2", 0));
        txtExplodeAmount->setPlaceholderText(QString());
#ifndef QT_NO_TOOLTIP
        slideSlider->setToolTip(QApplication::translate("aperioClass", "Slide", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        btnHinge->setToolTip(QApplication::translate("aperioClass", "Hinge", 0));
#endif // QT_NO_TOOLTIP
        btnHinge->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnExplode->setToolTip(QApplication::translate("aperioClass", "Explode", 0));
#endif // QT_NO_TOOLTIP
        btnExplode->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnSlide->setToolTip(QApplication::translate("aperioClass", "Slide", 0));
#endif // QT_NO_TOOLTIP
        btnSlide->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnHide->setToolTip(QApplication::translate("aperioClass", "Show/Hide (H)", 0));
#endif // QT_NO_TOOLTIP
        btnHide->setText(QApplication::translate("aperioClass", "Show/Hide", 0));
#ifndef QT_NO_TOOLTIP
        btnGlass->setToolTip(QApplication::translate("aperioClass", "Toggle Previewer (T)", 0));
#endif // QT_NO_TOOLTIP
        btnGlass->setText(QString());
#ifndef QT_NO_TOOLTIP
        btnSlice->setToolTip(QApplication::translate("aperioClass", "Slice (L)", 0));
#endif // QT_NO_TOOLTIP
        btnSlice->setText(QString());
        menuFile->setTitle(QApplication::translate("aperioClass", "File", 0));
        menuHelp->setTitle(QApplication::translate("aperioClass", "Help", 0));
    } // retranslateUi

};

namespace Ui {
    class aperioClass: public Ui_aperioClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APERIO_H
