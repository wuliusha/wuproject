#ifndef THREADSERVER_H
#define THREADSERVER_H

#include <QObject>
#include <QObject>
#include<QPushButton>
#include<QThread>
#include<QDateTime>
#include<QTimer>
#include<QElapsedTimer>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>

#include "comm/datebaseConnectPool/connectpoolfactory.h"

struct materialInfo             //物料件号信息
{
    QString materialNumber="";  //物料件号
    QString unit="";            //单位
    QString batch="";           //批次
    QString store="";           //仓号
    QString plant;              //工厂
    QString describe;           //描述
    QString unitOfWeigh;        //物料净重
    QString weighUnit;          //重量单位
    QString customsType;        //报关类型 Local国内 Tax保税 Import征税
    double weightChange=1.00;        //  --kg 的换算系数
};

struct materialShelfbin        //物料件号 与架位对应表
{
    QString materialNumber="";  //物料件号
    int shelfBinIndex=0;        //所属架位索引
    double qty;                 //订单里数量
    int status;                 //0 预存还未提交   1 实际库存
};

struct shelfBinInfo {           //架位信息
    int shelfBinIndex=0;
    int podId;
    QString storeCode;
    QString storeType;
    double upWeight;
    double downWeight;
    int binEnable;
    QString desc;
    QString podDesc;
    int side;
    double weight;
    int layer;

};

struct CurrentAction             //当前出入库 操作状态
{
    int currentTaskikey=0;      //任务的唯一标识
    QString LabelNo;        //贴纸凭证号
    QString materialNumber="";  //物料件号
    int shelfBinIndex;          //架位Bin
    QString shelfBindesc;           //条码描述
    int side;                   //  A/B面
    QString TaskAction="";      //操作步骤  0 1
    double qty;             //订单里数量

    int errorType=0;          //错误类型
    double updateQty;       //errorType 不为空， 更新数量
    int updateShelfBin;     //更新 新的架位 Bin
};

struct missionTable{         //任务信息-->Table
    int ikey;               //唯一标识
    QString orderId;        //订单ID
    QString LabelNo;        //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
    QString materialNumber; //物料件号
    QString binDesc="";     //架位描述
    QString qty;            //订单里数量
    QString unit;           //单位
    QString store;          //仓号
    QString status;         //状态
    QString createTime;     //创建时间
};

struct shelfBinChage{
    QString materialNumber="";  //物料件号
    int oldBinIndex;            //原来架位Bin
    QString oldBinDesc;
    int newBinIndex;            //新的架位Bin
    QString newBinDesc;
    QString taskType;          //更改类型
    int taskStatus;            //任务状态
};

struct operaterInfo{
    int operaterIndex;  //拣选台ID
    QString desc;       //拣选台描述
    QString currentOrderId;//当前拣选订单ID
    int progress;//进度
    int finishCount;//完成数量
    int allCount;//总数量
    QString efficiency;//效率
    QString KPI;//
    QString status;//状态

    QString userNumber; //操作员工号
    QString workTime;   //工作时间
    int taskMax;        //单次最大配送任务数量
    int iding=0;          //登录没有任务时默认是0， 任务没有执行完不能切换到1，1的状态下分配任务进入
};

struct massegaInfo{//下架操作时 用于提示员工物料应放位置
    int index=100;
    int operaterIndex=0;
    QString massegaBin="";
    QString materialType="";
};

struct record_IntOut{//出入库操作记录，供客户端查询
   QString LabelNo;         //标签
   QString MaterialNumber;  //物料件号
   QString StrageBin;       //架位号
   QString Quantity;        //数量
   QString Unit;            //单位
   QString ActionType;      //出入库类型
   QString Time;            //时间
   QString UserNumber;      //操作员
};

//SAP 上载任务 Excel 对应关系
struct SAPExcelInfo{
    QString status;   //是否获取成功 1 成功 0 失败
    QString IssueType;   //发料类型
    QString LabelType;   //贴纸类型
    QString LabelNo;     //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
    QString LabelItem="0";   //贴纸项目   （当LabelItem>基数上浮%20，分为两个工作，并分给不同工作台）
    QString Order_PO;    //生产或采购订单
    QString Order_POItem;//生产或采购订单项目
    QString Material;    //物料号
    double Quantity=0.000;    //数量
    QString Unit;        //单位
    QString Batch;       //批次
    QString Rev;         //版次
    QString IssueStore;  //发料仓号
    QString IssueBin;    //发料货位号
    QString To;          //收货单位
    QString ToBin;       //收货单位地址
    QString Describe;    //物料描述
    QString SO;          //销售订单
    QString SOItem;      //销售订单项目
    QString ProductCode; //产品代码
    QString Ex_ImGroup;  //进出口群组
    QString PackingNote; //包装注解
    QString Route;       //运输线路
    QString BillOfLading;//提货单
    QString HeaderText;  //标题文本
    QString LM_SL;       //订单状态
    QString Reservatio;  //预约编码
    QString Date;        //日期
    QString Time;        //时间
    QString AssembleNo;  //成品物料号
    QString Text;        //文本
    QString Remark;      //备注
    QString unitOfWeigh;    //物料净重
    QString weighUnit;      //重量单位
    QString customsType;    //报关类型 Local国内 Tax保税 Import征

    QString plant;              //工厂
    int shelfBinIndex;         //架位Bin的索引
    int podId;                //货架号
    int taskType;              //任务类型 1入库 2出库 3盘点 4查仓 5改位入库
    int taskStatus;           //任务状态
    QString creatTime;        //创建时间
    int pickStation;

};

struct Get_MtInfo{//按订单号查询返回信息
    QString material="";//物料号
    QString plant="";   //工厂
    QString store="";   //仓号
    QString bin="";     //架位
    QString qty="";     //数量
    QString unit="";    //单位
    QString batch="";   //批次
    QString result="";  //结果, S为成功, E为错误
    QString message=""; //系统讯息
};

struct Post_MtInfo{//往SAP 提交上架数据
    QString labelNo=""; //标签号
    QString material="";//物料号
    QString plant="";   //工厂
    QString store="";   //仓号
    QString systemBin="";     //SAP架位或AGV架位
    QString actualBin="";     //实际扫瞄的架位
    QString createdBy="";         //操作员工号
};

struct UserInfo//用户个人信息
{
    QString UserNuber="";      //编号
    QString UserName="";       //姓名
    QString PassWord="";       //密码
    QString Jurisdiction="";   //权限
    QString UserGrage="";      // 等级
    QString Status="0";        //编辑状态 -1删除
};

struct missionInfo{//任务信息
    int missionId;          //自动生成
    QString orderId;        //订单ID
    QString materialNumber; //物料件号
    int pickStation;        //操作台ID
    double qty;             //订单里数量
    int shelfBinIndex;      //架位Bin
    int side;               //  A/B面
    QString operatorNumber; //操作员
    QString LabelNo;        //贴纸凭证号  （LabelNo+LabelItem    生产唯一凭证 ==打印的条形码）
    QString LabelItem;      //贴纸项目   （当LabelItem>基数上浮%20，分为两个工作，并分给不同工作台）
    QString labelType;      //贴纸类型
    QDateTime  createTime;  //创建时间
    int status;             //状态  -1读不到货架号
    int agvId;              //AGVID
    int errorType;          //错误类型
    double updateQty;       //errorType 不为空， 更新数量
    int updateShelfBin;     //更新 新的架位 Bin
    double priority;        //优先级
    int ikey;               //唯一标识
};

class threadServer : public QObject
{
    Q_OBJECT
public:
    explicit threadServer(QObject *parent = nullptr);
public:
    QTimer *threadTimer;int threadServercount=0;

public slots:
    void threadServerTimer();

public:
    /////////////////////////////////////////////////////
    /// \brief queryNewMissionInfo    读取当前操作台 当前操作状态的任务
    /// \param pickStation   操作台ID
    /// \param status        拣选状态
    /// \param taskType      出入库类型   1--入库     2--发料
    /// \return

    int taskCount=0;
    QMap<int, missionInfo>incurrentMissionMap;
    QMap<int, missionInfo>outcurrentMissionMap;QMap<int, missionInfo> outwaitMissionMap;
    QMap<int ,missionInfo > queryNewMissionInfo(QMap<int, missionInfo>missionInfoMap,int pickStation,int status,int taskType);
    QMap<int ,missionInfo > queryoutwaitMission(QMap<int, missionInfo>missionInfoMap,int pickStation,int taskType);


    //////////////////////////////////////////////////
    /// \brief readSAPExcelInfoTask   读取总的任务
    /// \param SAPExcelInfoTask       任务类型 1 入库 2出库
    /// \param taskStatus
    QMap<QString, SAPExcelInfo> inSAPExcelInfoTask;
    QMap<QString, SAPExcelInfo> outSAPExcelInfoTask;
    QMap<QString, SAPExcelInfo> readSAPExcelInfoTask(QMap<QString, SAPExcelInfo> SAPExcelInfoTask,int taskType);



    ///////////////////////////////////////////
    /// \brief threadServerInit  关键性 Map 初始化
    ///
    void  threadServerInit();

signals:

public slots:

private:

    QMap<int ,massegaInfo>massegaInfoMap;//下架操作时 用于提示员工物料应放位置

    QMap<int ,shelfBinInfo >shelfBinInfoMap;
    QMap<QString ,materialInfo >materialInfoMap;
    QMap<QString ,double >weighUnitMap;//单位类型与转换系数
    QMap<QString ,materialShelfbin >materialshelfBinMap;//ikey -架位  多对一


};

#endif // THREADSERVER_H
