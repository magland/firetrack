/********************************************************************************
** Form generated from reading UI file 'extractclipsdialog.ui'
**
** Created: Fri Jul 24 08:35:01 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRACTCLIPSDIALOG_H
#define UI_EXTRACTCLIPSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ExtractClipsDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *labels;
    QLabel *label_2;
    QLineEdit *clipsize;
    QCheckBox *fixed_clipsize;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *view_in_window;
    QRadioButton *save_to_disk;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ExtractClipsDialog)
    {
        if (ExtractClipsDialog->objectName().isEmpty())
            ExtractClipsDialog->setObjectName(QString::fromUtf8("ExtractClipsDialog"));
        ExtractClipsDialog->resize(398, 272);
        verticalLayout_2 = new QVBoxLayout(ExtractClipsDialog);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(ExtractClipsDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        labels = new QLineEdit(ExtractClipsDialog);
        labels->setObjectName(QString::fromUtf8("labels"));

        formLayout->setWidget(0, QFormLayout::FieldRole, labels);

        label_2 = new QLabel(ExtractClipsDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        clipsize = new QLineEdit(ExtractClipsDialog);
        clipsize->setObjectName(QString::fromUtf8("clipsize"));

        formLayout->setWidget(1, QFormLayout::FieldRole, clipsize);

        fixed_clipsize = new QCheckBox(ExtractClipsDialog);
        fixed_clipsize->setObjectName(QString::fromUtf8("fixed_clipsize"));
        fixed_clipsize->setChecked(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, fixed_clipsize);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        view_in_window = new QRadioButton(ExtractClipsDialog);
        view_in_window->setObjectName(QString::fromUtf8("view_in_window"));
        view_in_window->setChecked(true);

        horizontalLayout_2->addWidget(view_in_window);

        save_to_disk = new QRadioButton(ExtractClipsDialog);
        save_to_disk->setObjectName(QString::fromUtf8("save_to_disk"));

        horizontalLayout_2->addWidget(save_to_disk);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout_2);


        verticalLayout_2->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(ExtractClipsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout_2->addWidget(buttonBox);


        retranslateUi(ExtractClipsDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), ExtractClipsDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), ExtractClipsDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(ExtractClipsDialog);
    } // setupUi

    void retranslateUi(QDialog *ExtractClipsDialog)
    {
        ExtractClipsDialog->setWindowTitle(QApplication::translate("ExtractClipsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ExtractClipsDialog", "Labels:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        labels->setToolTip(QApplication::translate("ExtractClipsDialog", "Examples: all; 1; 1,2;", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        labels->setText(QApplication::translate("ExtractClipsDialog", "all", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ExtractClipsDialog", "Clip size:", 0, QApplication::UnicodeUTF8));
        clipsize->setText(QApplication::translate("ExtractClipsDialog", "50", 0, QApplication::UnicodeUTF8));
        fixed_clipsize->setText(QApplication::translate("ExtractClipsDialog", "Fixed clip size", 0, QApplication::UnicodeUTF8));
        view_in_window->setText(QApplication::translate("ExtractClipsDialog", "View in window", 0, QApplication::UnicodeUTF8));
        save_to_disk->setText(QApplication::translate("ExtractClipsDialog", "Save to disk", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ExtractClipsDialog: public Ui_ExtractClipsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRACTCLIPSDIALOG_H
