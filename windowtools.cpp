#include "windowtools.h"
#include <QDebug>
#include <QToolButton>
#include <QHBoxLayout>

WindowTools::WindowTools(QWidget *parent) :
    QWidget(parent)
{
    this->setFixedSize(80, 20);

    tbnWindowClose = new QToolButton;
    tbnWindowMax = new QToolButton;
    tbnWindowMini = new QToolButton;

    tbnWindowClose->setFixedSize(12, 12);
    tbnWindowMax->setFixedSize(12, 12);
    tbnWindowMini->setFixedSize(12, 12);

    layout = new QHBoxLayout;
    layout->addWidget(tbnWindowMini);
    layout->addWidget(tbnWindowMax);
    layout->addWidget(tbnWindowClose);
    layout->setSpacing(10);
    layout->setMargin(0);
    this->setLayout(layout);

    connect(tbnWindowClose, SIGNAL(clicked()), parentWidget(), SLOT(close()));
    connect(tbnWindowMax, SIGNAL(clicked()), this, SLOT(windowSwitchMax()));
    connect(tbnWindowMini, SIGNAL(clicked()), parentWidget(), SLOT(showMinimized()));

    tbnWindowClose->setObjectName("close");
    tbnWindowMax->setObjectName("max");
    tbnWindowMini->setObjectName("mini");

    this->setStyleSheet(
                "#close{"
                    "border-image: url(:Images/windowClose);"
                "}"
                "#close:hover{"
                    "border-image: url(:Images/windowCloseHover);"
                "}"

                "#max{"
                    "border-image: url(:Images/windowMax);"
                "}"
                "#max:hover{"
                    "border-image: url(:Images/windowMaxHover);"
                "}"

                "#mini{"
                    "border-image: url(:Images/windowMini);"
                "}"
                "#mini:hover{"
                    "border-image: url(:Images/windowMiniHover);"
                "}"
                );
}

void WindowTools::windowSwitchMax()
{
    if (parentWidget()->parentWidget()->isMaximized())
    {
        parentWidget()->parentWidget()->showNormal();
        tbnWindowMax->setStyleSheet(
                    "#max{"
                        "border-image: url(:Images/windowMax);"
                    "}"
                    "#max:hover{"
                        "border-image: url(:Images/windowMaxHover);"
                    "}"
                    );
    }
    else
    {
        parentWidget()->parentWidget()->showMaximized();
        tbnWindowMax->setStyleSheet(
                    "#max{"
                        "border-image: url(:Images/windowNormal);"
                    "}"
                    "#max:hover{"
                        "border-image: url(:Images/windowNormalHover);"
                    "}"
                    );
    }
}
