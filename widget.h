#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSet>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
private:
    QPushButton* saveButton;
    QTableWidget* filesTable;
    QVBoxLayout* mainLayout;
    QPushButton* loadButton;
    QPushButton* removeButton;
    QHBoxLayout* buttonLayout;
    QSet<QString>* fileList;
private slots:
    void saveToFile();
    void removeFiles();
    void addFiles();
};

#endif // WIDGET_H
