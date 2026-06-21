# Chapter 58: C++ In The Cloud

# C++ IN THE CLOUD

Modern C++ is a first-class citizen in Cloud Native architectures.

### 20.1 Microservices with C++

Using frameworks like **Drogon** or **Userver** (Yandex) for high-throughput services.

**Example: Simple HTTP Endpoint (Drogon style)**
```cpp
// Controller
void Handler::get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello from High-Performance Microservice!");
    callback(resp);
}
```

### 20.2 Serverless C++ (AWS Lambda)

Using the AWS Lambda C++ Runtime to run native binaries.
*   **Cold Start**: < 5ms (vs 100ms+ for Java/Node).
*   **Cost**: Lower duration due to speed.

```cpp
#include <aws/lambda-runtime/runtime.h>

aws::lambda_runtime::invocation_response handler(aws::lambda_runtime::invocation_request const& req) {
    return aws::lambda_runtime::invocation_response::success("Processed!", "application/json");
}

int main() {
    aws::lambda_runtime::run_handler(handler);
    return 0;
}
```

