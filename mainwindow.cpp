#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <hl7mllp.h>
QString P_ID,Pname,Page,Pgen,TName,Od_val,Res_OD,NrmlA,NrmlB,Unit,Date,Data_Time;
#include <QVector>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSqlDatabase mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("/home/pi/git/Bar_Data.db");
    if(!mydb.open())
        qDebug() << "Can't Connect to DB !";
    else
        qDebug() << "Connected Successfully to DB !";

     HL7MLLP sock( "192.168.137.1", "22222" );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QStringList headers5 = { "Patient ID", "Name", "Parameter name", "OD value", "Result OD", "Unit", "Date", "Time" };

    setTableModel(ui->tableView, "SELECT PID,PName,TName,ODVal,ResultOD,Unit,Date,Time FROM Reports", headers5);

}


void MainWindow::setTableModel(QTableView* tableView, const QString& queryStr, const QStringList& headers)
{
    auto* model = new QSqlQueryModel(this); // parented for cleanup
    QSqlQuery query(mydb);
    if (!query.exec(queryStr)) {
        qDebug() << "Query failed:";
        return;
    }
    model->setQuery(query);
    for (int i = 0; i < headers.size(); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
    }
    tableView->setModel(model);
    tableView->resizeColumnsToContents();
}


void MainWindow::on_pushButton_2_clicked()
{
    std::string data1 , data2 , data3, data4, data5,data6, data7, data8,data9 ;
    data1 = P_ID.toStdString(); //PID
    data2 = Pname.toStdString(); //PName
    data3 =  Page.toStdString();//PAge
    data4= Pgen.toStdString(); //PGender
    data5= TName.toStdString(); //TName
    data6 = Res_OD.toStdString(); //ResultOD
    data7 = Unit.toStdString(); //ResultOD
    data8 = NrmlA.toStdString(); //ResultOD
    data9 = NrmlB.toStdString(); //ResultOD

    std::string hl7Message = "MSH|^~\\&|MCA-11|MXLABS|||20250614170230||ORU^R01|||2.3|||||\r"
                             "PID|1|"+data1+"|||"+data2+"||"+data3+"|"+data4+"|\r"
                         "OBR|1|"+data1+"|MCA-11||||20250614170230||||||||||||||\r"
                "OBX|1|NM|MB1011^"+data5+"||"+data6+"|"+data7+"|"+data9+"-"+data8+"|||||||20250614170230||\r";

    HL7MLLP sock( "192.168.137.1", "22222" ); //IP , Port

    sock.send_msg_mllp( hl7Message );

}

void MainWindow::on_pushButton_3_clicked()
{
    QStringList List_PID, Pet_Name, Petage, PetGen, TesName, RangA, RangB, OD, Res, Uni, Dat, Tim;
    QSqlQuery qy;

    // Fetching all report data
    qy.prepare("SELECT PID, PName, PAge, PGender, TName, ANrmlRang, BNrmlRang, ODVal, ResultOD, Unit, Date, Time FROM Reports");
    if (!qy.exec()) {
        qDebug() << "Query Error:" ;
        return;
    }

    while (qy.next()) {
        List_PID.append(qy.value(0).toString());
        Pet_Name.append(qy.value(1).toString());
        Petage.append(qy.value(2).toString());
        PetGen.append(qy.value(3).toString());
        TesName.append(qy.value(4).toString());
        RangA.append(qy.value(5).toString());
        RangB.append(qy.value(6).toString());
        OD.append(qy.value(7).toString());
        Res.append(qy.value(8).toString());
        Uni.append(qy.value(9).toString());
        Dat.append(qy.value(10).toString());
        Tim.append(qy.value(11).toString());
    }

    // Grouping by PID
    QMap<QString, QList<int>> pidIndexMap;
    for (int i = 0; i < List_PID.size(); ++i) {
        pidIndexMap[List_PID[i]].append(i);
    }

    qDebug() << "=== Sending grouped HL7 Messages ===";

    for (auto it = pidIndexMap.begin(); it != pidIndexMap.end(); ++it) {
        const QString& pid = it.key();
        const QList<int>& indexes = it.value();

        if (indexes.isEmpty())
            continue;

        int firstIndex = indexes[0];  // Use the first entry for MSH, PID, OBR
        QString Patient_ID_Assign = List_PID[firstIndex];
        QString Patient_name_Assign = Pet_Name[firstIndex];
        QString Patient_age_Assign = Petage[firstIndex];
        QString Patient_Gender_Assign = PetGen[firstIndex];

        // Fixed date/time for this example; you can replace with QDateTime::currentDateTime().toString()
        QString datetime = "20250614170230";

        // Build MSH, PID, OBR
        std::string hl7Message = "MSH|^~\\&|MCA-11|MXLABS|||"+datetime.toStdString()+"||ORU^R01|||2.3|||||\r";
        hl7Message += "PID|1|" + Patient_ID_Assign.toStdString() + "|||" + Patient_name_Assign.toStdString()
                    + "||" + Patient_age_Assign.toStdString() + "|" + Patient_Gender_Assign.toStdString() + "|\r";
        hl7Message += "OBR|1|" + Patient_ID_Assign.toStdString() + "|MCA-11||||" + datetime.toStdString() + "||||||||||||||\r";

        // Add OBX segments
        int obxCounter = 1;
        for (int idx : indexes) {
            QString Testname_Assign = TesName[idx];
            QString ANrmlRang_Assign = RangA[idx];
            QString BNrmlRang_Assign = RangB[idx];
            QString ODVal_Assign = OD[idx];
            QString ResultOD_Assign = Res[idx];
            QString unit_Assign = Uni[idx];

            hl7Message += "OBX|" + std::to_string(obxCounter++) + "|NM|MB1011^" + Testname_Assign.toStdString()
                        + "||" + ResultOD_Assign.toStdString() + "|" + unit_Assign.toStdString() + "|"
                        + BNrmlRang_Assign.toStdString() + "-" + ANrmlRang_Assign.toStdString()
                        + "|||||||"+datetime.toStdString()+"||\r";
        }

        qDebug().noquote() << QString::fromStdString(hl7Message);

        // Optionally send message

        HL7MLLP sock("192.168.137.1", "22222");
        sock.send_msg_mllp(hl7Message);

    }
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{

    if (!index.isValid())
    {
        qDebug() << "Invalid index passed!";
        return;
    }
    int New_row=0;
    QSqlQueryModel * model = new QSqlQueryModel();
    QSqlQuery * qry=new QSqlQuery(mydb);
    qry->prepare("select PID,PName,TName,ODVal,ResultOD,Unit,Date,Time from Reports");
    qry->exec();
    model->setQuery(*qry);
    QModelIndexList indexList = ui->tableView->selectionModel()->selectedIndexes();
    foreach (const QModelIndex &index, indexList)
    {
        New_row = index.row();
    }
    P_ID = ui->tableView->model()->index(New_row , 0).data().toString();
    Pname = ui->tableView->model()->index(New_row , 1).data().toString();
    TName = ui->tableView->model()->index(New_row , 2).data().toString();
    Od_val = ui->tableView->model()->index(New_row , 3).data().toString();
    Res_OD = ui->tableView->model()->index(New_row , 4).data().toString();
    Unit = ui->tableView->model()->index(New_row , 5).data().toString();
    Date = ui->tableView->model()->index(New_row , 6).data().toString();
    Data_Time = ui->tableView->model()->index(New_row , 7).data().toString();
    QSqlQuery query;
    query.prepare("SELECT PAge, PGender, ANrmlRang, BNrmlRang FROM Reports WHERE PID = :pid AND Date = :date AND Time = :time");

    query.bindValue(":pid", P_ID);
    query.bindValue(":date", Date);
    query.bindValue(":time", Data_Time);
    if (query.exec()) {
        while(query.next())
        {
            Page = query.value(0).toString();
            Pgen = query.value(1).toString();
            NrmlA = query.value(2).toString();
            NrmlB = query.value(3).toString();
        }
    }
    qDebug()<<P_ID<<Pname<<Page<<Pgen<<TName<<Od_val<<Res_OD<<NrmlA<<NrmlB<<Unit<<Date<<Data_Time;
}

