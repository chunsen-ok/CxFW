#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QToolButton>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QScrollBar>
#include <QGraphicsDropShadowEffect>
#include <QListView>
#include <QModelIndex>
#include <QItemSelection>
#include <QItemSelectionModel>
#include <QDebug>
#include "cxstatusbar.h"
#include "cxbufferlistmodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_statusBar(new CxStatusBar(this))
    , m_listModel(new CxBufferListModel(this))
{
    initUI();

    showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    ui->setupUi(this);
    ui->listView->setModel(m_listModel);
    ui->listView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->contentLayout->addWidget(m_statusBar);
    ui->splitter->setStretchFactor(1,1);
    initStatusBar(m_statusBar);

    QLayout *layout = ui->contentWidget->layout();
    if (layout) {
        layout->setAlignment(Qt::AlignHCenter);
    }

    ui->markdownEdit->setWordWrapMode(QTextOption::WrapAnywhere);
    ui->markdownEdit->viewport()->setContentsMargins(100,10,100,10);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(ui->widget);
    effect->setBlurRadius(50);
    effect->setOffset(0);
    ui->widget->setGraphicsEffect(effect);

    reflectScrollBar();
    connect(ui->markdownEdit->document(),SIGNAL(modificationChanged(bool)),
            this,SLOT(onDocumentModificationChanged(bool)));
    connect(ui->listView->selectionModel(),SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),
            this, SLOT(setCurrent(const QItemSelection&, const QItemSelection&)));
}

void MainWindow::initStatusBar(CxStatusBar *statusbar)
{
    Q_UNUSED(statusbar)
    statusbar->addAction(ui->actionSwitchNavi);
}

void MainWindow::reflectScrollBar()
{
    QScrollBar *docBar = ui->markdownEdit->verticalScrollBar();
    QScrollBar *sideBar = ui->verticalScrollBar;

    connect(docBar,&QAbstractSlider::rangeChanged,[sideBar](int min, int max){
        sideBar->setRange(min,max);
    });
    connect(docBar,SIGNAL(valueChanged(int)),
            sideBar,SLOT(setValue(int)));

    connect(sideBar,&QAbstractSlider::actionTriggered,[docBar](int action){
        docBar->triggerAction(static_cast<QAbstractSlider::SliderAction>(action));
    });
    connect(sideBar,SIGNAL(sliderMoved(int)),
            docBar,SLOT(setValue(int)));

    docBar->setRange(0,1);
    docBar->setValue(0);
}

void MainWindow::on_actionOpenFile_triggered()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                           tr("Select files"),"","Markdown (*.md *.markdown)");

    for (const QString &fileName: fileNames) {
        QFileInfo info(fileName);
        openFile(info.absoluteFilePath());
    }

    if (!ui->listView->currentIndex().isValid()) {
        ui->listView->setCurrentIndex(m_listModel->index(m_listModel->rowCount()-1));
    }
}

void MainWindow::openFile(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QFile::ReadOnly)) {
        return;
    }

    m_listModel->appendRows({fileName});
}

void MainWindow::updateWindowTitle()
{
    QString titleStr;
    const QModelIndex curIndex = ui->listView->currentIndex();
    if (ui->markdownEdit->document()->isModified() && !curIndex.isValid()) {
        titleStr = QString("Untitled[*] - CxWriter");
    } else {
        if (!curIndex.isValid()) {
            titleStr = QString("CxWriter");
        } else {
            titleStr = QString("%1[*] - CxWriter").arg(curIndex.data(Qt::ToolTipRole).toString());
        }
    }
    setWindowTitle(titleStr);
}

void MainWindow::setCurrent(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected)

    QModelIndex idx = selected.indexes().value(0);
    CxFileBuffer *ptr = reinterpret_cast<CxFileBuffer*>(idx.internalPointer());
    if (ptr) {
        ui->markdownEdit->setPlainText(ptr->text());
    } else {
        ui->markdownEdit->setPlainText("");
    }
    updateWindowTitle();
}

void MainWindow::switchNavi(bool expand)
{
    if (expand) {
        ui->splitter->setSizes({300,ui->splitter->width() - 300});
    } else {
        ui->splitter->setSizes({0,ui->splitter->width()});
    }
    ui->actionSwitchNavi->setText(expand ? "<" : ">");
}

void MainWindow::onDocumentModificationChanged(bool changed)
{
    updateWindowTitle();
    setWindowModified(changed);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    return QMainWindow::eventFilter(obj,ev);
}

void MainWindow::on_actionSave_triggered()
{
    QModelIndex curIndex = ui->listView->currentIndex();
    QString filePath = curIndex.data(Qt::ToolTipRole).toString();
    if (!curIndex.isValid() && isWindowModified()) {
        filePath = QFileDialog::getSaveFileName(this,tr("Save file"),"","Markdown (*.md *.markdown)");
        if (filePath.isEmpty()) { return; }

        m_listModel->appendRows({filePath});

        const QModelIndex idx = m_listModel->index(m_listModel->rowCount()-1);
        curIndex = idx;
    }

    if (filePath.isEmpty() || !isWindowModified()) {
        return;
    }

    m_listModel->setData(curIndex,ui->markdownEdit->toPlainText(),Qt::DisplayRole);
    CxFileBuffer *buffer = reinterpret_cast<CxFileBuffer*>(curIndex.internalPointer());
    if (buffer) {
        buffer->save();
    }

    ui->listView->setCurrentIndex(curIndex);
    ui->markdownEdit->document()->setModified(false);
}

void MainWindow::on_actionNewFile_triggered()
{
    ui->listView->setCurrentIndex(QModelIndex());
}

void MainWindow::on_actionClose_triggered()
{
    QModelIndex curIndex = ui->listView->currentIndex();
    if (!curIndex.isValid() && !isWindowModified()) { return; }

    on_actionSave_triggered();

    curIndex = ui->listView->currentIndex();
    const int row = curIndex.row();
    ui->markdownEdit->clear();
    m_listModel->removeRows(row,1);

    const QModelIndex idx = m_listModel->index(row);
    ui->listView->setCurrentIndex(idx);
}

void MainWindow::on_actionSwitchNavi_triggered()
{
    const int s = ui->splitter->sizes().value(0);
    switchNavi(s == 0);
}
