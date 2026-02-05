# PRODUCTION & PROFESSIONAL


## LARGE-SCALE PROJECT ARCHITECTURE

## 1.1 Layered Architecture

```cpp
// src/layers/presentation/controller.h
#ifndef PRESENTATION_CONTROLLER_H
#define PRESENTATION_CONTROLLER_H

#include "../domain/user.h"
#include "../application/user_service.h"

namespace presentation {
    class UserController {
    private:
        application::UserService& service;
        
    public:
        UserController(application::UserService& s) : service(s) {}
        
        void create_user(const std::string& name, const std::string& email) {
            auto user = service.create(name, email);
            display_result(user);
        }
        
    private:
        void display_result(const domain::User& user);
    };
}

#endif
```

```cpp
// src/layers/application/user_service.h
#ifndef APPLICATION_USER_SERVICE_H
#define APPLICATION_USER_SERVICE_H

#include "../domain/user.h"
#include "../infrastructure/user_repository.h"

namespace application {
    class UserService {
    private:
        infrastructure::UserRepository& repo;
        
    public:
        UserService(infrastructure::UserRepository& r) : repo(r) {}
        
        domain::User create(const std::string& name, const std::string& email) {
            // Business logic
            domain::User user(name, email);
            validate_user(user);
            return repo.save(user);
        }
        
    private:
        void validate_user(const domain::User& user);
    };
}

#endif
```

```cpp
// src/layers/domain/user.h
#ifndef DOMAIN_USER_H
#define DOMAIN_USER_H

namespace domain {
    class User {
    private:
        int id;
        std::string name;
        std::string email;
        
    public:
        User(const std::string& n, const std::string& e)
            : id(0), name(n), email(e) {}
        
        // Domain methods
        bool is_valid() const;
        void update_email(const std::string& new_email);
    };
}

#endif
```

```cpp
// src/layers/infrastructure/user_repository.h
#ifndef INFRASTRUCTURE_USER_REPOSITORY_H
#define INFRASTRUCTURE_USER_REPOSITORY_H

#include "../domain/user.h"
#include "database_connection.h"

namespace infrastructure {
    class UserRepository {
    private:
        DatabaseConnection& db;
        
    public:
        UserRepository(DatabaseConnection& d) : db(d) {}
        
        domain::User save(const domain::User& user);
        std::optional<domain::User> find_by_id(int id);
        std::vector<domain::User> find_all();
    };
}

#endif
```

## 1.2 Microservices Architecture

```cpp
// Service 1: User Service
namespace user_service {
    class UserAPI {
    private:
        application::UserService& service;
        http::Server& server;
        
    public:
        void setup_routes() {
            server.post("/users", [this](const auto& req) {
                auto user = service.create(req.name, req.email);
                return http::Response::ok(user.to_json());
            });
            
            server.get("/users/:id", [this](const auto& req) {
                auto user = service.find(req.id);
                return http::Response::ok(user.to_json());
            });
        }
    };
}

// Service 2: Order Service
namespace order_service {
    class OrderAPI {
    private:
        application::OrderService& service;
        http::Client& http_client;
        
    public:
        void create_order(int user_id, const Order& order) {
            // Call user service
            auto user = http_client.get("http://user-service/users/" + std::to_string(user_id));
            
            // Create order
            service.create(user_id, order);
        }
    };
}
```

---

## CODE ORGANIZATION & PROJECT STRUCTURE

## 2.1 Modern CMake Project Structure

```cmake
# CMakeLists.txt - Project root
cmake_minimum_required(VERSION 3.20)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project structure
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find dependencies
find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
```

```cmake
# src/CMakeLists.txt - Main library
add_library(mylib
    domain/user.cpp
    domain/order.cpp
    application/user_service.cpp
    infrastructure/user_repository.cpp
)

target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mylib
    PUBLIC Boost::system
    PRIVATE Boost::thread
)

# Executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE mylib)
```

```cmake
# tests/CMakeLists.txt - Test suite
add_executable(tests
    test_user_service.cpp
    test_user_repository.cpp
)

target_link_libraries(tests
    PRIVATE mylib Catch2::Catch2WithMain
)

add_test(NAME AllTests COMMAND tests)
```

## 2.2 Header Organization

```cpp
// include/mylib/version.h
#ifndef MYLIB_VERSION_H
#define MYLIB_VERSION_H

#define MYLIB_VERSION_MAJOR 1
#define MYLIB_VERSION_MINOR 0
#define MYLIB_VERSION_PATCH 0

namespace mylib {
    struct Version {
        static constexpr int major = MYLIB_VERSION_MAJOR;
        static constexpr int minor = MYLIB_VERSION_MINOR;
        static constexpr int patch = MYLIB_VERSION_PATCH;
    };
}

#endif
```

```cpp
// include/mylib/mylib.h - Main header
#ifndef MYLIB_H
#define MYLIB_H

// Version
#include "mylib/version.h"

// Core components
#include "mylib/domain/user.h"
#include "mylib/domain/order.h"

// Services
#include "mylib/application/user_service.h"
#include "mylib/application/order_service.h"

// Infrastructure
#include "mylib/infrastructure/database.h"

// Re-export main classes
namespace mylib {
    using domain::User;
    using domain::Order;
    using application::UserService;
}

#endif
```

## 2.3 Modern CMake with Modules (C++20)

Using C++20 Modules requires CMake 3.28+.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(ModulesDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library with modules
add_library(math_engine)
target_sources(math_engine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            src/math.cppm
            src/vector.cppm
)

# Executable consuming modules
add_executable(app main.cpp)
target_link_libraries(app PRIVATE math_engine)
```

---

## BUILD SYSTEMS & COMPILATION

## 3.1 Conan Package Manager

```ini
# conanfile.txt
[requires]
boost/1.81.0
fmt/9.1.0
nlohmann_json/3.11.2
catch2/3.3.2

[generators]
CMakeDeps
CMakeToolchain

[options]
boost/*:shared=False
```

```python
# conanfile.py - Advanced
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake

class MyProjectConan(ConanFile):
    name = "myproject"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    requires = "boost/1.81.0", "fmt/9.1.0"
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
```

## 3.2 Incremental Build Optimization

```cmake
# Enable ccache for faster rebuilds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# Unity builds for faster compilation
set_target_properties(mylib PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 8
)

# Precompiled headers
target_precompile_headers(mylib PRIVATE
    <vector>
    <string>
    <memory>
    <boost/asio.hpp>
)
```

---

## TESTING STRATEGIES

## 4.1 Unit Testing with Catch2

```cpp
#include <catch2/catch_all.hpp>
#include "mylib/application/user_service.h"
#include "test_fixtures.h"

TEST_CASE("UserService creates users correctly", "[user_service]") {
    auto repo = MockUserRepository();
    UserService service(repo);
    
    SECTION("Valid user creation") {
        auto user = service.create("John Doe", "john@example.com");
        
        REQUIRE(user.name == "John Doe");
        REQUIRE(user.email == "john@example.com");
        REQUIRE(repo.save_called == true);
    }
    
    SECTION("Invalid email rejects") {
        REQUIRE_THROWS_AS(
            service.create("John", "invalid-email"),
            InvalidEmailException
        );
    }
}

TEST_CASE("UserService handles duplicates", "[user_service]") {
    auto repo = MockUserRepository();
    UserService service(repo);
    
    service.create("John", "john@example.com");
    
    REQUIRE_THROWS_AS(
        service.create("Jane", "john@example.com"),
        DuplicateEmailException
    );
}
```

## 4.2 Integration Testing

```cpp
TEST_CASE("User creation workflow", "[integration]") {
    // Setup
    Database db = setup_test_database();
    UserRepository repo(db);
    UserService service(repo);
    
    // Execute
    auto user = service.create("John Doe", "john@example.com");
    auto fetched = repo.find_by_id(user.id);
    
    // Verify
    REQUIRE(fetched.has_value());
    REQUIRE(fetched->name == "John Doe");
    
    // Cleanup
    cleanup_test_database(db);
}
```

## 4.3 Test Fixtures & Mocks

```cpp
class UserRepositoryMock : public UserRepository {
public:
    bool save_called = false;
    std::vector<User> saved_users;
    
    User save(const User& user) override {
        save_called = true;
        User u = user;
        u.id = ++last_id;
        saved_users.push_back(u);
        return u;
    }
    
private:
    static int last_id;
};

class UserServiceTest {
protected:
    UserRepositoryMock repo;
    UserService service{repo};
};

TEST_CASE_METHOD(UserServiceTest, "Multiple users") {
    auto u1 = service.create("John", "john@example.com");
    auto u2 = service.create("Jane", "jane@example.com");
    
    REQUIRE(repo.saved_users.size() == 2);
}
```

## 4.4 Advanced Mocking with Google Mock (GMock)

For complex interactions, use GMock.

```cpp
#include <gmock/gmock.h>

class MockDB : public Database {
public:
    MOCK_METHOD(bool, connect, (string), (override));
    MOCK_METHOD(void, query, (string), (override));
};

TEST(DBTest, LoginSequence) {
    MockDB db;
    
    // Expect connect called once with "admin"
    EXPECT_CALL(db, connect("admin"))
        .Times(1)
        .WillOnce(testing::Return(true));
        
    // Expect query called any number of times
    EXPECT_CALL(db, query(testing::_))
        .Times(testing::AtLeast(0));
        
    UserManager mgr(&db);
    mgr.login("admin");
}
```

---

## DEBUGGING & PROFILING

## 5.1 Debug Utilities

```cpp
// include/mylib/debug.h
#ifndef MYLIB_DEBUG_H
#define MYLIB_DEBUG_H

#include <iostream>
#include <source_location>

namespace mylib::debug {
    enum class Level { Debug, Info, Warning, Error };
    
    class Logger {
    private:
        static Level current_level;
        
    public:
        template<typename... Args>
        static void log(Level level, std::format_string<Args...> fmt, Args... args) {
            if (level < current_level) return;
            
            auto loc = std::source_location::current();
            std::cerr << std::format("[{}:{}:{}] {}",
                loc.file_name(), loc.line(), loc.column(),
                std::format(fmt, args...)
            ) << "\n";
        }
        
        static void set_level(Level l) { current_level = l; }
    };
    
    #ifdef DEBUG
        #define LOG_DEBUG(...) debug::Logger::log(debug::Level::Debug, __VA_ARGS__)
        #define LOG_INFO(...) debug::Logger::log(debug::Level::Info, __VA_ARGS__)
    #else
        #define LOG_DEBUG(...)
        #define LOG_INFO(...)
    #endif
}

#endif
```

## 5.2 Performance Profiling

```cpp
#include <chrono>
#include <map>

class PerformanceProfiler {
private:
    struct Measurement {
        std::chrono::nanoseconds total{0};
        int count = 0;
        std::chrono::nanoseconds min{LLONG_MAX};
        std::chrono::nanoseconds max{0};
    };
    
    std::map<std::string, Measurement> measurements;
    
public:
    class Scope {
    private:
        std::string name;
        PerformanceProfiler& profiler;
        std::chrono::high_resolution_clock::time_point start;
        
    public:
        Scope(std::string n, PerformanceProfiler& p) 
            : name(n), profiler(p), start(std::chrono::high_resolution_clock::now()) {}
        
        ~Scope() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            profiler.record(name, duration);
        }
    };
    
    void record(const std::string& name, std::chrono::nanoseconds duration) {
        auto& m = measurements[name];
        m.total += duration;
        m.count++;
        m.min = std::min(m.min, duration);
        m.max = std::max(m.max, duration);
    }
    
    void report() const {
        std::cout << "Performance Report:\n";
        std::cout << std::string(60, '=') << "\n";
        
        for (const auto& [name, m] : measurements) {
            auto avg = m.total.count() / m.count;
            std::cout << std::format("{:<30} | Count: {:>5} | Avg: {:>10}ns | Min: {:>10}ns | Max: {:>10}ns\n",
                name, m.count, avg, m.min.count(), m.max.count());
        }
    }
};

// Usage
#define PROFILE(name) PerformanceProfiler::Scope _scope(name, get_profiler())

void process_data(const std::vector<int>& data) {
    PROFILE("process_data");
    
    {
        PROFILE("sort");
        std::sort(data.begin(), data.end());
    }
    
    {
        PROFILE("filter");
        // Filter implementation
    }
}
```

## 5.3 Memory Profiling with AddressSanitizer

```cmake
# CMakeLists.txt - AddressSanitizer configuration
option(ENABLE_SANITIZER "Enable AddressSanitizer" OFF)

if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()
```

---

## VERSION CONTROL & COLLABORATION

## 6.1 Git Workflow

```bash
# .gitignore - Standard C++ project
build/
dist/
*.o
*.a
*.so
*.exe
.vscode/
.idea/
*.swp
CMakeLists.txt.user
*.qbs.user

# Git configuration - .git/config
[user]
    name = Developer
    email = dev@company.com

[core]
    editor = vim
    autocrlf = input

[pull]
    rebase = true

[branch]
    autosetuprebase = always
```

## 6.2 Feature Branch Workflow

```bash
# Main branch is production-ready
git checkout main
git pull origin main

# Create feature branch
git checkout -b feature/user-authentication

# Commit with conventional commits
git add src/
git commit -m "feat: implement JWT authentication"

# Rebase and squash commits
git rebase -i main

# Create PR and request review
git push origin feature/user-authentication
```

---

## DOCUMENTATION & KNOWLEDGE TRANSFER

## 7.1 Code Documentation with Doxygen

```cpp
/**
 * @file user_service.h
 * @brief User service implementation for managing user lifecycle
 * @author John Doe
 * @version 1.0.0
 * @date 2024-01-15
 */

namespace application {
    /**
     * @class UserService
     * @brief Service for managing user operations
     * 
     * UserService provides business logic for user management including
     * creation, deletion, and modification. It validates input and
     * persists data through the UserRepository.
     * 
     * @example
     * @code
     * UserService service(repository);
     * auto user = service.create("John Doe", "john@example.com");
     * @endcode
     */
    class UserService {
    public:
        /**
         * @brief Creates a new user
         * 
         * @param name The user's full name
         * @param email The user's email address
         * 
         * @return User The created user object with assigned ID
         * 
         * @throws InvalidNameException if name is empty
         * @throws InvalidEmailException if email is invalid
         * @throws DuplicateEmailException if email already exists
         * 
         * @complexity O(n) where n is the number of existing users
         */
        User create(const std::string& name, const std::string& email);
        
        /**
         * @brief Updates user information
         * 
         * @param id User ID
         * @param name New name
         * @param email New email
         * 
         * @return User Updated user object
         * 
         * @throws UserNotFoundException if user doesn't exist
         * @throws InvalidEmailException if email is invalid
         */
        User update(int id, const std::string& name, const std::string& email);
    };
}
```

## 7.2 Architecture Decision Records (ADR)

```markdown
# ADR-001: Use Layered Architecture

## Status
Accepted

## Context
The system needs to be scalable, testable, and maintainable.
Different concerns (UI, business logic, data access) must be separated.

## Decision
We will implement a layered architecture with:
- Presentation Layer (Controllers, Views)
- Application Layer (Services, Use Cases)
- Domain Layer (Business Logic)
- Infrastructure Layer (Database, External Services)

## Consequences
### Positive
- Clear separation of concerns
- Easy to test (mock layers)
- Scalable architecture

### Negative
- More files to navigate
- Increased complexity for simple features
- Potential performance overhead from layer crossing

## Alternatives Considered
- Hexagonal (Ports & Adapters) - More complex, chosen layered for simplicity
- Microservices - Future consideration for scalability
```

---

## SECURITY & SAFETY

## 8.1 Input Validation

```cpp
#include <regex>
#include <stdexcept>

namespace security {
    class InputValidator {
    public:
        static void validate_email(const std::string& email) {
            static const std::regex email_regex(
                R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
            );
            
            if (!std::regex_match(email, email_regex)) {
                throw std::invalid_argument("Invalid email format");
            }
        }
        
        static void validate_length(const std::string& str, 
                                   size_t min_length, size_t max_length) {
            if (str.length() < min_length || str.length() > max_length) {
                throw std::invalid_argument(
                    std::format("String length must be between {} and {}", 
                               min_length, max_length)
                );
            }
        }
        
        static void validate_integer_range(int value, int min, int max) {
            if (value < min || value > max) {
                throw std::out_of_range(
                    std::format("Value must be between {} and {}", min, max)
                );
            }
        }
    };
}
```

## 8.2 SQL Injection Prevention

```cpp
#include <sqlite3.h>

namespace database {
    class SafeQuery {
    private:
        sqlite3* db;
        
    public:
        std::vector<User> find_by_email_safe(const std::string& email) {
            const char* sql = "SELECT * FROM users WHERE email = ?";
            sqlite3_stmt* stmt;
            
            // Prepare statement (prevents SQL injection)
            int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("SQL error");
            }
            
            // Bind parameters safely
            sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
            
            std::vector<User> results;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // Extract results
                int id = sqlite3_column_int(stmt, 0);
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                
                results.emplace_back(id, name, email);
            }
            
            sqlite3_finalize(stmt);
            return results;
        }
    };
}
```

---

## PERFORMANCE ENGINEERING

## 9.1 Benchmarking Framework

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = "hello world";
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StringCreation);

static void BM_VectorOperations(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < 1000; i++) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_VectorOperations);

BENCHMARK_MAIN();
```

## 9.2 Load Testing

```cpp
class LoadTester {
public:
    struct Result {
        int total_requests;
        std::chrono::milliseconds total_time;
        double requests_per_second;
        double avg_latency_ms;
        double p99_latency_ms;
    };
    
    Result run_load_test(
        std::function<void()> operation,
        int num_threads,
        int requests_per_thread
    ) {
        std::vector<std::thread> threads;
        std::vector<std::chrono::nanoseconds> latencies;
        std::mutex latencies_mutex;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int t = 0; t < num_threads; t++) {
            threads.emplace_back([&, t] {
                for (int r = 0; r < requests_per_thread; r++) {
                    auto op_start = std::chrono::high_resolution_clock::now();
                    operation();
                    auto op_end = std::chrono::high_resolution_clock::now();
                    
                    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        op_end - op_start
                    );
                    
                    {
                        std::lock_guard lock(latencies_mutex);
                        latencies.push_back(latency);
                    }
                }
            });
        }
        
        for (auto& t : threads) t.join();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Calculate percentiles
        std::sort(latencies.begin(), latencies.end());
        int p99_idx = (latencies.size() * 99) / 100;
        
        Result result;
        result.total_requests = num_threads * requests_per_thread;
        result.total_time = total_time;
        result.requests_per_second = result.total_requests / (total_time.count() / 1000.0);
        
        long long sum = 0;
        for (auto l : latencies) sum += l.count();
        result.avg_latency_ms = (sum / latencies.size()) / 1e6;
        result.p99_latency_ms = latencies[p99_idx].count() / 1e6;
        
        return result;
    }
};
```

---

## ERROR HANDLING & RECOVERY

## 10.1 Exception Hierarchy

```cpp
#include <stdexcept>

namespace application {
    // Base exception
    class ApplicationError : public std::runtime_error {
    protected:
        int error_code;
        std::string error_context;
        
    public:
        ApplicationError(const std::string& msg, int code = -1)
            : std::runtime_error(msg), error_code(code) {}
        
        int get_error_code() const { return error_code; }
        void set_context(const std::string& ctx) { error_context = ctx; }
    };
    
    // Domain exceptions
    class DomainError : public ApplicationError {
    public:
        DomainError(const std::string& msg) 
            : ApplicationError(msg, 1001) {}
    };
    
    class ValidationError : public DomainError {
    public:
        ValidationError(const std::string& msg) 
            : DomainError("Validation: " + msg) {}
    };
    
    class InvalidEmailException : public ValidationError {
    public:
        InvalidEmailException(const std::string& email)
            : ValidationError("Invalid email: " + email) {}
    };
    
    // Repository exceptions
    class RepositoryError : public ApplicationError {
    public:
        RepositoryError(const std::string& msg) 
            : ApplicationError(msg, 2001) {}
    };
    
    class UserNotFoundException : public RepositoryError {
    public:
        UserNotFoundException(int id)
            : RepositoryError("User not found: " + std::to_string(id)) {}
    };
}
```

## 10.2 Error Recovery Patterns

```cpp
class RetryPolicy {
public:
    struct Config {
        int max_retries = 3;
        std::chrono::milliseconds initial_delay{100};
        double backoff_multiplier = 2.0;
        int max_delay_ms = 10000;
    };
    
    template<typename F>
    static auto execute_with_retry(F operation, const Config& config)
        -> std::invoke_result_t<F> {
        
        std::exception_ptr last_exception;
        auto delay = config.initial_delay;
        
        for (int attempt = 0; attempt <= config.max_retries; attempt++) {
            try {
                return operation();
            } catch (const std::exception& e) {
                last_exception = std::current_exception();
                
                if (attempt < config.max_retries) {
                    LOG_INFO("Retry attempt {} after {}ms", 
                            attempt + 1, delay.count());
                    std::this_thread::sleep_for(delay);
                    
                    delay = std::chrono::milliseconds(
                        std::min(static_cast<int>(delay.count() * config.backoff_multiplier),
                                config.max_delay_ms)
                    );
                }
            }
        }
        
        std::rethrow_exception(last_exception);
    }
};

// Usage
auto user = RetryPolicy::execute_with_retry(
    [&]() { return repository.find_user(id); },
    RetryPolicy::Config{.max_retries = 5}
);
```

---

## DEPLOYMENT & DEVOPS

## 11.1 Docker Containerization

```dockerfile
# Dockerfile - Multi-stage build
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    libboost-all-dev

WORKDIR /build
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)
RUN cmake --install build --prefix /install

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    ca-certificates

COPY --from=builder /install /usr/local

EXPOSE 8080
CMD ["/usr/local/bin/myapp"]
```

## 11.2 Kubernetes Deployment

```yaml
# deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: myapp
  labels:
    app: myapp
spec:
  replicas: 3
  selector:
    matchLabels:
      app: myapp
  template:
    metadata:
      labels:
        app: myapp
    spec:
      containers:
      - name: myapp
        image: myapp:1.0.0
        ports:
        - containerPort: 8080
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5
```

## 11.3 CI/CD Pipeline (GitHub Actions)

Automate building and testing.

```yaml
name: C++ CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install Dependencies
      run: sudo apt-get install -y libboost-dev cmake
      
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
    - name: Build
      run: cmake --build build
      
    - name: Test
      run: cd build && ctest --output-on-failure
```

---

## CODE REVIEW & QUALITY

## 12.1 Code Review Checklist

```markdown
# Code Review Checklist

## Functionality
- [ ] Code implements the required feature
- [ ] Code handles all edge cases
- [ ] Error handling is appropriate
- [ ] Tests cover the implementation

## Code Quality
- [ ] Variable/function names are clear
- [ ] Code is DRY (Don't Repeat Yourself)
- [ ] Functions are appropriately sized
- [ ] Complexity is reasonable

## Performance
- [ ] No obvious performance issues
- [ ] Memory usage is appropriate
- [ ] Algorithms are efficient
- [ ] No memory leaks

## Security
- [ ] Input validation is present
- [ ] No SQL injection vulnerabilities
- [ ] No buffer overflows
- [ ] Sensitive data is protected

## Testing
- [ ] Unit tests are present
- [ ] Integration tests pass
- [ ] Test coverage is adequate
- [ ] Edge cases are tested

## Documentation
- [ ] Code is well-commented
- [ ] Public APIs are documented
- [ ] Changes are documented
- [ ] Architecture decisions are recorded
```

## 12.2 Static Code Analysis

```cmake
# CMakeLists.txt - Clang-Tidy integration
find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}"
        "-checks=*"
        "-header-filter=.*"
        "-fix"
    )
endif()

# Cppcheck integration
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK}
            --enable=all
            --suppress=missingIncludeSystem
            ${CMAKE_SOURCE_DIR}/src
    )
endif()
```

---

## TECHNICAL DEBT MANAGEMENT

## 13.1 Tracking Technical Debt

```cpp
// Technical debt marker
namespace technical_debt {
    /**
     * @deprecated Refactor this when performance is not critical.
     * Use optimized_algorithm_v2 instead.
     * 
     * @todo Refactor by Q2 2024
     * @complexity O(n) - needs optimization
     */
    void inefficient_sort(std::vector<int>& data) {
        // Bubble sort - inefficient but simple
        for (size_t i = 0; i < data.size(); i++) {
            for (size_t j = 0; j < data.size() - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    /**
     * @deprecated Temporary workaround for issue #123.
     * Remove when backend API is fixed.
     * @todo Track: https://github.com/team/project/issues/123
     */
    std::string get_user_name(int id) {
        // Workaround: return hardcoded values until backend is fixed
        static const std::map<int, std::string> workaround{
            {1, "John Doe"},
            {2, "Jane Smith"}
        };
        return workaround.at(id);
    }
}
```

## 13.2 Refactoring Strategy

```cpp
// Old code
class User {
public:
    void save_to_db(const std::string& connection_string) {
        // Direct database access - tightly coupled
    }
    
    void send_email(const std::string& subject, const std::string& body) {
        // Email sending logic mixed with domain logic
    }
};

// Refactored code
class User {
private:
    int id;
    std::string name;
    std::string email;
    
public:
    int get_id() const { return id; }
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};

// Separated concerns
class UserRepository {
public:
    void save(const User& user);
};

class UserEmailService {
public:
    void send_notification(const User& user, const std::string& message);
};
```

---

## LEGACY CODE MODERNIZATION

## 14.1 Incremental Modernization

```cpp
// Legacy code (C++98 style)
class LegacyUser {
public:
    char* name;      // Raw pointer
    char* email;     // Raw pointer
    
    LegacyUser(const char* n, const char* e) {
        name = new char[strlen(n) + 1];
        strcpy(name, n);  // Unsafe
        email = new char[strlen(e) + 1];
        strcpy(email, e);  // Unsafe
    }
    
    ~LegacyUser() {
        delete[] name;
        delete[] email;
    }
};

// Step 1: Add modern wrapper
class ModernUserWrapper {
private:
    LegacyUser* legacy;
    
public:
    ModernUserWrapper(const std::string& name, const std::string& email) {
        legacy = new LegacyUser(name.c_str(), email.c_str());
    }
    
    ~ModernUserWrapper() { delete legacy; }
    
    std::string_view get_name() const { return legacy->name; }
    std::string_view get_email() const { return legacy->email; }
};

// Step 2: Full modernization
class ModernUser {
private:
    std::string name;
    std::string email;
    
public:
    ModernUser(std::string n, std::string e)
        : name(std::move(n)), email(std::move(e)) {}
    
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};
```

---

## LEADERSHIP & TEAM MANAGEMENT

## 15.1 Mentoring Framework

```markdown
# Mentoring Guidelines for C++ Teams

## Levels

### Junior Developer (0-1 year)
- Focus: Understanding fundamentals
- Guidance: Pair programming, code reviews, architecture training
- Goals: Contribute to features, improve C++ knowledge

### Mid-Level Developer (1-3 years)
- Focus: Mastering patterns and best practices
- Guidance: Design reviews, mentoring juniors, taking ownership
- Goals: Lead features, improve design decisions

### Senior Developer (3+ years)
- Focus: Architecture, leadership, knowledge sharing
- Guidance: Strategic decisions, cross-team collaboration
- Goals: Shape team direction, mentor other seniors

## Mentoring Actions
1. Code review with detailed feedback
2. Design discussions before implementation
3. Pair programming sessions
4. Knowledge sharing sessions
5. Challenge with progressively harder problems
```

## 15.2 Technical Decision Making

```markdown
# RFC (Request For Comments) Template

## Title
Brief description of the proposal

## Motivation
Why this change is needed

## Detailed Design
Technical approach and architecture

## Trade-offs
What we're giving up

## Alternatives
Other approaches considered

## Implementation Plan
Steps to implement

## Timeline
Expected completion

## Success Metrics
How we measure success

## Discussion
Team feedback period (1 week)

## Decision
Final decision and rationale
```

---

---

Final decision and rationale

```

---
