#include "widget.h"
#include <poppler/qt5/poppler-qt5.h>
#include <QHeaderView>
#include <QGridLayout>
#include <QFileDialog>
#include <QProcess>
#include <QString>
#include <QSet>
#include <iostream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    fileList = new QStringList();

    mainLayout = new QVBoxLayout(this);

    filesTable = new QTableWidget(0, 3);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Pages") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    filesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    filesTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);
    //filesTable->setContextMenuPolicy(Qt::CustomContextMenu);

    mainLayout->addWidget(filesTable);

    buttonLayout = new QHBoxLayout(this);
    mainLayout->addLayout(buttonLayout);

    loadButton = new QPushButton("Open...", this);
    connect(loadButton, SIGNAL(released()), this, SLOT(addFiles()));
    buttonLayout->addWidget(loadButton);

    removeButton = new QPushButton("Remove", this);
    connect(removeButton, SIGNAL(released()), this, SLOT(removeFiles()));
    buttonLayout->addWidget(removeButton);

    saveButton = new QPushButton("Save as...", this);
    connect(saveButton, SIGNAL(released()), this, SLOT(saveToFile()));
    buttonLayout->addWidget(saveButton);
}

void Widget::saveToFile()
{
    // input files
    QTableWidgetItem* currentItem;

    QString currentPath;

    QAbstractItemModel* model = filesTable->model();
    QModelIndex index;

    for( int row = 0; row < model->rowCount(); ++row )
    {


        //index = model->index(row, 0, QModelIndex());
        model->data(model->index(row,0), Qt::DisplayRole);
        fileList->append(QDir::toNativeSeparators(filesTable->item(row,0)->text()));
        std::cout << "bla: " << filesTable->item(row,0)->text().toUtf8().constData() << std::endl;
    }

    std::cout << "input files " << fileList->join(" ").toStdString() << std::endl;

    // output file
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setFileMode(QFileDialog::AnyFile);

    QString fileName;

    fileName = QDir::toNativeSeparators(dialog.getSaveFileName(this, tr("Save File"),
                                    "",
                                    tr("PDFs (*.pdf)")));

    std::cout << "Saving as " << fileName.toUtf8().constData() << std::endl;

    QProcess process;
    process.setProcessChannelMode(QProcess::ForwardedChannels);

    QStringList args;
    args << fileName;
    fileList->append(args);

    std::cout << "all args " << fileList->join(" ").toStdString() << std::endl;
    process.startDetached("pdfunite", QStringList() << *fileList);

}

void Widget::addFiles()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    QStringList files = dialog.getOpenFileNames(
                this,
                "Select one or more files to open",
                "~",
                "PDFs (*.pdf)");

    Poppler::Document* document;
    int numPages;

    for (int i = 0; i < files.size(); ++i) {
            const QString &fileName = files.at(i);
            const QString toolTip = QDir::toNativeSeparators(fileName);
            const QString absolutePath = fileName; //QDir::toNativeSeparators(currentDir.relativeFilePath(fileName));
            const qint64 size = QFileInfo(fileName).size();

            QTableWidgetItem *fileNameItem = new QTableWidgetItem(absolutePath);
            fileNameItem->setData(Qt::UserRole + 1, QVariant(fileName));
            fileNameItem->setToolTip(toolTip);
            fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

            document = Poppler::Document::load(fileName);
            numPages = document->numPages();
            QTableWidgetItem *pagesItem = new QTableWidgetItem(tr("%1").arg(numPages));
            pagesItem->setData(Qt::UserRole + 1, QVariant(numPages));
            pagesItem->setToolTip(toolTip);
            pagesItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
            delete document;

            QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1.1 MB")
                                                 .arg((size + 1023) / 1024 / 1024));
            sizeItem->setData(Qt::UserRole + 1, QVariant(fileName));
            sizeItem->setToolTip(toolTip);
            sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

            int row = filesTable->rowCount();
            filesTable->insertRow(row);
            filesTable->setItem(row, 0, fileNameItem);
            filesTable->setItem(row, 1, pagesItem);
            filesTable->setItem(row, 2, sizeItem);
        }
}

void Widget::removeFiles()
{
    QModelIndexList indices = filesTable->selectionModel()->selectedRows();

    for (QModelIndex index : indices) {
        filesTable->removeRow(index.row());
    }
}

Widget::~Widget()
{

}
