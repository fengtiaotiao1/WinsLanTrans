/**********************************************************************
 * sqlite 的实现
 * 
 * Author:  Ankie
 * Date:    2018/9/7
 **********************************************************************/
 
#include <stdio.h>
#include "sqlitehelp.h"
#include "tblsqlconst.h"


sqlite3 *g_pDatabase = NULL;


// 设置版本号，未实现
static void setVersion(sqlite3 *pDatabase, int version);
// 创建表格
static int createLanTransTable(sqlite3 *db);
// 升级数据库的版本
static int updateLanTransDatabase(sqlite3 *db, int oldVersion, int newVersion);

// 打开数据库的DB
int openLanTransDb(const char *strDbName) {
    if (NULL != g_pDatabase) {
        return DB_SUCCESS;
    }

    stDBFuncCreateOrUpdate stDbFunc;
    stDbFunc.createTable = createLanTransTable;
    stDbFunc.updateDatabase = updateLanTransDatabase;

    return openDatabase(strDbName, &g_pDatabase, &stDbFunc, DB_VERSION);
}

// 打开数据库
int openDatabase(const char *strDbName, sqlite3 **ppDatebase, stDBFuncCreateOrUpdate *func,
                 int iVersion) {
    if (NULL == strDbName) {
        printf("==db==, setVersion strDbName is null\n");
        return DB_FAILED;
    }

    int iRet = sqlite3_open(strDbName, ppDatebase);
    printf("==db==, open database iRet = %d!\n", iRet);

    if (SQLITE_OK != iRet) {
        return DB_FAILED;
    }

    if (0 != iVersion) {
        // TODO 未对 db version 升级进行处理
        if (func->createTable) {
            func->createTable(*ppDatebase);
        }
    }

    return DB_SUCCESS;

}

// 关闭数据库
int closeDatabase(sqlite3 *pDatabase) {
    int iRet = sqlite3_close(pDatabase);
    if (SQLITE_OK == iRet) {
        return DB_SUCCESS;
    }
    return DB_FAILED;
}

// 创建表
int createTable(sqlite3 *pDatabase, char *strSql) {
    char *pErrMsg = NULL;
    int iRet = sqlite3_exec(pDatabase, strSql, NULL, NULL, &pErrMsg);
    printf("==db==, createTable sqlite strSql is : %s\n", strSql);
    if (SQLITE_OK == iRet) {
        return DB_SUCCESS;
    }
    printf("==db==, createTable sqlite pErrMsg is : %s\n", pErrMsg);
    return DB_FAILED;
}

// 执行sql语句
int execSqlString(sqlite3 *pDatabase, char *strSql) {
    char *pErrMsg = NULL;
    int iRet = sqlite3_exec(pDatabase, strSql, NULL, NULL, &pErrMsg);
    printf("==db==, execSqlString sqlite strSql is : %s\n", strSql);
    if (SQLITE_OK == iRet) {
        return DB_SUCCESS;
    }
    printf("==db==, execSqlString sqlite pErrMsg is : %s\n", pErrMsg);
    return DB_FAILED;
}

// 执行查询表格
int execQuerySql(sqlite3 *pDatabase, char *strSql, char ***strResult, int *iRow, int *iColumn) {
    char *pErrMsg = NULL;
    int iRet = sqlite3_get_table(pDatabase, strSql, strResult, iRow, iColumn, &pErrMsg);
    printf("==db==, execQuerySql sqlite strSql is : %s\n", strSql);
    if (SQLITE_OK == iRet) {
        return DB_SUCCESS;
    }
    printf("==db==, execQuerySql sqlite pErrMsg is : %s\n", pErrMsg);
    return DB_FAILED;
}

// 释放表格
void freeTable(char **strRequest) {
    sqlite3_free_table(strRequest);
}

// 执行开始
void executeTransactionBegin(sqlite3 *hDbCon) {
    sqlite3_exec(hDbCon, "BEGIN", NULL, NULL, NULL);
}

// 执行回滚
void executeTransactionRollback(sqlite3 *hDbCon) {
    sqlite3_exec(hDbCon, "ROLLBACK", NULL, NULL, NULL);
}

// 执行结束
void executeTransactionEnd(sqlite3 *hDbCon) {
    sqlite3_exec(hDbCon, "END", NULL, NULL, NULL);
}


// 本地方法，升级版本
static void setVersion(sqlite3 *pDatabase, int version) {
    if (NULL == pDatabase) {
        printf("==db==, setVersion pDatabase is null!\n");
        return;
    }

    printf("==db==, setVersion pDatabase version is : %d!\n", version);

    char strSql[128];
    sprintf(strSql, "PRAGMA user_version = %d\n", version);
    printf("==db==, setVersion sql is : %s\n", strSql);

    sqlite3_exec(pDatabase, strSql, NULL, NULL, NULL);
}

// 创建LanTrans的表格
static int createLanTransTable(sqlite3 *db) {
    // 创建信息表格
    return createTable(db, CREATE_LAN_GROUP_TABLE_STR);
}

// 更新LanTrans数据库版本
static int updateLanTransDatabase(sqlite3 *db, int oldVersion, int newVersion) {
    printf("==db==, updateLanTransDatabase\n");
    return DB_SUCCESS;
}

