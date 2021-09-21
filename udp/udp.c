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

//#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"

static int sqlite3UdpSend(const unsigned char *sendbuf, int len)
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
	iResult = getaddrinfo("localhost", DEFAULT_PORT, &hints, &result);
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

	printf("Bytes Sent: %d\n", iResult);

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
	sqlite3_context *context,
	int argc,
	sqlite3_value **argv
){
	(void)argc;
	(void)argv;
	printf("DEBUG: %i\n", sqlite3_value_int(argv[0]));
	sqlite3UdpSend(sqlite3_value_text(argv[0]), sqlite3_value_bytes(argv[0]));
	//sqlite3_result_text(context, sqlite3_value_text(argv[0]), sqlite3_value_bytes(argv[0]), SQLITE_TRANSIENT);
	sqlite3_result_int(context, SQLITE_OK);
}


#ifdef _WIN32
__declspec(dllexport)
#endif
/* TODO: Change the entry point name so that "extension" is replaced by
** text derived from the shared library filename as follows:  Copy every
** ASCII alphabetic character from the filename after the last "/" through
** the next following ".", converting each character to lowercase, and
** discarding the first three characters if they are "lib".
*/
int sqlite3_udp_init(
	sqlite3 *db, 
	char **pzErrMsg, 
	const sqlite3_api_routines *pApi
){
	int rc = SQLITE_OK;
	SQLITE_EXTENSION_INIT2(pApi);
	/* Insert here calls to
	**     sqlite3_create_function_v2(),
	**     sqlite3_create_collation_v2(),
	**     sqlite3_create_module_v2(), and/or
	**     sqlite3_vfs_register()
	** to register the new features that your extension adds.
	*/
	rc = sqlite3_create_function(
		db, //sqlite3 *db,
		"udp", //const char *zFunctionName,
		1, //int nArg,
		SQLITE_UTF8 | SQLITE_DETERMINISTIC, //int eTextRep,
		NULL, //void *pApp,
		sqlite3UdpFunc, //void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
		NULL, //void (*xStep)(sqlite3_context*,int,sqlite3_value**),
		NULL//void (*xFinal)(sqlite3_context*)
	);
	
	return rc;
}