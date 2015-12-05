#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "xmlprocess.h"
class QLabel;
class QPushButton;
class QLineEdit;
class QSequentialAnimationGroup;
class QGraphicsOpacityEffect;
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    enum WordType{
        Retry,
        Pass,
        History,
        Over
    };

    void addNewWordFromFile(QString filePath);
    void addNewWordFromString(QString text);
    void getIniData();
    void initWidgets();
    void setAnimation();
    void addLearnedTimes(QString wordType, int currentTimes);
    void moveElement(QString sourceType, int idNum, WordType dest);
    void gradualMsg(QString msg);

    QLineEdit *lnNewWord;
    QLabel *labStateRetry;
    QLabel *labStatePass;
    QLabel *labStateHistory;
    QLabel *labWord;
    QLabel *labCommnet;
    QPushButton *pbnAdd;
    QPushButton *pbnShow;
    QPushButton *pbnRetry;
    QPushButton *pbnPass;
    QLabel *labGradual;
    QGraphicsOpacityEffect *gradualOpacitty;

    QGraphicsOpacityEffect *opacity;

    QSequentialAnimationGroup *animaion;

    const QString xmlPath;                  // 顺序不能颠倒
    XMLProcess xml;                         // 顺序不能颠倒

    QList<QStringList> wordsRetry;
    QList<QStringList> wordsPass;
    QList<QStringList> wordsHistory;

    WordType currentWordType;
    int wordLearnedTimes;
    bool wordsIsEmpty;

    int retryCurrentNum = -1;
    int passCurrentNum = -1;
    int historyCurrentNum = -1;

    const int LabWordWidth;
    const int LabWordHeight;

    QPoint offset;

private slots:
    void addNewWord();
    void showComment();
    void retry();
    void pass();
    void playAnimation();
    void clearGradualWidget();
    void learnAgain();
};

#endif // WIDGET_H
