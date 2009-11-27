/********************************************************************************
** Form generated from reading UI file 'miniature.ui'
**
** Created: Fri Nov 27 03:38:00 2009
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
#include <QtGui/QSpacerItem>
#include <QtGui/QTextEdit>
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
    QWidget *root;
    QVBoxLayout *verticalLayout;
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
    QSpacerItem *verticalSpacer;
    QMenuBar *menu_bar;
    QMenu *menu;

    void setupUi(QMainWindow *MMainWindow)
    {
        if (MMainWindow->objectName().isEmpty())
            MMainWindow->setObjectName(QString::fromUtf8("MMainWindow"));
        MMainWindow->resize(480, 800);
        MMainWindow->setMinimumSize(QSize(480, 800));
        new_game = new QAction(MMainWindow);
        new_game->setObjectName(QString::fromUtf8("new_game"));
        next_move = new QAction(MMainWindow);
        next_move->setObjectName(QString::fromUtf8("next_move"));
        prev_move = new QAction(MMainWindow);
        prev_move->setObjectName(QString::fromUtf8("prev_move"));
        toggle_debug_output = new QAction(MMainWindow);
        toggle_debug_output->setObjectName(QString::fromUtf8("toggle_debug_output"));
        root = new QWidget(MMainWindow);
        root->setObjectName(QString::fromUtf8("root"));
        root->setMinimumSize(QSize(480, 800));
        verticalLayout = new QVBoxLayout(root);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        player_box = new QWidget(root);
        player_box->setObjectName(QString::fromUtf8("player_box"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(player_box->sizePolicy().hasHeightForWidth());
        player_box->setSizePolicy(sizePolicy);
        player_box->setMinimumSize(QSize(0, 0));
        player_box->setMaximumSize(QSize(16777215, 100));
        verticalLayout_3 = new QVBoxLayout(player_box);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        player_names = new QWidget(player_box);
        player_names->setObjectName(QString::fromUtf8("player_names"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(player_names->sizePolicy().hasHeightForWidth());
        player_names->setSizePolicy(sizePolicy1);
        player_names->setMinimumSize(QSize(400, 50));
        horizontalLayout_3 = new QHBoxLayout(player_names);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetMinAndMaxSize);
        white_name = new QLabel(player_names);
        white_name->setObjectName(QString::fromUtf8("white_name"));
        QSizePolicy sizePolicy2(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(white_name->sizePolicy().hasHeightForWidth());
        white_name->setSizePolicy(sizePolicy2);
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
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(black_name->sizePolicy().hasHeightForWidth());
        black_name->setSizePolicy(sizePolicy3);
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
        sizePolicy2.setHeightForWidth(white_info->sizePolicy().hasHeightForWidth());
        white_info->setSizePolicy(sizePolicy2);
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
        sizePolicy.setHeightForWidth(clock->sizePolicy().hasHeightForWidth());
        clock->setSizePolicy(sizePolicy);
        clock->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout_4->addWidget(clock);

        black_info = new QWidget(player_info);
        black_info->setObjectName(QString::fromUtf8("black_info"));
        sizePolicy2.setHeightForWidth(black_info->sizePolicy().hasHeightForWidth());
        black_info->setSizePolicy(sizePolicy2);
        black_info->setMinimumSize(QSize(120, 0));
        black_info->setMaximumSize(QSize(120, 16777215));
        verticalLayout_7 = new QVBoxLayout(black_info);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        black_rating = new QLabel(black_info);
        black_rating->setObjectName(QString::fromUtf8("black_rating"));
        sizePolicy3.setHeightForWidth(black_rating->sizePolicy().hasHeightForWidth());
        black_rating->setSizePolicy(sizePolicy3);
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


        verticalLayout->addWidget(player_box);

        board_view = new Miniature::MBoardView(root);
        board_view->setObjectName(QString::fromUtf8("board_view"));
        sizePolicy3.setHeightForWidth(board_view->sizePolicy().hasHeightForWidth());
        board_view->setSizePolicy(sizePolicy3);
        board_view->setMinimumSize(QSize(480, 480));
        board_view->setFrameShape(QFrame::NoFrame);
        board_view->setLineWidth(0);
        board_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        board_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        verticalLayout->addWidget(board_view);

        info_box = new QWidget(root);
        info_box->setObjectName(QString::fromUtf8("info_box"));
        QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(info_box->sizePolicy().hasHeightForWidth());
        info_box->setSizePolicy(sizePolicy4);
        info_box->setMinimumSize(QSize(0, 0));
        verticalLayout_2 = new QVBoxLayout(info_box);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        last_move = new QLabel(info_box);
        last_move->setObjectName(QString::fromUtf8("last_move"));
        last_move->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        last_move->setIndent(10);

        verticalLayout_2->addWidget(last_move);

        debug = new QTextEdit(info_box);
        debug->setObjectName(QString::fromUtf8("debug"));
        debug->setMaximumSize(QSize(16777215, 60));
        debug->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        debug->setReadOnly(true);

        verticalLayout_2->addWidget(debug);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        verticalLayout->addWidget(info_box);

        MMainWindow->setCentralWidget(root);
        menu_bar = new QMenuBar(MMainWindow);
        menu_bar->setObjectName(QString::fromUtf8("menu_bar"));
        menu_bar->setGeometry(QRect(0, 0, 480, 25));
        menu = new QMenu(menu_bar);
        menu->setObjectName(QString::fromUtf8("menu"));
        MMainWindow->setMenuBar(menu_bar);

        menu_bar->addAction(menu->menuAction());
        menu->addAction(new_game);
        menu->addAction(toggle_debug_output);

        retranslateUi(MMainWindow);
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
        white_name->setText(QApplication::translate("MMainWindow", "<name of white player>", 0, QApplication::UnicodeUTF8));
        black_name->setText(QApplication::translate("MMainWindow", "<name of black player>", 0, QApplication::UnicodeUTF8));
        white_rating->setText(QApplication::translate("MMainWindow", "<rating>", 0, QApplication::UnicodeUTF8));
        white_material->setText(QApplication::translate("MMainWindow", "<material>", 0, QApplication::UnicodeUTF8));
        black_rating->setText(QApplication::translate("MMainWindow", "<rating>", 0, QApplication::UnicodeUTF8));
        black_material->setText(QApplication::translate("MMainWindow", "<material>", 0, QApplication::UnicodeUTF8));
        last_move->setText(QApplication::translate("MMainWindow", "<last move>", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("MMainWindow", "Main", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MMainWindow: public Ui_MMainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // MINIATURE_H
