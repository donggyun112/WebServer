#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>
#include <map>

enum StatusCode {
  // Information responses
  Continue_100 = 100,
  SwitchingProtocol_101 = 101,
  Processing_102 = 102,
  EarlyHints_103 = 103,

  // Successful responses
  OK_200 = 200,
  Created_201 = 201,
  Accepted_202 = 202,
  NonAuthoritativeInformation_203 = 203,
  NoContent_204 = 204,
  ResetContent_205 = 205,
  PartialContent_206 = 206,
  MultiStatus_207 = 207,
  AlreadyReported_208 = 208,
  IMUsed_226 = 226,

  // Redirection messages
  MultipleChoices_300 = 300,
  MovedPermanently_301 = 301,
  Found_302 = 302,
  SeeOther_303 = 303,
  NotModified_304 = 304,
  UseProxy_305 = 305,
  unused_306 = 306,
  TemporaryRedirect_307 = 307,
  PermanentRedirect_308 = 308,

  // Client error responses
  BadRequest_400 = 400,
  Unauthorized_401 = 401,
  PaymentRequired_402 = 402,
  Forbidden_403 = 403,
  NotFound_404 = 404,
  MethodNotAllowed_405 = 405,
  NotAcceptable_406 = 406,
  ProxyAuthenticationRequired_407 = 407,
  RequestTimeout_408 = 408,
  Conflict_409 = 409,
  Gone_410 = 410,
  LengthRequired_411 = 411,
  PreconditionFailed_412 = 412,
  PayloadTooLarge_413 = 413,
  UriTooLong_414 = 414,
  UnsupportedMediaType_415 = 415,
  RangeNotSatisfiable_416 = 416,
  ExpectationFailed_417 = 417,
  ImATeapot_418 = 418,
  MisdirectedRequest_421 = 421,
  UnprocessableContent_422 = 422,
  Locked_423 = 423,
  FailedDependency_424 = 424,
  TooEarly_425 = 425,
  UpgradeRequired_426 = 426,
  PreconditionRequired_428 = 428,
  TooManyRequests_429 = 429,
  RequestHeaderFieldsTooLarge_431 = 431,
  UnavailableForLegalReasons_451 = 451,

  // Server error responses
  InternalServerError_500 = 500,
  NotImplemented_501 = 501,
  BadGateway_502 = 502,
  ServiceUnavailable_503 = 503,
  GatewayTimeout_504 = 504,
  HttpVersionNotSupported_505 = 505,
  VariantAlsoNegotiates_506 = 506,
  InsufficientStorage_507 = 507,
  LoopDetected_508 = 508,
  NotExtended_510 = 510,
  NetworkAuthenticationRequired_511 = 511,

};

class Error {
public:
    static std::map<int, std::string> errors;
	Error() {
		errors[Continue_100] = "Continue";
		errors[SwitchingProtocol_101] = "Switching Protocol";
		errors[Processing_102] = "Processing";
		errors[EarlyHints_103] = "Early Hints";

		// Successful responses
		errors[OK_200] = "OK";
		errors[Created_201] = "Created";
		errors[Accepted_202] = "Accepted";
		errors[NonAuthoritativeInformation_203] = "Non-Authoritative Information";
		errors[NoContent_204] = "No Content";
		errors[ResetContent_205] = "Reset Content";
		errors[PartialContent_206] = "Partial Content";
		errors[MultiStatus_207] = "Multi-Status";
		errors[AlreadyReported_208] = "Already Reported";
		errors[IMUsed_226] = "IM Used";
		

		// Redirection messages
		errors[MultipleChoices_300] = "Multiple Choices";
		errors[MovedPermanently_301] = "Moved Permanently";
		errors[Found_302] = "Found";
		errors[SeeOther_303] = "See Other";
		errors[NotModified_304] = "Not Modified";
		errors[UseProxy_305] = "Use Proxy";
		errors[unused_306] = "unused";
		errors[TemporaryRedirect_307] = "Temporary Redirect";
		errors[PermanentRedirect_308] = "Permanent Redirect";

		// Client error responses
		errors[BadRequest_400] = "Bad Request";
		errors[Unauthorized_401] = "Unauthorized";
		errors[PaymentRequired_402] = "Payment Required";
		errors[Forbidden_403] = "Forbidden";
		errors[NotFound_404] = "Not Found";
		errors[MethodNotAllowed_405] = "Method Not Allowed";
		errors[NotAcceptable_406] = "Not Acceptable";
		errors[ProxyAuthenticationRequired_407] = "Proxy Authentication Required";
		errors[RequestTimeout_408] = "Request Timeout";
		errors[Conflict_409] = "Conflict";
		errors[Gone_410] = "Gone";
		errors[LengthRequired_411] = "Length Required";
		errors[PreconditionFailed_412] = "Precondition Failed";
		errors[PayloadTooLarge_413] = "Payload Too Large";
		errors[UriTooLong_414] = "URI Too Long";
		errors[UnsupportedMediaType_415] = "Unsupported Media Type";
		errors[RangeNotSatisfiable_416] = "Range Not Satisfiable";
		errors[ExpectationFailed_417] = "Expectation Failed";
		errors[ImATeapot_418] = "I'm a teapot";
		errors[MisdirectedRequest_421] = "Misdirected Request";
		errors[UnprocessableContent_422] = "Unprocessable Content";
		errors[Locked_423] = "Locked";
		errors[FailedDependency_424] = "Failed Dependency";
		errors[TooEarly_425] = "Too Early";
		errors[UpgradeRequired_426] = "Upgrade Required";
		errors[PreconditionRequired_428] = "Precondition Required";
		errors[TooManyRequests_429] = "Too Many Requests";
		errors[RequestHeaderFieldsTooLarge_431] = "Request Header Fields Too Large";
		errors[UnavailableForLegalReasons_451] = "Unavailable For Legal Reasons";

		// Server error responses
		errors[InternalServerError_500] = "Internal Server Error";
		errors[NotImplemented_501] = "Not Implemented";
		errors[BadGateway_502] = "Bad Gateway";
		errors[ServiceUnavailable_503] = "Service Unavailable";
		errors[GatewayTimeout_504] = "Gateway Timeout";
		errors[HttpVersionNotSupported_505] = "HTTP Version Not Supported";
		errors[VariantAlsoNegotiates_506] = "Variant Also Negotiates";
		errors[InsufficientStorage_507] = "Insufficient Storage";
		errors[LoopDetected_508] = "Loop Detected";
		errors[NotExtended_510] = "Not Extended";
		errors[NetworkAuthenticationRequired_511] = "Network Authentication Required";
	};
	const std::string& operator[](int status) const {
		return errors.at(status);
	}

	static std::string getError(int status) {
		return errors[status];
	}
};





typedef int FD;
typedef unsigned int Port;
typedef int Status;

enum STATE { SUCCESS=0, FAILURE=-1 };
#include <map>

// --> 가비지 컬렉터
// 사용법
// 	GarbageCollector<Socket> gc;
//  SmartPointer<Socket> tt(new Socket(), gc);
//  Socket *sock = tt.get(); or tt->socket();
//  FD fd = tt->socket();

template <typename T>
class SmartPointer {
private:
    T* object;
    static std::map<T*, int> referenceCount;

public:
    SmartPointer(T* obj) : object(obj) {
        referenceCount[object]++;
    }

    SmartPointer(const SmartPointer& other) : object(other.object) {
        referenceCount[object]++;
    }

    ~SmartPointer() {
        if (--referenceCount[object] == 0) {
            delete object;
            referenceCount.erase(object);
        }
    }

    T* operator->() {
        return object;
    }

    T& operator*() {
        return *object;
    }

    T* get() {
        return object;
    }
};

template <typename T>
std::map<T*, int> SmartPointer<T>::referenceCount;

#endif
