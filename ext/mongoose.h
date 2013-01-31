// Copyright (c) 2004-2012 Sergey Lyubka
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef MONGOOSE_HEADER_INCLUDED
#define  MONGOOSE_HEADER_INCLUDED

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

struct mg_context;     // Handle for the HTTP service itself
struct mg_connection;  // Handle for the individual connection

// Parsed Authorization header
struct mg_auth_header {
  const char *user, *uri, *cnonce, *response, *qop, *nc, *nonce; // Fields of the Authorization header
  // The following members can be set by MG_AUTHENTICATE callback
  // if non-NULL, will be freed by mongoose
  char *ha1;               // ha1 = md5(username:domain:password), used to compute expected_response
  char *expected_response; // Compared against response
};

// This structure contains information about the HTTP request.
struct mg_request_info {
  void *user_data;       // User-defined pointer passed to mg_start()
  char *request_method;  // "GET", "POST", etc
  char *uri;             // URL-decoded URI
  char *http_version;    // E.g. "1.0", "1.1"
  char *query_string;    // URL part after '?' (not including '?') or NULL
  char *remote_user;     // Authenticated user, or NULL if no auth used
  char *log_message;     // Mongoose error log message, MG_EVENT_LOG only
  long remote_ip;        // Client's IP address
  int remote_port;       // Client's port
  int status_code;       // HTTP reply status code, e.g. 200
  int is_ssl;            // 1 if SSL-ed, 0 if not
  int num_headers;       // Number of headers
  struct mg_header {
    char *name;          // HTTP header name
    char *value;         // HTTP header value
  } http_headers[64];    // Maximum 64 headers
  struct mg_auth_header *ah; // Parsed Authorization header, if present
};


// Various events on which user-defined function is called by Mongoose.
enum mg_event {
  MG_NEW_MASTER_PLUGIN,    // New master plugin thread created user can add plugin to it
                    // that will survive the whole server life
  MG_FREE_MASTER_PLUGIN,   // Master plgugin thread is exiting free user added plugin
  MG_NEW_PLUGIN,    // New worker thread created user can add plugin to it
                    // that will survive the whole thread life
  MG_FREE_PLUGIN,   // Worker thread is exiting free user added plugin
  MG_NEW_CONNECTION,// New TCP connection request has arrived from the client
  MG_NEW_REQUEST,   // New HTTP request has arrived from the client
  MG_REQUEST_COMPLETE, // Mongoose has finished handling the request
  MG_HTTP_ERROR,    // HTTP error must be returned to the client
  MG_EVENT_LOG,     // Mongoose logs an event, request_info.log_message
  MG_INIT_SSL,      // Mongoose initializes SSL. Instead of mg_connection *,
                    // SSL context is passed to the callback function.
  MG_WEBSOCKET_CONNECT, // Sent on HTTP connect, before websocket handshake.
                        // If user callback returns NULL, then mongoose proceeds
                        // with handshake, otherwise it closes the connection.
  MG_WEBSOCKET_READY,   // Handshake has been successfully completed.
  MG_WEBSOCKET_MESSAGE, // Incoming message from the client
  MG_WEBSOCKET_CLOSE,   // Client has closed the connection
  MG_AUTHENTICATE,  // Authenticate a new HTTP request.  request_info->ah
                    // is set, if available.  Callback should fill in request_info->ha1.
};


// Prototype for the user-defined function. Mongoose calls this function
// on every MG_* event.
//
// Parameters:
//   event: which event has been triggered.
//   conn: opaque connection handler. Could be used to read, write data to the
//         client, etc. See functions below that have "mg_connection *" arg.
//   request_info: Information about HTTP request.
//
// Return:
//   If handler returns non-NULL, that means that handler has processed the
//   request by sending appropriate HTTP reply to the client. Mongoose treats
//   the request as served.
//   If handler returns NULL, that means that handler has not processed
//   the request. Handler must not send any data to the client in this case.
//   Mongoose proceeds with request handling as if nothing happened.
typedef void * (*mg_callback_t)(enum mg_event event,
                                struct mg_connection *conn,
                                const struct mg_request_info *request_info);

void *mg_get_plugin(struct mg_connection *conn);
void *mg_lock_master_plugin(struct mg_connection *conn);
void mg_unlock_master_plugin(struct mg_connection *conn);
void *mg_get_user_data(struct mg_connection *conn);
const char *mg_get_conn_buf(struct mg_connection *conn, int *buf_size);

// Start web server.
//
// Parameters:
//   callback: user defined event handling function or NULL.
//   options: NULL terminated list of option_name, option_value pairs that
//            specify Mongoose configuration parameters.
//
// Side-effects: on UNIX, ignores SIGCHLD and SIGPIPE signals. If custom
//    processing is required for these, signal handlers must be set up
//    after calling mg_start().
//
//
// Example:
//   const char *options[] = {
//     "document_root", "/var/www",
//     "listening_ports", "80,443s",
//     NULL
//   };
//   struct mg_context *ctx = mg_start(&my_func, NULL, options);
//
// Please refer to http://code.google.com/p/mongoose/wiki/MongooseManual
// for the list of valid option and their possible values.
//
// Return:
//   web server context, or NULL on error.
struct mg_context *mg_start(mg_callback_t callback, void *user_data,
                            const char **options);

void mg_handle_cgi_request(struct mg_connection *conn, const char *prog);
int mg_strncasecmp(const char *s1, const char *s2, size_t len);
int mg_strcasecmp(const char *s1, const char *s2);
char * mg_strdup(const char *str);
char * mg_strndup(const char *ptr, size_t len);

// Stop the web server.
//
// Must be called last, when an application wants to stop the web server and
// release all associated resources. This function blocks until all Mongoose
// threads are stopped. Context pointer becomes invalid.
void mg_stop(struct mg_context *);


// Get the value of particular configuration parameter.
// The value returned is read-only. Mongoose does not allow changing
// configuration at run time.
// If given parameter name is not valid, NULL is returned. For valid
// names, return value is guaranteed to be non-NULL. If parameter is not
// set, zero-length string is returned.
const char *mg_get_option(const struct mg_context *ctx, const char *name);


// Return array of strings that represent valid configuration options.
// For each option, a short name, long name, and default value is returned.
// Array is NULL terminated.
const char **mg_get_valid_option_names(void);


// Add, edit or delete the entry in the passwords file.
//
// This function allows an application to manipulate .htpasswd files on the
// fly by adding, deleting and changing user records. This is one of the
// several ways of implementing authentication on the server side. For another,
// cookie-based way please refer to the examples/chat.c in the source tree.
//
// If password is not NULL, entry is added (or modified if already exists).
// If password is NULL, entry is deleted.
//
// Return:
//   1 on success, 0 on error.
int mg_modify_passwords_file(const char *passwords_file_name,
                             const char *domain,
                             const char *user,
                             const char *password);


// Return information associated with the request.
// These functions always succeed.
const struct mg_request_info *mg_get_request_info(const struct mg_connection *);
void *mg_get_user_data(struct mg_connection *);
const char *mg_get_log_message(const struct mg_connection *);
int mg_get_reply_status_code(const struct mg_connection *);
void *mg_get_ssl_context(const struct mg_connection *);


// Send data to the client.
// Return:
//  0   when the connection has been closed
//  -1  on error
//  number of bytes written on success
int mg_write(struct mg_connection *, const void *buf, size_t len);


// Send data to the browser using printf() semantics.
//
// Works exactly like mg_write(), but allows to do message formatting.
// Below are the macros for enabling compiler-specific checks for
// printf-like arguments.

#undef PRINTF_FORMAT_STRING
#if _MSC_VER >= 1400
#include <sal.h>
#if _MSC_VER > 1400
#define PRINTF_FORMAT_STRING(s) _Printf_format_string_ s
#else
#define PRINTF_FORMAT_STRING(s) __format_string s
#endif
#else
#define PRINTF_FORMAT_STRING(s) s
#endif

#ifdef __GNUC__
#define PRINTF_ARGS(x, y) __attribute__((format(printf, x, y)))
#else
#define PRINTF_ARGS(x, y)
#endif

int mg_printf(struct mg_connection *,
              PRINTF_FORMAT_STRING(const char *fmt), ...) PRINTF_ARGS(2, 3);


// Send contents of the entire file together with HTTP headers.
void mg_send_file(struct mg_connection *conn, const char *path);


// Read data from the remote end, return number of bytes read.
int mg_read(struct mg_connection *, void *buf, size_t len);

// Send a 401 Unauthorized response to the browser.
//
// This triggers a username/password entry in the browser.  The realm
// in the request is set to the AUTHENTICATION_DOMAIN option.
// If nonce is non-NULL, it is sent as the nonce of the authentication
// request, else a nonce is generated.
void mg_send_authorization_request(struct mg_connection *conn, const char *nonce);

// Get the value of particular HTTP header.
//
// This is a helper function. It traverses request_info->http_headers array,
// and if the header is present in the array, returns its value. If it is
// not present, NULL is returned.
const char *mg_get_header(const struct mg_connection *, const char *name);
//int mg_get_connection_count(const struct mg_connection *conn);
// Get a value of particular form variable.
//
// Parameters:
//   data: pointer to form-uri-encoded buffer. This could be either POST data,
//         or request_info.query_string.
//   data_len: length of the encoded data.
//   var_name: variable name to decode from the buffer
//   buf: destination buffer for the decoded variable
//   buf_len: length of the destination buffer
//
// Return:
//   On success, length of the decoded variable.
//   On error:
//      -1 (variable not found, or destination buffer is too small).
//      -2 (destination buffer is NULL or zero length).
//
// Destination buffer is guaranteed to be '\0' - terminated. In case of
// failure, dst[0] == '\0'.
int mg_find_var(const char *buf, size_t buf_len, const char *name,
                const char **start);
int mg_get_var(const char *data, size_t data_len,
    const char *var_name, char *buf, size_t buf_len);

// Fetch value of certain cookie variable into the destination buffer.
//
// Destination buffer is guaranteed to be '\0' - terminated. In case of
// failure, dst[0] == '\0'. Note that RFC allows many occurrences of the same
// parameter. This function returns only first occurrence.
//
// Return:
//   On success, value length.
//   On error, -1 (either "Cookie:" header is not present at all, or the
//   requested parameter is not found, or destination buffer is too small
//   to hold the value).
int mg_get_cookie(const struct mg_connection *,
    const char *cookie_name, char *buf, size_t buf_len);
int mg_find_cookie(const struct mg_connection *,
    const char *cookie_name, const char **start);

const char *mg_get_document_root(const struct mg_connection *);

// Connect to the remote web server.
// Return:
//   On success, valid pointer to the new connection
//   On error, NULL
struct mg_connection *mg_connect(struct mg_context *ctx,
                                 const char *host, int port, int use_ssl);


// Close the connection opened by mg_connect().
void mg_close_connection(struct mg_connection *conn);


// Download given URL to a given file.
//   url: URL to download
//   path: file name where to save the data
//   request_info: pointer to a structure that will hold parsed reply headers
//   buf, bul_len: a buffer for the reply headers
// Return:
//   On error, NULL
//   On success, opened file stream to the downloaded contents. The stream
//   is positioned to the end of the file. It is the user's responsibility
//   to fclose() the opened file stream.
FILE *mg_fetch(struct mg_context *ctx, const char *url, const char *path,
               char *buf, size_t buf_len, struct mg_request_info *request_info);


// Convenience function -- create detached thread.
// Return: 0 on success, non-0 on error.
typedef void * (*mg_thread_func_t)(void *);
int mg_start_thread(mg_thread_func_t f, void *p);


// Return builtin mime type for the given file name.
// For unrecognized extensions, "text/plain" is returned.
const char *mg_get_builtin_mime_type(const char *file_name);


// Return Mongoose version.
const char *mg_version(void);


// MD5 hash given strings.
// Buffer 'buf' must be 33 bytes long. Varargs is a NULL terminated list of
// ASCIIz strings. When function returns, buf will contain human-readable
// MD5 hash. Example:
//   char buf[33];
//   mg_md5(buf, "aa", "bb", NULL);
#define MD5_BUF_SIZE 33
typedef char md5_buf_t[MD5_BUF_SIZE];
void mg_md5(md5_buf_t buf, ...);

// Stringify binary data. Output buffer must be twice as big as input,
// because each byte takes 2 bytes in string representation
void mg_bin2str(char *to, const unsigned char *p, size_t len);

#ifdef USE_MG_MD5
#include <stdint.h>
typedef struct MD5Context {
  uint32_t buf[4];
  uint32_t bits[2];
  unsigned char in[64];
} MD5_CTX;
void MD5Init(MD5_CTX *ctx);
void MD5Transform(uint32_t buf[4], uint32_t const in[16]);
void MD5Update(MD5_CTX *ctx, unsigned char const *buf, unsigned len);
void MD5Final(unsigned char digest[16], MD5_CTX *ctx);
#endif //HAVE_MD5

// URL-decode input buffer into destination buffer.
// 0-terminate the destination buffer. Return the length of decoded data.
// form-url-encoded data differs from URI encoding in a way that it
// uses '+' as character for space, see RFC 1866 section 8.2.1
// http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
size_t mg_url_decode(const char *src, size_t src_len, char *dst,
                         size_t dst_len, int is_form_url_encoded);

const char * mg_url_encode_to(const char *src, char *dst, size_t dst_len);
char * mg_url_encode(const char *src);

#if defined(_WIN32) || defined(_WIN32_WCE)
#include <windows.h>
typedef HANDLE mg_thread_mutex_t;
typedef struct {HANDLE signal, broadcast;} mg_thread_cond_t;
typedef DWORD mg_thread_t;
typedef void mg_thread_mutexattr_t;
typedef void mg_thread_condattr_t;
#else
#include <pthread.h>
typedef pthread_mutex_t mg_thread_mutex_t;
typedef pthread_cond_t mg_thread_cond_t;
typedef pthread_t mg_thread_t;
typedef pthread_mutexattr_t mg_thread_mutexattr_t;
typedef pthread_condattr_t mg_thread_condattr_t;
#endif

#define HAS_MG_THREAD_FUNC_DEFINED
//typedef void * (*mg_thread_func_t)(void *);

mg_thread_t mg_thread_self(void);
int mg_thread_mutex_init(mg_thread_mutex_t *mutex, const mg_thread_mutexattr_t *attr);
int mg_thread_mutex_destroy(mg_thread_mutex_t *mutex);
int mg_thread_mutex_lock(mg_thread_mutex_t *mutex);
int mg_thread_mutex_unlock(mg_thread_mutex_t *mutex);
int mg_thread_cond_init(mg_thread_cond_t *cv, const mg_thread_condattr_t *attr);
int mg_thread_cond_wait(mg_thread_cond_t *cv, mg_thread_mutex_t *mutex);
int mg_thread_cond_signal(mg_thread_cond_t *cv);
int mg_thread_cond_broadcast(mg_thread_cond_t *cv);
int mg_thread_cond_destroy(mg_thread_cond_t *cv);
int mg_start_thread(mg_thread_func_t func, void *param);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // MONGOOSE_HEADER_INCLUDED
