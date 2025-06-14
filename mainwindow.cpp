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

    HL7MLLP sock( "192.168.137.1", "55" );
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

    HL7MLLP sock( "192.168.137.1", "55" ); //IP , Port

    sock.send_msg_mllp( hl7Message );

}

void MainWindow::on_pushButton_3_clicked()
{
    QAbstractItemModel *Model = ui->tableView->model();
    int rowcount=Model->rowCount();
    qDebug()<<"rcnt : "<<rowcount;
    QString Current_PID;
    std::string  Transfer_DT , Lab_name,Patient_ID,Patient_name,Patient_age,Patient_Gender,Testname,Test_DT,Result,Data_Testcode;
    QString PID , PName , PAge,PGender , TName, ANrmlRang, BNrmlRang,ODVal,ResultOD,unit,Date,Time;
    for (int row = 0; row < rowcount; ++row)
    {
        PID=Model->data(Model->index(row, 0)).toString();
        PName=Model->data(Model->index(row, 1)).toString();
        PAge=Model->data(Model->index(row, 2)).toString();
        PGender=Model->data(Model->index(row, 3)).toString();
        TName=Model->data(Model->index(row, 4)).toString();
        ANrmlRang=Model->data(Model->index(row, 5)).toString();
        BNrmlRang=Model->data(Model->index(row, 6)).toString();
        ODVal=Model->data(Model->index(row, 7)).toString();
        ResultOD=Model->data(Model->index(row, 8)).toString();
        unit=Model->data(Model->index(row, 9)).toString();
        Date=Model->data(Model->index(row, 10)).toString();
        Time=Model->data(Model->index(row, 11)).toString();

      }
    int Loop_da=1;
    QStringList List_PID,Pet_Name,Petage,PetGen,TesName,RangA,RangB,OD,Res,Uni,Dat,Tim;
    QSqlQuery qy;
    qy.prepare("SELECT PID,PName,PAge,PGender,TName,ANrmlRang,BNrmlRang,ODVal,ResultOD,Unit,Date,Time FROM Reports");
    qy.exec();
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

    // Map to track PID and their index positions
    QMap<QString, QList<int>> pidIndexMap;
    for (int i = 0; i < List_PID.size(); ++i) {
        pidIndexMap[List_PID[i]].append(i);
    }

    qDebug() << "PID Map: " << pidIndexMap;

    qDebug() << "=== Duplicates ===";
    for (auto it = pidIndexMap.begin(); it != pidIndexMap.end(); ++it) {
        if (it.value().size() > 1) {
            for (int idx : it.value()) {
                qDebug() << "Duplicate PID:" << it.key() << "i:" << idx<<" "<<"Pname : "<<Pet_Name[idx]<<"Page : "<<Petage[idx]<<"Pgen : "<<PetGen[idx];
                Patient_ID=List_PID[idx].toStdString();
                qDebug()<<"PatientID : "<<&Patient_ID;
                if(Loop_da==1)
                {
              /*  std::string hl7Message = "MSH|^~\\&|MCA-11|MXLABS|||20250614170230||ORU^R01|||2.3|||||\r"
                                         "PID|1|"+data1+"|||"+data2+"||"+data3+"|"+data4+"|\r"
                                                                                         "OBR|1|"+data1+"|MCA-11||||20250614170230||||||||||||||\r"
                  Loop_da=2;
                HL7MLLP sock( "192.168.137.1", "55" ); //IP , Port
                sock.send_msg_mllp( hl7Message );*/
                }
                else {
                  /*  std::string hl7Message ="OBX|1|NM|MB1011^"+data5+"||"+data6+"|"+data7+"|"+data9+"-"+data8+"|||||||20250614170230||\r";
                    HL7MLLP sock( "192.168.137.1", "55" ); //IP , Port
                    sock.send_msg_mllp( hl7Message );*/
                }


            }
        }
    }

    qDebug() << "=== Unique PIDs ===";
    for (auto it = pidIndexMap.begin(); it != pidIndexMap.end(); ++it) {
        if (it.value().size() == 1) {
            int idx = it.value().first();
            qDebug() << "Unique PID:" << it.key() << "i:" << idx;
         /*   std::string hl7Message = "MSH|^~\\&|MCA-11|MXLABS|||20250614170230||ORU^R01|||2.3|||||\r"
                                     "PID|1|"+data1+"|||"+data2+"||"+data3+"|"+data4+"|\r"
                                                                                     "OBR|1|"+data1+"|MCA-11||||20250614170230||||||||||||||\r"
                                                                                                    "OBX|1|NM|MB1011^"+data5+"||"+data6+"|"+data7+"|"+data9+"-"+data8+"|||||||20250614170230||\r";

            HL7MLLP sock( "192.168.137.1", "55" ); //IP , Port

            sock.send_msg_mllp( hl7Message );*/
        }
    }
}





      /*  QStringList List_PID;
        QSqlQuery qy;
        qy.prepare("SELECT PID FROM Reports");
        qy.exec();
        while (qy.next()) {
            List_PID.append(qy.value(0).toString());
        }
       /* for(int i=0;i<List_PID.size();i++)
        {

        }
        if (List_PID.contains(PID)) {
            qDebug() << "PID found:" << PID<<"i : "<<row;
        } else {
            qDebug() << "PID not found:" << PID;
        }*/

  //  }

    /* QAbstractItemModel *Model = ui->tableView_5->model();
    int rowcount=Model->rowCount();
    qDebug()<<"rcnt : "<<rowcount;
    QString Current_PID;
    std::string  Transfer_DT , Lab_name,Patient_ID,Patient_name,Patient_age,Patient_Gender,Testname,Test_DT,Result,Data_Testcode;
    QString PID , PName , PAge,PGender , TName, ANrmlRang, BNrmlRang,ODVal,ResultOD,unit,Date,Time;
    for (int row = 0; row < rowcount; ++row)
    {
        PID=Model->data(Model->index(row, 0)).toString();
        PName=Model->data(Model->index(row, 1)).toString();
        PAge=Model->data(Model->index(row, 2)).toString();
        PGender=Model->data(Model->index(row, 3)).toString();
        TName=Model->data(Model->index(row, 4)).toString();
        ANrmlRang=Model->data(Model->index(row, 5)).toString();
        BNrmlRang=Model->data(Model->index(row, 6)).toString();
        ODVal=Model->data(Model->index(row, 7)).toString();
        ResultOD=Model->data(Model->index(row, 8)).toString();
        unit=Model->data(Model->index(row, 9)).toString();
        Date=Model->data(Model->index(row, 10)).toString();
        Time=Model->data(Model->index(row, 11)).toString();


        QString System_date = ui->SysDate_Lbl->text();
        QString System_Time = ui->SysTime_Lbl->text();
        QString para_code;
        QSqlQuery qy;
        qy.prepare("select Pro_Code from test where name='"+TName+"'");
        qy.exec();
        while(qy.next())
            para_code=qy.value(0).toString();

        QByteArray ary;
        ary.append(Date);//Date
        std::string  Test_Date,pos1,pos2,pos3,pos4,pos5,pos6,pos7,pos8;
        std::string Test_Date1;
        pos1=ary[0];
        pos2=ary[1];
        pos3=ary[2];
        pos4=ary[3];
        pos5=ary[7];
        pos6=ary[8];
        pos7=ary[12];
        pos8=ary[13];
        Test_Date = pos1+pos2+pos3+pos4+pos5+pos6+pos7+pos8;

        QByteArray ary1;
        ary1.append(Time);//Time
        std::string Test_Time ,  post1,post2,post3,post4,post5,post6;
        post1=ary1[0];
        post2=ary1[1];
        post3=ary1[5];
        post4=ary1[6];
        post5=ary1[10];
        post6=ary1[11];
        Test_Time = post1+post2+post3+post4+post5+post6;

        QByteArray ary2;
        ary2.append(System_date);//SystemData
        std::string Transfer_Date ,  posd1,posd2,posd3,posd4,posd5,posd6,posd7,posd8;
        posd1=ary2[0];
        posd2=ary2[1];
        posd3=ary2[2];
        posd4=ary2[3];
        posd5=ary2[7];
        posd6=ary2[8];
        posd7=ary2[12];
        posd8=ary2[13];
        Transfer_Date = posd1+posd2+posd3+posd4+posd5+posd6+posd7+posd8;

        QByteArray ary3;
        ary3.append(System_Time);//SystemTime
        std::string Transfer_Time ,  posti1,posti2,posti3,posti4,posti5,posti6;
        posti1=ary3[0];
        posti2=ary3[1];
        posti3=ary3[5];
        posti4=ary3[6];
        posti5=ary3[10];
        posti6=ary3[11];
        Transfer_Time = posti1+posti2+posti3+posti4+posti5+posti6;

        std::string data1 , data2 , data3, data4, data5,data6, data7, data8 ;
        data1 = Transfer_Date;
        data2 = Transfer_Time;
        data3 = PID.toStdString(); //PID
        data4 = PName.toStdString(); //PName
        data5 =  PAge.toStdString();//PAge
        data6= PGender.toStdString(); //PGender
        data7= TName.toStdString(); //TName
        data8 = ResultOD.toStdString(); //ResultOD

        Transfer_DT = data1+data2;
        Patient_ID = data3;
        Patient_name = data4;
        Patient_age = data5;
        Patient_Gender = data6;
        Testname = data7;
        Test_DT = Test_Date+Test_Time;
        Result = data8;
        Data_Testcode = para_code.toStdString();
        std::string ResUnit , aNrmlRang , bNrmlrang;
        ResUnit = Unit.toStdString(); //unit
        aNrmlRang = ANrmlRang.toStdString(); //Anrmlrang
        bNrmlrang = BNrmlRang.toStdString(); //bnrmlrang
        std::string G, A ;
        if(PGender == "Male")
            G="M";
        else if(PGender == "Female")
            G="F";
        else if(PGender == "Others")
            G="O";
        int page = PAge.toInt();
        int Current_Year=QDate::currentDate().year();
        int year = Current_Year-page;
        QString Pa_Age = QString::number(year);
        A = Pa_Age.toStdString();
        QString Lab_name;
        QSqlQuery qrry;
        qrry.prepare("select Lab_name from Lab_details where sno=1");
        qrry.exec();
        while(qrry.next())
            Lab_name= qrry.value(0).toString();

        std::string  Labname;
        Labname=Lab_name.toStdString();
        // std::string Encode = "|^~\&|";

        std::string hl7Message = "MSH|^~\\&|MCA-11|"+Labname+"|||"+Transfer_DT+"||ORU^R01|||2.3|||||\r"
                                                                               "PID|1|"+Patient_ID+"|||"+Patient_name+"||"+A+"|"+G+"|\r"
                                                                                                                                   "OBR|1|"+Patient_ID+"|MCA-11||||"+Test_DT+"||||||||||||||\r"
                                                                                                                                                                             "OBX|1|NM|"+Data_Testcode+"^"+Testname+"||"+Result+"|"+ResUnit+"|"+bNrmlrang+"-"+aNrmlRang+"|||||||"+Test_DT+"||\r";
        QString IP , Port;
        QSqlQuery query;
        query.prepare("select Address1,Port from LIS where sno=1");
        query.exec();
        while (query.next())
        {
            IP = query.value(0).toString();
            Port = query.value(1).toString();
        }
        qDebug()<<"IP : "<<IP <<" "<<"Port : "<<Port;
        if(IP == "" && Port == "")
        {
            showOverlayedMessage("The ip-address & port number is empty, \n Please configure the correct ip-Address & port number.",
                                 "/home/pi/git/Biochemistry-Updates/BCUID/img/notification.png",
                                 [=]() {
               ui->stackedWidget->setCurrentIndex(0);
            });
        }
        else
        {
            qDebug()<<"LIS_Data : "<<LIS_data;
            if(LIS_data==0)
            {
                // Create Non-blocking Custom QMessageBox
                auto *overlay = new QWidget(this);
                overlay->setGeometry(this->rect());
                overlay->setStyleSheet("background-color: rgba(0, 0, 0, 100);");
                overlay->show();
                overlay->raise();

                auto *box = new ModernMessageBox(this);
                box->setMessage("Check to see if there is a valid ip-address and port number.");
                box->setIconPixmap(QPixmap("/home/pi/git/Biochemistry-Updates/BCUID/img/notification.png"));
                box->setButtonLabels("Already verified", "Ok");
                box->showSingleButton(false);

                connect(box, &QDialog::accepted, this, [=]() {
                    overlay->deleteLater();
                    LIS_data=1;
                    isConnected=sock->isConnectedd();
                    if (isConnected)
                    {
                        std::string IPAddress , PortNumber;
                        IPAddress = IP.toStdString();
                        PortNumber =  Port.toStdString();
                        sock->send_msg_mllp(hl7Message);
                    }
                    else
                    {

                    }
                });
                connect(box, &QDialog::rejected, this, [=]() {
                    overlay->deleteLater();
                    ui->stackedWidget->setCurrentIndex(0);
                });
                box->open(); // Non-blocking
            }
            else
            {
                qApp->processEvents();
                isConnected=sock->isConnectedd();
                if (isConnected)
                {
                    std::string IPAddress , PortNumber;
                    IPAddress = IP.toStdString();
                    PortNumber =  Port.toStdString();
                    sock->send_msg_mllp(hl7Message);
                }
                else
                {

                }
            }
        }
    }

    qDebug()<<"connection status : "<<sock->isConnectedd();
    if (isConnected)
    {
        QString messageText,Icontext;
        messageText= "All data was successfully transferred to the LIS server.";
        Icontext="/home/pi/git/Biochemistry-Updates/BCUID/img/check-mark.png";

        showOverlayedMessage(messageText,Icontext,[=]() {
            ui->stackedWidget->setCurrentIndex(9);
        });
    }
    else
    {
        showOverlayedMessage("The LIS server is not connected.","/home/pi/git/Biochemistry-Updates/BCUID/img/notification.png",[=]() {
            LIS_connect_data=0;
            ui->stackedWidget->setCurrentIndex(9);
        });
    }*/
//}

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
