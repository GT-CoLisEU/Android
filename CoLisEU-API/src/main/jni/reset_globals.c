#include "./Iperf-2.0.5/include/Condition.h"
#include "./Iperf-2.0.5/include/List.h"
#include "./Iperf-2.0.5/include/Thread.h"

extern Condition ReportCond;
extern Condition ReportDoneCond;
extern struct ReportHeader *ReportRoot;
extern int __gnu_getopt_initialized;
extern char buffer[64]; // Buffer for printing
extern struct Iperf_ListEntry *clients;
extern Mutex clients_mutex;
extern report_connection connection_reports[];
//extern nthread_t dwErr;
extern char *gnu_optarg;
extern int gnu_opterr;
extern int gnu_optind;
extern int gnu_optopt;
extern Mutex groupCond;
extern int groupID;
extern report_statistics multiple_reports[];
extern int nonterminating_num;
extern int sInterupted;
//extern nthread_t sThread;
extern report_serverstatistics serverstatistics_reports[];
extern report_settings settings_reports[];
//extern SERVICE_STATUS ssStatus; // current status of the service
//extern SERVICE_STATUS_HANDLE sshStatusHandle;
extern report_statistics statistics_reports[];
//extern TCHAR szErr[256];
extern int thread_sNum;
extern Condition thread_sNum_cond;

void reset_all() {
    ReportRoot = NULL;
    //__gnu_getopt_initialized = 0;
    clients = NULL;
    //dwErr = 0;
    //gnu_optarg = NULL;
    //gnu_opterr = 1;
    gnu_optind = 1;
    //gnu_optopt = '?';
    groupID = 0;
    nonterminating_num = 0;
    sInterupted = 0;
    thread_sNum = 0;
    Condition_Destroy ( &ReportCond );
    Condition_Destroy ( &ReportDoneCond );
    Condition_Destroy ( &thread_sNum_cond );
    memset(buffer, 0, 64);
    Mutex_Destroy (&clients_mutex);
    Mutex_Destroy (&groupCond);
}
