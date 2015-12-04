#ifndef WINDOWTOOLS_H
#define WINDOWTOOLS_H

#include <QWidget>
class QToolButton;
class QHBoxLayout;
class WindowTools : public QWidget
{
    Q_OBJECT
public:
    explicit WindowTools(QWidget *parent = 0);

private:
    QToolButton *tbnWindowClose;
    QToolButton *tbnWindowMax;
    QToolButton *tbnWindowMini;
    QHBoxLayout *layout;

signals:

public slots:

private slots:
    void windowSwitchMax();

};

#endif // WINDOWTOOLS_H
