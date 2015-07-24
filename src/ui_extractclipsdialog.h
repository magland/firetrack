/********************************************************************************
** Form generated from reading UI file 'extractclipsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXTRACTCLIPSDIALOG_H
#define UI_EXTRACTCLIPSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>

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
            ExtractClipsDialog->setObjectName(QStringLiteral("ExtractClipsDialog"));
        ExtractClipsDialog->resize(398, 272);
        verticalLayout_2 = new QVBoxLayout(ExtractClipsDialog);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(ExtractClipsDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        labels = new QLineEdit(ExtractClipsDialog);
        labels->setObjectName(QStringLiteral("labels"));

        formLayout->setWidget(0, QFormLayout::FieldRole, labels);

        label_2 = new QLabel(ExtractClipsDialog);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        clipsize = new QLineEdit(ExtractClipsDialog);
        clipsize->setObjectName(QStringLiteral("clipsize"));

        formLayout->setWidget(1, QFormLayout::FieldRole, clipsize);

        fixed_clipsize = new QCheckBox(ExtractClipsDialog);
        fixed_clipsize->setObjectName(QStringLiteral("fixed_clipsize"));
        fixed_clipsize->setChecked(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, fixed_clipsize);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        view_in_window = new QRadioButton(ExtractClipsDialog);
        view_in_window->setObjectName(QStringLiteral("view_in_window"));
        view_in_window->setChecked(true);

        horizontalLayout_2->addWidget(view_in_window);

        save_to_disk = new QRadioButton(ExtractClipsDialog);
        save_to_disk->setObjectName(QStringLiteral("save_to_disk"));

        horizontalLayout_2->addWidget(save_to_disk);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout_2);


        verticalLayout_2->addLayout(formLayout);

        buttonBox = new QDialogButtonBox(ExtractClipsDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
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
        ExtractClipsDialog->setWindowTitle(QApplication::translate("ExtractClipsDialog", "Dialog", 0));
        label->setText(QApplication::translate("ExtractClipsDialog", "Labels:", 0));
#ifndef QT_NO_TOOLTIP
        labels->setToolTip(QApplication::translate("ExtractClipsDialog", "Examples: all; 1; 1,2;", 0));
#endif // QT_NO_TOOLTIP
        labels->setText(QApplication::translate("ExtractClipsDialog", "all", 0));
        label_2->setText(QApplication::translate("ExtractClipsDialog", "Clip size:", 0));
        clipsize->setText(QApplication::translate("ExtractClipsDialog", "50", 0));
        fixed_clipsize->setText(QApplication::translate("ExtractClipsDialog", "Fixed clip size", 0));
        view_in_window->setText(QApplication::translate("ExtractClipsDialog", "View in window", 0));
        save_to_disk->setText(QApplication::translate("ExtractClipsDialog", "Save to disk", 0));
    } // retranslateUi

};

namespace Ui {
    class ExtractClipsDialog: public Ui_ExtractClipsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXTRACTCLIPSDIALOG_H
