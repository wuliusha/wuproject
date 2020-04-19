#include "threadserver.h"
#define ASMORDERDBSTR ConnectPoolFactory::getInstance()->getConnectPool(ConnectPoolFactory::AsmOrderDataBase)

threadServer::threadServer(QObject *parent) : QObject(parent)
{
    threadTimer = new QTimer(this);
    connect(threadTimer,&QTimer::timeout,this,&threadServer::threadServerTimer);
    threadTimer->start(1000);
}

void threadServer::threadServerTimer()
{
    QElapsedTimer timer;timer.start();
    threadServercount++;
    if(threadServercount%2==0){
        QTime  calcTime;
        calcTime.start();
        threadServerInit();
        qDebug() << "threadServerInit" << calcTime.restart();
//        queryServer::getInstance()->threadServerInit(massegaInfoMap,shelfBinInfoMap,materialInfoMap,
//                                        weighUnitMap, materialshelfBinMap);
//        if(threadServercount>3000){threadServercount=0;}
    }

//    if(threadServercount%5==0){
//        inSAPExcelInfoTask=readSAPExcelInfoTask(inSAPExcelInfoTask,1);
//        if(!queryServer::getInstance()->setSAPExcelInfoTask(inSAPExcelInfoTask,1)){
//            inSAPExcelInfoTask.clear();
//        }
//    }

//    if(threadServercount%5==1){
//        outSAPExcelInfoTask=readSAPExcelInfoTask(outSAPExcelInfoTask,2);
//        if(!queryServer::getInstance()->setSAPExcelInfoTask(outSAPExcelInfoTask,2)){
//            outSAPExcelInfoTask.clear();
//        }
//    }

//    if(threadServercount%2==0){
//        incurrentMissionMap= queryNewMissionInfo(incurrentMissionMap,1,4,1);
//        taskCount =queryServer::getInstance()->setcurrentMissionMap(incurrentMissionMap,"in",taskCount);
//        if(taskCount!=0){
//            incurrentMissionMap.clear();
//            incurrentMissionMap= queryNewMissionInfo(incurrentMissionMap,1,4,1);
//            taskCount =queryServer::getInstance()->setcurrentMissionMap(incurrentMissionMap,"in",taskCount);
//        }
//    }

//    if(threadServercount%2==1){
//        outcurrentMissionMap= queryNewMissionInfo(outcurrentMissionMap,1,4,2);
//        taskCount =queryServer::getInstance()->setcurrentMissionMap(outcurrentMissionMap,"out",taskCount);
//        if(taskCount!=0){
//            outcurrentMissionMap.clear();
//            outcurrentMissionMap= queryNewMissionInfo(outcurrentMissionMap,1,4,2);
//            taskCount =queryServer::getInstance()->setcurrentMissionMap(outcurrentMissionMap,"out",taskCount);

//            outwaitMissionMap=queryoutwaitMission(outwaitMissionMap,1,2);
//            queryServer::getInstance()->setcurrentMissionMap(outwaitMissionMap,"wait",taskCount);
//        }
//    }

    if(timer.elapsed()>1000){
        qDebug()<<"read data cost:"<<timer.elapsed()<<"ms  "<<QDateTime::currentDateTime().toString("hh:mm ss");timer.restart();
    }

}

QMap<int, missionInfo> threadServer::queryNewMissionInfo(QMap<int, missionInfo> missionInfoMap, int pickStation, int status, int taskType)
{
    connectBuilder build(ASMORDERDBSTR);
    QSqlDatabase sqlDatabase =  build.getOpenDatabase();

    sqlDatabase.transaction();
    QSqlQuery sqlQuery(sqlDatabase);
    //"where pickStation =:pickStation and ((orderId =:orderId and missionId =:missionId) and (status =:status and taskType =:taskType));"
    sqlQuery.prepare("select * from missionTask "
                     "where pickStation =:pickStation and (status =:status and taskType =:taskType);");
    sqlQuery.addBindValue(pickStation);
    sqlQuery.addBindValue(status);       //任务状态 4 到达操作台
    sqlQuery.addBindValue(taskType);
    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            missionInfo missionInfoI;
            missionInfoI.missionId=sqlQuery.value("missionId").toInt();          //自动生成
            missionInfoI.orderId=sqlQuery.value("orderId").toString();         //订单ID
            missionInfoI.LabelNo=sqlQuery.value("LabelNo").toString();        //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
            missionInfoI.LabelItem=sqlQuery.value("LabelItem").toString();      //贴纸项目   （当LabelItem>基数上浮%20，分为两个工作，并分给不同工作台）
            //missionInfoI.taskType=static_cast<taskTypeEnum>(sqlQuery.value("taskType").toInt());//出入库类型
            missionInfoI.materialNumber=sqlQuery.value("materialNumber").toString();  //物料件号
            missionInfoI.pickStation=sqlQuery.value("pickStation").toInt();        //操作台ID
            missionInfoI.qty=sqlQuery.value("qty").toDouble();                //订单里数量
            missionInfoI.shelfBinIndex=sqlQuery.value("shelfBinIndex").toInt();      //架位Bin
            missionInfoI.side=sqlQuery.value("side").toInt();               //  A/B面
            missionInfoI.operatorNumber=sqlQuery.value("operatorNumber").toString();  //操作员
            missionInfoI.labelType=sqlQuery.value("labelType").toString();       //贴纸类型
            missionInfoI.createTime=sqlQuery.value("createTime").toDateTime();    //创建时间
            missionInfoI.status=sqlQuery.value("status").toInt();             //状态
            missionInfoI.agvId=sqlQuery.value("agvId").toInt();              //AGVID
            missionInfoI.errorType=sqlQuery.value("errorType").toInt();          //错误类型
            missionInfoI.updateQty=sqlQuery.value("updateQty").toDouble();          //errorType 不为空， 更新数量
            missionInfoI.updateShelfBin=sqlQuery.value("updateShelfBin").toInt();     //更新 新的架位 Bin
            missionInfoI.priority=sqlQuery.value("priority").toDouble();           //优先级
            missionInfoI.ikey=sqlQuery.value("ikey").toInt();               //唯一标识

            if(!missionInfoMap.contains(missionInfoI.missionId)){
                 missionInfoMap.insert(missionInfoI.missionId,missionInfoI);
            }
            //qDebug()<<missionInfoI.ikey<<missionInfoI.LabelNo;
        }

    }else {
        qDebug()<<"queryNewMissionInfo"<<sqlQuery.record();
    }
    if(!sqlDatabase.commit()){
        sqlDatabase.rollback();
    }
    //SqlConnectionPool::closeSqlConnection(sqlDatabase);
    return missionInfoMap;
}

QMap<int, missionInfo> threadServer::queryoutwaitMission(QMap<int, missionInfo> missionInfoMap, int pickStation, int taskType)
{
    connectBuilder build(ASMORDERDBSTR);
    QSqlDatabase sqlDatabase =  build.getOpenDatabase();

    sqlDatabase.transaction();
    QSqlQuery sqlQuery(sqlDatabase);
    //"where pickStation =:pickStation and ((orderId =:orderId and missionId =:missionId) and (status =:status and taskType =:taskType));"
    sqlQuery.prepare("select * from missionTask "
                     "where pickStation =:pickStation and taskType =:taskType;");
    sqlQuery.addBindValue(pickStation);
    sqlQuery.addBindValue(taskType);
    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            missionInfo missionInfoI;
            missionInfoI.missionId=sqlQuery.value("missionId").toInt();          //自动生成
            missionInfoI.orderId=sqlQuery.value("orderId").toString();         //订单ID
            missionInfoI.LabelNo=sqlQuery.value("LabelNo").toString();        //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
            missionInfoI.LabelItem=sqlQuery.value("LabelItem").toString();      //贴纸项目   （当LabelItem>基数上浮%20，分为两个工作，并分给不同工作台）
            //missionInfoI.taskType=static_cast<taskTypeEnum>(sqlQuery.value("taskType").toInt());//出入库类型
            missionInfoI.materialNumber=sqlQuery.value("materialNumber").toString();  //物料件号
            missionInfoI.pickStation=sqlQuery.value("pickStation").toInt();        //操作台ID
            missionInfoI.qty=sqlQuery.value("qty").toDouble();                //订单里数量
            missionInfoI.shelfBinIndex=sqlQuery.value("shelfBinIndex").toInt();      //架位Bin
            missionInfoI.side=sqlQuery.value("side").toInt();               //  A/B面
            missionInfoI.operatorNumber=sqlQuery.value("operatorNumber").toString();  //操作员
            missionInfoI.labelType=sqlQuery.value("labelType").toString();       //贴纸类型
            missionInfoI.createTime=sqlQuery.value("createTime").toDateTime();    //创建时间
            missionInfoI.status=sqlQuery.value("status").toInt();             //状态
            missionInfoI.agvId=sqlQuery.value("agvId").toInt();              //AGVID
            missionInfoI.errorType=sqlQuery.value("errorType").toInt();          //错误类型
            missionInfoI.updateQty=sqlQuery.value("updateQty").toDouble();          //errorType 不为空， 更新数量
            missionInfoI.updateShelfBin=sqlQuery.value("updateShelfBin").toInt();     //更新 新的架位 Bin
            missionInfoI.priority=sqlQuery.value("priority").toDouble();           //优先级
            missionInfoI.ikey=sqlQuery.value("ikey").toInt();               //唯一标识

            if(!missionInfoMap.contains(missionInfoI.missionId)
                    && missionInfoI.status<4){
                 missionInfoMap.insert(missionInfoI.missionId,missionInfoI);
            }
            //qDebug()<<missionInfoI.ikey<<missionInfoI.LabelNo;
        }

    }else {
        qDebug()<<"queryNewMissionInfo"<<sqlQuery.record();
    }
    if(!sqlDatabase.commit()){
        sqlDatabase.rollback();
    }
    //SqlConnectionPool::closeSqlConnection(sqlDatabase);
    return missionInfoMap;
}

QMap<QString, SAPExcelInfo> threadServer::readSAPExcelInfoTask(QMap<QString, SAPExcelInfo> SAPExcelInfoTask, int taskType)
{
    connectBuilder build(ASMORDERDBSTR);
    QSqlDatabase sqlDatabase =  build.getOpenDatabase();

    sqlDatabase.transaction();
    QSqlQuery sqlQuery(sqlDatabase);
    sqlQuery.prepare("select * from sapExcelinfo "
                     "where taskType =:taskType ;");
    sqlQuery.addBindValue(taskType);

    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            SAPExcelInfo SAPExcelInfoI;
            SAPExcelInfoI.IssueType=sqlQuery.value("IssueType").toString();   //发料类型
            SAPExcelInfoI.LabelType=sqlQuery.value("LabelType").toString();   //贴纸类型
            SAPExcelInfoI.LabelNo=sqlQuery.value("LabelNo").toString();     //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
            SAPExcelInfoI.LabelItem=sqlQuery.value("LabelItem").toString();   //贴纸项目   （当LabelItem>基数上浮%20，分为两个工作，并分给不同工作台）
            SAPExcelInfoI.Order_PO=sqlQuery.value("Order_PO").toString();    //生产或采购订单
            SAPExcelInfoI.Order_POItem=sqlQuery.value("Order_POItem").toString();//生产或采购订单项目
            SAPExcelInfoI.Material=sqlQuery.value("Material").toString();    //物料号
            SAPExcelInfoI.Quantity=sqlQuery.value("Quantity").toDouble();    //数量
            SAPExcelInfoI.Unit=sqlQuery.value("Unit").toString();        //单位
            SAPExcelInfoI.Batch=sqlQuery.value("Batch").toString();       //批次
            SAPExcelInfoI.Rev=sqlQuery.value("Rev").toString();         //版次
            SAPExcelInfoI.IssueStore=sqlQuery.value("IssueStore").toString();  //发料仓号
            SAPExcelInfoI.IssueBin=sqlQuery.value("IssueBin").toString();    //发料货位号
            SAPExcelInfoI.To=sqlQuery.value("_To").toString();          //收货单位
            SAPExcelInfoI.ToBin=sqlQuery.value("ToBin").toString();       //收货单位地址
            SAPExcelInfoI.Describe=sqlQuery.value("Describe").toString();    //物料描述
            SAPExcelInfoI.SO=sqlQuery.value("SO").toString();          //销售订单
            SAPExcelInfoI.SOItem=sqlQuery.value("SOItem").toString();      //销售订单项目
            SAPExcelInfoI.ProductCode=sqlQuery.value("ProductCode").toString(); //产品代码
            SAPExcelInfoI.Ex_ImGroup=sqlQuery.value("Ex_ImGroup").toString();  //进出口群组
            SAPExcelInfoI.PackingNote=sqlQuery.value("PackingNote").toString(); //包装注解
            SAPExcelInfoI.Route=sqlQuery.value("Route").toString();       //运输线路
            SAPExcelInfoI.BillOfLading=sqlQuery.value("BillOfLading").toString();//提货单
            SAPExcelInfoI.HeaderText=sqlQuery.value("HeaderText").toString();  //标题文本
            SAPExcelInfoI.LM_SL=sqlQuery.value("LM_SL").toString();       //订单状态
            SAPExcelInfoI.Reservatio=sqlQuery.value("Reservatio").toString();  //预约编码
            SAPExcelInfoI.Date=sqlQuery.value("Date").toString();        //日期
            SAPExcelInfoI.Time=sqlQuery.value("Time").toString();        //时间
            SAPExcelInfoI.AssembleNo=sqlQuery.value("AssembleNo").toString();  //成品物料号
            SAPExcelInfoI.Text=sqlQuery.value("Text").toString();        //文本
            SAPExcelInfoI.Remark=sqlQuery.value("Remark").toString();      //备注
            SAPExcelInfoI.unitOfWeigh=sqlQuery.value("unitOfWeigh").toString();    //物料净重
            SAPExcelInfoI.weighUnit=sqlQuery.value("weighUnit").toString();      //重量单位
            SAPExcelInfoI.customsType=sqlQuery.value("customsType").toString();    //报关类型 Local国内 Tax保税 Import征

            SAPExcelInfoI.plant=sqlQuery.value("plant").toString();              //工厂
            SAPExcelInfoI.shelfBinIndex=sqlQuery.value("shelfBinIndex").toInt();         //架位Bin的索引
            SAPExcelInfoI.podId=sqlQuery.value("podId").toInt();                //货架号
            SAPExcelInfoI.taskType=sqlQuery.value("taskType").toInt();              //任务类型 1出库 2入库 3盘点 4查仓 5改位入库
            SAPExcelInfoI.taskStatus=sqlQuery.value("taskStatus").toInt();           //任务状态
            SAPExcelInfoI.creatTime=sqlQuery.value("creatTime").toString();        //创建时间

            SAPExcelInfoTask.insert(SAPExcelInfoI.Order_PO,SAPExcelInfoI);
            if(!SAPExcelInfoTask.contains(SAPExcelInfoI.Order_PO)){

            }
            //qDebug()<<missionInfoI.ikey<<missionInfoI.updateQty<<sqlQuery.record();
        }
    }else {
        qDebug()<<"queryNewMissionInfo"<<sqlQuery.record();
    }

    if(!sqlDatabase.commit()){
        sqlDatabase.rollback();
    }
    //SqlConnectionPool::closeSqlConnection(sqlDatabase);

    return SAPExcelInfoTask;
}

void threadServer::threadServerInit()
{
//    QSqlDatabase sqlDatabase = SqlConnectionPool::openSqlConnection();
//    QSqlDatabase sqlDatabase = QSqlDatabase::addDatabase("QODBC", "connectionName");
//    sqlDatabase.setDatabaseName(QString("DRIVER={SQL SERVER};"
//                               "SERVER=%1;"
//                               "DATABASE=%2;"
//                               "UID=%3;"
//                               "PWD=%4;").arg("10.16.208.150")  //hostName
//                       .arg("ASM_AGV")                          //datebaseName
//                       .arg("sa")                         //userName
//                       .arg("123456"));

//    sqlDatabase.open();
    connectBuilder build(ASMORDERDBSTR);
    QSqlDatabase sqlDatabase =  build.getOpenDatabase();

//    sqlDatabase.transaction();
    QSqlQuery sqlQuery(sqlDatabase);
    QSqlQuery sqlQuery1(sqlDatabase);
    QSqlQuery sqlQuery2(sqlDatabase);
    QSqlQuery sqlQuery3(sqlDatabase);
sqlQuery.setForwardOnly(true);
    sqlQuery.prepare("select * from shelfInfo ;");

    QTime t1,t2,t3,t4;
    t1.start();

    if(sqlQuery.exec())
    {

        while(sqlQuery.next())
        {
            int index = sqlQuery.value("shelfBinIndex").toInt();
//            if(shelfBinInfoMap.contains(index)){
//                continue;
//            }
            shelfBinInfo shelfBinInfoI;
            shelfBinInfoI.shelfBinIndex=index;//sqlQuery.value("shelfBinIndex").toInt();
            shelfBinInfoI.podId=sqlQuery.value("podId").toInt();
            shelfBinInfoI.storeCode=sqlQuery.value("storeCode").toString();
            shelfBinInfoI.storeType=sqlQuery.value("storeType").toString();
            shelfBinInfoI.upWeight=sqlQuery.value("upWeight").toInt();
            shelfBinInfoI.downWeight=sqlQuery.value("downWeight").toInt();
            shelfBinInfoI.binEnable=sqlQuery.value("binEnable").toInt();
            shelfBinInfoI.desc=sqlQuery.value("desc").toString();
            shelfBinInfoI.podDesc=sqlQuery.value("podDesc").toString();
            shelfBinInfoI.side=sqlQuery.value("side").toInt();
            shelfBinInfoI.layer=sqlQuery.value("layer").toInt();
            //qDebug()<<shelfBinInfoI.shelfBinIndex<<shelfBinInfoI.podDesc<<shelfBinInfoI.desc;
            if(!shelfBinInfoMap.contains(shelfBinInfoI.shelfBinIndex)){
                shelfBinInfoMap.insert(shelfBinInfoI.shelfBinIndex,shelfBinInfoI);
            }
        }
    }

    qDebug() << "t1 " << t1.restart();

    t2.start();
    sqlQuery1.prepare("select * from materialShelfbinMap;");
    if(sqlQuery1.exec())
    {
        while(sqlQuery1.next())
        {
            materialShelfbin materialShelfbinI;
            materialShelfbinI.materialNumber=sqlQuery1.value("materialNumber").toString();  //物料件号
            materialShelfbinI.shelfBinIndex=sqlQuery1.value("shelfBinIndex").toInt();        //所属架位索引
            materialShelfbinI.qty=sqlQuery1.value("quantity").toDouble();
            materialShelfbinI.status=sqlQuery1.value("status").toInt();
            QString ikey=materialShelfbinI.materialNumber+QString::number(materialShelfbinI.status);
            if(!materialshelfBinMap.contains(ikey)){
                materialshelfBinMap.insert(materialShelfbinI.materialNumber,materialShelfbinI);
            }
        }
    }

    qDebug() << "t2 " << t2.restart();
      t3.start();
    sqlQuery2.prepare("select * from materialInfo;");
    if(sqlQuery2.exec())
    {
        while(sqlQuery2.next())
        {
            materialInfo materialInfoI;
            materialInfoI.materialNumber=sqlQuery2.value("materialNumber").toString();  //物料件号
            materialInfoI.unit=sqlQuery2.value("unit").toString();            //单位
            materialInfoI.batch=sqlQuery2.value("batch").toString();           //批次
            materialInfoI.store=sqlQuery2.value("store").toString();           //仓号

            materialInfoI.plant=sqlQuery2.value("plant").toString();         //工厂
            materialInfoI.describe=sqlQuery2.value("describe").toString(); //描述
            materialInfoI.unitOfWeigh=sqlQuery2.value("unitOfWeigh").toString(); //物料净重
            materialInfoI.weighUnit=sqlQuery2.value("weighUnit").toString();//重量单位
            materialInfoI.customsType=sqlQuery2.value("customsType").toString(); //报关类型 Local国内 Tax保税 Import征税
            materialInfoI.weightChange=sqlQuery2.value("weightChange").toDouble();//  --kg 的换算系数

            if(!materialInfoMap.contains(materialInfoI.materialNumber)){
                materialInfoMap.insert(materialInfoI.materialNumber,materialInfoI);
            }
        }
    }

    qDebug() << "t3 " << t3.restart();

    sqlQuery3.prepare("select * from massegaInfo;");
    if(sqlQuery3.exec())
    {
        while(sqlQuery3.next())
        {
            massegaInfo massegaInfoI;
            massegaInfoI.index=sqlQuery3.value("_index").toInt();  //物料件号
            massegaInfoI.operaterIndex=sqlQuery3.value("operaterIndex").toInt();            //单位
            massegaInfoI.massegaBin=sqlQuery3.value("massegaBin").toString();           //批次
            massegaInfoI.materialType=sqlQuery3.value("materialType").toString();           //仓号
            if(!massegaInfoMap.contains(massegaInfoI.index)){
                //qDebug()<<massegaInfoI.index<<massegaInfoI.operaterIndex<<massegaInfoI.massegaBin;
                massegaInfoMap.insert(massegaInfoI.index,massegaInfoI);
            }
        }
    }

    t4.restart();
    sqlQuery.prepare("select * from weighUnitMap;");
    if(sqlQuery.exec())
    {
        while(sqlQuery.next())
        {
            //单位类型与转换系数
            weighUnitMap.insert(sqlQuery.value("weighUnit").toString(),sqlQuery.value("weightChange").toDouble());
        }
    }

    qDebug() << "t4 " << t4.restart();

//    if(!sqlDatabase.commit()){
//        sqlDatabase.rollback();
//    }

//    SqlConnectionPool::closeSqlConnection(sqlDatabase);
}
