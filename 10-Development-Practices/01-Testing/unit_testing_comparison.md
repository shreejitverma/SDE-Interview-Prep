# Unit Testing Frameworks Comparison

Distinguished Engineers care about **Testability** and **Tooling**.

| Feature | **Google Test (GTest)** (C++) | **PyTest** (Python) | **JUnit 5** (Java) |
| :--- | :--- | :--- | :--- |
| **Philosophy** | Macros, Strict Typing, OOP | Functions, Fixtures, Magic Asserts | OOP, Annotations |
| **Assertions** | `ASSERT_EQ(a, b)` | `assert a == b` (Simple!) | `assertEquals(a, b)` |
| **Setup/Teardown** | `SetUp()`, `TearDown()` methods | `@pytest.fixture` (Dependency Injection) | `@BeforeEach`, `@AfterEach` |
| **Parameterization** | `INSTANTIATE_TEST_SUITE_P` | `@pytest.mark.parametrize` | `@ParameterizedTest`, `@ValueSource` |
| **Mocking** | **GMock** (Complex, requires virtual methods) | `unittest.mock` (Easy, monkey-patching) | **Mockito** (Standard, uses Reflection) |

## 1. C++ GTest Example
```cpp
#include <gtest/gtest.h>

int add(int a, int b) { return a + b; }

TEST(MathTest, AddsPositiveNumbers) {
    EXPECT_EQ(add(2, 3), 5);
}
```

## 2. Python PyTest Example
```python
import pytest

def add(a, b): return a + b

@pytest.mark.parametrize("a,b,expected", [(2,3,5), (-1,1,0)])
def test_add(a, b, expected):
    assert add(a, b) == expected
```

## 3. Java JUnit 5 Example
```java
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.assertEquals;

class MathTest {
    @Test
    void addsPositiveNumbers() {
        assertEquals(5, MathUtils.add(2, 3));
    }
}
```
