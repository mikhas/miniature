/********************************************************************************
** Form generated from reading UI file 'miniature.ui'
**
** Created: Sun Nov 29 14:57:36 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef MINIATURE_H
#define MINIATURE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "src/board_view.h"

QT_BEGIN_NAMESPACE

class Ui_MMainWindow
{
public:
    QAction *new_game;
    QAction *next_move;
    QAction *prev_move;
    QAction *toggle_debug_output;
    QAction *about;
    QAction *show_move_list;
    QAction *show_board;
    QWidget *root;
    QStackedWidget *stack;
    QWidget *board_central;
    QWidget *player_box;
    QVBoxLayout *verticalLayout_3;
    QWidget *player_names;
    QHBoxLayout *horizontalLayout_3;
    QLabel *white_name;
    QLabel *black_name;
    QWidget *player_info;
    QHBoxLayout *horizontalLayout_4;
    QWidget *white_info;
    QVBoxLayout *verticalLayout_6;
    QLabel *white_rating;
    QLabel *white_material;
    QSpacerItem *verticalSpacer_4;
    QWidget *clock;
    QWidget *black_info;
    QVBoxLayout *verticalLayout_7;
    QLabel *black_rating;
    QLabel *black_material;
    QSpacerItem *verticalSpacer_5;
    Miniature::MBoardView *board_view;
    QWidget *info_box;
    QVBoxLayout *verticalLayout_2;
    QLabel *last_move;
    QTextEdit *debug;
    QPushButton *move_list_button;
    QSpacerItem *verticalSpacer;
    QWidget *move_list_central;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QTreeWidget *move_list;
    QPushButton *board_button;
    QSpacerItem *verticalSpacer_2;
    QMenuBar *menu_bar;
    QMenu *menu;
    QMenu *help;

    void setupUi(QMainWindow *MMainWindow)
    {
        if (MMainWindow->objectName().isEmpty())
            MMainWindow->setObjectName(QString::fromUtf8("MMainWindow"));
        MMainWindow->resize(480, 800);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MMainWindow->sizePolicy().hasHeightForWidth());
        MMainWindow->setSizePolicy(sizePolicy);
        MMainWindow->setMinimumSize(QSize(0, 0));
        new_game = new QAction(MMainWindow);
        new_game->setObjectName(QString::fromUtf8("new_game"));
        next_move = new QAction(MMainWindow);
        next_move->setObjectName(QString::fromUtf8("next_move"));
        prev_move = new QAction(MMainWindow);
        prev_move->setObjectName(QString::fromUtf8("prev_move"));
        toggle_debug_output = new QAction(MMainWindow);
        toggle_debug_output->setObjectName(QString::fromUtf8("toggle_debug_output"));
        about = new QAction(MMainWindow);
        about->setObjectName(QString::fromUtf8("about"));
        show_move_list = new QAction(MMainWindow);
        show_move_list->setObjectName(QString::fromUtf8("show_move_list"));
        show_board = new QAction(MMainWindow);
        show_board->setObjectName(QString::fromUtf8("show_board"));
        root = new QWidget(MMainWindow);
        root->setObjectName(QString::fromUtf8("root"));
        root->setMinimumSize(QSize(0, 0));
        stack = new QStackedWidget(root);
        stack->setObjectName(QString::fromUtf8("stack"));
        stack->setGeometry(QRect(0, 0, 480, 760));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(stack->sizePolicy().hasHeightForWidth());
        stack->setSizePolicy(sizePolicy1);
        board_central = new QWidget();
        board_central->setObjectName(QString::fromUtf8("board_central"));
        sizePolicy1.setHeightForWidth(board_central->sizePolicy().hasHeightForWidth());
        board_central->setSizePolicy(sizePolicy1);
        player_box = new QWidget(board_central);
        player_box->setObjectName(QString::fromUtf8("player_box"));
        player_box->setGeometry(QRect(0, 0, 480, 100));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(player_box->sizePolicy().hasHeightForWidth());
        player_box->setSizePolicy(sizePolicy2);
        player_box->setMinimumSize(QSize(0, 0));
        player_box->setMaximumSize(QSize(16777215, 100));
        verticalLayout_3 = new QVBoxLayout(player_box);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        player_names = new QWidget(player_box);
        player_names->setObjectName(QString::fromUtf8("player_names"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(player_names->sizePolicy().hasHeightForWidth());
        player_names->setSizePolicy(sizePolicy3);
        player_names->setMinimumSize(QSize(400, 50));
        horizontalLayout_3 = new QHBoxLayout(player_names);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinAndMaxSize);
        white_name = new QLabel(player_names);
        white_name->setObjectName(QString::fromUtf8("white_name"));
        QSizePolicy sizePolicy4(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(white_name->sizePolicy().hasHeightForWidth());
        white_name->setSizePolicy(sizePolicy4);
        white_name->setMinimumSize(QSize(200, 0));
        white_name->setMaximumSize(QSize(16777215, 16777215));
        QFont font;
        font.setPointSize(12);
        white_name->setFont(font);
        white_name->setLineWidth(0);
        white_name->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        white_name->setIndent(0);

        horizontalLayout_3->addWidget(white_name);

        black_name = new QLabel(player_names);
        black_name->setObjectName(QString::fromUtf8("black_name"));
        sizePolicy.setHeightForWidth(black_name->sizePolicy().hasHeightForWidth());
        black_name->setSizePolicy(sizePolicy);
        black_name->setMinimumSize(QSize(200, 0));
        black_name->setMaximumSize(QSize(16777215, 16777215));
        black_name->setFont(font);
        black_name->setLineWidth(0);
        black_name->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        black_name->setIndent(0);

        horizontalLayout_3->addWidget(black_name);


        verticalLayout_3->addWidget(player_names);

        player_info = new QWidget(player_box);
        player_info->setObjectName(QString::fromUtf8("player_info"));
        player_info->setMinimumSize(QSize(0, 0));
        horizontalLayout_4 = new QHBoxLayout(player_info);
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        white_info = new QWidget(player_info);
        white_info->setObjectName(QString::fromUtf8("white_info"));
        sizePolicy4.setHeightForWidth(white_info->sizePolicy().hasHeightForWidth());
        white_info->setSizePolicy(sizePolicy4);
        white_info->setMinimumSize(QSize(120, 0));
        white_info->setMaximumSize(QSize(120, 16777215));
        verticalLayout_6 = new QVBoxLayout(white_info);
        verticalLayout_6->setSpacing(0);
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        white_rating = new QLabel(white_info);
        white_rating->setObjectName(QString::fromUtf8("white_rating"));
        white_rating->setLineWidth(0);
        white_rating->setIndent(0);

        verticalLayout_6->addWidget(white_rating);

        white_material = new QLabel(white_info);
        white_material->setObjectName(QString::fromUtf8("white_material"));
        white_material->setLineWidth(0);
        white_material->setIndent(0);

        verticalLayout_6->addWidget(white_material);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer_4);


        horizontalLayout_4->addWidget(white_info);

        clock = new QWidget(player_info);
        clock->setObjectName(QString::fromUtf8("clock"));
        sizePolicy2.setHeightForWidth(clock->sizePolicy().hasHeightForWidth());
        clock->setSizePolicy(sizePolicy2);
        clock->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_4->addWidget(clock);

        black_info = new QWidget(player_info);
        black_info->setObjectName(QString::fromUtf8("black_info"));
        sizePolicy4.setHeightForWidth(black_info->sizePolicy().hasHeightForWidth());
        black_info->setSizePolicy(sizePolicy4);
        black_info->setMinimumSize(QSize(120, 0));
        black_info->setMaximumSize(QSize(120, 16777215));
        verticalLayout_7 = new QVBoxLayout(black_info);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        black_rating = new QLabel(black_info);
        black_rating->setObjectName(QString::fromUtf8("black_rating"));
        sizePolicy.setHeightForWidth(black_rating->sizePolicy().hasHeightForWidth());
        black_rating->setSizePolicy(sizePolicy);
        QFont font1;
        font1.setPointSize(10);
        black_rating->setFont(font1);
        black_rating->setLineWidth(0);
        black_rating->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        black_rating->setIndent(0);

        verticalLayout_7->addWidget(black_rating);

        black_material = new QLabel(black_info);
        black_material->setObjectName(QString::fromUtf8("black_material"));
        black_material->setLineWidth(0);
        black_material->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        black_material->setIndent(0);

        verticalLayout_7->addWidget(black_material);

        verticalSpacer_5 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(verticalSpacer_5);


        horizontalLayout_4->addWidget(black_info);


        verticalLayout_3->addWidget(player_info);

        board_view = new Miniature::MBoardView(board_central);
        board_view->setObjectName(QString::fromUtf8("board_view"));
        board_view->setGeometry(QRect(0, 100, 480, 480));
        sizePolicy.setHeightForWidth(board_view->sizePolicy().hasHeightForWidth());
        board_view->setSizePolicy(sizePolicy);
        board_view->setMinimumSize(QSize(480, 480));
        board_view->setFrameShape(QFrame::NoFrame);
        board_view->setLineWidth(0);
        board_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        board_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        info_box = new QWidget(board_central);
        info_box->setObjectName(QString::fromUtf8("info_box"));
        info_box->setGeometry(QRect(0, 580, 480, 220));
        sizePolicy1.setHeightForWidth(info_box->sizePolicy().hasHeightForWidth());
        info_box->setSizePolicy(sizePolicy1);
        info_box->setMinimumSize(QSize(0, 0));
        verticalLayout_2 = new QVBoxLayout(info_box);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        last_move = new QLabel(info_box);
        last_move->setObjectName(QString::fromUtf8("last_move"));
        last_move->setMinimumSize(QSize(0, 30));
        last_move->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        last_move->setIndent(10);

        verticalLayout_2->addWidget(last_move);

        debug = new QTextEdit(info_box);
        debug->setObjectName(QString::fromUtf8("debug"));
        debug->setMaximumSize(QSize(16777215, 60));
        debug->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        debug->setReadOnly(true);

        verticalLayout_2->addWidget(debug);

        move_list_button = new QPushButton(info_box);
        move_list_button->setObjectName(QString::fromUtf8("move_list_button"));

        verticalLayout_2->addWidget(move_list_button);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        stack->addWidget(board_central);
        move_list_central = new QWidget();
        move_list_central->setObjectName(QString::fromUtf8("move_list_central"));
        sizePolicy1.setHeightForWidth(move_list_central->sizePolicy().hasHeightForWidth());
        move_list_central->setSizePolicy(sizePolicy1);
        widget = new QWidget(move_list_central);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(0, 0, 480, 760));
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setMinimumSize(QSize(0, 0));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        move_list = new QTreeWidget(widget);
        move_list->setObjectName(QString::fromUtf8("move_list"));
        move_list->setMinimumSize(QSize(0, 615));

        verticalLayout->addWidget(move_list);

        board_button = new QPushButton(widget);
        board_button->setObjectName(QString::fromUtf8("board_button"));

        verticalLayout->addWidget(board_button);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        stack->addWidget(move_list_central);
        MMainWindow->setCentralWidget(root);
        menu_bar = new QMenuBar(MMainWindow);
        menu_bar->setObjectName(QString::fromUtf8("menu_bar"));
        menu_bar->setGeometry(QRect(0, 0, 480, 25));
        menu = new QMenu(menu_bar);
        menu->setObjectName(QString::fromUtf8("menu"));
        help = new QMenu(menu_bar);
        help->setObjectName(QString::fromUtf8("help"));
        MMainWindow->setMenuBar(menu_bar);

        menu_bar->addAction(menu->menuAction());
        menu_bar->addAction(help->menuAction());
        menu->addAction(new_game);
        menu->addAction(toggle_debug_output);
        help->addAction(about);

        retranslateUi(MMainWindow);

        QMetaObject::connectSlotsByName(MMainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MMainWindow)
    {
        MMainWindow->setWindowTitle(QApplication::translate("MMainWindow", "Miniature", 0, QApplication::UnicodeUTF8));
        new_game->setText(QApplication::translate("MMainWindow", "New Game", 0, QApplication::UnicodeUTF8));
        new_game->setShortcut(QApplication::translate("MMainWindow", "Ctrl+G", 0, QApplication::UnicodeUTF8));
        next_move->setText(QApplication::translate("MMainWindow", "Next Move", 0, QApplication::UnicodeUTF8));
        next_move->setShortcut(QApplication::translate("MMainWindow", "Right", 0, QApplication::UnicodeUTF8));
        prev_move->setText(QApplication::translate("MMainWindow", "Previous Move", 0, QApplication::UnicodeUTF8));
        prev_move->setShortcut(QApplication::translate("MMainWindow", "Left", 0, QApplication::UnicodeUTF8));
        toggle_debug_output->setText(QApplication::translate("MMainWindow", "Show/Hide Debug Output", 0, QApplication::UnicodeUTF8));
        about->setText(QApplication::translate("MMainWindow", "About", 0, QApplication::UnicodeUTF8));
        show_move_list->setText(QApplication::translate("MMainWindow", "Show move list", 0, QApplication::UnicodeUTF8));
        show_board->setText(QApplication::translate("MMainWindow", "show_board", 0, QApplication::UnicodeUTF8));
        white_name->setText(QApplication::translate("MMainWindow", "<name of white player>", 0, QApplication::UnicodeUTF8));
        black_name->setText(QApplication::translate("MMainWindow", "<name of black player>", 0, QApplication::UnicodeUTF8));
        white_rating->setText(QApplication::translate("MMainWindow", "<rating>", 0, QApplication::UnicodeUTF8));
        white_material->setText(QApplication::translate("MMainWindow", "<material>", 0, QApplication::UnicodeUTF8));
        black_rating->setText(QApplication::translate("MMainWindow", "<rating>", 0, QApplication::UnicodeUTF8));
        black_material->setText(QApplication::translate("MMainWindow", "<material>", 0, QApplication::UnicodeUTF8));
        last_move->setText(QApplication::translate("MMainWindow", "<last move>", 0, QApplication::UnicodeUTF8));
        move_list_button->setText(QApplication::translate("MMainWindow", "Show move list", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = move_list->headerItem();
        ___qtreewidgetitem->setText(2, QApplication::translate("MMainWindow", "Black", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(1, QApplication::translate("MMainWindow", "White", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("MMainWindow", "#", 0, QApplication::UnicodeUTF8));
        board_button->setText(QApplication::translate("MMainWindow", "Show board", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MMainWindow", "Main", 0, QApplication::UnicodeUTF8));
        help->setTitle(QApplication::translate("MMainWindow", "Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MMainWindow: public Ui_MMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MINIATURE_H
