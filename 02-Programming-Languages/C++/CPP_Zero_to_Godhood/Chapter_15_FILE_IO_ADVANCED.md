# FILE I/O ADVANCED


## 14.1 Binary File I/O

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write binary data
    ofstream outfile("data.bin", ios::binary);
    
    int numbers[] = {10, 20, 30, 40, 50};
    outfile.write((char*)numbers, sizeof(numbers));
    outfile.close();
    
    // Read binary data
    ifstream infile("data.bin", ios::binary);
    
    int buffer[5];
    infile.read((char*)buffer, sizeof(buffer));
    
    for (int i = 0; i < 5; i++) {
        cout << buffer[i] << " ";
    }
    cout << endl;
    
    infile.close();
    
    return 0;
}
```

## 14.2 Stream Positioning

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write to file
    ofstream outfile("test.txt");
    outfile << "0123456789";
    outfile.close();
    
    // Read with positioning
    ifstream infile("test.txt");
    
    // Tell position
    cout << "Current position: " << infile.tellg() << endl;
    
    // Seek to position
    infile.seekg(5);
    char c;
    infile.get(c);
    cout << "Character at position 5: " << c << endl;  // '5'
    
    // Seek from end
    infile.seekg(-3, ios::end);
    infile.get(c);
    cout << "Third from end: " << c << endl;  // '7'
    
    infile.close();
    
    return 0;
}
```

---
