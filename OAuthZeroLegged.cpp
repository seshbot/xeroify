#include "OAuthZeroLegged.h"

#include <QUrl>
#include <QPair>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>


namespace
{

// OAuth 1/1.1 Request Parameters
auto O2_OAUTH_CALLBACK = QString("oauth_callback");
auto O2_OAUTH_CONSUMER_KEY = QString("oauth_consumer_key");
auto O2_OAUTH_NONCE = QString("oauth_nonce");
auto O2_OAUTH_SIGNATURE = QString("oauth_signature");
auto O2_OAUTH_SIGNATURE_METHOD = QString("oauth_signature_method");
auto O2_OAUTH_TIMESTAMP = QString("oauth_timestamp");
auto O2_OAUTH_VERSION = QString("oauth_version");

// OAuth 1/1.1 Response Parameters
auto O2_OAUTH_TOKEN = QString("oauth_token");
auto O2_OAUTH_TOKEN_SECRET = QString("oauth_token_secret");
auto O2_OAUTH_CALLBACK_CONFIRMED = QString("oauth_callback_confirmed");
auto O2_OAUTH_VERFIER = QString("oauth_verifier");

// Standard HTTP headers
auto O2_HTTP_AUTHORIZATION_HEADER = "Authorization";
auto O2_MIME_TYPE_XFORM = "application/x-www-form-urlencoded";


/// Get HTTP operation name.
QString getOperationName(QNetworkAccessManager::Operation op)
{
    switch (op) {
    case QNetworkAccessManager::GetOperation: return "GET";
    case QNetworkAccessManager::PostOperation: return "POST";
    case QNetworkAccessManager::PutOperation: return "PUT";
    case QNetworkAccessManager::DeleteOperation: return "DEL";
    default: return "";
    }
}

QByteArray nonce()
{
    static bool firstTime = true;
    if (firstTime)
    {
        firstTime = false;
        qsrand(QTime::currentTime().msec());
    }
    QString u = QString::number(QDateTime::currentDateTimeUtc().toTime_t());
    u.append(QString::number(qrand()));
    return u.toLatin1();
}


//
// crypto
//

RSA* getRsaFromKey(const QString &key)
{
    BIO *bufio;
    QByteArray data = key.toLocal8Bit();
    char *pem_key_buffer = data.data();
    int pem_key_buffer_len = strlen(pem_key_buffer);

    bufio = BIO_new_mem_buf((void*)pem_key_buffer, pem_key_buffer_len);
    RSA *rsa = 0;
    rsa = RSA_new();
    rsa = PEM_read_bio_RSAPrivateKey(bufio, 0, 0, NULL);
    if (rsa == 0) {
        printf("Can not parse RSA data. Errors:\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }
    return rsa;
}

QString rsa_sha1(const QString &message, const QString &key)
{
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests();

    EVP_PKEY *evpKey = 0;
    evpKey = EVP_PKEY_new();

    RSA *rsa = 0;
    rsa = getRsaFromKey(key);

    EVP_PKEY_set1_RSA(evpKey, rsa);

    EVP_MD_CTX* ctx = 0;
    ctx = EVP_MD_CTX_create();
    EVP_SignInit_ex(ctx, EVP_sha1(), 0);
    QByteArray latinMessage = message.toLatin1();
    EVP_SignUpdate(ctx, latinMessage.data(), strlen(latinMessage.data()));

    const int MAX_LEN = 1024;
    unsigned char *sig = new unsigned char[MAX_LEN];
    unsigned int sigLen;

    EVP_SignFinal(ctx, sig, &sigLen, evpKey);
    sig[sigLen] = '\0';

    EVP_MD_CTX_destroy(ctx);
    RSA_free(rsa);
    EVP_PKEY_free(evpKey);
    ERR_free_strings();

    QByteArray ret((char *)sig, sigLen);

    return QString(ret.toBase64());
}


//
// sign
//

QByteArray createQueryParameters(const QList<QPair<QString, QString>>& parameters) {
    QByteArray ret;
    bool first = true;
    foreach (auto& pair, parameters) {
        if (first) {
            first = false;
        } else {
            ret.append("&");
        }
        ret.append(QUrl::toPercentEncoding(pair.first) + "=" + QUrl::toPercentEncoding(pair.second));
    }
    return ret;
}

QByteArray createRequestBase(const QUrl &url,
                             QNetworkAccessManager::Operation op,
                             const QList<QPair<QString, QString>>& oauthParams,
                             const QList<QPair<QString, QString>>& otherParams)
{
    QByteArray base;

    // Initialize base string with the operation name (e.g. "GET") and the base URL
    base.append(getOperationName(op).toUtf8() + "&");
    base.append(QUrl::toPercentEncoding(url.toString(QUrl::RemoveQuery)) + "&");

    // Append a sorted+encoded list of all request parameters to the base string
    QList<QPair<QString, QString>> headers(oauthParams);
    headers.append(otherParams);
    qSort(headers);

    base.append(QUrl::toPercentEncoding(createQueryParameters(headers)));
    return base;
}


//
// oauth
//

QList<QPair<QString, QString>> createResourceAccessOAuthParams(const QUrl& url,
                                                                     QNetworkAccessManager::Operation operation,
                                                                     const QList<QPair<QString, QString>>& params,
                                                                     const QString& consumerKey,
                                                                     const QString& rsaKey)
{
   QList<QPair<QString, QString>> result;
   result.append(qMakePair(O2_OAUTH_CONSUMER_KEY, consumerKey));
   result.append(qMakePair(O2_OAUTH_VERSION, QString("1.0")));
   result.append(qMakePair(O2_OAUTH_TOKEN, consumerKey));
   result.append(qMakePair(O2_OAUTH_SIGNATURE_METHOD, QString("RSA-SHA1")));
   result.append(qMakePair(O2_OAUTH_NONCE, nonce()));
   result.append(qMakePair(O2_OAUTH_TIMESTAMP, QString::number(QDateTime::currentDateTimeUtc().toTime_t())));

   auto requestBase = createRequestBase(url, operation, result, params);
   auto oauthSignature = rsa_sha1(requestBase, rsaKey);

   // Add signature parameter
   result.append(qMakePair(O2_OAUTH_SIGNATURE, oauthSignature));

   return result;

}


//
// HTTP operations
//

QByteArray buildAuthorizationHeader(const QList<QPair<QString, QString>>& oauthParams)
{
    bool first = true;
    QByteArray ret("OAuth ");
    QList<QPair<QString, QString>> headers(oauthParams);
    qSort(headers);
    foreach (auto& pair, headers) {
        if (first) {
            first = false;
        } else {
            ret.append(",");
        }
        ret.append(pair.first);
        ret.append("=\"");
        ret.append(QUrl::toPercentEncoding(pair.second));
        ret.append("\"");
    }
    return ret;
}

} // anonymous namespace


//
// OAuthOneLegged
//

OAuthZeroLegged::OAuthZeroLegged(const QString& consumerKey, const QString& rsaPrivateKey)
   : consumerKey_(consumerKey)
   , rsaPrivateKey_(rsaPrivateKey)
{
}

void OAuthZeroLegged::setConsumerKey(const QString& consumerKey)
{
   consumerKey_ = consumerKey;
}

void OAuthZeroLegged::setRsaPrivateKey(const QString& rsaPrivateKey)
{
   rsaPrivateKey_ = rsaPrivateKey;
}

QNetworkRequest& OAuthZeroLegged::authorize(QNetworkRequest& request,
                                           QNetworkAccessManager::Operation op,
                                           const QList<QPair<QString, QString>>& params)
{
   if (op == QNetworkAccessManager::PostOperation)
   {
      request.setHeader(QNetworkRequest::ContentTypeHeader, O2_MIME_TYPE_XFORM);
   }

   auto oauthParams = createResourceAccessOAuthParams(request.url(), op, params, consumerKey_, rsaPrivateKey_);
   request.setRawHeader(O2_HTTP_AUTHORIZATION_HEADER, buildAuthorizationHeader(oauthParams));

   return request;
}
