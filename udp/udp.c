#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#include "debug.h"

#define MAJOR 0
#define MINOR 1
#define PATCH 0

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define VERSION STR(MAJOR) "." STR(MINOR) "." STR(PATCH)
#define COMPILATION VERSION " " __DATE__ " " __TIME__

//#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

static int getSingeValue(void *value, int argc, char **argv, char **azColName){
	char** szValue = (char**)value;
	DBGPRINT("%s = %s\n", azColName[0], argv[0] ? argv[0] : "NULL");
	*szValue = sqlite3_malloc(strlen(argv[0]) + 1);
	strcpy(*szValue, argv[0]);
	return 0;
}

static char* getUdpServer(sqlite3* db)
{
	static char* server = 0;
	if (server == 0)
	{
		DBGPRINT("Initializing 'server'.");
		sqlite3_exec(db, "SELECT Server FROM _udp", getSingeValue, &server, NULL);
	}
	DBGPRINT("server is %s", server);
	return server;
}

static char* getUdpPort(sqlite3* db)
{
	static char* port = 0;
	if (port == 0)
	{
		DBGPRINT("Initializing 'port'.");
		sqlite3_exec(db, "SELECT Port FROM _udp", getSingeValue, &port, NULL);
	}
	DBGPRINT("port is %s", port);
	return port;
}

static int sqlite3UdpSend(sqlite3* db, const unsigned char *sendbuf, int len)
{
	WSADATA wsaData;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}
	
	struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family   = AF_INET;     // IPv4
	hints.ai_socktype = SOCK_DGRAM;  // UDP
	hints.ai_protocol = IPPROTO_UDP; // UDP
	
	// Resolve the server address and port
	iResult = getaddrinfo(getUdpServer(db), getUdpPort(db), &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	SOCKET ConnectSocket = INVALID_SOCKET;
	
	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr=result;
	
	// Create a SOCKET for connecting to server
	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
		ptr->ai_protocol);
		
	if (ConnectSocket == INVALID_SOCKET) {
		printf("Error at socket(): %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	
	// Connect to server.
	iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	
	
	// send data

	// Send an initial buffer
	iResult = send(ConnectSocket, (const char*)sendbuf, len, 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	DBGPRINT("Bytes Sent: %d\n", iResult);

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	//iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	
	return WSACleanup();
}

/* Insert your extension code here */
static void sqlite3UdpFunc(
	sqlite3_context *context, int argc, sqlite3_value **argv)
{
	(void)argc;
	(void)argv;
	sqlite3* db = sqlite3_context_db_handle(context);
	sqlite3UdpSend(db, sqlite3_value_text(argv[0]), sqlite3_value_bytes(argv[0]));
	sqlite3_result_int(context, SQLITE_OK);
}

static void sqlite3UdpPort(
	sqlite3_context *context, int argc, sqlite3_value **argv)
{
	(void)argc;
	(void)argv;
	sqlite3* db = sqlite3_context_db_handle(context);
	sqlite3_result_text(context, getUdpPort(db), -1, NULL);
}

static void sqlite3UdpVersion(
	sqlite3_context *context, int argc, sqlite3_value **argv)
{
	(void)argc;
	(void)argv;
	sqlite3_result_text(context, COMPILATION, -1, NULL);
}

#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_udp_init(
	sqlite3 *db, 
	char **pzErrMsg, 
	const sqlite3_api_routines *pApi
){
	DBGPRINT("udp %s %s %s", VERSION, __DATE__, __TIME__);
	DBGPRINT("sqlite3_udp_init");
	int rc = SQLITE_OK;
	SQLITE_EXTENSION_INIT2(pApi);

	// create _udp table and insert default value if they do not exist
	DBGPRINT("CREATE TABLE IF NOT EXISTS _udp (Server TEXT NOT NULL, Port INTEGER NOT NULL)");
	sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS _udp (Server TEXT NOT NULL, Port INTEGER NOT NULL)", NULL, 0, NULL);
	DBGPRINT("INSERT INTO _udp SELECT 'localhost', 27015 WHERE NOT EXISTS(SELECT 1 FROM _udp);");
	sqlite3_exec(db, "INSERT INTO _udp SELECT 'localhost', 27015 WHERE NOT EXISTS(SELECT 1 FROM _udp);", NULL, 0, NULL);
	// create trigger to prevent deletion and insertion of furter rows to _udp
	DBGPRINT("CREATE TRIGGER IF NOT EXISTS TR__udp_NoInsert BEFORE INSERT ON _udp BEGIN SELECT RAISE (ABORT, 'INSERT forbidden: _udp'); END;");
	sqlite3_exec(db, "CREATE TRIGGER IF NOT EXISTS TR__udp_NoInsert BEFORE INSERT ON _udp BEGIN SELECT RAISE (ABORT, 'INSERT forbidden: _udp'); END;", NULL, 0, NULL);
	DBGPRINT("CREATE TRIGGER IF NOT EXISTS TR__udp_NoDelete BEFORE DELETE ON _udp BEGIN SELECT RAISE (ABORT, 'DELETE forbidden: _udp'); END;");
	sqlite3_exec(db, "CREATE TRIGGER IF NOT EXISTS TR__udp_NoDelete BEFORE DELETE ON _udp BEGIN SELECT RAISE (ABORT, 'DELETE forbidden: _udp'); END;", NULL, 0, NULL);

	DBGPRINT("sqlite3_create_function: sqlite3UdpFunc");
	rc = sqlite3_create_function(
		db,                                 //sqlite3 *db,
		"udp",                              //const char *zFunctionName,
		1,                                  //int nArg,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC, //int eTextRep,
		NULL,                               //void *pApp,
		sqlite3UdpFunc,                     //void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		NULL,                               //void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		NULL                                //void (*xFinal)(sqlite3_context*)
	);
	
	if (rc != SQLITE_OK)
	{
		printf("Could not create function: sqlite3UdpFunc");
		return rc;
	}
	
	DBGPRINT("sqlite3_create_function: sqlite3UdpPort");
	rc = sqlite3_create_function(
		db,                                 //sqlite3 *db,
		"udp_port",                         //const char *zFunctionName,
		0,                                  //int nArg,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC, //int eTextRep,
		NULL,                               //void *pApp,
		sqlite3UdpPort,                     //void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		NULL,                               //void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		NULL                                //void (*xFinal)(sqlite3_context*)
	);
	
	if (rc != SQLITE_OK)
	{
		printf("Could not create function: sqlite3UdpPort");
		return rc;
	}
	
	DBGPRINT("sqlite3_create_function: sqlite3UdpVersion");
	rc = sqlite3_create_function(
		db,                                 //sqlite3 *db,
		"udp_version",                      //const char *zFunctionName,
		0,                                  //int nArg,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC, //int eTextRep,
		NULL,                               //void *pApp,
		sqlite3UdpVersion,                  //void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		NULL,                               //void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		NULL                                //void (*xFinal)(sqlite3_context*)
	);
	
	if (rc != SQLITE_OK)
	{
		printf("Could not create function: sqlite3UdpFunc");
		return rc;
	}
	
	return rc;
}