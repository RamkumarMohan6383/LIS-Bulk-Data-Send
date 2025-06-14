#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QTableView>
#include <QSqlQueryModel>
extern QString P_ID,Pname,Page,Pgen,TName,Od_val,Res_OD,NrmlA,NrmlB,Unit,Date,Data_Time;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    QSqlDatabase mydb;

    void connclose()
    {
        mydb.close();

    }
    bool connOpen()
    {

        mydb = QSqlDatabase :: addDatabase("QSQLITE" , "First");
        mydb.setDatabaseName("/home/pi/git/Bar_Data.db");
        if(!mydb.open())
        {
            qDebug()<<("Failed topen the Database");
            return false;
        }
        else
        {
            qDebug()<<("Connected...");
            return true;
        }
    }
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void setTableModel(QTableView* tableView, const QString& queryStr, const QStringList& headers);


    void on_tableView_clicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
