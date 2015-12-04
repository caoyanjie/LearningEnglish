#include "widget.h"
#include "windowtools.h"
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QKeyEvent>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    ,LabWordWidth(200)
    ,LabWordHeight(200)
    ,xmlPath("word.xml")
    ,xml(xmlPath)
{
    this->setFixedSize(502, 400);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);

    getIniData();       // 获取配置文件数据
    initWidgets();      // 初始化部件
    setAnimation();     // 设置动画
    playAnimation();    // 播放动画
}

Widget::~Widget()
{
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        offset = event->globalPos() - this->pos();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        this->move(event->globalPos() - offset);
    }
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_Left:
        retry();
        break;
    case Qt::Key_Right:
        pass();
        break;
    case Qt::Key_Up:
    case Qt::Key_Down:
        showComment();
        break;
    default:
        break;
    }
}

// 从文件导入新单词
void Widget::addNewWordFromFile(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(0, "错误！", "文件打开失败！", QMessageBox::Ok);
        return;
    }

    QList<QMap<QString, QMap<QString, QString> > > elements;
    QList<QStringList> words;
    QStringList children;
    children << "English" << "Comment" << "Times";

    QTextStream out(&file);
    int line = 0;
    while(!out.atEnd())
    {
        line++;
        QString text = out.readLine();
        QStringList word = text.split(":");
        if (word.length() != 2)
        {
            QMessageBox msg(QMessageBox::Warning, "错误！", tr("文件第%1行格式错误！\n请用英文冒号(:)将单词和注释分开！").arg(line), QMessageBox::Ok | QMessageBox::Cancel);
            msg.setButtonText(QMessageBox::Ok, "继续");
            msg.setButtonText(QMessageBox::Cancel, "取消");
            int ret = msg.exec();
            line--;
            if (ret == QMessageBox::Ok)
            {
                continue;
            }
            else if (ret == QMessageBox::Cancel)
            {
                break;
            }
        }
        QString English = word.at(0);
        QString comment = word.at(1);

        QMap<QString, QMap<QString, QString> > element;
        QMap<QString, QString> attribute;
        attribute.insert("id", "null");
        element.insert(xml.WordElement, attribute);
        elements.append(element);

        QStringList textNode;
        textNode.append(English);
        textNode.append(comment);
        textNode.append("0");
        words.append(textNode);
    }
    xml.addRecursiveElement(xml.RetryElement, elements, children, words);

    gradualMsg(tr("添加成功！\n共添加%1个单词。").arg(line));
}

// 一字符串形式添加新单词
void Widget::addNewWordFromString(QString text)
{
    QStringList word = text.split(":");
    if (word.length() != 2)
    {
        QMessageBox::warning(0, "错误！", "请用英文冒号(:)将单词和注释分开！", QMessageBox::Ok);
        return;
    }
    QString English = word.at(0);
    QString comment = word.at(1);

    QList<QMap<QString, QMap<QString, QString> > > elements;
    QMap<QString, QMap<QString, QString> > element;
    QMap<QString, QString> attribute;
    attribute.insert("id", "null");
    element.insert(xml.WordElement, attribute);
    elements.append(element);

    QList<QStringList> words;
    QStringList textNode;
    textNode.append(English);
    textNode.append(comment);
    textNode.append("0");
    words.append(textNode);
    QStringList children;
    children << "English" << "Comment" << "Times";

    xml.addRecursiveElement(xml.RetryElement, elements, children, words);
    lnNewWord->clear();
}

// 获取配置文件数据
void Widget::getIniData()
{
    QStringList children;
    children << "English" << "Comment" << "Times";

    wordsRetry = xml.getChildrenText(xml.RetryElement, children);qDebug() << wordsRetry.size();
    wordsPass = xml.getChildrenText(xml.PassElement, children);qDebug() << wordsPass.size();
    wordsHistory = xml.getChildrenText(xml.HistoryElement, children);qDebug() << wordsHistory.size();
}

// 初始化部件
void Widget::initWidgets()
{
    QLabel *labBg = new QLabel(this);
    labBg->resize(this->size());
    labBg->setStyleSheet("border-image: url(:/Images/bg);");

    WindowTools *windowTool = new WindowTools(this);
    windowTool->move(this->width()-windowTool->width(), 5);

    lnNewWord = new QLineEdit(this);
    lnNewWord->setPlaceholderText("请用英文冒号(:)将单词和注释分开！");
    pbnAdd = new QPushButton("添加/导入 生词",  this);
    lnNewWord->resize(402, 20);
    pbnAdd->resize(95, 20);
    lnNewWord->move(2, 30);
    pbnAdd->move(405, 30);

    labStateRetry = new QLabel(this);
    labStatePass = new QLabel(this);
    labStateHistory = new QLabel(this);
    labStateRetry->move(70, 70);
    labStatePass->move(220, 70);
    labStateHistory->move(370, 70);

    labWord = new QLabel(this);
    labCommnet = new QLabel(this);
    labCommnet->setAlignment(Qt::AlignTop);
    labWord->setFixedSize(200, 25);
    labCommnet->setFixedSize(200, 100);
    labCommnet->move(150, 180);

    pbnShow = new QPushButton("提示", this);
    pbnRetry = new QPushButton("retry", this);
    pbnPass = new QPushButton("pass", this);

    opacity = new QGraphicsOpacityEffect;
    pbnShow->setGraphicsEffect(opacity);

    connect(pbnAdd, SIGNAL(clicked()), this, SLOT(addNewWord()));
    connect(pbnShow, SIGNAL(clicked()), this, SLOT(showComment()));
    connect(pbnRetry, SIGNAL(clicked()), this, SLOT(retry()));
    connect(pbnPass, SIGNAL(clicked()), this, SLOT(pass()));

    this->setObjectName("main");
    labBg->setObjectName("labBg");
    lnNewWord->setObjectName("lnNewWord");
    labWord->setObjectName("labWord");
    labCommnet->setObjectName("labComment");
    pbnAdd->setObjectName("pbnAdd");

    this->setStyleSheet(
                "#main, #labBg{"
                    "border: 1px solid white;"
                "}"
                "#lnNewWord{"
                    "background: rgba(0, 0, 0, 0);"
                    "color: white;"
                    "border: 1px solid rgba(255, 255, 255, 200);"
                    "padding: 0 0 0 4px;"
                    "border-top-left-radius: 10px;"
                    "border-bottom-left-radius: 10px;"
                    "border-top-right-radius: 0;"
                    "border-bottom-right-radius: 0;"
                "}"
                "#pbnAdd{"
                    "border-top-left-radius: 0;"
                    "border-bottom-left-radius: 0;"
                "}"
                "#labWord{"
                    "font-size: 16pt;"
                "}"
                "#labComment{"
                    "color: red;"
                    "font-size: 12pt;"
                "}"
                "QPushButton{"
                    "background: rgba(0, 0, 255, 120);"
                    "background:rgba(94, 169, 184, 190);"
                    "background:rgba(78, 142, 154, 190);"
                    "background:rgba(112, 203, 154, 221);"
                    "width: 75px;"
                    "height: 20px;"
                    "color: black;"
                    "border-radius: 10px;"
                "}"
                "QPushButton:hover{"
                    "background: rgba(0, 0, 255, 160);"
                    "background:rgba(94, 169, 184, 190);"
                "}"
                "QPushButton:pressed{"
                    "background: rgba(0, 0, 255, 120);"
                    "background:rgba(112, 203, 154, 221);"
                "}"
                "QLabel{"
                    "color: white;"
                "}"
                );
}

// 设置动画
void Widget::setAnimation()
{
    // 单词下落动画
    QPropertyAnimation *aniWord = new QPropertyAnimation(labWord, "pos");
    aniWord->setDuration(1000);
    aniWord->setStartValue(QPoint((this->width()-LabWordWidth)/2, -LabWordHeight));
    aniWord->setEndValue(QPoint((this->width()-LabWordWidth)/2, 150));

    // 并行动画组
    QPropertyAnimation *pbn1Anim = new QPropertyAnimation(pbnRetry, "pos");
    pbn1Anim->setDuration(1000);
    pbn1Anim->setStartValue(QPoint(-230, 300));
    pbn1Anim->setEndValue(QPoint(120, 300));

    QPropertyAnimation *pbn2Anim = new QPropertyAnimation(pbnPass, "pos");
    pbn2Anim->setDuration(1000);
    pbn2Anim->setStartValue(QPoint(-80, 300));
    pbn2Anim->setEndValue(QPoint(270, 300));

    QPropertyAnimation *animShow = new QPropertyAnimation(opacity, "opacity");
    animShow->setDuration(1000);
    animShow->setStartValue(0);
    animShow->setEndValue(1);

    QParallelAnimationGroup *animSeqGoup = new QParallelAnimationGroup;
    animSeqGoup->addAnimation(pbn1Anim);
    animSeqGoup->addAnimation(pbn2Anim);
    animSeqGoup->addAnimation(animShow);

    // 总串行动画组
    animaion = new QSequentialAnimationGroup;
    animaion->addAnimation(aniWord);
    animaion->addAnimation(animSeqGoup);
}

// 学习次数+1
void Widget::addLearnedTimes(QString wordType, int currentTimes)
{
    QMap<QString, QString> attribute;
    attribute.insert("id", tr("%1-%2").arg(wordType).arg(currentTimes));
    xml.alterElementTextByAttribute(xml.WordElement, attribute, "Times", tr("%1").arg(wordLearnedTimes+1));
}

// 移动元素
void Widget::moveElement(QString sourceType, int idNum, WordType dest)
{
    // remove
    {
        QMap<QString, QString> attribute;
        attribute.insert("id", tr("%1-%2").arg(sourceType).arg(idNum));
        xml.removeElementByAttribute(xml.WordElement, attribute);
    }

    // add
    QList<QMap<QString, QMap<QString, QString> > > elements;
    QMap<QString, QMap<QString, QString> > element;
    QMap<QString, QString> attribute;
    attribute.insert("id", "null");
    element.insert(xml.WordElement, attribute);
    elements.append(element);

    QList<QStringList> words;
    QStringList text;
    text.append(labWord->text());
    text.append(labCommnet->text());
    text.append("0");
    words.append(text);
    QStringList children;
    children << "English" << "Comment" << "Times";

    switch(dest)
    {
    case Retry:
        xml.addRecursiveElement(xml.RetryElement, elements, children, words);
        break;
    case Pass:
        xml.addRecursiveElement(xml.PassElement, elements, children, words);
        break;
    case History:
        xml.addRecursiveElement(xml.HistoryElement, elements, children, words);
        break;
    default:
        Q_ASSERT_X(false, "case index", "out of range!");
    }
}

// 渐变弹窗
void Widget::gradualMsg(QString msg)
{
    labGradual = new QLabel(msg, this);
    labGradual->move((this->width()-labGradual->width())/2, 80);
    labGradual->resize(100, 40);
    labGradual->setAlignment(Qt::AlignCenter);
    gradualOpacitty = new QGraphicsOpacityEffect;
    labGradual->setGraphicsEffect(gradualOpacitty);
    gradualOpacitty->setOpacity(0);
    labGradual->show();

    labGradual->setStyleSheet("background: rgba(255, 0, 0, 255); color: white;");

    QPropertyAnimation *animGradual = new QPropertyAnimation(gradualOpacitty, "opacity");
    animGradual->setDuration(4000);
    animGradual->setStartValue(0);
    animGradual->setKeyValueAt(0.5, 1);
    animGradual->setEndValue(0);
    connect(animGradual, SIGNAL(finished()), this, SLOT(clearGradualWidget()));
    animGradual->start(QPropertyAnimation::DeleteWhenStopped);
}

// 增加新单词
void Widget::addNewWord()
{
    QString text = lnNewWord->text();
    if (text.isEmpty())
    {
        QString filePath = QFileDialog::getOpenFileName(0);
        if (filePath.isEmpty())
        {
            return;
        }
        addNewWordFromFile(filePath);
    }
    else
    {
        addNewWordFromString(text);
    }
}

// 提示
void Widget::showComment()
{
    labCommnet->show();
}

// 不熟
void Widget::retry()
{
    switch(currentWordType)
    {
    case Retry:
        break;
    case Pass:
        moveElement("pass", passCurrentNum, Retry);
        break;
    case History:
        moveElement("history", historyCurrentNum, Pass);
        break;
    case Over:
        break;
    default:
        Q_ASSERT_X(false, "case index", "out of range!");
    }

    playAnimation();
}

// 已掌握
void Widget::pass()
{
    switch(currentWordType)
    {
    case Retry:
    {
        switch(wordLearnedTimes)
        {
        case 0:
        case 1:
        case 2:
        {
            addLearnedTimes("retry", retryCurrentNum);
            break;
        }
        case 3:
        {
            moveElement("retry", retryCurrentNum, Pass);
            break;
        }
        default:
            Q_ASSERT_X(false, "case index", "out of range!");
        }
        break;
    }
    case Pass:
        switch(wordLearnedTimes)
        {
        case 0:
        case 1:
        case 2:
        {
            addLearnedTimes("pass", passCurrentNum);
            break;
        }
        case 3:
        {
            moveElement("pass", passCurrentNum, History);
            break;
        }
        default:
            Q_ASSERT_X(false, "case index", "out of range!");
        }
        break;
    case History:
        addLearnedTimes("history", historyCurrentNum);
        break;
    case Over:
        break;
    default:
        Q_ASSERT_X(false, "case index", "out of range!");
    }

    playAnimation();
}

// 播放动画
void Widget::playAnimation()
{
    labCommnet->hide();
    pbnShow->move(400, 200);
    pbnRetry->move(-80, 300);
    pbnPass->move(-80, 300);
    opacity->setOpacity(0);

    labStateRetry->setText(tr("已掌握: %1").arg(wordsHistory.length()));
    labStatePass->setText(tr("待复习: %1").arg(wordsPass.length()));
    labStateHistory->setText(tr("要学习: %1").arg(wordsRetry.length()));

    QStringList word;
    if (wordsRetry.size() > 0)
    {
        word = wordsRetry.takeFirst();
        currentWordType = Retry;
        retryCurrentNum++;
    }
    else if (wordsPass.size() > 0)
    {
        word = wordsPass.takeFirst();
        currentWordType = Pass;
        passCurrentNum++;
    }
    else if (wordsHistory.size() > 0)
    {
        word = wordsHistory.takeFirst();
        currentWordType = History;
        historyCurrentNum++;
    }
    else
    {
        labWord->setText("恭喜，已学习完！");
        labCommnet->setText(" 请添加生词 ^_^ ！");
        currentWordType = Over;
    }
    if (!word.isEmpty())
    {
        labWord->setText(word.at(0));
        labCommnet->setText(word.at(1));
        wordLearnedTimes = word.at(2).toInt();
    }

    animaion->start();
}

void Widget::clearGradualWidget()
{
    labGradual->deleteLater();
    gradualOpacitty->deleteLater();
}
